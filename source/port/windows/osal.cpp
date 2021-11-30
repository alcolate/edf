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
#include <stdio.h>
#include "osal.h"
#include "edf.h"

static DWORD WINAPI ThreadExe(LPVOID p) 
{
    (static_cast<CActive*>(p))->Run();

    return 0;
}

T_HANDLE TaskCreate(	TaskExec pxTaskCode,
                        const char * const pcName,
                        uint16_t usStackDepth,
                        void * const pvParameters,
                        uint32_t uxPriority, 
                        Q_HANDLE *Q, 
                        uint32_t Q_Size)
{
    T_HANDLE CreatedTask;
    DWORD id;

    CreatedTask = CreateThread(
        NULL,
        1024U,
        &ThreadExe,
        pvParameters,
        0,
        &id);

    *Q = (Q_HANDLE)id;

    QueueCreate(Q_Size, 0);

    return CreatedTask;
}


Q_HANDLE QueueCreate( uint32_t uxQueueLength, uint32_t uxItemSize)
{
    MSG    msg;

    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    return 0;
}

bool QueueReceive(Q_HANDLE Q, void * const pvBuffer, uint32_t TimeOut)
{
    MSG    msg;

    BOOL result = GetMessage(&msg, NULL, 0, 0);

    if (!result)
    {
        return FALSE;
    }
    else
    {
        memcpy(pvBuffer, &(msg.wParam), sizeof(msg.wParam));

        return TRUE;
    }
}

/*..........................................................................*/
/*..........................................................................*/
bool QueueSend(Q_HANDLE q, void const * const p, bool FromISR)
{

    BOOL status = PostThreadMessage ( (DWORD)q , WM_USER, (WPARAM)p, 0 );

    return status;
}


/*..........................................................................*/

extern void TimeEvent_tickFromISR();

void vApplicationTickHook(void)
{
    while (1)
    {
        Sleep(TICK_RATE_MS);
        TimeEvent_tickFromISR();
    }
       

}

