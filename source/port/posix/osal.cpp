/*****************************************************************************
* MIT License:
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*
* Contact information:
* <9183399@qq.com>
*****************************************************************************/
#include <limits.h>        
#include <sys/mman.h>     
#include <sys/select.h>
#include <sys/ioctl.h>
#include <string.h>        
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/msg.h>
#include <cerrno>
#include <mqueue.h>
#include <time.h>
#include <stdio.h>
#include "osal.h"
#include "Edf.h"

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

static void * ThreadExe(void *p)
{
    (static_cast<CActive*>(p))->Run();

    return NULL;
}

T_HANDLE TaskCreate(	const char * const pcName,
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

    *Q = QueueCreate(Q_Size, 0);

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
    int MsgId = (int)Q;
    
    while (-1 != msgrcv(HANDLE_TO_MSGID(Q), &mbuf, sizeof(void *), MSG_TYPE, MSG_NOERROR | IPC_NOWAIT));
}

Q_HANDLE QueueCreate( uint32_t uxQueueLength, uint32_t uxItemSize)
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
	}
    else
    {
		printf("mq_open success: %d \n",mq);
	}

    return (Q_HANDLE)mq;
#endif
}

bool QueueReceive(Q_HANDLE Q, void * const P, uint32_t TimeOut)
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
    LOG_DEBUG("%s: msg = %d, addr = %llx \r\n", __FUNCTION__, (mqd_t)Q, *(uint64_t*)P);
    if (result == -1)
    {
        perror("mq_receive");        
        return false;
    }
    else
    {       
        if (msg.TimeStamp < StartTime) 
        {
            LOG_DEBUG("msg = %d time out (start = %d, stamp = %d)\r\n", (mqd_t)Q, StartTime, msg.TimeStamp);
            return false;
        }
        return true;
    }

#endif

}


bool QueueSend(Q_HANDLE Q, void const * const P, bool FromISR)
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
    LOG_DEBUG("%s: msg = %d, addr = %llx \r\n", __FUNCTION__, (mqd_t)Q, (uint64_t)P);
    MQMessage msg;
    msg.Type = 0;
    msg.P = P;
    time(&msg.TimeStamp);  
    struct timespec timeout;
    timeout.tv_sec = msg.TimeStamp + 1;
    timeout.tv_nsec = 0;  
    int result = mq_timedsend((mqd_t)Q, (char *)&msg, sizeof(MQMessage), 0, &timeout);
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
void OS_EnterCritical(void)
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
}
void OS_ExitCritical(void)
{
    ASSERT(g_CriticalMutex != NULL);
    pthread_mutex_unlock(g_CriticalMutex);
}

/*..........................................................................*/

extern void TimeEvent_tickFromISR();

void vApplicationTickHook(void)
{
    while (1)
    {   
        usleep(TICK_RATE_MS * 1000);
        TimeEvent_tickFromISR();
    }      

}

