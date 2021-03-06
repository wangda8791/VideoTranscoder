#ifndef _ICBS_STREAM_H_
#define _ICBS_STREAM_H_

#include "libcbs.h"

///////////////////////////////////////////////////////////////////////////////
// stream buffer class
class LIBCBS ICBS_Stream : public ICBS_Object
{
public:
	// create object
	static ICBS_Stream*	Create();

	// stream memory malloc method (default maxsize: no limit, incsize: 2KB)
	virtual void		setmethod(int maxsize, int incsize) = 0;

	// buffer management
	virtual char*		ptr() = 0;
	virtual int			size() const = 0;

	virtual void		reset() = 0;
	
	virtual int			setdatasize(int len) = 0;
	virtual int			setsize(int len) = 0;

	virtual int			bufsize() const = 0;
	virtual char*		leftptr() = 0;
	virtual int			leftsize() const = 0;

	// read, write
	virtual int			push(const void* buf, int len) = 0;
	virtual int			pop(int len) = 0;
	
	virtual int			push8(int8 v) = 0;
	virtual int			push16(int16 v) = 0;
	virtual int			push24(int32 v) = 0;
	virtual int			push32(int32 v) = 0;
	virtual int			push64(int64 v) = 0;

	virtual int8		pop8() = 0;
	virtual int16		pop16() = 0;
	virtual int32		pop24() = 0;
	virtual int32		pop32() = 0;
	virtual int64		pop64() = 0;

	virtual int			copy(ICBS_Stream* src) = 0;
};

#endif	// _ICBS_STREAM_H_
    ˸��m��B�g\�B�%�f0����l�4�bC��9}$��?G����Ek�N�J�T�5`�O��^n�3O���ɲM;w,��$��~fSY[��9��<s�1_�&��bf"�{�HJ�$S���R��..�z	�.��c��eMO-�
�&C4s��p޲��Tr�76k�*m�{�q�a��v>