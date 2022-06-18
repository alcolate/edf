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
#include "osal.h"
#include "queue.h"
#include "Edf.h"

extern void SystemReset(void);

static void ThreadExe(void *p)
{
    (static_cast<Edf::CActive*>(p))->Run();
}

T_HANDLE OS_TaskCreate(    const char * const pcName,
                        uint16_t usStackDepth,
                        void * const pvParameters,
                        uint32_t uxPriority, 
                        Q_HANDLE *Q, 
                        uint32_t Q_Size)
{
    T_HANDLE CreatedTask = 0;
    
    *Q = OS_QueueCreate(Q_Size, sizeof(void *));

    configASSERT(*Q);

    xTaskCreate(ThreadExe, pcName, usStackDepth, pvParameters, uxPriority, &CreatedTask);

    return CreatedTask;
}

void OS_TastSetPriority(T_HANDLE Task, uint32_t Priority)
{
	vTaskPrioritySet(Task, Priority);
}

Q_HANDLE OS_QueueCreate( uint32_t uxQueueLength, uint32_t uxItemSize)
{
    return xQueueCreate(uxQueueLength, uxItemSize);
}

bool OS_QueueReceive(Q_HANDLE Q, void * const pvBuffer, uint32_t TimeOut)
{
    return xQueueReceive(Q, pvBuffer, TimeOut) == pdTRUE;
}

bool OS_QueueSend(Q_HANDLE q, void const * const p, bool FromISR)
{
	BaseType_t status;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (FromISR)
	{
		status = xQueueSendFromISR(q, (void *)&p, &xHigherPriorityTaskWoken);
	}
	else
	{
		status = xQueueSend(q, (void *)&p, (TickType_t)0);
	}

    return (status == pdTRUE);
}


uint32_t OS_EnterCritical(bool FromISR)
{
	uint32_t Flag = 0;

	if (FromISR)
		Flag = taskENTER_CRITICAL_FROM_ISR();
	else
		taskENTER_CRITICAL();

	return Flag;
}
void OS_ExitCritical(uint32_t Flag, bool FromISR)
{
	if (FromISR)
		taskEXIT_CRITICAL_FROM_ISR(Flag);
	else
		taskEXIT_CRITICAL();
}

void OS_Start(void)
{
	vTaskStartScheduler();
}

void OS_Restart()
{
	SystemReset();
}

uint32_t OS_Tick(void)
{
	return xTaskGetTickCount();
}

void OS_MemoryUsage(size_t &Free, size_t &Minimum)
{
	Free = xPortGetFreeHeapSize();
	Minimum = xPortGetMinimumEverFreeHeapSize();
}
/*..........................................................................*/

extern void TimeEvent_Tick(bool FromISR);
extern "C"
{

void vApplicationTickHook(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    TimeEvent_Tick(true);
    
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
}
