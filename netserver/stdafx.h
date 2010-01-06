/*
 *
 * Copyright (C) 2009 jack.wgm, microcai.
 * For conditions of distribution and use, see copyright notice 
 * in (http://code.google.com/p/netsever/source/browse/trunk/COPYING)
 *
 * Author: jack.wgm
 * Email:  jack.wgm@gmail.com
 */

#if _MSC_VER
#pragma once
#endif

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
#include <stdio.h>

#ifndef _WIN32

// 标准的linux类型定义文件.
#include <stdint.h>

// 兼容VC的类型.
typedef char CHAR;
typedef unsigned int UINT;

#endif

//////////////////////////////////////////////////////////////////////////
#include "inc.h"
#include "defs.h"
#include "protocol.pb.h"