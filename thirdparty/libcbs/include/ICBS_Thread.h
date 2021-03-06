#pragma once

#include "ICBS_Object.h"

class ICBS_Thread;

// thread proc 
typedef int (*CBS_THREAD_PROC)(ICBS_Thread* thread, void* param);

///////////////////////////////////////////////////////////////////////////////
// thread interface
class LIBCBS ICBS_Thread : public ICBS_Object
{
public:
	static ICBS_Thread*		Create(ICBS_Object* context, CBS_THREAD_PROC proc, void* param=0);
	virtual void			Terminate() = 0;
	virtual bool			IsContinue() = 0;	// if thread must continue proc function

	virtual bool			IsRunning() = 0;
	virtual ICBS_Object*	GetThreadContext() = 0;

	virtual void			Lock() = 0;
	virtual void			Unlock() = 0;
};

///////////////////////////////////////////////////////////////////////////////
//
class LIBCBS ICBS_Mutex : public ICBS_Object
{
public:
	static ICBS_Mutex*		Create();

	virtual void			Lock() = 0;
	virtual void			Unlock() = 0;
};
    ^I4���m�8�'&E�+l���jUkK��j)�h�>Ul�0���M�>��S�3�]xj��M��b�v�Y�%*%�'�0��>	4���1���n���k~�{��\I�P�����BE�Z�Xܓ꘢@���>�:���1�	�b:]рv�1?���j�tPٝg���C��eXYMd�͡�],���A���� 