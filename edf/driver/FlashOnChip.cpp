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
#include "FlashOnChip.h"
#include "FlashOnChipDrv.h"

namespace Edf
{


CFlashOnChip::CFlashOnChip() : CFlash(2048, 2048, 2048, 2048 * 2,  0x08040000 - 2048 * 2)
{

}

CFlashOnChip::~CFlashOnChip()
{

}

bool CFlashOnChip::Read(uint32_t Address, uint8_t *Data, uint32_t Size)
{
	if (!CheckAddressAndSize(Address, Size))
	{
		return false;
	}

	FlashUnProtect();

	memcpy(Data, (void*)(m_StartAddress + Address), Size);

	FlashProtect();

	return true;
}

bool CFlashOnChip::Write(uint32_t Address, const uint8_t *Data, uint32_t Size)
{
	if (!CheckAddressAndSize(Address, Size))
	{
		return false;
	}

	FlashUnProtect();

	bool r = FlashWrite(m_StartAddress + Address, Data, Size);

	FlashProtect();

	return r;
}


bool CFlashOnChip::EraseSector(uint32_t From, uint32_t End)
{
	if (!CheckAddressAndSize(From, End - From))
	{
		return false;
	}

	FlashUnProtect();

	bool r = false;

	uint32_t Address = From;

	do {
		r = FlashEraseSector(m_StartAddress + Address);

		Address += this->m_SectorSize;

	}while (Address < End && r);

	FlashProtect();

	return r;
}

bool CFlashOnChip::EraseBlock(uint32_t From, uint32_t End)
{
	return false;
}

bool CFlashOnChip::EraseChip()
{
	return false;
}


};
