#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/noncopyable.hpp"

struct iovec;

namespace acl
{

class socket_stream;

/**
 * tcp ipc ͨ�ŷ����࣬�ڲ��Զ����
 */
class ACL_CPP_API tcp_sender : public noncopyable
{
public:
	tcp_sender(socket_stream& conn);
	~tcp_sender(void);

	/**
	 * ���ͷ���
	 * @param data {const void*} Ҫ���͵����ݰ���ַ
	 * @param len {unsigned int} ���ݰ�����
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool send(const void* data, unsigned int len);

	/**
	 * �������������
	 * @return {acl::socket_stream&}
	 */
	acl::socket_stream& get_conn(void) const
	{
		return *conn_;
	}

private:
	acl::socket_stream* conn_;
	struct iovec* v2_;
};

} // namespace acl
