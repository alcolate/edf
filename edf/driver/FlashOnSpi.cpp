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
#include <memory.h>
#include "FlashOnSpi.h"
#include "SpiDev.h"

namespace Edf
{


CFlashOnSpi::CFlashOnSpi(CDevice *Spi) : CFlash(256, 4096, 4096 * 16, 4096 * 1024, 0)
{
	 m_Spi = static_cast<CSPI*>(Spi);
}

CFlashOnSpi::~CFlashOnSpi()
{

}

bool CFlashOnSpi::Read(uint32_t Address, uint8_t *Data, uint32_t Size)
{
	uint8_t Cmd[4];
	Cmd[0] = 0x03;
	Cmd[1] = (uint8_t)((Address >> 16) & 0x0FF);
	Cmd[2] = (uint8_t)((Address >> 8) & 0x0FF);
	Cmd[3] = (uint8_t)((Address) & 0x0FF);

	return Send(Cmd, sizeof(Cmd), Data, Size);
}

bool CFlashOnSpi::Write(uint32_t Address, const uint8_t *Data, uint32_t Size)
{
	const uint8_t *p = (uint8_t *)Data;

	uint8_t *Cmd = new  uint8_t[4 + this->m_PageSize];

	uint32_t Len = Size;

	bool Result = true;

	ASSERT(Cmd);

	while (Len)
	{
		if (!WriteEnable())
		{
			Result = false;
			break;
		}

		uint32_t Rest = this->m_PageSize - (Address & (this->m_PageSize - 1));
		uint32_t ToWrite = 0;

		ToWrite = (Len > Rest)? Rest: Len;

		Cmd[0] = 0x02;
		Cmd[1] = (uint8_t)((Address >> 16) & 0x0FF);
		Cmd[2] = (uint8_t)((Address >> 8) & 0x0FF);
		Cmd[3] = (uint8_t)((Address) & 0x0FF);

		memcpy(&Cmd[4], p, ToWrite);

		if (!Send(&Cmd, 4 + ToWrite, nullptr, 0))
		{
			Result = false;
			break;
		}

		Len -= ToWrite;

		Address += ToWrite;
		p += ToWrite;

	}

	delete [] Cmd;

	return Result;
}


bool CFlashOnSpi::EraseSector(uint32_t From, uint32_t End)
{
	if (!CheckAddressAndSize(From, End - From))
	{
		return false;
	}

	if (!WriteEnable())
	{
		return false;
	}

	uint8_t Cmd[4];

	bool r = false;

	uint32_t Address = From;

	do {
		Cmd[0] = 0x20;
		Cmd[1] = (uint8_t)((Address >> 16) & 0x0FF);
		Cmd[2] = (uint8_t)((Address >> 8) & 0x0FF);
		Cmd[3] = (uint8_t)((Address) & 0x0FF);

		r = Send(Cmd, sizeof(Cmd), nullptr, 0);

		Address += this->m_SectorSize;

	}while (Address < End && r);

	return r;
}

bool CFlashOnSpi::EraseBlock(uint32_t From, uint32_t End)
{
	return false;
}

bool CFlashOnSpi::EraseChip()
{
	return false;
}

bool CFlashOnSpi::WaitIdle()
{
	uint8_t Cmd[] = {0x05};
	uint8_t Status = 0;

	do {

		if (!m_Spi->SendSync(Cmd, sizeof(Cmd), &Status, 1))
		{
			return false;
		}

	}while (Status & 0x01);

	return true;
}

bool CFlashOnSpi::Send(void *Tx, uint16_t TxLen, void *Rx, uint16_t RxLen)
{
	if (m_Spi->SendSync((uint8_t *)Tx, TxLen, (uint8_t *)Rx, RxLen))
	{
		return WaitIdle();
	}

	return false;
}

bool CFlashOnSpi::WriteEnable()
{
	uint8_t Cmd[] = {0x06};

	return m_Spi->SendSync(Cmd, sizeof(Cmd), nullptr, 0);
}

bool CFlashOnSpi::ReadID(uint16_t &Id)
{
	uint8_t Cmd[] = {0x90, 0, 0, 0};

	return Send(Cmd, sizeof(Cmd), (uint8_t *)&Id, 2);
}

bool CFlashOnSpi::SelfCheck()
{
	uint16_t Id;

	if (ReadID(Id) && Id == 0x16ef)
	{
		return true;
	}

	return false;
}

};



