#pragma once
#include "../acl_cpp_define.hpp"

#if !defined(ACL_CLIENT_ONLY) && !defined(ACL_DB_DISABLE)

namespace acl {

class ACL_CPP_API pgsql_conf
{
public:
	/**
	 * ���캯��
	 * @param dbaddr {const char*} ��������ַ����ַ��ʽΪ��ip:port����
	 *  unix_domain_path����Ϊ unix ���׽ӿ�ʱ��ӦΪ unix ���׽ӿ��ļ�
	 *  ����Ŀ¼�Ҳ������ļ��������� postgresql ���ڼ��� unix ���׽ӿ�
	 *  ���ļ�Ϊ��/tmp/.s.PGSQL.5432���� dbaddr ��ַӦ��Ϊ /tmp
	 *  ע�⣺ע�������� unix ���׽ӿڵ��� mysql �Ĳ�ͬ��mysql �����׽�
	 *  ��Ϊȫ·��
	 * @param dbname {const char*} ���ݿ���
	 */
	pgsql_conf(const char* dbaddr, const char* dbname);

	/**
	 * �������캯��
	 * @param conf {const pgsql_conf&} �ڲ����ᴴ�������ö��󲢿����ò���
	 *  ���������
	 */
	pgsql_conf(const pgsql_conf& conf);

	~pgsql_conf(void);

	/**
	 * �����������ݿ�ʱ���û��˺ţ��������ô˷���ʱ�����˺�
	 * @param dbuser {const char*} �û��˺ţ�Ϊ�ǿ��ַ���ʱ����Ч
	 * @return {pgsql_conf&}
	 */
	pgsql_conf& set_dbuser(const char* dbuser);

	/**
	 * �����������ݿ�ʱ���˺����룬�������ô˷���ʱ��������
	 * @param dbpass {const char*} �˺����룬Ϊ�ǿ��ַ���ʱ����Ч
	 * @return {pgsql_conf&}
	 */
	pgsql_conf& set_dbpass(const char* dbpass);

	/**
	 * �������ݿ����ӳ������������
	 * @param dblimit {size_t} ���ӳ�������������ƣ���Ϊ 0 ʱ������
	 * @return {pgsql_conf&}
	 */
	pgsql_conf& set_dblimit(size_t dblimit);

	/**
	 * �����������ݿ�ĳ�ʱʱ��
	 * @param timeout {int}
	 * @return {pgsql_conf&}
	 */
	pgsql_conf& set_conn_timeout(int timeout);

	/**
	 * ���ö�ȡ���ݿ����ĳ�ʱʱ��
	 * @param timeout {int}
	 * @return {pgsql_conf&}
	 */
	pgsql_conf& set_rw_timeout(int timeout);

	/**
	 * �������ݿ����ӵ��ַ���
	 * @param charset {const char*}
	 * @return {pgsql_conf&}
	 */
	pgsql_conf& set_charset(const char* charset);

	const char* get_dbaddr() const
	{
		return dbaddr_;
	}

	const char* get_dbname() const
	{
		return dbname_;
	}

	const char* get_dbkey() const
	{
		return dbkey_;
	}

	const char* get_dbuser() const
	{
		return dbuser_;
	}

	const char* get_dbpass() const
	{
		return dbpass_;
	}

	size_t get_dblimit() const
	{
		return dblimit_;
	}

	int get_conn_timeout() const
	{
		return conn_timeout_;
	}

	int get_rw_timeout() const
	{
		return rw_timeout_;
	}

	const char* get_charset() const
	{
		return charset_;
	}

private:
	char* dbaddr_;
	char* dbname_;
	char* dbkey_;
	char* dbuser_;
	char* dbpass_;
	char* charset_;
	size_t dblimit_;
	int   conn_timeout_;
	int   rw_timeout_;
};

} // namespace acl

#endif // !defined(ACL_CLIENT_ONLY) && !defined(ACL_DB_DISABLE)
