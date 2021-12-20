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
#include "I2CDrv.h"
#include "Device.h"

namespace Edf
{

class CI2C : public CDevice
{
public:
	CI2C(const char* Name, DEV_HANDLE I2C);
	~CI2C();

	bool Read(uint8_t Address, uint8_t Reg, uint8_t DataLen);

	bool Write(uint8_t Address, uint8_t Reg, uint8_t *Data, uint8_t DataLen);

	bool ReadSync(uint8_t Address, uint8_t Reg, uint8_t *Data, uint8_t DataLen);

	bool WriteSync(uint8_t Address, uint8_t Reg, uint8_t *Data, uint8_t DataLen);

protected:
	virtual bool Send(Event const* const e) override;
	static void PostIrqRecvEvent(CDevice *Device);
	static bool MacCall(CDevice *Device, uint8_t *Data, uint32_t Len);
	virtual void Initial(CActive* Owner) override;
	bool DoWriteSync(uint8_t Address, uint8_t Reg, uint8_t *Data, uint8_t DataLen);

public:
	MODE m_Mode;

private:

	CI2CEvent *m_IrqRecvEvent;

	uint8_t m_Slave;
	uint8_t m_Reg;

	I2CConfig 	m_Config;

	Q_HANDLE   m_RecvQ;
	static Q_HANDLE   m_SyncQ;


};


}
