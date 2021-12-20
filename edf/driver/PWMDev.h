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
#pragma once

#include <memory.h>
#include "PWMDrv.h"
#include "Device.h"

namespace Edf
{



class CPWM : public CDevice
{
public:
	using MACCALLBACK = bool (*)(void);

	CPWM(const char *Name, DEV_HANDLE PWM, uint32_t Channel, bool EnTune = false);
	~CPWM();

	bool Start(uint32_t Steps);
	bool Stop();

	bool SetPeriod(uint16_t Period, uint16_t Duty = 500);

	bool SetStepInts(uint32_t Ints[], uint32_t Size);

	uint32_t GetRestSteps();
	uint32_t GetRunSteps();
	bool IsRunning();

	void SetMacCall(MACCALLBACK MacCB);

	void SetStepMacCall(MACCALLBACK MacCB);

protected:
	virtual void Initial(CActive *Owner) override;
	static void PostIrqRecvEvent(CDevice *Device);
	static bool MacCall(CDevice *Device, uint8_t *Data, uint32_t Len);
	virtual bool Send(Event const* const e) override;

private:

	CPWMEvent m_IrqRecvEvent;

	uint32_t m_Steps;

	uint32_t m_StepCount;

	uint32_t *m_StepInts;
	uint32_t m_StepIntsSize;

	enum {Shift_Max = 10};
	uint16_t m_PeriodShifts[Shift_Max * 2 + 1];
	uint32_t m_StepShifts[Shift_Max * 2 + 1];
	uint32_t m_ShiftIndex;

	uint32_t m_INTIndex;

	uint16_t m_Period;

	uint16_t m_Duty;

	PWMConfig m_Config;

	uint32_t m_Channel;

	MACCALLBACK m_MacCall;

	MACCALLBACK m_StepMacCall;

	bool m_EnTune;
	bool m_EnINT;

	bool m_Running;

};



} // namespace Edf

