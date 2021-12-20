/*
 * Flash.cpp
 *
 *  Created on: Jan 10, 2023
 *      Author: Administrator
 */

#include "Flash.h"
#include "SpiDev.h"
#include "FlashOnSpi.h"
#include "FlashOnChip.h"

namespace Edf
{

CFlash::CFlash(uint32_t PageSize, uint32_t SectorSize, uint32_t BlockSize, uint32_t MemSize, uint32_t StartAddress)
			: m_PageSize(PageSize), m_SectorSize(SectorSize), m_BlockSize(BlockSize), m_MemSize(MemSize), m_StartAddress(StartAddress)
{

}

CFlash::~CFlash()
{

}

CFlash * CFlash::CreateFlash(ChipType Type, CDevice *Device)
{
	switch (Type)
	{
	case ChipType::OnChip:
		return new CFlashOnChip();

//	case ChipType::OnSpi:
//		return new CFlashOnSpi(Device);

	default:
		ASSERT(false);
		return nullptr;
	}
}

bool CFlash::Read(uint32_t Address, uint8_t *Data, uint32_t Size)
{
	return false;
}

bool CFlash:: Write(uint32_t Address, const uint8_t *Data, uint32_t Size)
{
	return false;
}


bool CFlash::EraseSector(uint32_t From, uint32_t To)
{
	return false;
}

bool CFlash::EraseBlock(uint32_t From, uint32_t To)
{
	return false;
}

bool CFlash::EraseChip()
{
	return false;
}

uint32_t CFlash::GetSectorSize()
{
	return m_SectorSize;
}

uint32_t CFlash::GetSize()
{
	return m_MemSize;
}

bool CFlash::CheckAddressAndSize(uint32_t Address, uint32_t Size)
{
	if (Size > this->m_MemSize)
	{
		return false;
	}

	if ((Address + Size) > this->m_MemSize)
	{
		return false;
	}

	return true;
}

}

