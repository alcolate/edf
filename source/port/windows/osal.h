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


#pragma once

#define MAX_PRIORITIES 			configMAX_PRIORITIES
#define MINIMAL_STACK_SIZE 		configMINIMAL_STACK_SIZE
#define MAX_DELAY 				portMAX_DELAY

#define MilliSecond(t)  		((t) / portTICK_RATE_MS)

#define ASSERT   				configASSERT

#define OS_PRINT				

typedef void *Q_HANDLE;
typedef void *T_HANDLE;
typedef void (*TaskExec)(void*);

T_HANDLE TaskCreate(TaskExec pxTaskCode, const char *const pcName,
		uint16_t usStackDepth, void *const pvParameters,
		uint32_t uxPriority, Q_HANDLE *Q, uint32_t Q_Size);

Q_HANDLE QueueCreate(uint32_t uxQueueLength, uint32_t uxItemSize);
uint32_t QueueReceive(Q_HANDLE Q, void *const pvBuffer, uint32_t TimeOut);

bool QueueSend(Q_HANDLE q, void const *const p, bool FromISR = false);

