/*
 *
 * Copyright (C) 2009 jack.wgm, microcai.
 * For conditions of distribution and use, see copyright notice 
 * in (http://code.google.com/p/netsever/source/browse/trunk/COPYING)
 *
 * Author: jack.wgm
 * Email:  jack.wgm@gmail.com
 */

#ifndef CONFIG_H__
#define CONFIG_H__

#if _MSC_VER
#pragma once
#endif

//////////////////////////////////////////////////////////////////////////
// 编译配置选项.

// 使用ssl加密认证通信.
#define SOCKET_SSL

// 是否使用系统同步接收数据.
#define USE_SYNC

#endif // CONFIG_H__