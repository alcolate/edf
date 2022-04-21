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
#include <string.h>
#include <stdio.h>
#include "PWMDev.h"

namespace Edf
{

CPWMEvent::CPWMEvent(Signals Sig, DEV_HANDLE PWM, uint32_t Steps, bool Dynamic)
	: CDeviceEvent(Sig, PWM, 0, Dynamic)
{
	m_Steps = Steps;
}

CPWMEvent::~CPWMEvent()
{

}

CPWM::CPWM(char *Name, DEV_HANDLE PWM, uint32_t Channel)
		: CDevice(Name, PWM, EDeviceType::PWM, 1)
		  ,m_IrqRecvEvent(PWM_RSP_SIG, PWM, 0, false)
{
	m_Channel = Channel;

	m_Running = false;

	CDevKeeper::Instance()->RegDevice(this);

}
CPWM::~CPWM()
{
}

void CPWM::Initial(CActive *Owner)
{
//	bool result = PWM_Init(m_Device, &m_Config);
//	ASSERT(result);

	CDevice::Initial(Owner);
}
bool CPWM::Send(Event const* const e)
{
	const CPWMEvent *evt = EventCast(CPWMEvent);

	m_Steps = evt->m_Steps;
	if (m_Steps != 0)
	{
		Start(m_Steps);
	}
	else
	{
		Stop();
		Publish(new CPWMEvent(PWM_RSP_SIG, m_HwHandle, m_Steps));
	}

	return false;
}


bool CPWM::Start(uint32_t Steps)
{
	if (Steps == 0 || m_Running == true)
	{
		return false;
	}

	m_Running = true;

	const float FreqS[Shift_Max + 1]={
			1,
			0.98201379,
			0.952574127,
			0.880797078,
			0.731058579,
			0.5,
			0.268941421,
			0.119202922,
			0.047425873,
			0.01798621,
			0
	};
	uint16_t MaxPeriod = m_Period * Shift_Max;

	m_Steps = Steps;

	uint16_t Diff = MaxPeriod - m_Period;
//{260, 255, 248, 232, 197, 143, 88, 53, 37, 30, 26, 30, 37, 53, 88, 143, 197, 232, 248, 255, 260}
	for (int i = 0; i <= Shift_Max; i ++)
	{
		m_PeriodShifts[Shift_Max * 2 - i] = m_PeriodShifts[i] = m_Period + Diff * FreqS[i];
	}

	m_StepShifts[0] = Steps;

	m_StepShifts[Shift_Max * 2] = 0;

	uint32_t StepShift = Steps / 3 / Shift_Max;

	for (uint32_t i = 0; i < Shift_Max; i ++)
	{
		m_StepShifts[i] = Steps - i * StepShift;
		m_StepShifts[Shift_Max * 2 - i] = i * StepShift;
	}
// {6400, 6187, 5974, 5761, 5548, 5335, 5122, 4909, 4696, 4483, 2130, 1917, 1704, 1491, 1278, 1065, 852, 639, 426, 213, 0}
	m_StepShifts[Shift_Max] = m_StepShifts[Shift_Max + 1] + StepShift;

	m_ShiftIndex = 0;

	PWM_SetPeriod(m_HwHandle, m_Channel, m_PeriodShifts[m_ShiftIndex], m_Duty);

	return PWM_Start(m_HwHandle, m_Channel);
}
bool CPWM::Stop()
{
	m_Running = false;
	return PWM_Stop(m_HwHandle, m_Channel);
}

bool CPWM::SetPeriod(uint16_t Period, uint16_t Duty)
{
	m_Period = Period;
	m_Duty = Duty;
	return PWM_SetPeriod(m_HwHandle, m_Channel, Period, Duty);
}
uint32_t CPWM::GetSteps()
{
	return m_Steps;
}
void CPWM::PostIrqRecvEvent()
{
	m_IrqRecvEvent.m_Steps = m_Steps;
	Publish(&m_IrqRecvEvent, true);
}

bool CPWM::MacCall(uint8_t *Data, uint32_t Len)
{
	-- m_Steps;

	if (m_Steps == 0)
	{
		m_Running = false;
		PWM_Stop(m_HwHandle, m_Channel);
		return true;
	}

	if (m_Steps < m_StepShifts[m_ShiftIndex])
	{
//		++ m_ShiftIndex;
//		PWM_SetPeriod(m_HwHandle, m_Channel, m_PeriodShifts[m_ShiftIndex], m_Duty);
	}

	return false;
}

} // namespace Edf


void PWM_Complete(DEV_HANDLE PWM)
{
	CDevKeeper::Instance()->Receive(PWM, NULL, 0);
}
