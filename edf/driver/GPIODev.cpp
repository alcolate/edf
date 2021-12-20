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

DEV_HANDLE  GPIOGROUP = (DEV_HANDLE)(0xFFFF0000);

namespace Edf
{

CGPIOEvent::CGPIOEvent(Signal Sig, DEV_HANDLE GPIO, MODE Mode, bool Releasable)
	: CDeviceEvent(Sig, GPIO, 0, Releasable)
{
	m_Mode = Mode;
}

CGPIOEvent::~CGPIOEvent()
{

}

CGPIO::CGPIO(const char *Name, DEV_HANDLE GPIO, uint32_t IrqSig)
{
	if (IrqSig)
	{
		m_IrqRecvEvent = new CGPIOEvent(IrqSig, GPIO, CGPIOEvent::HIGH, false);
	}
	else
	{
		m_IrqRecvEvent = nullptr;
	}

	if (Name)
	{
		snprintf(m_Name, sizeof(m_Name), "%s", Name);
	}
	else
	{
		m_Name[0] = 0;
	}

	m_HwHandle = GPIO;

	m_MacCall = nullptr;

	m_TriggerEnabled = false;

	CGPIOGroup::Instance().Add(this);
}
CGPIO::~CGPIO()
{
	if (m_IrqRecvEvent)
	{
		delete m_IrqRecvEvent;
	}
}

void CGPIO::High()
{
	GPIO_Set(m_HwHandle, GPIO_HIGH);
}
void CGPIO::Low()
{
	GPIO_Set(m_HwHandle, GPIO_LOW);
}
void CGPIO::Toggle()
{
	GPIO_Toggle(m_HwHandle);
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

void CGPIO::SetMacCall(MACCALLBACK MacCB)
{
	m_MacCall = MacCB;
}

void CGPIO::PostIrqRecvEvent()
{
	Publish(m_IrqRecvEvent, true);
}

bool CGPIO::MacCall()
{
	if (!m_TriggerEnabled)
	{
		return true;
	}

	if (m_MacCall)
	{
		m_MacCall();
	}
	PostIrqRecvEvent();
	return true;
}

void CGPIO::SetTrigger(bool Enable)
{
	m_TriggerEnabled = Enable;
}

CGPIOGroup::CGPIOGroup(const char *Name, DEV_HANDLE GPIO) :
		CDevice(Name, GPIO, EDeviceType::GPIO, 0)
{
	SetCallback(CGPIOGroup::MacCall, CGPIOGroup::PostIrqRecvEvent);
	CDevKeeper::Instance()->RegDevice(this);
}
CGPIOGroup::~CGPIOGroup()
{
}

CGPIOGroup & CGPIOGroup::Instance()
{
	static CGPIOGroup ins((char*)"GPIOG", GPIOGROUP);
	return ins;
}
void CGPIOGroup::Add(CGPIO *Gpio)
{
	ASSERT(!m_GpioList.IsExist([&Gpio](CGPIO *Item)->bool {
		return Item->m_HwHandle == Gpio->m_HwHandle;
	}));
	m_GpioList.AddTail(Gpio);
}
CGPIO * CGPIOGroup::Add(const char *Name, DEV_HANDLE GPIO, uint32_t IrqSig)
{
	return new CGPIO(Name, GPIO, IrqSig);
}
CGPIO * CGPIOGroup::Get(DEV_HANDLE DevHandle)
{
	return m_GpioList.FindItem([&DevHandle](CGPIO *Dev)->bool {
			return Dev->m_HwHandle == DevHandle;
		});
}

CGPIO * CGPIOGroup::GetDevice(const char *Name)
{
	return m_GpioList.FindItem([&Name](CGPIO *Dev)->bool {
			return (strcmp(Name, Dev->m_Name) == 0);
		});
}

void CGPIOGroup::Initial(CActive *Owner)
{
	CDevice::Initial(Owner);
}
bool CGPIOGroup::Send(Event const* const e)
{
	return false;
}
void CGPIOGroup::PostIrqRecvEvent(CDevice *Device)
{

}

bool CGPIOGroup::MacCall(CDevice *Device, uint8_t *Data, uint32_t Len)
{
	CGPIOGroup *me = static_cast<CGPIOGroup*> (Device);
	DEV_HANDLE DevHandle = *(DEV_HANDLE*)(Data);
	CGPIO *Gpio = me->Get(DevHandle);

	if (Gpio)
	{
		return Gpio->MacCall();
	}
	return false;
}

} // namespace Edf


void GPIO_Complete(DEV_HANDLE GPIO)
{
	Edf::CDevKeeper::Instance()->Receive(GPIOGROUP, (uint8_t *)&GPIO, sizeof(GPIO));
}
