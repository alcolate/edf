/*
 * Flash.h
 *
 *  Created on: Jan 10, 2023
 *      Author: Administrator
 */

#pragma once

#include "Device.h"


namespace Edf
{

class CFlash
{
public:
	enum class ChipType{
		OnChip,
		OnSpi,
	};

public:
	CFlash(uint32_t PageSize, uint32_t SectorSize, uint32_t BlockSize, uint32_t MemSize, uint32_t StartAddress);
	CFlash(const CFlash &Flash) = delete;
	CFlash& operator = (const CFlash &Flash) = delete;
	virtual ~CFlash();

	static CFlash * CreateFlash(ChipType Type, CDevice *Device);

	virtual bool Read(uint32_t Address, uint8_t *Data, uint32_t Size);

	virtual bool Write(uint32_t Address, const uint8_t *Data, uint32_t Size);

	virtual bool EraseSector(uint32_t From, uint32_t To);
	virtual bool EraseBlock(uint32_t From, uint32_t To);
	virtual bool EraseChip();

	uint32_t GetSectorSize();
	uint32_t GetSize();

protected:
	bool CheckAddressAndSize(uint32_t Address, uint32_t Size);

	const uint32_t m_PageSize;
	const uint32_t m_SectorSize;
	const uint32_t m_BlockSize;
	const uint32_t m_MemSize;
	const uint32_t m_StartAddress;

};

}
