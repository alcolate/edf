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
#include "CanDrv.h"
#include "Driver.h"
#include "Edf.h"

namespace Edf
{


class CCanEvent : public CDeviceEvent
{
public:
	CCanEvent(Signals Sig, CAN_HANDLE CanHandle, uint32_t BuffSize, bool Dynamic = true);

	virtual ~CCanEvent();
};


class CCan : public CDevice
{
public:
	CCan(char* Name, CAN_HANDLE Can,
		uint16_t MaxFrameLen, MACCALLBACK MacCall, uint32_t DQSize = 2);
	~CCan();

	virtual void Initial(CActive* Owner);

protected:
	virtual bool Send(uint8_t* Data, uint32_t Len);

public:

	CanConfig 	m_Config;

};



} // namespace Edf
