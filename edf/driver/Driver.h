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

#include "Edf.h"

namespace Edf
{


typedef void * DEV_HANDLE;

class CDeviceEvent : public Event
{
public:
	CDeviceEvent(Signals Sig, DEV_HANDLE Device_H, uint32_t BuffSize, bool Dynamic = true);

	CDeviceEvent(Signals Sig, DEV_HANDLE Device_H, const uint8_t* Data, uint16_t Len);
	virtual ~CDeviceEvent();

	void SetSig(Signals Sig);
	void CopyData(uint8_t* Data, uint16_t Len);

	DEV_HANDLE	m_Device;
	uint8_t* 	m_Data;
	uint32_t 	m_DataCount;
	uint32_t	m_DataSize;

};

using MACCALLBACK = bool (*)(uint8_t* Buff, uint16_t& BuffSize, uint16_t& BuffCount, uint8_t *Data, uint32_t Len);

class CDevice
{
public:
	CDevice(char *Name, DEV_HANDLE Device, uint16_t MaxFrameLen, 
				MACCALLBACK MacCall, uint32_t DQSize = 2);
	virtual ~CDevice();

	void Dispatcher(Event const* const e);

	virtual void Initial(CActive *Owner);

	void S_Idle(Event const* const e);
	void S_WaitComplete(Event const* const e);

	void PostIrqEvent(Signals Sig);

	bool DeferEvent(Event const* const e);

	void FetchDeferedEvent();

	bool operator == (DEV_HANDLE DevHandle) 
	{
		return this->m_Device == DevHandle;
	}

protected:
	virtual bool Send(uint8_t *Data, uint32_t Len) = 0;

public:

	char m_Name[10];

	DEV_HANDLE  m_Device;			// the handle. of Device hardware

	uint8_t* m_Buff4MacCall;
	uint16_t 	m_BuffSize;
	uint16_t 	m_BuffCount;
	MACCALLBACK m_MacCall;
	CDeviceEvent* m_IrqEvent;
	uint32_t	m_IrqEventIndex;

	CEventQ* m_DQ;
	CActive* m_Owner;
	USE_LINK(CDevice);

public:
	DEF_STATEMACHINE(CDevice);
};

class CDevKeeper : public CActive
{
public:

	static CDevKeeper* Instance();

	void RegDevice(CDevice* Device);

	virtual void Initial();

	void SendComplete(DEV_HANDLE DeviceH);

	void Receive(DEV_HANDLE DeviceH, uint8_t *Data, uint32_t Len);

private:
	void S_Run(Event const* const e);

	CDevice* GetDevice(DEV_HANDLE DeviceH);

	void AddDevice(CDevice* Device);

	CDevKeeper() : CActive((char*)"DevKeeper")
	{

	}

	CList<CDevice> m_Device;

public:
	DEF_STATEMACHINE(CDevKeeper);
};

} // namespace Edf

