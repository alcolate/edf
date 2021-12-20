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
#include "Device.h"

namespace Edf
{

CDeviceEvent::CDeviceEvent(Signal Sig, DEV_HANDLE HwHandle, uint32_t BuffSize, bool Releasable) : Event(Sig, Releasable)
{
	this->m_HwHandle = HwHandle;
	this->m_Size = BuffSize;
	this->m_DataLen = 0;
	this->m_Data = nullptr;
	if (BuffSize)
	{
		this->m_Data = new uint8_t[BuffSize];
		ASSERT(this->m_Data);
	}
}

CDeviceEvent::CDeviceEvent(Signal Sig, DEV_HANDLE HwHandle, uint8_t *Data, uint32_t DataLen, bool Releasable) : Event(Sig, Releasable)
{
	this->m_HwHandle = HwHandle;
	this->m_Size = DataLen;
	this->m_DataLen = 0;
	this->m_Data = nullptr;

	if (DataLen)
	{
		this->m_Data = new uint8_t[DataLen];
		ASSERT(this->m_Data);
		memcpy(this->m_Data, Data, DataLen);
	}
}

CDeviceEvent::~CDeviceEvent()
{
	if (m_Data)
	{
		delete [] m_Data;
	}
}

bool CDeviceEvent::Copy(const uint8_t *Data, uint32_t Len)
{
	if (Len > m_Size || Data == nullptr)
	{
		return false;
	}

	m_DataLen = Len;
	memcpy(m_Data, Data, Len);

	return true;
}

CDevice::CDevice(const char *Name, DEV_HANDLE HwHandle, EDeviceType Type, uint32_t DQSize)
{
	if (Name)
	{
		snprintf(m_Name, sizeof(m_Name), "%s", Name);
	}
	else
	{
		m_Name[0] = 0;
	}

	m_HwHandle = HwHandle;

	m_Type = Type;

	if (DQSize)
	{
		m_DQ = new CEventQ(DQSize);
	}
	else
	{
		m_DQ = nullptr;
	}

	m_IrqSendCompleteEvent = nullptr;

}

CDevice::~CDevice()
{

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

bool CDevice::Reset()
{
	return true;
}
void CDevice::S_Idle(Event const* const e)
{
	switch (e->Sig)
	{
	case MAC_REQ_SIG:
	{
		bool Ret = DeferEvent(e);
		ASSERT(Ret);
		TRANS(&CDevice::S_Sending);
	}
		break;

	// synchronously send
	case HW_OUT_COMPLETE_SIG:
		SendSuccess(m_SendingEvent);
		break;

	default:
		break;
	}
}
void CDevice::S_Sending(Event const* const e)
{
	switch (e->Sig)
	{
	case ENTRY_SIG:
		m_SendingEvent = FetchDeferedEvent();
		if (!Send(m_SendingEvent))
		{
			RecycleEvent(m_SendingEvent);
			TRANS(&CDevice::S_Idle);
		}
		break;
	case HW_OUT_COMPLETE_SIG:
		SendSuccess(m_SendingEvent);
		RecycleEvent(m_SendingEvent);
		m_SendingEvent = FetchDeferedEvent();
		if (m_SendingEvent)
		{
			Send(m_SendingEvent);
		}
		else
		{
			TRANS(&CDevice::S_Idle);
		}
		break;
	case MAC_REQ_SIG:
		DeferEvent(e);
		break;
	default:
		break;
	}
}

void CDevice::SendSuccess(Event const* const e)
{

}

void  CDevice::SendFail(Event const* const e)
{

}

void CDevice::CreateIrqSendCompleteEvent()
{
	m_IrqSendCompleteEvent = new CDeviceEvent(HW_OUT_COMPLETE_SIG, m_HwHandle, 0, false);
}
// call from Device driver
void CDevice::PostIrqSendCompleteEvent()
{
	ASSERT(m_IrqSendCompleteEvent);
	Publish(m_IrqSendCompleteEvent, true);
}
void CDevice::SetCallback(LowMacCallBack MacCB, LowPostIrqRecvEvent IrqRecvCB)
{
	m_MacCallback = MacCB;
	m_PostIrqRecvEvent = IrqRecvCB;
}

bool CDevice::DeferEvent(Event const* const e)
{
	ASSERT(e);

	return m_DQ->Defer(e);
}

Event const * CDevice::FetchDeferedEvent()
{
	return m_DQ->Fetch();
}

void CDevice::RecycleEvent(Event const* const e)
{
	m_DQ->Recycle(e);
}

bool CDevice::IsMe(DEV_HANDLE DeviceH)
{
	return (m_HwHandle == DeviceH);
}

bool CDevice::IsMe(char* Name, EDeviceType Type)
{
	return (strcmp(Name, m_Name) == 0 && m_Type == Type);
}

CDevKeeper::CDevKeeper() : CActive((char*)"DevKeeper")
{

}

CDevKeeper* CDevKeeper::Instance()
{
	static CDevKeeper dk;

	return &dk;
}

void CDevKeeper::RegDevice(CDevice* Device)
{
	if (GetDevice(Device->m_Name, Device->m_Type))
	{
		ASSERT(false);
		return;
	}
	AddDevice(Device);
	Device->Initial(this);
}

void CDevKeeper::AddDevice(CDevice* Device)
{
	OS_EnterCritical();
	m_Device.AddHead(Device);
	OS_ExitCritical();
}

CDevice* CDevKeeper::GetDevice(DEV_HANDLE DevHandle)
{	
	return m_Device.FindItem([&DevHandle](CDevice *Dev)-> bool {
			//return Dev->IsMe(DevHandle);
			return (Dev->m_HwHandle == DevHandle);
		});
}


CDevice * CDevKeeper::GetDevice(char* Name, EDeviceType Type)
{
	return m_Device.FindItem([&Name, &Type](CDevice *Dev) -> bool {
				return Dev->IsMe(Name, Type);
			});
}

void CDevKeeper::Start()
{
	CActive::Start(DEF_PRIOITY + 1, DEF_STACK_SIZE, DEF_EQ_SIZE * 3);
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
		GetDevice(EventCast(CDeviceEvent)->m_HwHandle)->Dispatcher(e);
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
		Device->PostIrqSendCompleteEvent();
	}
}

void CDevKeeper::Receive(DEV_HANDLE DevHandle, uint8_t* Data, uint32_t Len)
{
	CDevice* Device = GetDevice(DevHandle);

	if (Device && Device->m_MacCallback(Device, Data, Len))
	{
		Device->m_PostIrqRecvEvent(Device);
	}
}

}
