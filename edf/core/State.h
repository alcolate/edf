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
			LOG_DEBUG("Init:\t%s of %s\r\n", m_StateName, m_Name);
#endif
			break;

		case ENTRY_SIG:
#if (TRACE_STATE == 1)
			LOG_DEBUG("Enter:\t%s of %s\r\n", m_StateName, m_Name);
#endif
			RunState(e);
			break;

		case EXIT_SIG:
#if (TRACE_STATE == 1)
			LOG_DEBUG("Exit:\t%s of %s\r\n", m_StateName, m_Name);
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

