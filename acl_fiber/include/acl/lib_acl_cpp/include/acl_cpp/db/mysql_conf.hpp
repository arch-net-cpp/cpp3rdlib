#pragma once
#include "../acl_cpp_define.hpp"

#if !defined(ACL_CLIENT_ONLY) && !defined(ACL_DB_DISABLE)

namespace acl {

class ACL_CPP_API mysql_conf
{
public:
	/**
	 * ���캯��
	 * @param dbaddr {const char*} ���ݿ����ӵ�ַ������Ϊ TCP �׽ӿڻ�
	 *  UNIX ���׽ӿڣ���Ϊ TCP �׽ӿ�ʱ����ַ��ʽΪ��ip:port, ��Ϊ UNIX
	 *  ���׽ӿ�ʱ����ַ��ʽ��/xxx/xxx/xxx.sock
	 * @param dbname {const char*} ���ݿ���
	 */
	mysql_conf(const char* dbaddr, const char* dbname);

	/**
	 * �������캯��
	 * @param conf {const mysql_conf&} �ڲ������¶���ͬʱ���������������
	 */
	mysql_conf(const mysql_conf& conf);

	~mysql_conf();

	/**
	 * �����������ݿ�ʱ���û��˺�
	 * @param dbuser {const char*} ��Ϊ�ǿ��ַ���ʱָ���û��˺�
	 * @return {mysql_conf&}
	 */
	mysql_conf& set_dbuser(const char* dbuser);

	/**
	 * �����������ݿ�ʱ���˺�����
	 * @param dbpass {const char*} ��Ϊ�ǿ��ַ���ʱָ���˺�����
	 * @return {mysql_conf&}
	 */
	mysql_conf& set_dbpass(const char* dbpass);

	/**
	 * �������ݿ����ӳ������������
	 * @param dblimit {size_t} ���ӳ�������������ƣ���Ϊ 0 ʱ������
	 * @return {mysql_conf&}
	 */
	mysql_conf& set_dblimit(size_t dblimit);

	/**
	 * ���� mysql ���ݿ��һЩ�����־λ
	 * @param dbflags {unsigned long}
	 * @return {mysql_conf&}
	 */
	mysql_conf& set_dbflags(unsigned long dbflags);

	/**
	 * ���õ��޸����ݿ�����ʱ�Ƿ������Զ��ύ��Ĭ��Ϊ�Զ��ύ
	 * @param on {bool}
	 * @return {mysql_conf&}
	 */
	mysql_conf& set_auto_commit(bool on);

	/**
	 * �����������ݿ�ĳ�ʱʱ��
	 * @param timeout {int}
	 * @return {mysql_conf&}
	 */
	mysql_conf& set_conn_timeout(int timeout);

	/**
	 * ���ö�ȡ���ݿ����ĳ�ʱʱ��
	 * @param timeout {int}
	 * @return {mysql_conf&}
	 */
	mysql_conf& set_rw_timeout(int timeout);

	/**
	 * �������ݿ����ӵ��ַ���
	 * @param charset {const char*}
	 * @return {mysql_conf&}
	 */
	mysql_conf& set_charset(const char* charset);

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

	unsigned long get_dbflags() const
	{
		return dbflags_;
	}

	bool get_auto_commit() const
	{
		return auto_commit_;
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
	char* dbaddr_;          // ���ݿ������ַ
	char* dbname_;          // ���ݿ���
	char* dbkey_;           // dbname@dbaddr
	char* dbuser_;          // ���ݿ��˺�
	char* dbpass_;          // ���ݿ��˺�����
	char* charset_;         // �������ݿ�ʱ���ַ���
	size_t dblimit_;        // ���ݿ����ӳ�����������
	unsigned long dbflags_; // �����ݿ�ʱ�ı�־λ
	bool  auto_commit_;     // �Ƿ��Զ��ύ�޸ĺ������
	int   conn_timeout_;    // �������ݿ�ĳ�ʱʱ��
	int   rw_timeout_;      // �����ݿ�ͨ�ŵĳ�ʱʱ��
};

} // namespace acl

#endif // !defined(ACL_CLIENT_ONLY) && !defined(ACL_DB_DISABLE)
