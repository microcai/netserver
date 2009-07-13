#ifndef BASEPACKET_H__
#define BASEPACKET_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
/*
   ͨ�÷����,����.

class CBasePacket
{
public:
CString m_oriPacket;      // ԭʼ���.
CString m_packetType;     // ����ʶ����.

public:
CBasePacket(void);
virtual ~CBasePacket(void);
bool parsePacket(CString oriPacket);  // �������.
};

// �����.
class Pack_Heart : public CBasePacket
{
public:
CString m_session; // �Ự��Ϣ.

bool parsePacket(CString oriPacket);  // �������.
CString toString();					  // �ϲ����.
};

// ����ǳ������.
class Pack_Logon : public CBasePacket
{
public:
CString m_user;		// �û���.
CString m_pass;		// ����.
CString m_flag;		// ��־����½���ǳ�.
CString m_session;

bool parsePacket(CString oriPacket);	// �����.
CString toString();						// �ϲ����.
};

*/
//////////////////////////////////////////////////////////////////////////

// �����.
typedef struct _tagHeart
{
	UINT type;          // �������.
	UINT packsize;      // ����С.

	UINT session;       // �Ự��־.
}* packHeartPtr, packHeart;

#define packHeartSize sizeof(packHeart)


// ��½��.
typedef struct _tagLogon
{
	UINT type;          // �������.
	UINT packsize;      // ����С.

	UINT session;       // �Ự��־.
	CHAR user[20];		// �û���.
	CHAR pass[20];		// ����.
	UINT flag;			// ��־����½���ǳ�.
}* packLogonPtr, packLogon;

#define packLogonSize sizeof(packLogon)

typedef union _tagMsg
{
	struct _tagMsgHead
	{
		UINT type;										// �������.
		UINT packsize;									// ����С.
	}MsgHead;

	packHeart heart;									// �����.
	packLogon logon;									// ��½��.
} packMsg, *packMsgPtr;


//////////////////////////////////////////////////////////////////////////


#endif // BASEPACKET_H__

