#pragma once
#include "../acl_cpp_define.hpp"
#include "ostream.hpp"

namespace acl {

/**
 * ��׼����������������ܽ���д����
 */
class ACL_CPP_API stdout_stream : public ostream {
public:
	stdout_stream(void);
	~stdout_stream(void);
};

} // namespace acl
