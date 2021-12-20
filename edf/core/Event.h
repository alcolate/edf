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

namespace Edf
{

typedef uint16_t Signal; /* event signal */

enum Signals 
{
	INIT_SIG, 
	ENTRY_SIG,
	EXIT_SIG,
	TIMEOUT_SIG,
	HW_RSP_SIG,   // get from serial
	HW_OUT_COMPLETE_SIG, // send completely
	UART_RSP_SIG,    //  in data
	CAN_RSP_SIG,	//  in data
	SPI_RSP_SIG,	//  in data
	I2C_RSP_SIG,	//  in data
	PWM_RSP_SIG,		//  in data
	ADC_RSP_SIG,		//  in data
	GPIO_RSP_SIG,		//  gpio interrupt
	MAC_REQ_SIG,  // send to serial
	MAC_RSP_SIG,
	PC_REQ_SIG,
	APP_REQ_SIG,
	UART_SIM_SIG,
	USER_SIG,  /* first signal available to the users */
};


/* Event base class */
class Event 
{
public:
	Event(Signal s, bool Releasable = true);
	virtual ~Event();
	Signal Sig; 
	uint32_t	RefCount;	
	const bool  Releasable;
	bool		Freeing;
	void InitRef(uint32_t Ref, bool FromISR = false);
	void IncRef(bool FromISR = false);
	void DecRef(bool FromISR = false);

};

#define EventCast(T)  (static_cast<T const *>(e))

class CEventQ
{
public:
	CEventQ(uint32_t MaxItems = DEF_ITEMS);
	~CEventQ();

public:
	bool Defer(Event const* const Evt);

	const Event* Fetch(void);

	void Recycle(Event const* const Evt);

private:

	CQueue<Event> m_Queue;
	enum { DEF_ITEMS = 10 };
	uint32_t m_MaxItems;

};

extern Event const InitEvt;
extern Event const EntryEvent;
extern Event const ExitEvent;

} // namaspace Edf
