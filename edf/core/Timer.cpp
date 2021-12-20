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

static CTimeEvent *gTimer = NULL;
static uint32_t gTimerNum = 0; 

CTimeEvent::CTimeEvent(Signal Sig, CActive *Act):Event(Sig)
{
	ASSERT(Act);
	this->m_Act = Act;
	this->m_Timeout = 0U;
	this->m_Interval = 0U;

	m_Next = gTimer;
	gTimer = this;
	++gTimerNum;
}

/*..........................................................................*/
void CTimeEvent::Trigger(uint32_t Timeout, uint32_t Interval)
{
	OS_EnterCritical();
	this->m_Timeout = Timeout;
	this->m_Interval = Interval;
	OS_ExitCritical();
}

/*..........................................................................*/
void CTimeEvent::UnTrigger()
{
	OS_EnterCritical();
	this->m_Timeout = 0U;
	OS_ExitCritical();
}

void CTimeEvent::Touch()
{
	m_Act->Post(this, true);
}

void CTimeEvent::Tick()
{
	CTimeEvent* timer, * p;
	for (p = gTimer; p; p = p->m_Next)
	{
		timer = NULL;
		uint32_t flag = OS_EnterCritical(true);
		if (p->m_Timeout > 0U)
		{
			if (--p->m_Timeout == 0U)
			{
				timer = p;
				p->m_Timeout = p->m_Interval;
			}
		}
		OS_ExitCritical(flag, true);
		if (timer != NULL)
		{
			timer->Touch();
		}
	}
}
} // namespace Edf

void TimeEvent_tickFromISR()
{
	Edf::CTimeEvent::Tick();
}
