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
#include "Active.h"

extern uint32_t MAX_SIG;

namespace Edf
{
class CSubscriber
{
public:
	CSubscriber(CActive const *  Act, uint32_t Number = 0)
	{
		ASSERT(Act);
		this->m_Act = Act;
		this->m_Number = Number + 1;
	}

	void Update(const Event * const e, bool FromISR = false)
	{
		if (false == const_cast<CActive *>(m_Act)->Post(e, FromISR))
		{
			const_cast<Event *>(e)->DecRef();
		}
	}

public:
	CActive const *  m_Act;	
	uint32_t	m_Number;

	USE_LINK(CSubscriber);
};

class CPublisher
{
public:
	static CPublisher *Instance();

	void Subscribe(Signal Sig, CActive const * const Act);

	void UnSubscribe(Signal Sig, CActive const * const Act);

	void Publish(Event const * const e, bool FromISR = false);



private:
	void AddTail(CSubscriber **Head, CActive const * const Act);

	void AddHead(CSubscriber **Head, CActive const * const Act);

	void Delete(CSubscriber **Head, CActive const * const Act);

private:
	CPublisher();
	~CPublisher();
private:
	CList <CSubscriber> *m_Subs;

};

} // namespace Edf

namespace Edf
{
CPublisher * CPublisher::Instance()
{
	static CPublisher puber;
	return &puber;
}

void CPublisher::Subscribe(Signal Sig, CActive const * const Act)
{
	OS_EnterCritical();

	if (!m_Subs[Sig].IsExist([&Act](CSubscriber *Item) -> bool {return Item->m_Act == Act;}))
	{
		m_Subs[Sig].AddHead(new CSubscriber(Act, m_Subs[Sig].Head() ? m_Subs[Sig].Head()->m_Number : 0));
	}	
	
	OS_ExitCritical();

}

void CPublisher::UnSubscribe(Signal Sig, CActive const * const Act)
{
	OS_EnterCritical();

	CSubscriber* Item = m_Subs[Sig].RemoveItem([&Act](CSubscriber* Item) -> bool {return Item->m_Act == Act; });

	if (Item)
	{
		delete Item;
	}
	else
	{
		ASSERT(Item);
	}
	
	OS_ExitCritical();
}

void CPublisher::Publish(Event const * const e, bool FromISR)
{
	CSubscriber *suber = m_Subs[e->Sig].Head();
	
	if (suber)
	{		
		const_cast<Event *>(e)->InitRef(suber->m_Number, FromISR);
	}
	else
	{
		const_cast<Event*>(e)->DecRef(FromISR);
	}
	
	while (suber)
	{		
		suber->Update(e, FromISR);
		suber = m_Subs[e->Sig].Next(suber);
	}
}


CPublisher::CPublisher()
{
	this->m_Subs = new CList <CSubscriber> [MAX_SIG] ;
}

CPublisher::~CPublisher()
{
	for (uint32_t i = 0; i < MAX_SIG; i ++)
	{
		CSubscriber* p;
		while ((p = m_Subs[i].RemoveHead()) != nullptr)
		{
			delete p;
		}
	}
}



} // namespace Edf

namespace Edf
{
void Subscribe(Signal Sig, CActive const * const Act)
{
	ASSERT(Sig < MAX_SIG);
	ASSERT(Act);
	Edf::CPublisher::Instance()->Subscribe(Sig, Act);
}

void UnSubscribe(Signal Sig, CActive const * const Act)
{
	ASSERT(Sig < MAX_SIG);
	ASSERT(Act);
	Edf::CPublisher::Instance()->UnSubscribe(Sig, Act);
}

void Publish(Event const * const e, bool FromISR)
{
	ASSERT(e);
	ASSERT(e->Sig < MAX_SIG);

	Edf::CPublisher::Instance()->Publish( e, FromISR);
}
}
