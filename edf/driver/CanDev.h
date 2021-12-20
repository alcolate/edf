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
#include "Device.h"

namespace Edf
{


class CCan : public CDevice
{
public:
	using MACCALLBACK = bool (*)(uint8_t* Buff, const uint32_t& BuffSize, uint32_t& BuffCount, uint8_t *Data, uint32_t Len);

	CCan(const char * Name, DEV_HANDLE Can, uint32_t RspSig, uint16_t MaxFrameLen, uint32_t DQSize = 2);
	~CCan();

	void SetMacCall(MACCALLBACK MacCB);

	void Send(const uint8_t *Data, uint32_t Len);

protected:
	virtual void Initial(CActive* Owner);
	static void PostIrqRecvEvent(CDevice *Device);
	static bool MacCall(CDevice *Device, uint8_t *Data, uint32_t Len);

	virtual bool Send(Event const* const e) override;

private:
	uint8_t* m_Buff4MacCall;
	uint32_t 	m_BuffSize;
	uint32_t 	m_BuffCount;
	MACCALLBACK m_MacCall;

	CDeviceEvent* m_IrqRecvEvent;
	uint32_t	m_IrqRecvEventIndex;
	CanConfig 	m_Config;

};



} // namespace Edf
