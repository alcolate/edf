#pragma once


#include "Hdlc.h"




void CHdlc::Ack(yahdlc_control_t *Control)
{
    CUartEvent *ue = new CUartEvent(SERIAL_OUT_SIG, m_MacLayer->m_Uart->m_Device, 10);

    yahdlc_frame_data(Control, NULL, 0, (char *)ue->Data, &ue->DataCount);

    Publish(ue);
}

 

