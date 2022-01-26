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
static const uint32_t NEVER = (uint32_t)(-1);

static CList<CTimeEvent> gTimer;

CTimeEvent::CTimeEvent(Signal Sig, CActive *Act):Event(Sig)
{
	ASSERT(Act);
	this->m_Act = Act;
	this->m_Timeout = NEVER;
	this->m_Interval = NEVER;
}

/*..........................................................................*/
void CTimeEvent::Trigger(uint32_t Timeout, uint32_t Interval)
{
	OS_EnterCritical();
	gTimer.RemoveItem(this);
	this->m_Timeout = Timeout;
	this->m_Interval = Interval? Interval: NEVER;
	gTimer.AddSort(this, [this](CTimeEvent* Item)->bool { return this->m_Timeout <= Item->m_Timeout; });
	OS_ExitCritical();
}

/*..........................................................................*/
void CTimeEvent::UnTrigger()
{
	OS_EnterCritical();
	gTimer.RemoveItem(this);
	OS_ExitCritical();
}

void CTimeEvent::Touch()
{
	m_Act->Post(this, true);
}

void CTimeEvent::Tick(bool FromISR)
{
	gTimer.ForEach(gTimer.Head(), 
		[&FromISR](CTimeEvent *Timer)  -> void 
		{
			CTimeEvent* runTimer = NULL;
				
			uint32_t flag = OS_EnterCritical(FromISR);
			if (Timer->m_Timeout != NEVER)
			{
				Timer->m_Timeout --;
				if (Timer->m_Timeout == 0)
				{
					runTimer = Timer;
					Timer->m_Timeout = Timer->m_Interval;
				}
			}
			OS_ExitCritical(flag, FromISR);
			if (runTimer != NULL)
			{
				runTimer->Touch();				
			}
		}
	);
}
} // namespace Edf

void TimeEvent_Tick(bool FromISR)
{
	Edf::CTimeEvent::Tick(FromISR);
}
