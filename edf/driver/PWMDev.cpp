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

CPWMEvent::CPWMEvent(Signal Sig, DEV_HANDLE PWM, uint32_t Steps, bool Releasable)
	: CDeviceEvent(Sig, PWM, 0, Releasable)
{
	m_Steps = Steps;
}

CPWMEvent::~CPWMEvent()
{

}

CPWM::CPWM(const char *Name, DEV_HANDLE PWM, uint32_t Channel, bool EnTune)
		: CDevice(Name, PWM, EDeviceType::PWM, 1)
		  ,m_IrqRecvEvent(PWM_RSP_SIG, PWM, 0, false)
{
	m_Channel = Channel;

	m_Running = false;

	m_EnTune = EnTune;

	m_MacCall = nullptr;

	m_StepMacCall = nullptr;

	m_StepInts = nullptr;

	SetCallback(CPWM::MacCall, CPWM::PostIrqRecvEvent);

	CreateIrqSendCompleteEvent();

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

bool CPWM::SetStepInts(uint32_t Ints[], uint32_t Size)
{
	if (m_StepInts)
	{
		delete [] m_StepInts;
	}

	m_StepInts = new uint32_t[Size];
	m_StepIntsSize = Size;
	memset(m_StepInts, 0, sizeof(m_StepInts[0]) * Size);
	for (uint32_t i = 0; i < Size; i ++)
	{
		m_StepInts[i] = Ints[i];
	}

	m_INTIndex = 0;
	m_EnINT = true;
	return true;
}

void CPWM::SetMacCall(MACCALLBACK MacCB)
{
	m_MacCall = MacCB;
}

void CPWM::SetStepMacCall(MACCALLBACK MacCB)
{
	m_StepMacCall = MacCB;
}

bool CPWM::Start(uint32_t Steps)
{
	if (Steps == 0 || m_Running == true)
	{
		return false;
	}

	m_Running = true;

	m_Steps = Steps;

	m_StepCount = 0;

	if (m_EnTune)
	{
		m_StepShifts[0] = Steps;

		m_StepShifts[Shift_Max * 2] = 0;

		uint32_t StepShift = Steps / 8 / Shift_Max;

		for (uint32_t i = 0; i < Shift_Max; i ++)
		{
			m_StepShifts[i] = Steps - i * StepShift;
			m_StepShifts[Shift_Max * 2 - i] = i * StepShift;
		}
		m_StepShifts[Shift_Max] = m_StepShifts[Shift_Max + 1] + StepShift;

		m_ShiftIndex = 0;

		PWM_SetPeriod(m_HwHandle, m_Channel, m_PeriodShifts[m_ShiftIndex], m_Duty);
	}
	return PWM_Start(m_HwHandle, m_Channel);
}
bool CPWM::Stop()
{
	m_Running = false;
	return PWM_Stop(m_HwHandle, m_Channel);
}

bool CPWM::IsRunning()
{
	return m_Running;
}
bool CPWM::SetPeriod(uint16_t Period, uint16_t Duty)
{
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

	m_Period = Period;
	m_Duty = Duty;

	uint16_t Times = 2;
	uint16_t MaxPeriod = m_Period * Times;

	uint16_t Diff = MaxPeriod - m_Period;
	for (int i = 0; i <= Shift_Max; i ++)
	{
		m_PeriodShifts[Shift_Max * 2 - i] = m_PeriodShifts[i] = m_Period + Diff * FreqS[i];
	}

	return PWM_SetPeriod(m_HwHandle, m_Channel, m_Period, Duty);
}
uint32_t CPWM::GetRestSteps()
{
	return m_Steps;
}

uint32_t CPWM::GetRunSteps()
{
	return m_StepCount;
}

void CPWM::PostIrqRecvEvent(CDevice *Device)
{
	CPWM *me = static_cast<CPWM*> (Device);
	me->m_IrqRecvEvent.m_Steps = me->m_Steps;
	Publish(&me->m_IrqRecvEvent, true);
}

bool CPWM::MacCall(CDevice *Device, uint8_t *Data, uint32_t Len)
{
	CPWM *me = static_cast<CPWM*> (Device);

	-- me->m_Steps;
	++ me->m_StepCount;

	if (me->m_StepMacCall)
	{
		me->m_StepMacCall();
	}

	if (me->m_Steps == 0)
	{
		me->m_Running = false;
		me->m_EnINT = false;
		if (me->m_MacCall)
		{
			me->m_MacCall();
		}
		PWM_Stop(me->m_HwHandle, me->m_Channel);
		return true;
	}

	if (me->m_EnINT)
	{
		if (me->m_INTIndex < me->m_StepIntsSize && me->m_StepCount == me->m_StepInts[me->m_INTIndex])
		{
			++ me->m_INTIndex;
			if (me->m_MacCall)
			{
				me->m_MacCall();
			}
		}
	}

	if (me->m_EnTune)
	{
		if (me->m_Steps < me->m_StepShifts[me->m_ShiftIndex])
		{
			++ me->m_ShiftIndex;
			PWM_SetPeriod(me->m_HwHandle, me->m_Channel, me->m_PeriodShifts[me->m_ShiftIndex], me->m_Duty);
		}
	}

	return false;
}

} // namespace Edf


void PWM_Complete(DEV_HANDLE PWM)
{
	Edf::CDevKeeper::Instance()->Receive(PWM, nullptr, 0);
}
