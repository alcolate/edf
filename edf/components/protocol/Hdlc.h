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

#include "yahdlc.h"
#include "protocol.h"

class CHdlc;

class CHdlcImp
{
public:
	CHdlcImp(CHdlc* MacLayer);

	enum
	{
		DELIMETER = YAHDLC_FLAG_SEQUENCE
	};

	void PacketData(const uint8_t* Data, uint16_t Len);

	void Ack(uint8_t SeqNo);

	bool Parser(const uint8_t* Data, uint32_t Len);

private:
	yahdlc_control_t m_ControlSend;
	yahdlc_control_t m_ControlRecv;

	CHdlc *m_MacLayer;

};

class CHdlc : public CMacLayer
{
public:
	CHdlc();
	~CHdlc();
	static bool MacCall(uint8_t* Buff, uint16_t& BuffSize, uint16_t& BuffCount, uint8_t* Data, uint32_t Len);

	virtual void Initial() override;

	void S_Idle(Event const* const e);
	void S_WaitResponse(Event const* const e);

	void S_ReSend(Event const* const e);

	inline bool IsToMe(Event const* const e);

	void Request(const uint8_t* Data, uint32_t Len);

public:
	uint32_t m_Retries;
	// lower layer
	CUart *m_Uart;
	CHdlcImp *m_Hdlc;
	CTimeEvent m_Timer;
	// upper layer


public:
	DEF_STATEMACHINE(CHdlc);
};
