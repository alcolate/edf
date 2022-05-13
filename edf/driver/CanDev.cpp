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
#include "CanDev.h"


namespace Edf
{



CCan::CCan(char* Name, DEV_HANDLE Can, uint16_t MaxFrameLen, MACCALLBACK MacCall, uint32_t DQSize) 
	: CDevice(Name, Can, EDeviceType::CAN, DQSize)
{
	m_IrqRecvEvent = new  CDeviceEvent[2]{
		CDeviceEvent(CAN_RSP_SIG, m_HwHandle, MaxFrameLen, false),
		CDeviceEvent(CAN_RSP_SIG, m_HwHandle, MaxFrameLen, false)
	};
	ASSERT(m_IrqRecvEvent);
	m_IrqRecvEventIndex = 0;

	m_BuffSize = MaxFrameLen;
	m_BuffCount = 0;
	m_Buff4MacCall = m_IrqRecvEvent[m_IrqRecvEventIndex].m_Data;

	m_MacCall = MacCall;

	CDevKeeper::Instance()->RegDevice(this);
}
CCan::~CCan()
{
	delete [] m_IrqRecvEvent;
}

void CCan::Initial(CActive* Owner)
{
	bool result = Can_Init(m_HwHandle, &m_Config);
	ASSERT(result);

	CDevice::Initial(Owner);
}
bool CCan::Send(Event const* const e)
{
	return Can_Send(m_HwHandle, const_cast<uint8_t *>(EventCast(CDeviceEvent)->m_Data), EventCast(CDeviceEvent)->m_DataLen);
}

void CCan::PostIrqRecvEvent()
{
	Publish(&m_IrqRecvEvent[m_IrqRecvEventIndex], true);
	m_IrqRecvEventIndex ^= 0x01;
	m_BuffCount = 0;
	m_Buff4MacCall = m_IrqRecvEvent[m_IrqRecvEventIndex].m_Data;
}

bool CCan::MacCall(uint8_t *Data, uint32_t Len)
{
	return m_MacCall && m_MacCall(m_Buff4MacCall, m_BuffSize, m_BuffCount, Data, Len);
}
} // namespace Edf

void Can_SendComplete(DEV_HANDLE Can)
{
	Edf::CDevKeeper::Instance()->SendComplete(Can);
}
void Can_Recv(DEV_HANDLE Can, uint8_t* Data, uint32_t Len)
{
	Edf::CDevKeeper::Instance()->Receive(Can, Data, Len);
}


