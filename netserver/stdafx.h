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

#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų���ʹ�õ�����
#include <stdio.h>

#ifndef _WIN32

// ��׼��linux���Ͷ����ļ�.
#include <stdint.h>

// ����VC������.
typedef char CHAR;
typedef unsigned int UINT;

#endif

//////////////////////////////////////////////////////////////////////////
#include "inc.h"
#include "defs.h"

#include "BasePacket.h"
