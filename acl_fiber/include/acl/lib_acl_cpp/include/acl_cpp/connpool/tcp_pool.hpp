#pragma once
#include "../acl_cpp_define.hpp"
#include "connect_pool.hpp"

namespace acl
{

class string;
class connect_client;

class ACL_CPP_API tcp_pool : public connect_pool
{
public:
	tcp_pool(const char* addr, size_t count, size_t idx = 0);
	virtual ~tcp_pool(void);

	/**
	 * �����������ָ�����ȵ����ݰ����÷������Զ������ӳػ�ȡ���ӽ��з���
	 * @param data {const void*} Ҫ���͵����ݰ���ַ
	 * @param len {unsigned int} ���ݳ���
	 * @param out {string*} ���ö���� NULL ʱ������Ҫ�ӷ�������ȡ��Ӧ���ݣ�
	 *  ��Ӧ�����������ڸû������У�����ö���Ϊ NULL�����ʾ�����ȡ
	 *  ����������Ӧ����
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool send(const void* data, unsigned int len, string* out = NULL);

protected:
	// @override
	virtual connect_client* create_connect(void);
};

} // namespace acl
