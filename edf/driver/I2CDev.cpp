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

CI2CEvent::CI2CEvent(Signals Sig, DEV_HANDLE I2C, uint8_t *Data, uint32_t DataLen, bool Releasable)
		: CDeviceEvent(Sig, I2C, DataLen, Releasable)
{
	ASSERT(DataLen <= CI2CEvent::MAX_SIZE);
	Copy(Data, DataLen);
}


CI2CEvent::~CI2CEvent()
{

}


CI2C::CI2C(char* Name, DEV_HANDLE I2C)
	: CDevice(Name, I2C, EDeviceType::I2C, 8)
{
	m_IrqRecvEvent = new CI2CEvent(I2C_RSP_SIG, I2C, (uint8_t *)"hello world", CI2CEvent::MAX_SIZE, false);

	CDevKeeper::Instance()->RegDevice(this);
}
CI2C::~CI2C()
{
}

void CI2C::Initial(CActive* Owner)
{
	m_RecvQ = OS_QueueCreate(1, sizeof(void *));
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

	if (ie->m_Mode == CI2CEvent::READ)
	{
		result = I2C_Read7(ie->m_HwHandle, m_Slave, m_Reg, m_Buff, ie->m_DataLen);
	}
	else
	{
		memcpy(m_Buff, ie->m_Data, ie->m_DataLen);
		result = I2C_Write7(m_HwHandle, m_Slave, m_Reg, m_Buff, ie->m_DataLen);
	}

	return (result == 1);
}

bool CI2C::Read(uint8_t Address, uint8_t Reg, uint8_t DataLen)
{
	ASSERT(DataLen <= CI2CEvent::MAX_SIZE);

	m_Mode = CDevice::MODE::MODE_ASYNC;

	CI2CEvent *ie = new CI2CEvent(MAC_REQ_SIG, m_HwHandle, NULL, DataLen);
	ie->m_SlaveAddress = Address << 1;
	ie->m_RegAddress = Reg;
	ie->m_Mode = CI2CEvent::READ;

	Publish(ie);

	return true;
}

bool CI2C::Write(uint8_t Address, uint8_t Reg, uint8_t *Data, uint8_t DataLen)
{

	ASSERT(DataLen <= CI2CEvent::MAX_SIZE);

	m_Mode = CDevice::MODE::MODE_ASYNC;

	CI2CEvent *ie = new CI2CEvent(MAC_REQ_SIG, m_HwHandle, Data, DataLen);
	ie->m_SlaveAddress = Address << 1;
	ie->m_RegAddress = Reg;
	ie->m_Mode = CI2CEvent::WRITE;

	Publish(ie);

	return true;
}

bool CI2C::ReadSync(uint8_t Address, uint8_t Reg, uint8_t *Data, uint8_t DataLen)
{
	m_Mode = CDevice::MODE::MODE_SYNC;

	if (I2C_Read7(m_HwHandle, Address << 1, Reg, Data, DataLen) == 0)
	{
		return false;
	}

	CSpiEvent *e;

	return OS_QueueReceive(m_RecvQ, &e, 100);
}

bool CI2C::WriteSync(uint8_t Address, uint8_t Reg, uint8_t *Data, uint8_t DataLen)
{
	m_Mode = CDevice::MODE::MODE_SYNC;

	if (I2C_Write7(m_HwHandle, Address << 1, Reg, Data, DataLen) == 0)
	{
		return false;
	}

	CSpiEvent *e;

	return OS_QueueReceive(m_RecvQ, &e, 100);;
}

void CI2C::PostIrqRecvEvent()
{
	if (m_Mode == CDevice::MODE::MODE_ASYNC)
	{
		m_IrqRecvEvent->m_SlaveAddress = m_Slave;
		m_IrqRecvEvent->m_RegAddress = m_Reg;
		Publish(m_IrqRecvEvent, true);
	}
	else
	{
		OS_QueueSend(m_RecvQ, m_IrqRecvEvent, true);
	}
}

bool CI2C::MacCall(uint8_t *Data, uint32_t Len)
{
	ASSERT(Len <= m_IrqRecvEvent->m_Size);
	if (Len)
	{
		m_IrqRecvEvent->Copy(Data, Len);
	}
	else
	{
		m_IrqRecvEvent->m_DataLen = 0;
	}
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


