#pragma once

#include "yahdlc.h"
#include "Edf.h"
#include "Serial.h"


class CHdlcEvent : public Event
{
public:
	CHdlcEvent(Signals Sig, const uint8_t *Data, uint32_t Len) : Event(Sig, true)
	{
        if (Len)
        {
		    m_Data = new uint8_t [Len];
			ASSERT(m_Data);
			memcpy(m_Data, Data, Len);			
        }
		m_DataLen = Len;
	}
	~CHdlcEvent()
	{
        if (m_DataLen)
		    delete [] m_Data;
	}

public:
	uint8_t *m_Data;
	uint32_t m_DataLen;
};

class CMacLayer;

class CHdlc
{
public:
    CHdlc(CMacLayer *MacLayer) : m_MacLayer(MacLayer)
    {
        m_ControlSend.seq_no = 0;
        m_ControlRecv.seq_no = 0;
    }

    enum
    {
        DELIMETER = YAHDLC_FLAG_SEQUENCE
    };

    bool Packet(const uint8_t *Data, uint16_t Len, uint8_t *Frame, uint32_t *FrameLen)
    {
        int ret;
        
        
        m_ControlSend.frame = YAHDLC_FRAME_DATA;
        m_ControlSend.seq_no ++;
        ret = yahdlc_frame_data(&m_ControlSend, (char *)Data, Len, (char *)Frame, FrameLen);

        return (ret == 0);
    }

	void Ack(yahdlc_control_t* Control);

    bool Parser(const uint8_t *Data, uint32_t Len)
    {
        int ret;

        CHdlcEvent *he = new CHdlcEvent(CMD_RESULT_SIG, Data, Len);

        ret = yahdlc_get_data(&m_ControlRecv, (char *)Data, Len, (char *)he->m_Data, &he->m_DataLen);

        if (ret > 0)
        {
            if (m_ControlRecv.frame == YAHDLC_FRAME_ACK
                    || m_ControlRecv.frame == YAHDLC_FRAME_NACK)
            {
				delete he;

                if (((m_ControlSend.seq_no + 1) == m_ControlRecv.seq_no)
					|| (m_ControlSend.seq_no == 7 && m_ControlRecv.seq_no == 0))
                {                    
                    return true;
                }
            }
            else if (m_ControlRecv.frame == YAHDLC_FRAME_DATA)
            {
                yahdlc_control_t control;
                control.seq_no = m_ControlRecv.seq_no + 1;
                control.frame = YAHDLC_FRAME_ACK;
                //Ack(&control);
                Publish(he);
            }
			else
			{
				delete he;
				ASSERT(false);
			}
        }
		else
		{
			delete he;
		}

        return false;
    }

private:
    yahdlc_control_t m_ControlSend;
    yahdlc_control_t m_ControlRecv;

    CMacLayer *m_MacLayer;

};

class CMacLayer : public CActive
{
public:
	CMacLayer() : CActive((char*)"Maclayer", 1), m_Timer(TIMEOUT_SIG, this)
	{

	}
	~CMacLayer()
	{
        delete m_Uart;
        delete m_Hdlc;

	}
	static bool MacCall(uint8_t *Buff, uint16_t &BuffSize, uint16_t &BuffCount, uint8_t AByte)
	{
		Buff[BuffCount++] = AByte;
		// get a frame?
		if (AByte == CHdlc::DELIMETER && BuffCount > 1)
		{
			return true;
		}
		else
		{
			// discard all data
			if (BuffCount == BuffSize)
			{
				BuffCount = 0;
			}
			return false;
		}
	}

    void Initial()
    {
		m_Uart = new CUart((char*)"AppUart", 
							UART_0, 9600, Parity_None, StopBit_1Bit, 
							128, CMacLayer::MacCall);		
		ASSERT(m_Uart);

        m_Hdlc = new CHdlc(this);
		ASSERT(m_Hdlc);

		Subscribe(CMD_OUT_SIG, this);
		Subscribe(SERIAL_IN_SIG, this);

    	INIT_TRANS(&CMacLayer::S_Idle);
    }

    void S_Idle(Event const* const e)
    {
    	switch (e->Sig)
    	{
		
    	case CMD_OUT_SIG:
		
			DeferEvent(e);
            Request(EventCast(CHdlcEvent)->m_Data, EventCast(CHdlcEvent)->m_DataLen);
			m_Retries = 0;
    		TRANS(&CMacLayer::S_WaitResponse);
    		break;
		
    	case SERIAL_IN_SIG:
			if (IsMe(e))
			{
	    		if (m_Hdlc->Parser(EventCast(CUartEvent)->Data, EventCast(CUartEvent)->DataCount))
				{

				}
			}
    		
    		break;		
    	default:
    		break;
    	}
    }
    void S_WaitResponse(Event const* const e)
    {
    	switch (e->Sig)
    	{
    	case SERIAL_IN_SIG:
			if (IsMe(e))
			{
	    		if (m_Hdlc->Parser(EventCast(CUartEvent)->Data, EventCast(CUartEvent)->DataCount))
				{
					ClearDeferedEvent();
					TRANS(&CMacLayer::S_Idle);
				}
			}
    		break;

		case TIMEOUT_SIG:
			if (m_Retries == 0)
			{
				TRANS(&CMacLayer::S_ReSend);
			}
			else
			{
				TRANS(&CMacLayer::S_Idle);
			}
			
			break;

    	default:
    		break;
    	}
    }

    void S_ReSend(Event const* const e)
    {
        switch (e->Sig)
        {
        case ENTRY_SIG:
            FetchDeferedEvent();
            break;
     	case CMD_OUT_SIG:
		{
            Request(EventCast(CHdlcEvent)->m_Data, EventCast(CHdlcEvent)->m_DataLen);
    		TRANS(&CMacLayer::S_WaitResponse);
    		break;
		}       
        default:
            break;
        }
    }

	inline bool IsMe(Event const* const e)
	{
		return EventCast(CUartEvent)->Device == m_Uart->m_Device;
	}

	void Request(const uint8_t *Data, uint32_t Len)
	{		
		CUartEvent *ue = new CUartEvent(SERIAL_OUT_SIG, m_Uart->m_Device, Len * 2, true);
		m_Hdlc->Packet(Data, Len, ue->Data, &ue->DataCount);
		Publish(ue);
	}

	void GetResponse()
	{

	}

	void Response(const uint8_t* Data, uint16_t Len)
	{
		const uint8_t* response = Data;
		LOG_DEBUG("app get: %s\r\n", response);
		// do your work
	}

	uint32_t m_Retries;
	// lower layer
	CUart *m_Uart;
	CHdlc *m_Hdlc;
	CTimeEvent m_Timer;
	// upper layer


public:
	DEF_STATEMACHINE(CMacLayer);
};
