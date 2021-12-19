/*****************************************************************************
* MIT License:
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*
* Contact information:
* <9183399@qq.com>
*****************************************************************************/
#include "Active.h"

namespace Edf
{
CActive::CActive(char *Name, uint32_t DQSize)
{
	m_Name = Name;
	m_Queue = 0;
	m_Thread = 0;
	m_Priority = DefPrioity;
	m_StackSize = MINIMAL_STACK_SIZE;
	m_DQ = new CActive::CEventQ(DQSize);
	
}

CActive::~CActive()
{
	if (m_DQ)
		delete m_DQ;
}

void CActive::Start()
{
	Start(m_Priority, EQ_SIZE, sizeof(Event *));
}

void CActive::Start(uint32_t prio, uint32_t queueLen, uint32_t itemSize)
{

	this->m_Thread = OS_TaskCreate(
			m_Name,
			m_StackSize,               
			this,                       
			prio, &(this->m_Queue), EQ_SIZE);  

	ASSERT(this->m_Queue);

	ASSERT(this->m_Thread); 
}

bool CActive::Post(Event const *const e, bool FromISR)
{
	//LOG_DEBUG("%s post sig = %d, ref = %d\r\n", m_Name, e->Sig, e->RefCount);
	bool result = OS_QueueSend(Q(), e, FromISR);
	ASSERT(result);
	return result;
}

void CActive::Run(void)
{
	EventLoop();
}

void CActive::EventLoop()
{
	Event* e;
	
	this->Initial();

	for (;;) 
	{

		if (OS_QueueReceive(this->m_Queue, &e, MAX_DELAY))
		{			
			//LOG_DEBUG("%s get event sig = %d, ref = %d\r\n", m_Name, e->Sig, e->RefCount);
			this->RunState(e);

			e->DecRef();
		}
	}
}

void CActive::RunState(Event const* const e)
{

}

CActive::CEventQ::CEventQ(uint32_t ItemCount)
{
	m_Head = 0;
	m_ItemCount = ItemCount;
	m_Items = new CItem[ItemCount];
}
CActive::CEventQ::~CEventQ()
{
	if (m_ItemCount)
		delete[] m_Items;
}
CActive::CEventQ::CItem* CActive::CEventQ::GetFreeItem()
{
	for (uint32_t i = 0; i < m_ItemCount; i++)
	{
		if (m_Items[i].Evt == 0) return m_Items;
	}

	return NULL;
}

void CActive::CEventQ::LinkItem(CItem* Item)
{
	CItem* p = m_Head;
	if (NULL == p)
	{
		p = Item;
	}
	else
	{
		for (; p->Next; p = p->Next);
		p->Next = Item;
	}
}

bool CActive::CEventQ::Defer(Event const* const Evt)
{
	CItem* item = GetFreeItem();

	if (!item)
	{
		return false;
	}
	else
	{
		(const_cast<Event*>(Evt))->IncRef(1);
		item->Evt = Evt;
		item->Next = 0;

		LinkItem(item);
		return true;
	}
}

const Event* CActive::CEventQ::Fetch(void)
{
	CItem* Item = 0;
	const Event* Evt = 0;

	if (m_Head)
	{
		Item = m_Head;
		m_Head = m_Head->Next;
		Evt = Item->Evt;
		Item->Evt = 0;
		Item->Next = 0;

	}

	return Evt;
}

void EdfStart(void)
{
	OS_Start();
}

} // namespace Edf
