#include "Edf.h"


class CMotorEvent : public Event
{
public:
	CMotorEvent();




}

class CMotor
{
public:
	CMotor()
	{

	}

	void S_Idle(Event const* const e)
	{
		switch (e->Sig)
		{
		case Start_SIG:
			// send moter command
			// transfer state
			break;

		case Reset_Sig:
			

			break;
		
		default:
			break;
		}
	}

	void S_WaitResponse(Event const* const e)
	{
		switch (e->Sig)
		{
		case ActComplete_Sig:
			
			break;
		
		default:
			break;
		}
	}

public:
	uint8_t		m_Id;
	USE_LINK(CMotor);
public:
	DEF_STATEMACHINE(CMotor);
};

class CController : public CActive
{
public:
	CController() : CActive((char *)"Controller")
	{

	}

};

