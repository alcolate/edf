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
#include <stdio.h>
#include "osal.h"
#include "Edf.h"

struct MsgBuf 
{
    long mtype;       /* message type, must be > 0 */
    char mtext[4];    /* message data */
};

#define Dbg  printf("%s (%d) \r\n", __FUNCTION__, __LINE__)
static void * ThreadExe(void *p)
{
    Dbg;
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

    *Q = QueueCreate(0, 0);

    result = pthread_attr_init(&attr);
    ASSERT(result == 0);

    pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

    struct sched_param param;
    param.sched_priority = uxPriority
                           + (sched_get_priority_max(SCHED_FIFO)
                              - MAX_PRIORITIES - 3U);
    printf("prio = %d\r\n", param.sched_priority);
    result = pthread_attr_setschedparam(&attr, &param);
    ASSERT(result == 0);

    pthread_attr_setstacksize(&attr, usStackDepth);

    Dbg;
    int err = pthread_create(&CreatedTask, &attr, &ThreadExe, pvParameters);
        Dbg;
    pthread_attr_destroy(&attr);
    Dbg;
    if (err != 0) 
    {
        Dbg;
        // Creating p-thread with the SCHED_FIFO policy failed. Most likely
        // this application has no superuser privileges, so we just fall
        // back to the default SCHED_OTHER policy and priority 0.
        //
        //pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
        //param.sched_priority = 0;
        //pthread_attr_setschedparam(&attr, &param);
        err = pthread_create(&CreatedTask, NULL, &ThreadExe, pvParameters);
        printf("err = %d\r\n", err);
    }
    Dbg;
    //pthread_attr_destroy(&attr);
    //Dbg;


    return (T_HANDLE)CreatedTask;
}


Q_HANDLE QueueCreate( uint32_t uxQueueLength, uint32_t uxItemSize)
{

    key_t MsgKey = ftok(".", 89);
    int MsgId = msgget(MsgKey, IPC_CREAT | 0x0666);
    perror("msg id: ");
    ASSERT(MsgId != -1);
    return (Q_HANDLE)MsgId;
}

bool QueueReceive(Q_HANDLE Q, void * const pvBuffer, uint32_t TimeOut)
{
    MsgBuf mbuf;
//Dbg;
    int result = msgrcv((int)Q, &mbuf, sizeof(mbuf.mtext), 1, 0);
    perror("msg id: ");
    if (-1 == result)
    {
        return false;
    }
    else
    {
        memcpy(pvBuffer, mbuf.mtext, sizeof(void *));

        return true;
    }
}


bool QueueSend(Q_HANDLE q, void const * const p, bool FromISR)
{
    MsgBuf mbuf;
    mbuf.mtype = 1;
Dbg;
    memcpy(&mbuf.mtext, (void *)&p, sizeof(mbuf.mtext));

    return msgsnd((int)q, &mbuf, sizeof(mbuf.mtext), 0) != -1;
}

static pthread_mutex_t *g_CriticalMutex = NULL;
void OS_EnterCritical(void)
{
    if (g_CriticalMutex == 0)
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

