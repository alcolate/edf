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
#include "Hdlc.h"

CHdlc::CHdlc(CMacLayer* MacLayer) : m_MacLayer(MacLayer)
{
    m_ControlSend.seq_no = 0;
    m_ControlRecv.seq_no = 0;
}

void CHdlc::Ack(uint8_t SeqNo)
{
    yahdlc_control_t Control;

    Control.seq_no = SeqNo;
    Control.seq_no++;
    Control.frame = YAHDLC_FRAME_ACK;

    CSerialEvent *ue = new CSerialEvent(MAC_REQ_SIG, m_MacLayer->m_Uart->m_Device, 10);

    yahdlc_frame_data(&Control, NULL, 0, (char *)ue->m_Data, &ue->m_DataCount);

    Publish(ue);
}

void CHdlc::PacketData(const uint8_t* Data, uint16_t Len)
{
    CSerialEvent* ue = new CSerialEvent(MAC_REQ_SIG, m_MacLayer->m_Uart->m_Device, Len * 2, true);

    m_ControlSend.frame = YAHDLC_FRAME_DATA;
    m_ControlSend.seq_no++;
    yahdlc_frame_data(&m_ControlSend, (char*)Data, Len, (char*)ue->m_Data, &ue->m_DataCount);

    Publish(ue);
}

bool CHdlc::Parser(const uint8_t* Data, uint32_t Len)
{
    int ret;

    yahdlc_control_t RcvControl;

    CMacEvent* he = new CMacEvent(MAC_RSP_SIG, CMacEvent::GET_DATA, Data, Len);

    ret = yahdlc_get_data(&RcvControl, (char*)Data, Len, (char*)he->m_Data, &he->m_DataLen);

    if (