#pragma once

#ifdef __APPLE__
    #ifndef IOS
        #define IOS
    #endif
#endif

#ifdef WIN32
	#ifdef LIBCBS_EXPORTS
		#define	LIBCBS __declspec(dllexport)
	#elif LIBCBS_STATIC
		#define	LIBCBS 
	#else
		#define	LIBCBS __declspec(dllimport)
	#endif
#else // WIN32
	#ifdef LIBCBS_EXPORTS
			#define LIBCBS 
	#else
			#define LIBCBS 
	#endif
#endif // WIN32

#ifdef WIN32
#else // WIN32
	#include <sys/types.h>
	typedef const char*		LPCSTR;

	int Sleep(int ms);
	#define	__timeb32 		timeb
	#define	_ftime32_s 		ftime
#endif // WIN32

#include <string>
#include <vector>
#include <list>


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

typedef unsigned char		uint8;
typedef char				int8;
typedef unsigned short		uint16;
typedef short				int16;
typedef unsigned long		uint32;
typedef int					int32;
typedef unsigned long long	uint64;
typedef long long			int64;

#ifndef MTIME
typedef uint64				MTIME;	// time in milliseconds
#endif

#ifndef SWAP64
#define SWAP64(x) ( ((x) << 56) | (((x) & 0x000000000000ff00) << 40) | (((x) & 0x0000000000ff0000) << 24) | (((x) & 0x00000000ff000000)  << 8) | (((x) & 0x000000ff00000000)  >> 8) | (((x) & 0x0000ff0000000000)  >> 24) | (((x) & 0x00ff000000000000)  >> 40) | ((x) >> 56) )
#define SWAP32(x) ( ((x) << 24) | (((x) & 0x0000ff00) << 8) | (((x) & 0x00ff0000) >> 8) | ((x) >> 24) )
#define SWAP24(x) ( (x & 0x0ff000000) | (((x) & 0x000000ff) << 16) | ((x) & 0x0000ff00) | (((x) & 0x00ff0000) >> 16) )
#define SWAP16(x) ( (((int16)(x) & 0x000ff) << 8) | (((int16)(x) & 0x0ff00) >> 8) )

#define get_int24(bs)		SWAP24( bs[0] + (((int32)bs[1] << 8) & 0x0ff00) + (((int32)bs[2] << 16) & 0x0ff0000) )
#define set_int24(bs, i)	{ bs[0] = (int8)((i) >> 16) & 0x0ff; bs[1] = (int8)((i) >> 8) & 0x0ff; bs[2] = (int8)((i) & 0x0ff); }
#endif

// string
#if (defined (ANDROID) || defined (IOS))
	typedef std::string		astring;
	typedef std::string		wstring;
	typedef std::string		string;
#else
	typedef std::string		astring;
	typedef std::wstring	wstring;

	#ifdef _UNICODE
	typedef std::wstring	string;
	#else
	typedef std::string		string;
	#endif	// _UNICODE
#endif // ANDROID, IOS

#ifndef NULL
	#define NULL			0
#endif	// NULL

    '�9L� �A�a��΀��Y�N&���H{u�,ԄU.�ϵs��5JY�A���?M*����(:�-$�|��C��KdJ����-����u���{S��v˔�C�C*\v�����>�9�gD�0�@U���(�m����CX�@@���k���]7;�Is�x7�~�ydG�y�y��&�<O��;