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
#include <stdint.h>
#include <FreeRTOS.h>
#include <event_groups.h>
#include <queue.h>

#pragma once

#define MAX_PRIORITIES 			configMAX_PRIORITIES
#define MINIMAL_STACK_SIZE 		(configMINIMAL_STACK_SIZE * 4)
#define MAX_DELAY 				portMAX_DELAY
#define TICK_RATE_MS			portTICK_RATE_MS

#define MilliSecond(t)  		((t) / TICK_RATE_MS)

#define ASSERT   				configASSERT

#define OS_LOG

#define LOG_ERROR				OS_LOG
#define LOG_DEBUG				OS_LOG
#define LOG_WARNING				OS_LOG
#define LOG_INFO				OS_LOG			

#define LOG_POS					OS_LOG("%s(%d)\r\n", __FUNCTION__, __LINE__)

typedef void *Q_HANDLE;
typedef void *T_HANDLE;
typedef void (*TaskExec)(void*);

T_HANDLE TaskCreate(const char *const pcName,
		uint16_t usStackDepth, void *const pvParameters,
		uint32_t uxPriority, Q_HANDLE *Q, uint32_t Q_Size);

Q_HANDLE QueueCreate(uint32_t uxQueueLength, uint32_t uxItemSize);
bool QueueReceive(Q_HANDLE Q, void *const P, uint32_t TimeOut);

bool QueueSend(Q_HANDLE Q, void const *const P, bool FromISR = false);

uint32_t OS_EnterCritical(bool FromISR = false);
void OS_ExitCritical(uint32_t Flag = 0, bool FromISR = false);

void OS_Start(void);
