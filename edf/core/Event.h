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
	USER_SIG,  /* first signal available to the users */
	TIMEOUT_SIG,
	TEST_SIG,
	TEST2_SIG,
	TEST3_SIG,
	TEST4_SIG,
	TEST5_SIG,
	SERIAL_RSP_SIG,   // get from serial
	SERIAL_HW_OUT_COMPLETE_SIG, // send completely
	MAC_REQ_SIG,  // send to serial
	MAC_RSP_SIG,
	APP_REQ_SIG,
	UART_SIM_SIG,
	MAX_SIG,
};


/* Event base class */
class Event 
{
public:
	Event(Signal s, bool DynAlloc = false);
	virtual ~Event();
	Signal Sig; 
	uint32_t	RefCount;
	const bool  DynamicAlloc;
	void InitRef(uint32_t Ref, bool FromISR = false);
	void IncRef(uint32_t Ref, bool FromISR = false);
	void DecRef(bool FromISR = false);

};

#define EventCast(T)  (static_cast<T const *>(e))

class CEventQ
{
public:
	CEventQ(uint32_t ItemCount = DEF_ITEMS);
	~CEventQ();

public:
	bool Defer(Event const* const Evt);

	const Event* Fetch(void);

private:
	class CItem
	{
	public:
		CItem(){}
		Event const* m_Evt = 0;
		USE_LINK(CItem);
	};

	CItem* GetFreeItem();
	CQueue<CItem> m_Queue;
	enum { DEF_ITEMS = 10 };
	uint32_t m_ItemCount;
	CItem *m_Items;

};

extern Event const InitEvt;
extern Event const EntryEvent;
extern Event const ExitEvent;

} // namaspace Edf
