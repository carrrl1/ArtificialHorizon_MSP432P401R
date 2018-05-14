#include "Mailbox.hpp"

// - Mailbox constructor
Mailbox::Mailbox()
{
    for(int i = 0; i < MAILBOX_SLOTS; i++)
    {
        m_aInbox[i] = (uintptr_t) 0; // Init to an invalid pointer
    }
    return;
}
// - The send function, inserts a message into the inbox slots.
uint8_t Mailbox::SendMessage(st_Message * i_stMessage)
{
    uint8_t l_ErrorCode = NO_ERR;

    uint8_t l_u8Receiver = i_stMessage->u8Receiver;
    bool l_bReaded = false;

    if (m_aInbox[l_u8Receiver] != (uintptr_t) 0)
    {
        l_bReaded = m_aInbox[l_u8Receiver]->bReaded;
        if (l_bReaded)
        {
            delete m_aInbox[l_u8Receiver];
        }
    } 
    else 
    {
        l_bReaded = true;
    }

    if(l_bReaded)
    {
        m_aInbox[l_u8Receiver] =  i_stMessage;
    }
    else
    {
        l_ErrorCode = RET_ERR;
    }
    return l_ErrorCode;
}

// - The get function, get a message from the inbox slots usign the receiver ID.
st_Message * Mailbox::GetMessage(uint8_t i_u8Receiver)
{
    m_aInbox[i_u8Receiver]->bReaded=true;
    return m_aInbox[i_u8Receiver];
}
