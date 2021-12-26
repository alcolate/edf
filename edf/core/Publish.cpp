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

namespace Edf
{
class CSubscriber
{
public:
	CActive const *  m_Act;
	CSubscriber *m_Next;
	uint32_t	m_Number;

	CSubscriber(CActive const *  Act, CSubscriber *Next = 0)
	{
		ASSERT(Act);
		this->m_Act = Act;
		this->m_Next = Next;
		if (Next)
		{		
			this->m_Number = Next->m_Number + 1;
		}
		else
		{
			this->m_Number = 1;
		}
	}

	void Update(const Event * const e, bool FromISR = false)
	{
		if (false == const_cast<CActive *>(m_Act)->Post(e, FromISR))
		{
			const_cast<Event *>(e)->DecRef();
		}
	}

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
	CList <CSubscriber> m_Subs[MAX_SIG];

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

	CSubscriber *sub = new CSubscriber(Act, m_Subs[Sig].Head());

	if (m_Subs[Sig].IsExist([](CSubscriber *This, CSubscriber *That) -> bool {return This->m_Act == That->m_Act;}, sub))
	{
		delete sub;
	}
	else
	{
		m_Subs[Sig].AddHead(sub);
	}	
	
	OS_ExitCritical();

}

void CPublisher::UnSubscribe(Signal Sig, CActive const * const Act)
{
	OS_EnterCritical();

	CSubscriber Sub(Act, 0);

	CSubscriber* Item = m_Subs[Sig].RemoveItem([](CSubscriber* This, CSubscriber* That) -> bool {return This->m_Act == That->m_Act; },  &Sub);

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

}

CPublisher::~CPublisher()
{
	for (uint32_t i = 0; i < sizeof(m_Subs) / sizeof(m_Subs[0]); i ++)
	{
		CSubscriber* p;
		while (p = m_Subs[i].RemoveHead())
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
	ASSERT(e->Sig < MAX_SIG);
	ASSERT(e);
	Edf::CPublisher::Instance()->Publish( e, FromISR);
}
}
