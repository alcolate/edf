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

#if (MAX_PRIORITIES < 5)
	#error "The maximum priority must be greater than 5"
#endif
constexpr uint32_t DEF_PRIOITY = (MAX_PRIORITIES - 5);

class CActive
{
public:
	CActive(char* Name, uint32_t DQSize = 0);
	virtual ~CActive();

	void Start();

	void Start(uint32_t Priority, uint32_t StackSize, uint32_t EQSize);

	bool Post(Event const *const e, bool FromISR = false);

	void Run(void);

	virtual void Initial() = 0;

	bool DeferEvent(Event const* const e);

	void FetchDeferedEvent();

	void ClearDeferedEvent();

protected:
	virtual void RunState(Event const* const e) = 0;

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

	enum {EQ_SIZE = 20};

};

void EdfStart(void);
    
} // namespace Edf

#include "Timer.h"
#include "Publish.h"
