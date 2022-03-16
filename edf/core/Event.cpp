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

Event::Event(Signal s, bool DynAlloc) : Sig(s), RefCount(0), DynamicAlloc(DynAlloc), Freeing(false)
{

}
Event::~Event() 
{
	
}
void Event::InitRef(uint32_t Ref, bool FromISR)
{	
	if (DynamicAlloc)
	{
		uint32_t flag = OS_EnterCritical(FromISR);
		ASSERT(RefCount == 0);
		RefCount = Ref;
		OS_ExitCritical(flag, FromISR);
	}	
}
void Event::IncRef(bool FromISR)
{
	if (DynamicAlloc)
	{        
		uint32_t flag = OS_EnterCritical(FromISR);
		++ RefCount;
		OS_ExitCritical(flag, FromISR);
	}
}
void Event::DecRef(bool FromISR)
{
	bool ToFree = false;
	
	if (DynamicAlloc)
	{
		uint32_t flag = OS_EnterCritical(FromISR);
		if (RefCount)
		{
			-- RefCount;
		}
		if (RefCount == 0)
		{
			if (!Freeing)
			{
				ToFree = true;
				Freeing = true;
			}
		}
		OS_ExitCritical(flag, FromISR);
	}

	if (ToFree)
	{
		delete this;
	}
}

CEventQ::CEventQ(uint32_t ItemCount)
{
	m_ItemCount = ItemCount;
	m_Items = new CItem[ItemCount];
}
CEventQ::~CEventQ()
{
	if (m_ItemCount)
		delete[] m_Items;
}
CEventQ::CItem* CEventQ::GetFreeItem()
{
	for (uint32_t i = 0; i < m_ItemCount; i++)
	{
		if (m_Items[i].m_Evt == 0) return &m_Items[i];
	}
	LOG_INFO("DQ is full\r\n");
	return NULL;
}

bool CEventQ::Defer(Event const* const Evt)
{
	CItem* item = GetFreeItem();

	if (!item)
	{
		return false;
	}
	else
	{
		(const_cast<Event*>(Evt))->IncRef();
		item->m_Evt = Evt;
		item->m_Next = 0;

		m_Queue.Push(item);
		return true;
	}
}

const Event* CEventQ::Fetch(void)
{	
	const Event* Evt = 0;
	CItem* Item = m_Queue.Pop();

	if (Item)
	{
		Evt = Item->m_Evt;
		Item->m_Evt = 0;
		Item->m_Next = 0;
	}

	return Evt;
}

Event const InitEvt(INIT_SIG);
Event const EntryEvent (ENTRY_SIG);
Event const ExitEvent (EXIT_SIG);

}
