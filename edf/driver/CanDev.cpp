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

	CCanEvent::CCanEvent(Signals Sig, CAN_HANDLE CanHandle, uint32_t BuffSize, bool Dynamic)
		: CDeviceEvent(Sig, CanHandle, BuffSize, Dynamic)
	{

	}

	CCanEvent::CCanEvent(Signals Sig, CAN_HANDLE CanHandle, const uint8_t* Data, uint16_t Len)
		: CDeviceEvent(Sig, CanHandle, Data, Len)
	{


	}
	CCanEvent::~CCanEvent()
	{

	}


	CCan::CCan(char* Name, CAN_HANDLE Can, uint16_t MaxFrameLen, MACCALLBACK MacCall, uint32_t DQSize) 
		: CDevice(Name, Can, MaxFrameLen, MacCall, DQSize)
	{
		CDevKeeper::Instance()->RegDevice(this);
	}
	CCan::~CCan()
	{

	}

	void CCan::Initial(CActive* Owner)
	{
		bool result = Can_Init(m_Device, &m_Config);
		ASSERT(result);

		CDevice::Initial(Owner);
	}
	bool CCan::Send(uint8_t* Data, uint32_t Len)
	{
		return Can_Send(m_Device, Data, Len);
	}

} // namespace Edf

void Can_SendComplete(CAN_HANDLE Can)
{
	CDevKeeper::Instance()->SendComplete(Can);
}
void Can_Recv(CAN_HANDLE Can, uint8_t* Data, uint32_t Len)
{
	CDevKeeper::Instance()->Receive(Can, Data, Len);
}


