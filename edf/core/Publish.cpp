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

};

class CPublisher
{
public:
	static CPublisher * Instance()
	{
		static CPublisher puber;
		return &puber;
	}

	void Init(uint32_t SigNum)
	{
		m_SigNum = SigNum;
		this->m_Subs = new CList <CSubscriber> [SigNum];
	}

	inline uint32_t SigNum()
	{
		return m_SigNum;
	}

	void Subscribe(Signal Sig, CActive const * const Act)
	{
		OS_EnterCritical();

		if (!m_Subs[Sig].IsExist([&Act](CSubscriber *Item) -> bool {return Item->m_Act == Act;}))
		{
			m_Subs[Sig].AddHead(new CSubscriber(Act, m_Subs[Sig].Head() ? m_Subs[Sig].Head()->m_Number : 0));
		}	
		
		OS_ExitCritical();

	}

	void UnSubscribe(Signal Sig, CActive const * const Act)
	{
		OS_EnterCritical();

		m_Subs[Sig].RemoveItem([&Act](CSubscriber* Item) -> bool {
			if ( Item->m_Act == Act){
				delete Item;
				return true;
			}
			return false;
		});
		
		OS_ExitCritical();
	}

	void Publish(Event const * const e, bool FromISR = false)
	{
		CSubscriber *head = m_Subs[e->Sig].Head();
		
		if (head)
		{		
			const_cast<Event *>(e)->InitRef(head->m_Number, FromISR);
			m_Subs[e->Sig].ForEach(
				[&e, &FromISR](CSubscriber* suber)-> void {
					suber->Update(e, FromISR);
				}
			);
		}
		else
		{
			const_cast<Event*>(e)->DecRef(FromISR);
		}	

	}	

private:
	CPublisher()
	{
		
	}	
	~CPublisher()
	{
		for (uint32_t i = 0; i < m_SigNum; i ++)
		{
			for (CSubscriber* p = m_Subs[i].RemoveHead(); p; delete p, p = m_Subs[i].RemoveHead());
		}
	}

private:
	CList <CSubscriber> *m_Subs;
	uint32_t m_SigNum;

};

} // namespace Edf

namespace Edf
{
void InitPublish(uint32_t SigNum)
{
	Edf::CPublisher::Instance()->Init(SigNum);
}

void Subscribe(Signal Sig, CActive const * const Act)
{
	ASSERT(Sig < Edf::CPublisher::Instance()->SigNum());
	ASSERT(Act);
	Edf::CPublisher::Instance()->Subscribe(Sig, Act);
}

void UnSubscribe(Signal Sig, CActive const * const Act)
{
	ASSERT(Sig < Edf::CPublisher::Instance()->SigNum());
	ASSERT(Act);
	Edf::CPublisher::Instance()->UnSubscribe(Sig, Act);
}

void Publish(Event const * const e, bool FromISR)
{
	ASSERT(e);
	ASSERT(e->Sig < Edf::CPublisher::Instance()->SigNum());
	Edf::CPublisher::Instance()->Publish( e, FromISR);
}
}
