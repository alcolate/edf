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
uint32_t CTimeEvent::m_Tick = 0;

static CList<CTimeEvent> & GTimer()
{
	static CList<CTimeEvent> gTimer;

	return gTimer;
}

CTimeEvent::CTimeEvent(Signal Sig, CActive *Act):Event(Sig, false)
{
	ASSERT(Act);
	this->m_Act = Act;
	this->m_StartPoint = NEVER;
	this->m_Period = NEVER;
	this->m_Paused = true;
}
CTimeEvent::~CTimeEvent()
{
	OS_EnterCritical();
	GTimer().RemoveItem(this);
	OS_ExitCritical();
}
void CTimeEvent::Start(uint32_t StartPoint, uint32_t Period)
{
	OS_EnterCritical();
	GTimer().RemoveItem(this);
	this->m_StartPoint = StartPoint;
	this->m_Period = Period? Period: NEVER;
	this->m_Paused = false;
	GTimer().AddSort(this, [this](CTimeEvent* Item)->bool { return this->m_StartPoint <= Item->m_StartPoint; });
	OS_ExitCritical();
}

void CTimeEvent::Stop()
{
	OS_EnterCritical();
	GTimer().RemoveItem(this);
	OS_ExitCritical();
}

uint32_t CTimeEvent::Pause()
{
	OS_EnterCritical();
	this->m_Paused = true;
	uint32_t Tick = CTimeEvent::m_Tick;
	OS_ExitCritical();
	return Tick;
}

void CTimeEvent::Resume()
{
	OS_EnterCritical();
	this->m_Paused = false;
	OS_ExitCritical();
}

void CTimeEvent::Resume(uint32_t HisPoint)
{
	OS_EnterCritical();
	uint32_t Pass = (CTimeEvent::m_Tick >= HisPoint)? (CTimeEvent::m_Tick - HisPoint)
						: (NEVER - (HisPoint - CTimeEvent::m_Tick));
	this->m_StartPoint = (Pass >= this->m_StartPoint)? 0: (this->m_StartPoint - Pass);
	this->m_Paused = false;
	OS_ExitCritical();
}

void CTimeEvent::Touch(bool FromISR)
{
	m_Act->Post(this, FromISR);
}

uint32_t CTimeEvent::GetTimeRemain()
{
	OS_EnterCritical();
	uint32_t Tick = (this->m_StartPoint == NEVER? 0: this->m_StartPoint);
	OS_ExitCritical();
	return Tick;
}

uint32_t CTimeEvent::GetTick()
{
	OS_EnterCritical();
	uint32_t Tick = CTimeEvent::m_Tick;
	OS_ExitCritical();
	return Tick;
}

void CTimeEvent::Tick(bool FromISR)
{
	++ CTimeEvent::m_Tick;

	GTimer().ForEach([&FromISR](CTimeEvent *Timer)  -> void {
			CTimeEvent* runTimer = nullptr;

			uint32_t flag = OS_EnterCritical(FromISR);
			if (!Timer->m_Paused && Timer->m_StartPoint != NEVER)
			{				
				if (Timer->m_StartPoint == 0)
				{
					runTimer = Timer;
					Timer->m_StartPoint = Timer->m_Period;
				}
				else
				{
					-- Timer->m_StartPoint;
				}
			}
			OS_ExitCritical(flag, FromISR);
			if (runTimer != nullptr)
			{
				runTimer->Touch(FromISR);
			}
		});
}
} // namespace Edf

void TimeEvent_Tick(bool FromISR)
{
	Edf::CTimeEvent::Tick(FromISR);
}
