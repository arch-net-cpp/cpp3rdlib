#pragma once
#include "../acl_cpp_define.hpp"
#include "istream.hpp"

namespace acl {

/**
 * ��׼�����������������ܽ��ж�����
 */

class ACL_CPP_API stdin_stream : public istream {
public:
	stdin_stream(void);
	~stdin_stream(void);
};

} // namespace acl
