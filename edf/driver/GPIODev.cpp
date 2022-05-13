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
#include "GPIODev.h"

namespace Edf
{

CGPIOEvent::CGPIOEvent(Signals Sig, DEV_HANDLE GPIO, MODE Mode, bool Releasable)
	: CDeviceEvent(Sig, GPIO, 0, Releasable)
{
	m_Mode = Mode;
}

CGPIOEvent::~CGPIOEvent()
{

}

CGPIO::CGPIO(char *Name, DEV_HANDLE GPIO, uint32_t DQSize) :
		CDevice(Name, GPIO, EDeviceType::GPIO, DQSize)
		,m_IrqRecvEvent(GPIO_RSP_SIG, GPIO, CGPIOEvent::HIGH, false)
{
	CDevKeeper::Instance()->RegDevice(this);
}
CGPIO::~CGPIO()
{
}

void CGPIO::Initial(CActive *Owner)
{
	CDevice::Initial(Owner);
}
bool CGPIO::Send(Event const* const e)
{
	CGPIO::Set(EventCast(CGPIOEvent)->m_Mode);

	return false;
}

void CGPIO::Set(CGPIOEvent::MODE Mode)
{
	if (Mode == CGPIOEvent::HIGH)
	{
		GPIO_Set(m_HwHandle, GPIO_HIGH);
	}
	else if (Mode == CGPIOEvent::LOW)
	{
		GPIO_Set(m_HwHandle, GPIO_LOW);
	}
	else if (Mode == CGPIOEvent::TOGGLE)
	{
		GPIO_Toggle(m_HwHandle);
	}
}

CGPIOEvent::MODE CGPIO::Get()
{
	if (GPIO_HIGH == GPIO_Get(m_HwHandle))
	{
		return CGPIOEvent::HIGH;
	}
	else
	{
		return CGPIOEvent::LOW;
	}
}
void CGPIO::SetInputMode()
{
	GPIO_SetInputMode(m_HwHandle);
}
void CGPIO::SetOutputMode()
{
	GPIO_SetOutputMode(m_HwHandle);
}

void CGPIO::PostIrqRecvEvent()
{
	Publish(&m_IrqRecvEvent, true);
}

bool CGPIO::MacCall(uint8_t *Data, uint32_t Len)
{
	return true;
}

} // namespace Edf


void GPIO_Complete(DEV_HANDLE GPIO)
{
	Edf::CDevKeeper::Instance()->Receive(GPIO, NULL, 0);
}
