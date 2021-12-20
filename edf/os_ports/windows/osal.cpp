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
#include <windows.h>
#include <stdio.h>
#include "osal.h"
#include "Edf.h"

extern void TimeEvent_Tick(bool FromISR);

static DWORD WINAPI ThreadExe(LPVOID p) 
{
    OS_QueueCreate(0, 0);
    
    (static_cast<Edf::CActive*>(p))->Run();

    return 0;
}
static DWORD WINAPI TimerExe(LPVOID p)
{
    while (1)
    {
        Sleep(TICK_RATE_MS);
        TimeEvent_Tick(false);
    }
}
void OS_TastSetPriority(T_HANDLE Task, uint32_t Priority)
{
	//vTaskPrioritySet(Task, Priority);
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

void OS_Sleep(uint32_t Milliseconds)
{
    Sleep(Milliseconds);
}


void OS_Start(void)
{
    CreateThread(
        NULL,
        1024,
        &TimerExe,
        NULL,
        0,
        NULL);   

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



