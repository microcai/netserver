/*
 *
 * Copyright (C) 2009 jack.wgm, microcai.
 * For conditions of distribution and use, see copyright notice 
 * in (http://code.google.com/p/netsever/source/browse/trunk/COPYING)
 *
 * Author: jack.wgm
 * Email:  jack.wgm@gmail.com
 */

#ifndef BASEPACKET_H__
#define BASEPACKET_H__

#if _MSC_VER
#pragma once
#endif

// 这里,保存了该对齐方式,1字节对齐.
#pragma pack(push, 1)

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
		UINT checksum;									// 校检和,为其它三项之和. e.c type + rand + packsize.
		UINT rand;										// 随机数.
		UINT packsize;									// 封包大小.
	}MsgHead;

	packHeart heart;									// 心跳包.
	packLogon logon;									// 登陆包.
} packMsg, *packMsgPtr;

#define packHeadSize sizeof(MsgHead)

// 如果保存了对齐方式,用这个.
#pragma pack(pop)

#endif // BASEPACKET_H__

