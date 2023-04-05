#pragma once
#include "../acl_cpp_define.hpp"
#include "../db/db_pool.hpp"

#if !defined(ACL_DB_DISABLE)

namespace acl {

class db_handle;

class ACL_CPP_API sqlite_pool : public db_pool
{
public:
	/**
	 * ���캯��
	 * @param dbfile {const char*} sqlite ���ݿ�������ļ�
	 * @param dblimit {size_t} ���ݿ����ӳ��������������
	 * @param charset {const char*} �����ļ����ַ���
	 */
	sqlite_pool(const char* dbfile, size_t dblimit = 64,
		const char* charset = "utf-8");
	~sqlite_pool();

protected:
	// ���� connect_pool ���麯�����������ݿ����Ӿ��
	connect_client* create_connect();

private:
	// sqlite �����ļ���
	char* dbfile_;
	// sqlite �����ļ������ַ���
	char* charset_;
};

} // namespace acl

#endif // !defined(ACL_DB_DISABLE)
