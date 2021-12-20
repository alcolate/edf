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
#include <stdio.h>
#include "Active.h"

namespace Edf
{
CActive::CActive(char *Name, uint32_t DQSize)
{
	snprintf(m_Name, sizeof(m_Name), "%s", Name);
	m_Queue = 0;
	m_Thread = 0;
	m_Priority = DEF_PRIOITY;
	m_StackSize = MINIMAL_STACK_SIZE;
	m_DQ = DQSize? new CEventQ(DQSize): nullptr;
}

CActive::~CActive()
{
	if (m_DQ)
		delete m_DQ;
}

void CActive::Start()
{
	Start(m_Priority, m_StackSize, DEF_EQ_SIZE);
}

void CActive::Start(uint32_t Priority, uint32_t StackSize, uint32_t EQSize)
{	
	m_Priority = Priority;
	m_StackSize = StackSize;

	this->m_Thread = OS_TaskCreate(
			m_Name,
			m_StackSize,               
			this,                       
			m_Priority, &(this->m_Queue), EQSize);  

	ASSERT(this->m_Queue);

	ASSERT(this->m_Thread); 
}

void CActive::SetPriority(uint32_t Priority)
{
	OS_TastSetPriority(m_Thread, Priority);
}

bool CActive::Post(Event const *const e, bool FromISR)
{
	bool result = OS_QueueSend(Q(), e, FromISR);
	ASSERT(result);
	return result;
}

void CActive::Run(void)
{
	Event* e;
	
	this->Initial();

	for (;;) 
	{

		if (OS_QueueReceive(this->m_Queue, &e, MAX_DELAY))
		{			
			this->RunState(e);

			e->DecRef();
		}
	}
}

bool CActive::DeferEvent(Event const* const e)
{
	ASSERT(e);
	ASSERT(m_DQ);

	bool result = m_DQ->Defer(e);

	if (!result) LOG_INFO("%s: DQ is full\r\n", m_Name);

	return result;
}

Event const * CActive::FetchDeferedEvent()
{
	ASSERT(m_DQ);

	return m_DQ->Fetch();
}

void CActive::RecycleEvent(Event const* const e)
{
	m_DQ->Recycle(e);
}

void CActive::ClearDeferedEvent()
{
	const Event* e;
	ASSERT(m_DQ);

	while ((e = m_DQ->Fetch()) != nullptr)
	{
		(const_cast<Event*>(e))->DecRef();
	}
}



void EdfStart(uint32_t SigNum)
{
	InitPublish(SigNum);
	OS_Start();
}

} // namespace Edf
