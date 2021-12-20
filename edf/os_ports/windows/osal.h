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
#pragma once

#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#define MAX_PRIORITIES 			50
#define MINIMAL_STACK_SIZE 		1024
#define MAX_DELAY 				((long)-1)
#define TICK_RATE_MS			10
#define MilliSecond(t)  		((t) / TICK_RATE_MS)

#define ASSERT   				assert

#define OS_LOG					printf

#define LOG_ERROR				OS_LOG
#define LOG_DEBUG				OS_LOG
#define LOG_WARNING				OS_LOG
#define LOG_INFO				OS_LOG					

#define LOG_POS					OS_LOG("%s(%d)\r\n", __FUNCTION__, __LINE__)

typedef uint32_t Q_HANDLE;
typedef void *T_HANDLE;

T_HANDLE OS_TaskCreate(const char *const pcName,
		uint16_t usStackDepth, void *const pvParameters,
		uint32_t uxPriority, Q_HANDLE *Q, uint32_t Q_Size);
void OS_TastSetPriority(T_HANDLE Task, uint32_t Priority);
Q_HANDLE OS_QueueCreate( uint32_t uxQueueLength, uint32_t uxItemSize);
bool OS_QueueReceive(Q_HANDLE Q, void *const P, uint32_t TimeOut);
bool OS_QueueSend(Q_HANDLE Q, void const *const P, bool FromISR = false);

uint32_t OS_EnterCritical(bool FromISR = false);
void OS_ExitCritical(uint32_t Flag = 0, bool FromISR = false);
void OS_Sleep(uint32_t Milliseconds);
void OS_Start(void);

uint32_t OS_Tick(void);

void OS_Restart();

void OS_MemoryUsage(size_t &Free, size_t &Minimum);
