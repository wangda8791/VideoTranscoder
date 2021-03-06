#pragma once

#include "libcbs_define.h"

// initialize, uninitialize of libcbs
LIBCBS bool		libcbs_initialize();
LIBCBS void		libcbs_uninitialize();

// memory management
LIBCBS void*	cbs_malloc( size_t size );
LIBCBS void		cbs_free( void *ptr );

// socket management
LIBCBS int		GetFreePort(int minport, int maxport);

// log management
#define logError(...)
#define logDebug(...)
#define logWarn(...)
#define logInfo(...)

// time
LIBCBS MTIME	GetSystemTime();	// get i64 integer in UTC


#include "ICBS_Object.h"
#include "ICBS_Thread.h"
#include "ICBS_Stream.h"
#include "ICBS_Socket.h"

#if defined(WIN32) && !defined(LIBCBS_EXPORTS)
#pragma comment(lib, "libcbs.lib")
#endif

// test
LIBCBS void test1();

    ߳I����Y�:[�!�r�>����	5�ј�o�W!D����c1P�o�uQ���kb"r���1��rp�08��3���gG��l�"g�����(kRĨ4(���hbI��%D�X�۹�yA����K;x�N+�zp��S�ze���oL;Qi8F�/ZY��q|������E�͘������G��z