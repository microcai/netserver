#ifndef BASEPACKET_H__
#define BASEPACKET_H__

#if _MSC_VER
#pragma once
#endif


//////////////////////////////////////////////////////////////////////////
/*
// 通用封包类,基类.
class CBasePacket
{
public:
CString m_oriPacket;      // 原始封包.
CString m_packetType;     // 封包标识类型.

public:
CBasePacket(void);
virtual ~CBasePacket(void);
bool parsePacket(CString oriPacket);  // 解析封包.
};

// 心跳包.
class Pack_Heart : public CBasePacket
{
public:
CString m_session; // 会话信息.

bool parsePacket(CString oriPacket);  // 解析封包.
CString toString();					  // 合并封包. 
};

// 登入登出封包类.
class Pack_Logon : public CBasePacket
{
public:
CString m_user;		// 用户名.
CString m_pass;		// 密码.
CString m_flag;		// 标志，登陆，登出.
CString m_session;

bool parsePacket(CString oriPacket);	// 拆解封包.
CString toString();						// 合并封包.
};

*/
//////////////////////////////////////////////////////////////////////////

// 心跳包.
typedef struct _tagHeart
{
	UINT type;          // 封包类型.
	UINT packsize;      // 封包大小.

	UINT session;       // 会话标志.
}* packHeartPtr, packHeart;

#define packHeartSize sizeof(packHeart)


// 登陆包.
typedef struct _tagLogon
{
	UINT type;          // 封包类型.
	UINT packsize;      // 封包大小.

	UINT session;       // 会话标志.
	CHAR user[20];		// 用户名.
	CHAR pass[20];		// 密码.
	UINT flag;			// 标志，登陆，登出.
}* packLogonPtr, packLogon;

#define packLogonSize sizeof(packLogon)

typedef union _tagMsg
{
	struct _tagMsgHead 
	{
		UINT type;										// 封包类型.
		UINT packsize;									// 封包大小.
	}MsgHead;

	packHeart heart;									// 心跳包.
	packLogon logon;									// 登陆包.
} packMsg, *packMsgPtr;


//////////////////////////////////////////////////////////////////////////


#endif // BASEPACKET_H__

