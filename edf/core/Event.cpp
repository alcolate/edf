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

Event::Event(Signal s, bool Releasable) : Sig(s), RefCount(0), Releasable(Releasable), Freeing(false)
{

}
Event::~Event() 
{
	
}
void Event::InitRef(uint32_t Ref, bool FromISR)
{	
	if (Releasable)
	{
		uint32_t flag = OS_EnterCritical(FromISR);
		ASSERT(RefCount == 0);
		RefCount = Ref;
		OS_ExitCritical(flag, FromISR);
	}	
}
void Event::IncRef(bool FromISR)
{
	if (Releasable)
	{        
		uint32_t flag = OS_EnterCritical(FromISR);
		++ RefCount;
		OS_ExitCritical(flag, FromISR);
	}
}
void Event::DecRef(bool FromISR)
{
	bool ToFree = false;
	
	if (Releasable)
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

CEventQ::CEventQ(uint32_t MaxItems)
{
	m_MaxItems = MaxItems;
}
CEventQ::~CEventQ()
{

}

bool CEventQ::Defer(Event const* const Evt)
{
	if (m_Queue.Count() == m_MaxItems)
	{
		return false;
	}
	else
	{
		(const_cast<Event*>(Evt))->IncRef();

		m_Queue.Push(const_cast<Event*>(Evt));
		return true;
	}
}

const Event* CEventQ::Fetch(void)
{
	return m_Queue.Pop();
}

void CEventQ::Recycle(Event const* const Evt)
{
	const_cast<Event *>(Evt)->DecRef();
}

Event const InitEvt(INIT_SIG);
Event const EntryEvent (ENTRY_SIG);
Event const ExitEvent (EXIT_SIG);

}
