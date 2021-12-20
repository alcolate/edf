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
#include "GPIODrv.h"
#include "Device.h"

namespace Edf
{

class CGPIOGroup;

class CGPIO
{
public:
	using MACCALLBACK = bool (*)(void);

	CGPIO(const char *Name, DEV_HANDLE GPIO, uint32_t IrqSig = 0);
	~CGPIO();

	void High();
	void Low();
	void Toggle();
	CGPIOEvent::MODE Get();
	void SetInputMode();
	void SetOutputMode();
	void SetMacCall(MACCALLBACK MacCB);

	void SetTrigger(bool Enable);

	DEV_HANDLE  GetHwHandle() {return m_HwHandle;}

protected:
	void PostIrqRecvEvent();
	bool MacCall();
	void Set(CGPIOEvent::MODE Mode);

	DEV_HANDLE  m_HwHandle;

private:

	MACCALLBACK m_MacCall;

	bool m_TriggerEnabled;

	char m_Name[8];

	CGPIOEvent *m_IrqRecvEvent;

	friend CGPIOGroup;

};

class CGPIOGroup : public CDevice
{
public:

	static CGPIOGroup & Instance();
	void Add(CGPIO *Gpio);
	CGPIO * Add(const char *Name, DEV_HANDLE GPIO, uint32_t IrqSig = 0);
	CGPIO * Get(DEV_HANDLE DevHandle);
	CGPIO * GetDevice(const char *Name);

protected:
	virtual void Initial(CActive *Owner) override;
	static void PostIrqRecvEvent(CDevice *Device);
	static bool MacCall(CDevice *Device, uint8_t *Data, uint32_t Len);
	virtual bool Send(Event const* const e) override;

private:

	CGPIOGroup(const char *Name, DEV_HANDLE GPIO);
	~CGPIOGroup();

	CList<CGPIO> m_GpioList;
};


} // namespace Edf

