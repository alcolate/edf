#pragma once

#include <iostream>

#include "edf.h"


class CHello : public CActive
{
public:
	CHello() : CActive((char*)"AT"), m_Time(TIMEOUT_SIG, this)
	{

	}

	static CHello* Instance()
	{
		static CHello at;
		return &at;
	}

	virtual void Initial()
	{

		Edf::Subscribe(RF_WTR_SIG, this);
		INIT_TRANS(&CHello::S_Silence);
	}

	void S_Silence(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
			m_Time.Trigger(100, 0U);
			std::cout << "enter " << __FUNCTION__ << std::endl;
			break;
		case EXIT_SIG:
			std::cout <<  "exit " << __FUNCTION__ << std::endl;
			break;

		case TIMEOUT_SIG:
			m_Time.Trigger(MilliSecond(500 + (((uint32_t)this) % 500)), 0U);
			
			TRANS(&CHello::S_Noise);
			break;
		case RF_WTR_SIG:

			break;

		default:
			break;
		}
	}

	void S_Noise(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
			m_Time.Trigger(100, 0U);
			std::cout << "enter " << __FUNCTION__ << std::endl;
			break;
		case EXIT_SIG:
			std::cout << "exit " << __FUNCTION__ << std::endl;
			break;

		case TIMEOUT_SIG:
			m_Time.Trigger(MilliSecond(500 + (((uint32_t)this) % 500)), 0U);
			TRANS(&CHello::S_Silence);
			break;
		case RF_WTR_SIG:

			break;

		default:
			break;
		}
	}

public:

	CTimeEvent m_Time;

public:
	DEF_STATE(CHello);
};

