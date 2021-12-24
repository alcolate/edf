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
#include <stdint.h>
#include "osal.h"
#include "Event.h"
#include "State.h"


#pragma once

namespace Edf
{
constexpr uint32_t DefPrioity = (MAX_PRIORITIES - 5);

class CActive
{
public:
	CActive(char* Name, uint32_t DQSize = 0);
	virtual ~CActive();

	void Start();

	void Start(uint32_t Priority, uint32_t QueueLen, uint32_t ItemSize);

	bool Post(Event const *const e, bool FromISR = false);

	void Run(void);

	virtual void Initial() = 0;

	void SetPriority(uint32_t Priority)
	{
		m_Priority = Priority;
	}

	void SetStackSize(uint32_t StackSize)
	{
		m_StackSize = StackSize;
	}

	bool DeferEvent(Event const* const e);

	void FetchDeferedEvent();

	void ClearDeferedEvent();

protected:
	void EventLoop(void);

	virtual void RunState(Event const* const e) = 0;

public:
	Q_HANDLE Q() const
	{
		ASSERT(m_Queue);
		return m_Queue;
	}
private:
	class CEventQ
	{
	public:
		CEventQ(uint32_t ItemCount = DEF_ITEMS);
		~CEventQ();
	public:

		class CItem
		{
		public:
			CItem() : Evt(0), Next(0) {}
			Event const* Evt;
			CItem* Next;
		};

		CItem* GetFreeItem();

		void LinkItem(CItem* Item);

		bool Defer(Event const* const Evt);

		const Event* Fetch(void);

	public:
		CItem* m_Head;
		enum { DEF_ITEMS = 10 };
		uint32_t m_ItemCount;
		CItem *m_Items;

	};
public:
	char* m_Name;
	CEventQ *m_DQ;
private:
	T_HANDLE m_Thread;   
    Q_HANDLE m_Queue;    
    uint32_t m_Priority;
	uint32_t m_StackSize;

	enum {EQ_SIZE = 100};

};

void EdfStart(void);
    
} // namespace Edf

#include "Timer.h"
#include "Publish.h"
