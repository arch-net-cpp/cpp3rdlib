#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/string.hpp"
#include "../db/db_handle.hpp"

#if !defined(ACL_CLIENT_ONLY) && !defined(ACL_DB_DISABLE)

typedef struct st_mysql MYSQL;

namespace acl {

class mysql_conf;

class ACL_CPP_API db_mysql : public db_handle
{
public:
	/**
	 * ���캯����ʽһ
	 * @param dbaddr {const char*} ���ݿ������ַ������Ϊ TCP �׽ӿڻ��� UNIX
	 *  ƽ̨�µ����׽ӿڣ���ʽ�磺127.0.0.1:3306���� /tmp/mysql.sock
	 * @param dbname {const char*} ���ݿ����ƣ��� NULL
	 * @param dbuser {const char*} �������ݿ�ʱ���û���
	 * @param dbpass {const char*} �������ݿ�ʱ���û�����
	 * @param dbflags {unsigned long} ���� MYSQL ʱ�ı�־λ
	 * @param auto_commit {bool} �������ݿ�����޸�ʱ�Ƿ��Զ��ύ����
	 * @param conn_timeout {int} �������ݿ�ĳ�ʱʱ�䣨�룩
	 * @param rw_timeout {int} �������ݿ����ʱ�ĳ�ʱʱ�䣨�룩
	 * @param charset {const char*} �������ݿ�ʱ�ı����ַ�����gbk, utf8, ...��
	 */
	db_mysql(const char* dbaddr, const char* dbname,
		const char* dbuser, const char* dbpass,
		unsigned long dbflags = 0, bool auto_commit = true,
		int conn_timeout = 60, int rw_timeout = 60,
		const char* charset = "utf8");

	/**
	 * ���캯����ʽ����ʹ�ò��������������й���
	 * @param conf {const mysql_conf&} mysql ���ݿ��������������
	 */
	db_mysql(const mysql_conf& conf);
	~db_mysql(void);

	/**
	 * ��� mysql �ͻ��˿�İ汾��
	 * @return {unsigned long}
	 */
	unsigned long mysql_libversion(void) const;

	/**
	 * ��� mysql �ͻ��˿����Ϣ
	 * @return {const char*}
	 */
	const char* mysql_client_info(void) const;

	/**
	 * ֱ�ӻ�� mysql �����Ӿ����������� NULL ���ʾ mysql ��û�д�
	 * �����ʱ�ڲ��Զ��ر��� mysql ����
	 * @return {MYSQL*}
	 */
	MYSQL* get_conn(void) const
	{
		return conn_;
	}

	/**
	 * ����̬���� libmysqlclient.so / libmysqlclient.dll ʱ�����Ե��ñ�
	 * ��̬������ʽ��̬���� mysql �ͻ��˿⣬�������ʧ�ܣ��ڲ����Զ�����
	 * ���ԣ���������ʱ����Ҳ�ɲ����ñ�������ʹ db_mysql ������ڲ���
	 * ʹ��ʱ��ʽ���� mysql ��̬��
	 */
	static void load(void);

	/********************************************************************/
	/*         ����Ϊ���� db_handle ����ӿ�                            */
	/********************************************************************/

	/**
	 * @override
	 */
	const char* dbtype(void) const;

	/**
	 * @override
	 */
	int get_errno(void) const;

	/**
	 * @override
	 */
	const char* get_error(void) const;

	/**
	 * @override
	 */
	bool dbopen(const char* charset = NULL);

	/**
	 * @override
	 */
	bool is_opened(void) const;

	/**
	 * @override
	 */
	bool close(void);

	/**
	 * @override
	 */
	bool tbl_exists(const char* tbl_name);

	/**
	 * @override
	 */
	bool sql_select(const char* sql, db_rows* result = NULL);

	/**
	 * @override
	 */
	bool sql_update(const char* sql);

	/**
	 * @override
	 */
	int affect_count(void) const;

	/**
	 * @override
	 * ���� db_handle ���麯����������ʾ����Ŀ�ʼ��ע����Ҫʹ������ʽ��
	 * ����Ҫ�� db_mysql �Ĺ��캯���д���Ĳ��� auto_commit Ϊ false
	 */
	bool begin_transaction(void);

	/**
	 * @override
	 */
	bool commit(void);

	/**
	 * @override
	 */
	bool rollback(void);

private:
	char*  dbaddr_;  // ���ݿ������ַ
	char*  dbname_;  // ���ݿ���
	char*  dbuser_;  // ���ݿ��˺�
	char*  dbpass_;  // ���ݿ��˺�����
	string charset_; // �������ݿ���õ��ַ���

	unsigned long dbflags_;
	int    conn_timeout_;
	int    rw_timeout_;
	bool   auto_commit_;
	MYSQL* conn_;

	bool sane_mysql_query(const char* sql);
	void sane_mysql_init(const char* dbaddr, const char* dbname,
		const char* dbuser, const char* dbpass,
		unsigned long dbflags, bool auto_commit,
		int conn_timeout, int rw_timeout,
		const char* charset);
};

} // namespace acl

#endif // !defined(ACL_CLIENT_ONLY) && !defined(ACL_DB_DISABLE)
