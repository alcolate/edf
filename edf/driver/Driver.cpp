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
#include "Driver.h"

CDeviceEvent::CDeviceEvent(Signals Sig, DEV_HANDLE Device_H, uint32_t BuffSize, bool Dynamic) : Event(Sig, Dynamic)
{
	this->m_Device = Device_H;
	this->m_DataCount = BuffSize;
	this->m_Data = new uint8_t[BuffSize];
	ASSERT(this->m_Data);
	this->m_DataSize = BuffSize;
}

CDeviceEvent::~CDeviceEvent()
{
	delete m_Data;
}

void CDeviceEvent::SetSig(Signals Sig)
{
	this->Sig = Sig;
}


CDevice::CDevice(char *Name, DEV_HANDLE Device, uint16_t MaxFrameLen, 
				MACCALLBACK MacCall, uint32_t DQSize)
{
	if (Name)
	{
		snprintf(m_Name, sizeof(m_Name), "%s", Name);
	}
	else
	{
		m_Name[0] = 0;
	}

	m_Device = Device;

	m_IrqEvent = new  CDeviceEvent[2]{
		CDeviceEvent(HW_RSP_SIG, m_Device, MaxFrameLen, false),
		CDeviceEvent(HW_RSP_SIG, m_Device, MaxFrameLen, false)
	};
	ASSERT(m_IrqEvent);
	m_IrqEventIndex = 0;

	m_BuffSize = MaxFrameLen;
	m_BuffCount = 0;
	m_Buff4MacCall = m_IrqEvent[m_IrqEventIndex].m_Data;

	m_MacCall = MacCall;

	m_DQ = new CEventQ(DQSize);
	ASSERT(m_DQ);

	
}

CDevice::~CDevice()
{
	delete[] m_IrqEvent;
}

void CDevice::Dispatcher(Event const* const e)
{
	RunState(e);
}

void CDevice::Initial(CActive *Owner)
{
	m_Owner = Owner;
	INIT_TRANS(&CDevice::S_Idle);
}

void CDevice::S_Idle(Event const* const e)
{
	switch (e->Sig)
	{
	case ENTRY_SIG:
		FetchDeferedEvent();
		break;

	case MAC_REQ_SIG:
		Send(const_cast<uint8_t*>(EventCast(CDeviceEvent)->m_Data), EventCast(CDeviceEvent)->m_DataCount);
		TRANS(&CDevice::S_WaitComplete);
		break;

	default:
		break;
	}
}
void CDevice::S_WaitComplete(Event const* const e)
{
	switch (e->Sig)
	{
	case HW_OUT_COMPLETE_SIG:

		TRANS(&CDevice::S_Idle);
		break;
	case MAC_REQ_SIG:
		DeferEvent(e);
		break;
	default:
		break;
	}
}

// call from Device driver
void CDevice::PostIrqEvent(Signals Sig)
{
	m_IrqEvent[m_IrqEventIndex].SetSig(Sig);
	Publish(&m_IrqEvent[m_IrqEventIndex], true);
	m_IrqEventIndex ^= 0x01;
	m_BuffCount = 0;
	m_Buff4MacCall = m_IrqEvent[m_IrqEventIndex].m_Data;
}


bool CDevice::DeferEvent(Event const* const e)
{
	ASSERT(e);

	return m_DQ->Defer(e);
}

void CDevice::FetchDeferedEvent()
{
	const Event* e = m_DQ->Fetch();

	if (e)
	{
		m_Owner->Post(e);
	}
}

CDevKeeper* CDevKeeper::Instance()
{
	static CDevKeeper dk;
	return &dk;
}

void CDevKeeper::RegDevice(CDevice* Device)
{
	AddDevice(Device);
	Device->Initial(this);
}

void CDevKeeper::AddDevice(CDevice* Device)
{
	m_Device.AddHead(Device);
}

CDevice* CDevKeeper::GetDevice(DEV_HANDLE DevHandle)
{
	CDevice *p = m_Device.Head();

	for (; p && p->m_Device != DevHandle; p = m_Device.Next(p));

	ASSERT(p);
	return p;

}

void CDevKeeper::Initial()
{

	Edf::Subscribe(MAC_REQ_SIG, this);
	Edf::Subscribe(HW_OUT_COMPLETE_SIG, this);
	INIT_TRANS(&CDevKeeper::S_Run);
}

void CDevKeeper::S_Run(Event const* const e)
{
	switch (e->Sig)
	{

	case MAC_REQ_SIG:
	case HW_OUT_COMPLETE_SIG:
		GetDevice(EventCast(CDeviceEvent)->m_Device)->Dispatcher(e);
		break;

	default:
		break;
	}
}


void CDevKeeper::SendComplete(DEV_HANDLE DevHandle)
{
	CDevice* Device = GetDevice(DevHandle);

	if (Device)
	{
		LOG_DEBUG("keeper send completely: %s \r\n", Device->m_Name);

		Device->PostIrqEvent(HW_OUT_COMPLETE_SIG);
	}
}

void CDevKeeper::Receive(DEV_HANDLE DevHandle, uint8_t* Data, uint32_t Len)
{
	CDevice* Device = GetDevice(DevHandle);

	if (Device && Device->m_MacCall(Device->m_Buff4MacCall, Device->m_BuffSize, Device->m_BuffCount, Data, Len))
	{
		LOG_DEBUG("keeper get: %d \r\n", Device->m_BuffCount);

		Device->PostIrqEvent(HW_RSP_SIG);
	}
}

