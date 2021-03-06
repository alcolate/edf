/*****************************************************************************
Copyright 2021 The Edf Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Contact information:
<9183399@qq.com>
*****************************************************************************/
#include <string.h>        
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/msg.h>
#include <cerrno>
#include <mqueue.h>
#include <time.h>
#include "osal.h"
#include "Edf.h"

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter" 
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

#define USE_MQV  1

struct MsgBuf 
{
    long mtype;       /* message type, must be > 0 */
    char mtext[40];    /* message data */
};

struct MQMessage
{
    long Type;
    time_t TimeStamp;
    void * P;
};

#define MSG_TYPE  1

#define MSGID_TO_HANDLE(id)  ((Q_HANDLE)((id) + 1))
#define HANDLE_TO_MSGID(handle)  ((int)(handle) - 1)

static time_t StartTime = (time_t)0;

extern void TimeEvent_Tick(bool FromISR);

static void * ThreadExe(void *p)
{
    (static_cast<Edf::CActive*>(p))->Run();

    return NULL;
}

static void * TimerExe(void *p)
{
    while (1)
    {
        usleep(TICK_RATE_MS * 1000);
        TimeEvent_Tick(false);
    }
}

void OS_TastSetPriority(T_HANDLE Task, uint32_t Priority)
{
	
}

T_HANDLE OS_TaskCreate(	const char * const pcName,
                        uint16_t usStackDepth,
                        void * const pvParameters,
                        uint32_t uxPriority, 
                        Q_HANDLE *Q, 
                        uint32_t Q_Size)
{
    pthread_t CreatedTask;

    pthread_attr_t attr;
    int result;

    if (StartTime == 0)
    {
        time(&StartTime);
    }

    *Q = OS_QueueCreate(Q_Size, 0);

    result = pthread_attr_init(&attr);
    ASSERT(result == 0);

    pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

    struct sched_param param;
    param.sched_priority = uxPriority
                           + (sched_get_priority_max(SCHED_FIFO)
                              - MAX_PRIORITIES - 3U);
    //LOG_DEBUG("prio = %d\r\n", param.sched_priority);
    result = pthread_attr_setschedparam(&attr, &param);
    ASSERT(result == 0);

    pthread_attr_setstacksize(&attr, usStackDepth);

    int err = pthread_create(&CreatedTask, &attr, &ThreadExe, pvParameters);

    pthread_attr_destroy(&attr);

    if (err != 0) 
    {
        err = pthread_create(&CreatedTask, NULL, &ThreadExe, pvParameters);
    }

    //pthread_attr_destroy(&attr);
    //Dbg;


    return (T_HANDLE)CreatedTask;
}

void QueueClear(Q_HANDLE Q)
{
    MsgBuf mbuf;

    while (-1 != msgrcv(HANDLE_TO_MSGID(Q), &mbuf, sizeof(void *), MSG_TYPE, MSG_NOERROR | IPC_NOWAIT));
}

Q_HANDLE OS_QueueCreate( uint32_t uxQueueLength, uint32_t uxItemSize)
{
#if USE_MQV == 1
    static int proj_id = 0;
    ASSERT(proj_id < 255);
    key_t MsgKey = ftok(".", proj_id ++);
    int MsgId = msgget(MsgKey, IPC_CREAT | 0666);
    LOG_DEBUG("key id = %x, key = %x, msgId = %x \r\n", uxQueueLength, MsgKey, MsgId);
    perror("msgget");
    LOG_DEBUG("msg id: %d\r\n", MsgId);
    
    ASSERT(MsgId != -1);

    QueueClear(MSGID_TO_HANDLE(MsgId));

    return MSGID_TO_HANDLE(MsgId);

#else
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = uxQueueLength;
    attr.mq_msgsize = sizeof(MQMessage);
    attr.mq_curmsgs = 0;
    static uint32_t qid = 0;
    char qname[20] = {0};
    snprintf(qname, sizeof(qname) - 1, "%s%03d", "/mqedf", ++qid);

    mqd_t mq = mq_open(qname, O_RDWR | O_CREAT, 0666, &attr);
	if (mq < 0)
    {
		perror("mq_open");
        exit(0);
	}
    else
    {
		LOG_DEBUG("mq_open success: %d \n",mq);        
	}

    return (Q_HANDLE)mq;
#endif
}

bool OS_QueueReceive(Q_HANDLE Q, void * const P, uint32_t TimeOut)
{
#if USE_MQV == 1    
    MsgBuf mbuf;

    int result = msgrcv(HANDLE_TO_MSGID(Q), &mbuf, sizeof(void *), MSG_TYPE, 0);
    //perror("msgrcv");
    //LOG_DEBUG("%s: addr = %llx \r\n", __FUNCTION__, *(uint64_t*)mbuf.mtext);
    if (-1 == result)
    {
        perror("msgrcv");
        return false;
    }
    else
    {
        memcpy(P, mbuf.mtext, sizeof(void *));
        return true;
    }
#else
    MQMessage msg;
    msg.TimeStamp = 0;
    int result = mq_receive((mqd_t)Q, (char *)&msg, sizeof(MQMessage), NULL);
    memcpy(P, (char*)&(msg.P), sizeof(void *));
    //LOG_DEBUG("%s: msg = %d, addr = %llx \r\n", __FUNCTION__, (mqd_t)Q, *(uint64_t*)P);
    if (result == -1)
    {
        perror("mq_receive");        
        return false;
    }
    else
    {       
        if (msg.TimeStamp < StartTime) 
        {
            //LOG_DEBUG("msg = %d time out (start = %d, stamp = %d)\r\n", (mqd_t)Q, StartTime, msg.TimeStamp);
            return false;
        }
        return true;
    }

#endif

}


bool OS_QueueSend(Q_HANDLE Q, void const * const P, bool FromISR)
{
#if USE_MQV == 1      
    MsgBuf mbuf;
    mbuf.mtype = MSG_TYPE;
    memcpy(&mbuf.mtext, (void *)&P, sizeof(void *));
    //LOG_DEBUG("%s: addr = %llx \r\n", __FUNCTION__, *(uint64_t*)mbuf.mtext);
    int result = msgsnd(HANDLE_TO_MSGID(Q), &mbuf, sizeof(void *), 0);
    //perror("msgsnd");
    if (-1 == result)
    {
        perror("msgsnd");
        return false;
    }
    else
    {
        return true;
    }

#else
    //LOG_DEBUG("%s: msg = %d, addr = %llx \r\n", __FUNCTION__, (mqd_t)Q, (uint64_t)P);
    MQMessage msg;
    msg.Type = 0;
    msg.P = P;
    time(&msg.TimeStamp);
    int result = mq_send((mqd_t)Q, (char *)&msg, sizeof(MQMessage), 0);
    if (result == -1)
    {
        perror("mq_send");
        ASSERT(false);
        return false;
    }
    else
    {
        return true;
    }
#endif    
}

static pthread_mutex_t *g_CriticalMutex = NULL;
uint32_t OS_EnterCritical(bool FromISR)
{
    if (g_CriticalMutex == NULL)
    {
        static pthread_mutex_t CriticalMutex;
        if (0 != pthread_mutex_init(&CriticalMutex, NULL))
        {
            ASSERT(0);            
        }
        g_CriticalMutex = &CriticalMutex;
           
    }

    pthread_mutex_lock(g_CriticalMutex);

    return 0;
}
void OS_ExitCritical(uint32_t Flag, bool FromISR)
{
    ASSERT(g_CriticalMutex != NULL);
    pthread_mutex_unlock(g_CriticalMutex);
}

void OS_Sleep(uint32_t Milliseconds)
{
    usleep(Milliseconds * 1000);
}


void OS_Start(void)

{
    pthread_t CreatedTask;

    pthread_attr_t attr;
    int result;


    result = pthread_attr_init(&attr);
    ASSERT(result == 0);

    pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

    struct sched_param param;
    param.sched_priority = (sched_get_priority_max(SCHED_FIFO) - MAX_PRIORITIES - 3U);
    //LOG_DEBUG("prio = %d\r\n", param.sched_priority);
    result = pthread_attr_setschedparam(&attr, &param);
    ASSERT(result == 0);

    pthread_attr_setstacksize(&attr, 1024);

    int err = pthread_create(&CreatedTask, &attr, &TimerExe, NULL);

    pthread_attr_destroy(&attr);

    if (err != 0) 
    {
        err = pthread_create(&CreatedTask, NULL, &TimerExe, NULL);
    }

}

void OS_Restart()
{

}

uint32_t OS_Tick(void)
{
    return 0;
}

void OS_MemoryUsage(size_t &Free, size_t &Minimum)
{

}
/*..........................................................................*/

