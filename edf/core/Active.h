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
#include "osal.h"
#include "Link.h"
#include "Event.h"
#include "State.h"

namespace Edf
{

#if (MAX_PRIORITIES < 30)
	#error "The maximum priority must be greater than 30"
#endif

class CActive
{
public:
	static constexpr uint32_t DEF_PRIOITY = (MAX_PRIORITIES - 30);
	static constexpr uint32_t DEF_EQ_SIZE = 20;
	static constexpr uint32_t DEF_STACK_SIZE = MINIMAL_STACK_SIZE;
public:
	CActive(char* Name, uint32_t DQSize = 0);
	virtual ~CActive();

	virtual void Start();

	void SetPriority(uint32_t Priority);

	bool Post(Event const *const e, bool FromISR = false);

	void Run(void);

	virtual void Initial() = 0;

	bool DeferEvent(Event const* const e);

	Event const * FetchDeferedEvent();

	void RecycleEvent(Event const* const e);

	void ClearDeferedEvent();

protected:
	virtual void RunState(Event const* const e) = 0;

	void Start(uint32_t Priority, uint32_t StackSize, uint32_t EQSize);
public:
	Q_HANDLE Q() const
	{
		ASSERT(m_Queue);
		return m_Queue;
	}
private:

public:
	char m_Name[10];
	
private:
	CEventQ* m_DQ;
	T_HANDLE m_Thread;   
    Q_HANDLE m_Queue;    
    uint32_t m_Priority;
	uint32_t m_StackSize;
};

void EdfStart(uint32_t SigNum);
    
} // namespace Edf

#include "Timer.h"
#include "Publish.h"
