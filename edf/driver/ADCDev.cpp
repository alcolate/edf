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

CAdcEvent::CAdcEvent(Signals Sig, DEV_HANDLE ADC, bool Dynamic)
	: CDeviceEvent(Sig, ADC, 0, Dynamic)
{

}

CAdcEvent::~CAdcEvent()
{

}

CAdc::CAdc(char *Name, DEV_HANDLE Adc) :
		CDevice(Name, Adc, EDeviceType::ADC, 1)
{

	m_IrqRecvEvent = new  CAdcEvent(ADC_RSP_SIG, m_HwHandle, false);
	ASSERT(m_IrqRecvEvent);

	m_ChannelNum = ADC_GetChannels(m_HwHandle);
	ASSERT(m_ChannelNum <= CAdcEvent::MAX_CHANNEL)
	m_Channels = new uint16_t[m_ChannelNum + 1];
	ASSERT(m_Channels);

	CDevKeeper::Instance()->RegDevice(this);
}
CAdc::~CAdc()
{
	delete [] m_IrqRecvEvent;
	delete [] m_Channels;
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
	return ADC_Start(m_HwHandle);
}

bool CAdc::Stop()
{
	return ADC_Stop(m_HwHandle);
}

void CAdc::PostIrqRecvEvent()
{
	CAdcEvent *ae = static_cast<CAdcEvent*>(m_IrqRecvEvent);
	memcpy(ae->m_Result, m_Channels, m_ChannelNum * sizeof(m_Channels[0]));
	ae->m_ResLen = m_ChannelNum;
	Publish(m_IrqRecvEvent, true);
}

bool CAdc::MacCall(uint8_t *Data, uint32_t Len)
{
	m_Channels[m_CurChannel] = *(uint16_t*)Data;

	++ m_CurChannel;

	if (m_CurChannel == m_ChannelNum)
	{
		ADC_Stop(m_HwHandle);
		return true;
	}
	else
	{
		return false;
	}
}

} // namespace Edf

void ADC_Complete(DEV_HANDLE ADC, uint16_t Data)
{
	CDevKeeper::Instance()->Receive(ADC, (uint8_t *)&Data, 2);
}
