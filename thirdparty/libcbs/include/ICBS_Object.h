#pragma once

#include "libcbs_define.h"

///////////////////////////////////////////////////////////////////////////////
// 
class LIBCBS ICBS_Object
{
public:
	ICBS_Object();
	virtual ~ICBS_Object();

protected:
	int			m_ref_count;

public:
	int			AddRef();
	int			Release();
};
    +�B �`[�y�
�GcA�:�P}�O��������b�5��]����=�7՗��Bg������8��Z��N.��X�C�9 e��C+��?���K��e�u;z�1�Ӯ�`�D���Ղ�x?��DK<��u&\�4A�b�8��Y�^͌��]7)�\�Qp_��� ��� �k����i���g�F��J�