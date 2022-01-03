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

#define TRACE_STATE		0

namespace Edf
{
template <class T>
class CStateMachine
{
public:
	CStateMachine() 
	{
		m_Obj = 0;
	}

	T* m_Obj;

	using STATE = void (T::*)(Event const* const e);

	STATE  m_State;
	const char* m_StateName;

	void RunState(Event const* const e)
	{
		(m_Obj->*m_State)(e);
	}

	void Init(T *Obj, STATE State, const char* Name)
	{
		ASSERT(Obj);
		m_Obj = Obj;
		m_State = State;
		m_StateName = Name;
		this->Dispatcher(&EntryEvent);
	}

	void Trans(STATE State, const char* Name)
	{
		this->Dispatcher(&ExitEvent);
		m_State = State;
		m_StateName = Name;
		this->Dispatcher(&EntryEvent);
	}

	void Dispatcher(Event const* const e)
	{
		switch (e->Sig)
		{
		case INIT_SIG:
			//Initial();
#if (TRACE_STATE == 1)
			LOG_DEBUG("Init:\t%s\r\n", m_StateName);
#endif
			break;

		case ENTRY_SIG:
#if (TRACE_STATE == 1)
			LOG_DEBUG("Enter:\t%s\r\n", m_StateName);
#endif
			RunState(e);
			break;

		case EXIT_SIG:
#if (TRACE_STATE == 1)
			LOG_DEBUG("Exit:\t%s\r\n", m_StateName);
#endif
			RunState(e);
			break;

		default:
			RunState(e);
			break;

		}
	}

};
} // namespace Edf

#define DEF_STATEMACHINE(T) \
		Edf::CStateMachine<T> m_StateMachine; \
		virtual void RunState(Event const * const e)\
		{\
			this->m_StateMachine.Dispatcher(e);\
		}



#define INIT_TRANS(state) \
		do {\
			this->m_StateMachine.Init(this, state, #state);\
		}while(0)

#define TRANS(to) \
		do {\
			this->m_StateMachine.Trans(to, #to); \
		}while(0)

