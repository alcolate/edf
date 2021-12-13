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
#include "Edf.h"

Q_HANDLE OS_QueueCreate(uint32_t uxQueueLength, uint32_t uxItemSize);

static DWORD WINAPI ThreadExe(LPVOID p) 
{
    OS_QueueCreate(0, 0);
    
    (static_cast<CActive*>(p))->Run();

    return 0;
}

T_HANDLE OS_TaskCreate(	const char * const pcName,
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
        usStackDepth,
        &ThreadExe,
        pvParameters,
        0,
        &id);

    *Q = (Q_HANDLE)id;

    return CreatedTask;
}


Q_HANDLE OS_QueueCreate( uint32_t uxQueueLength, uint32_t uxItemSize)
{
    MSG    msg;

    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    return 0;
}

bool OS_QueueReceive(Q_HANDLE Q, void * const P, uint32_t TimeOut)
{
    MSG    msg;

    BOOL result = GetMessage(&msg, NULL, 0, 0);

    if (!result)
    {
        return FALSE;
    }
    else
    {
        memcpy(P, &(msg.wParam), sizeof(msg.wParam));

        return TRUE;
    }
}


bool OS_QueueSend(Q_HANDLE Q, void const * const P, bool FromISR)
{

    BOOL status = PostThreadMessage ( (DWORD)Q , WM_USER, (WPARAM)P, 0 );

    return status;
}

CRITICAL_SECTION*  g_hMutex = NULL;

uint32_t OS_EnterCritical(bool FromISR)
{
    if (g_hMutex == NULL)
    {
        static CRITICAL_SECTION l_win32CritSect;
        InitializeCriticalSection(&l_win32CritSect);
        g_hMutex = &l_win32CritSect;
        ASSERT(g_hMutex != NULL);   
    }

    EnterCriticalSection(g_hMutex);
    return 0;
}
void OS_ExitCritical(uint32_t Flag, bool FromISR)
{
    ASSERT(g_hMutex != NULL);
    LeaveCriticalSection(g_hMutex);
}


void OS_Start(void)
{
    extern void TimeEvent_tickFromISR();
    
    while (1)
    {
        Sleep(TICK_RATE_MS);
        TimeEvent_tickFromISR();
    }
}
/*..........................................................................*/



