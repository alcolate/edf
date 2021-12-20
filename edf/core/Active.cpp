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
#include "Active.h"

namespace Edf
{
CActive::CActive(char *Name, uint32_t DQSize)
{
	m_Name = Name;
	m_Queue = 0;
	m_Thread = 0;
	m_Priority = DefPrioity;
	m_StackSize = MINIMAL_STACK_SIZE;
	m_DQ = new CActive::CEventQ(DQSize);
	
}

CActive::~CActive()
{
	if (m_DQ)
		delete m_DQ;
}

void CActive::Start()
{
	Start(m_Priority, EQ_SIZE, sizeof(Event *));
}

void CActive::Start(uint32_t Priority, uint32_t QueueLen, uint32_t ItemSize)
{
	QueueLen = QueueLen;
	ItemSize = ItemSize;
	
	this->m_Thread = OS_TaskCreate(
			m_Name,
			m_StackSize,               
			this,                       
			Priority, &(this->m_Queue), EQ_SIZE);  

	ASSERT(this->m_Queue);

	ASSERT(this->m_Thread); 
}

bool CActive::Post(Event const *const e, bool FromISR)
{
	//LOG_DEBUG("%s post sig = %d, ref = %d\r\n", m_Name, e->Sig, e->RefCount);
	bool result = OS_QueueSend(Q(), e, FromISR);
	ASSERT(result);
	return result;
}

void CActive::Run(void)
{
	EventLoop();
}

void CActive::EventLoop()
{
	Event* e;
	
	this->Initial();

	for (;;) 
	{

		if (OS_QueueReceive(this->m_Queue, &e, MAX_DELAY))
		{			
			//LOG_DEBUG("%s get event sig = %d, ref = %d\r\n", m_Name, e->Sig, e->RefCount);
			this->RunState(e);

			e->DecRef();
		}
	}
}


CActive::CEventQ::CEventQ(uint32_t ItemCount)
{
	m_Head = 0;
	m_ItemCount = ItemCount;
	m_Items = new CItem[ItemCount];
}
CActive::CEventQ::~CEventQ()
{
	if (m_ItemCount)
		delete[] m_Items;
}
CActive::CEventQ::CItem* CActive::CEventQ::GetFreeItem()
{
	for (uint32_t i = 0; i < m_ItemCount; i++)
	{
		if (m_Items[i].Evt == 0) return m_Items;
	}

	return NULL;
}

void CActive::CEventQ::LinkItem(CItem* Item)
{
	CItem* p = m_Head;
	if (NULL == p)
	{
		p = Item;
	}
	else
	{
		for (; p->Next; p = p->Next);
		p->Next = Item;
	}
}

bool CActive::CEventQ::Defer(Event const* const Evt)
{
	CItem* item = GetFreeItem();

	if (!item)
	{
		return false;
	}
	else
	{
		(const_cast<Event*>(Evt))->IncRef(1);
		item->Evt = Evt;
		item->Next = 0;

		LinkItem(item);
		return true;
	}
}

const Event* CActive::CEventQ::Fetch(void)
{
	CItem* Item = 0;
	const Event* Evt = 0;

	if (m_Head)
	{
		Item = m_Head;
		m_Head = m_Head->Next;
		Evt = Item->Evt;
		Item->Evt = 0;
		Item->Next = 0;

	}

	return Evt;
}

void EdfStart(void)
{
	OS_Start();
}

} // namespace Edf
