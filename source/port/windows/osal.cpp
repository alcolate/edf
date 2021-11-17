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
#include <windows.h>
#include "osal.h"


T_HANDLE TaskCreate(	TaskExec pxTaskCode,
                        const char * const pcName,
                        uint16_t usStackDepth,
                        void * const pvParameters,
                        uint32_t uxPriority, 
                        Q_HANDLE *Q, 
                        uint32_t Q_Size)
{
    T_HANDLE CreatedTask;

    CreatedTask = CreateThread(
        NULL,
        (usStackDepth < 1024U ? 1024U : usStackDepth),
        &pxTaskCode,
        pvParameters,
        0,
        &Q);

    QueueCreate(Q_Size, 0);

    return CreatedTask;
}


Q_HANDLE QueueCreate( uint32_t uxQueueLength, uint32_t uxItemSize)
{
    PeekMessage ( &msg , NULL , 0 , 0 , PM_REMOVE );

    return 0;
}

bool QueueReceive(Q_HANDLE Q, void * const pvBuffer, uint32_t TimeOut)
{
    return GetMessage ( &msg , NULL ,  0 , 0 );            
}



/*..........................................................................*/
/*..........................................................................*/
void QueueSend(Q_HANDLE q, void const * const p, bool FromISR)
{
	BaseType_t status;

    BOOL status = PostThreadMessage ( (LPDWORD)q ,  (UINT)&p , 0 , 0 );


    //configASSERT(status == pdTRUE);
}


/*..........................................................................*/




extern void TimeEvent_tickFromISR();
/* Application hooks used in this project ==================================*/
/* NOTE: only the "FromISR" API variants are allowed in vApplicationTickHook*/
extern "C"
{

void vApplicationTickHook(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;


    /* perform clock tick processing */
    TimeEvent_tickFromISR();

    /* notify FreeRTOS to perform context switch from ISR, if needed */
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
}
