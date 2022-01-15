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
static uint32_t ticks = 0;

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
	gTimer.RemoveItem([](CTimeEvent* This, CTimeEvent* That)->bool { return That == This; }, this);
	this->m_Timeout = Timeout;
	this->m_Interval = Interval? Interval: NEVER;
	gTimer.AddSort([](CTimeEvent* This, CTimeEvent* That)->bool { return That->m_Timeout <= This->m_Timeout; }, this);
	OS_ExitCritical();
}

/*..........................................................................*/
void CTimeEvent::UnTrigger()
{
	OS_EnterCritical();
	gTimer.RemoveItem([](CTimeEvent* This, CTimeEvent* That)->bool { return That == This; }, this);
	this->m_Timeout = NEVER;
	this->m_Interval = NEVER;
	gTimer.AddTail(this);
	OS_ExitCritical();
}

void CTimeEvent::Touch()
{
	m_Act->Post(this, true);
}

void CTimeEvent::Tick(bool FromISR)
{
	CTimeEvent* timer, * p;

	if (++ticks == gTimer.Head()->m_Timeout)
	{
		for (p = gTimer.Head(); p; p = p->m_Next)
		{
			timer = NULL;
			uint32_t flag = OS_EnterCritical(FromISR);
			if (p->m_Timeout != NEVER)
			{
				p->m_Timeout -= ticks;
				if (p->m_Timeout == 0)
				{
					timer = p;
					p->m_Timeout = p->m_Interval;
				}
			}
			OS_ExitCritical(flag, FromISR);
			if (timer != NULL)
			{
				timer->Touch();
				gTimer.AddSort([](CTimeEvent* This, CTimeEvent* That)->bool { return That->m_Timeout <= This->m_Timeout; }, timer);
			}
		}
		ticks = 0;
	}

}
} // namespace Edf

void TimeEvent_Tick(bool FromISR)
{
	Edf::CTimeEvent::Tick(FromISR);
}
