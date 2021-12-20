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
#include "I2CDev.h"


namespace Edf
{

CI2CEvent::CI2CEvent(Signal Sig, DEV_HANDLE I2C, uint8_t *Data, uint32_t DataLen, bool Releasable)
		: CDeviceEvent(Sig, I2C, DataLen, Releasable)
{
	Copy(Data, DataLen);
}


CI2CEvent::~CI2CEvent()
{

}

Q_HANDLE CI2C::m_SyncQ = nullptr;

CI2C::CI2C(const char* Name, DEV_HANDLE I2C)
	: CDevice(Name, I2C, EDeviceType::I2C, 8)
{
	SetCallback(CI2C::MacCall, CI2C::PostIrqRecvEvent);
	CreateIrqSendCompleteEvent();
	CDevKeeper::Instance()->RegDevice(this);
}
CI2C::~CI2C()
{
}

void CI2C::Initial(CActive* Owner)
{
	if (CI2C::m_SyncQ == nullptr)
	{
		CI2C::m_SyncQ = OS_QueueCreate(1, 0);
		ASSERT(m_SyncQ);
		OS_QueueSend(m_SyncQ, nullptr);
	}
	m_RecvQ = OS_QueueCreate(1, 0);
	ASSERT(m_RecvQ);

	I2C_Init(m_HwHandle);

	CDevice::Initial(Owner);
}
bool CI2C::Send(Event const* const e)
{
	uint8_t result;

	const CI2CEvent *ie = EventCast(CI2CEvent);

	m_Slave = ie->m_SlaveAddress;
	m_Reg = ie->m_RegAddress;

	m_Mode = CDevice::MODE::MODE_ASYNC;

	m_IrqRecvEvent = new CI2CEvent(I2C_RSP_SIG, m_HwHandle, nullptr, ie->m_DataLen);

	m_IrqRecvEvent->m_SlaveAddress = m_Slave;
	m_IrqRecvEvent->m_RegAddress = m_Reg;

	if (ie->m_Mode == CI2CEvent::READ)
	{
		result = I2C_Read7(ie->m_HwHandle, m_Slave, m_Reg, m_IrqRecvEvent->m_Data, m_IrqRecvEvent->m_DataLen);
	}
	else
	{
		m_IrqRecvEvent->Copy(ie->m_Data, ie->m_DataLen);
		result = I2C_Write7(m_HwHandle, m_Slave, m_Reg, m_IrqRecvEvent->m_Data, m_IrqRecvEvent->m_DataLen);
	}

	return (result == 1);
}

bool CI2C::Read(uint8_t Address, uint8_t Reg, uint8_t DataLen)
{
	CI2CEvent *ie = new CI2CEvent(MAC_REQ_SIG, m_HwHandle, nullptr, DataLen);
	ie->m_SlaveAddress = Address << 1;
	ie->m_RegAddress = Reg;
	ie->m_Mode = CI2CEvent::READ;

	Publish(ie);

	return true;
}

bool CI2C::Write(uint8_t Address, uint8_t Reg, uint8_t *Data, uint8_t DataLen)
{
	CI2CEvent *ie = new CI2CEvent(MAC_REQ_SIG, m_HwHandle, Data, DataLen);
	ie->m_SlaveAddress = Address << 1;
	ie->m_RegAddress = Reg;
	ie->m_Mode = CI2CEvent::WRITE;

	Publish(ie);

	return true;
}

bool CI2C::ReadSync(uint8_t Address, uint8_t Reg, uint8_t *Data, uint8_t DataLen)
{
	bool res = false;

	OS_QueueReceive(m_SyncQ, nullptr, MAX_DELAY);

	m_Mode = CDevice::MODE::MODE_SYNC;

	if (I2C_Read7(m_HwHandle, Address << 1, Reg, Data, DataLen) == 1)
	{
		res = OS_QueueReceive(m_RecvQ, nullptr, 10);
	}

	OS_QueueSend(m_SyncQ, nullptr);

	return res;
}

bool CI2C::DoWriteSync(uint8_t Address, uint8_t Reg, uint8_t *Data, uint8_t DataLen)
{
	bool res = false;

	OS_QueueReceive(m_SyncQ, nullptr, MAX_DELAY);

	m_Mode = CDevice::MODE::MODE_SYNC;

	if (I2C_Write7(m_HwHandle, Address << 1, Reg, Data, DataLen) == 1)
	{
		res = OS_QueueReceive(m_RecvQ, nullptr, 10);
	}

	OS_QueueSend(m_SyncQ, nullptr);

	return res;
}

bool CI2C::WriteSync(uint8_t Address, uint8_t Reg, uint8_t *Data, uint8_t DataLen)
{
	bool res = false;

	for (uint32_t i = 0; i < 3 && !res; i ++)
	{
		res = DoWriteSync(Address, Reg, Data, DataLen);
	}

	return res;
}

void CI2C::PostIrqRecvEvent(CDevice *Device)
{
	CI2C *me = static_cast<CI2C*> (Device);
	if (me->m_Mode == CDevice::MODE::MODE_ASYNC)
	{
		Publish(me->m_IrqRecvEvent, true);
	}
	else
	{
		OS_QueueSend(me->m_RecvQ, nullptr, true);
	}
}

bool CI2C::MacCall(CDevice *Device, uint8_t *Data, uint32_t Len)
{
	return true;
}
} // namespace Edf

void I2C_SendComplete(DEV_HANDLE I2C)
{
	Edf::CI2C *dev = static_cast<Edf::CI2C *>(Edf::CDevKeeper::Instance()->GetDevice(I2C));
	if (dev->m_Mode == Edf::CDevice::MODE::MODE_ASYNC)
	{
		Edf::CDevKeeper::Instance()->SendComplete(I2C);
	}
}

void I2C_Recv(DEV_HANDLE I2C, uint8_t* Data, uint32_t Len)
{
	Edf::CDevKeeper::Instance()->Receive(I2C, Data, Len);
}


