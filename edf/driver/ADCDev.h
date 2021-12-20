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
#include "ADCDrv.h"
#include "Device.h"

namespace Edf
{

class CAdc : public CDevice
{
private:
	template <typename T>
	class CFilter
	{
	public:
	//	CFilter(uint32_t Size)
	//	{
	//		m_Size = Size;
	//		m_Count = 0;
	//		m_Sum = 0;
	//	}

		CFilter()
		{
			m_Size = 1 << 6;
			m_Count = 0;
			m_Sum = 0;
		}

		void Reset()
		{
			m_Count = 0;
			m_Sum = 0;
		}

		bool Input(T Data, T &Result)
		{

			m_Count ++;

			m_Sum += Data;

			if (m_Count >= m_Size)
			{
				Result = m_Sum >> 6;

				Reset();

				return true;
			}

			return false;
		}

	private:
		uint32_t m_Size;
		uint32_t m_Count;
		uint32_t m_Sum;
	};
public:
	CAdc(const char * Name, DEV_HANDLE Adc, uint32_t RspSig);
	~CAdc();

	bool Start();
	bool Stop();
	void ResetFilter();

protected:
	virtual void Initial(CActive* Owner) override;
	static void PostIrqRecvEvent(CDevice *Device);
	static bool MacCall(CDevice *Device, uint8_t *Data, uint32_t Len);

	virtual bool Send(Event const* const e) override;

private:

	CAdcEvent m_IrqRecvEvent;

	uint16_t m_Channels[CAdcEvent::MAX_CHANNEL];
	uint32_t m_ChannelNum;
	uint32_t m_CurChannel;

	ADCConfig 	m_Config;

	CFilter<uint16_t> m_Filter;

};


}
