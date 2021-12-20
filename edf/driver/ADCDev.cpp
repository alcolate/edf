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
#include "ADCDev.h"

namespace Edf
{

CAdcEvent::CAdcEvent(Signal Sig, DEV_HANDLE ADC, bool Releasable)
	: CDeviceEvent(Sig, ADC, 0, Releasable)
{

}

CAdcEvent::~CAdcEvent()
{

}

CAdc::CAdc(const char *Name, DEV_HANDLE Adc, uint32_t RspSig) :
		CDevice(Name, Adc, EDeviceType::ADC, 1),
		m_IrqRecvEvent((Signal)RspSig, Adc, false)
{
	m_ChannelNum = ADC_GetChannels(m_HwHandle);
	ASSERT(m_ChannelNum <= CAdcEvent::MAX_CHANNEL);

	m_Filter.Reset();

	SetCallback(CAdc::MacCall, CAdc::PostIrqRecvEvent);

	CDevKeeper::Instance()->RegDevice(this);
}
CAdc::~CAdc()
{

}

void CAdc::Initial(CActive *Owner)
{
	CDevice::Initial(Owner);
}
bool CAdc::Send(Event const* const e)
{
	return false;
}

bool CAdc::Start()
{
	m_CurChannel = 0;

//	if (ADC_GetMode(m_HwHandle) == DMA)
//	{
//		return ADC_Start_DMA(m_HwHandle, m_Channels, m_ChannelNum);
//	}
//	else
	{
		return ADC_Start(m_HwHandle);
	}
}

bool CAdc::Stop()
{
	return ADC_Stop(m_HwHandle);
}

void CAdc::ResetFilter()
{
	m_Filter.Reset();
}

void CAdc::PostIrqRecvEvent(CDevice *Device)
{
	CAdc *me = static_cast<CAdc*> (Device);
	me->m_IrqRecvEvent.m_Result[0] = me->m_Channels[0];
	me->m_IrqRecvEvent.m_ResLen = 0;
	Publish(&me->m_IrqRecvEvent, true);
}

bool CAdc::MacCall(CDevice *Device, uint8_t *Data, uint32_t Len)
{
	CAdc *me = static_cast<CAdc*> (Device);
//	if (ADC_GetMode(m_HwHandle) == INTERRUPT)
	{

		//m_Channels[m_CurChannel] = *(uint16_t*)Data;
		if (me->m_Filter.Input(*(uint16_t*)Data, me->m_Channels[0]))
		{
			me->Stop();
			return true;
		}
		else
		{
			//me->Start();
			return false;
		}
	}
//	else
//	{
//		++ m_CurChannel;
//	}

	//ADC_Stop(m_HwHandle);
	//return true;
}

} // namespace Edf

void ADC_Complete(DEV_HANDLE ADC, uint16_t Data)
{
	Edf::CDevKeeper::Instance()->Receive(ADC, (uint8_t *)&Data, 2);
}
