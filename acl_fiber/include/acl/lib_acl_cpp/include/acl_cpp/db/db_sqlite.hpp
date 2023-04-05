#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/string.hpp"
#include "../db/db_handle.hpp"

#if !defined(ACL_DB_DISABLE)

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

namespace acl {

class charset_conv;
class sqlite_cursor;

class ACL_CPP_API db_sqlite : public db_handle
{
public:
	/**
	 * ���캯��
	 * @param charset {const char*} �����ַ���(gbk, utf-8, ...)
	 */
	db_sqlite(const char* dbfile, const char* charset = "utf-8");
	~db_sqlite(void);

	/**
	 * ���ص�ǰ�� sqlite �İ汾��Ϣ
	 * @return {const char*}
	 */
	const char* version(void) const;

	/**
	 * �����ݿ�򿪺�ͨ���˺��������ݿ�Ĳ�������������ã�
	 * �������õ�������Ҫ�ϸ���ѭ sqlite ���������ѡ��Ҫ��
	 * @param pragma {const char*} ����ѡ�����ݣ���ʽΪ��
	 *  PRAGMA xxx=xxx
	 *  �磺PRAGMA synchronous = NORMAL
	 * @return {bool} �������ݿ��Ƿ�ɹ�
	 */
	bool set_conf(const char* pragma);

	/**
	 * �����ݿ�򿪵��ô˺�������������������ѡ��
	 * @param pragma {const char*} ����ѡ�����ݣ���ʽΪ��
	 *  PRAGMA xxx
	 *  �磺PRAGMA synchronous
	 * @param out {string&} �������ֵ�ǿ���洢���
	 * @return {const char*} Ϊ����˵�������ò����ڻ����ݿ�δ��
	 */
	const char* get_conf(const char* pragma, string& out);

	/**
	 * �����ݿ�򿪵�������������ݿ����������ѡ��
	 * @param pragma {const char*} ָ��������ѡ�����ò���Ϊ�գ�
	 *  ��������е�����ѡ���ʽΪ��PRAGMA xxx���磺PRAGMA synchronous
	 */
	void show_conf(const char* pragma = NULL);

	/**
	 * �����ݿ�򿪺����е�Ӱ��ļ�¼����
	 * @return {int} Ӱ���������-1 ��ʾ����
	 */
	int affect_total_count(void) const;

	/**
	 * ֱ�ӻ�� sqlite �ľ����������� NULL ���ʾ sqlite ��û�д�
	 * �����ʱ�ڲ��Զ��ر��� sqlite
	 * @return {sqlite3*}
	 */
	sqlite3* get_conn(void) const
	{
		return db_;
	}

	/**
	 * ׼���α�
	 * @param cursor {sqlite_cursor&}
	 * @return {bool}
	 */
	bool prepare(sqlite_cursor& cursor);

	/**
	 * ִ����һ��������ǲ�ѯ����̣��򽫲�ѯ�����������Ĳ�����
	 * @param cursor {sqlite_cursor&}
	 * @return {bool}
	 */
	bool next(sqlite_cursor& cursor, bool* done);

	/********************************************************************/
	/*            ����ΪһЩ sqlite3 ��˽�нӿ�                         */
	/********************************************************************/

	/**
	 * ��zSql��ʼ��Ϊ prepared statement
	 * @param zSql {const char*} utf-8�����sql
	 * @param nByte {int} zSql������ֽڳ���
	 * @param ppStmt {sqlite3_stmt**} OUT: prepared statement���
	 * @param pzTail {const char**} OUT: ָ��zSqlδʹ�ò��ֵ�ָ��
	 * @return {int} �ɹ����� SQLITE_OK�����򷵻���Ӧ�Ĵ������
	 */
	int sqlite3_prepare_v2(const char *zSql,
		int nByte, sqlite3_stmt **ppStmt, const char **pzTail);

	/**
	 * ���� prepared statement
	 * @param stmt {sqlite3_stmt*} prepared statement
	 * @return {int} ���� SQLITE_BUSY, SQLITE_DONE, SQLITE_ROW,
	 *          SQLITE_ERROR, �� SQLITE_MISUSE
	 */
	int sqlite3_step(sqlite3_stmt *stmt);

	/**
	 * ��prepared statement����Ϊ��ʼ��״̬
	 * @param pStmt {sqlite3_stmt*} prepared statement
	 * @return {int} SQLITE_ROW, SQLITE_DONE �� SQLITE_OK
	 */
	int sqlite3_reset(sqlite3_stmt *pStmt);

	/**
	 * �ͷ� prepared statement ��Դ
	 * @param stmt {sqlite3_stmt*} prepared statement���
	 * @return {int} SQLITE_OK �������������
	 */
	int sqlite3_finalize(sqlite3_stmt *stmt);

	/**
	 * �󶨶���������
	 * @param stmt {sqlite3*} prepared statement
	 * @param iCol {int} ���󶨵�sql�еĲ�������
	 * @param value {const void*} ���󶨵�sql�еĲ�����ֵ
	 * @param n {int} �������ֽڳ���
	 * @param destory {void(*)(void*)} �����������������
	 * @return {int} �ɹ����� SQLITE_OK�����򷵻���Ӧ�Ĵ������
	 */
	int sqlite3_bind_blob(sqlite3_stmt *stmt, int iCol,
		const void *value, int n, void(*destory)(void*));

	/**
	 * ��int��������
	 * @param stmt {sqlite3*} prepared statement
	 * @param iCol {int} ���󶨵�sql�еĲ�������
	 * @param value {int} ���󶨵�sql�еĲ�����ֵ
	 * @return {int} �ɹ����� SQLITE_OK�����򷵻���Ӧ�Ĵ������
	 */
	int sqlite3_bind_int(sqlite3_stmt *stmt, int iCol, int value);

	/**
	 * ��int64����
	 * @param stmt {sqlite3*} prepared statement
	 * @param iCol {int} ���󶨵�sql�еĲ�������
	 * @param value {long long int} ���󶨵�sql�еĲ�����ֵ
	 * @return {int} �ɹ����� SQLITE_OK�����򷵻���Ӧ�Ĵ������
	 */
	int sqlite3_bind_int64(sqlite3_stmt* stmt, int iCol, long long int value);

	/**
	 * ��text����
	 * @param stmt {sqlite3*} prepared statement
	 * @param iCol {int} ���󶨵�sql�еĲ�������
	 * @param value {const void*} ���󶨵�sql�еĲ�����ֵ
	 * @param n {int} �������ֽڳ���
	 * @param destory {void(*)(void*)} �����������������
	 * @return {int} �ɹ����� SQLITE_OK�����򷵻���Ӧ�Ĵ������
	 */
	int sqlite3_bind_text(sqlite3_stmt *stmt, int iCol,
		const char *value, int n, void(*destory)(void*));

	/**
	 * ���� prepared statement �����������
	 * @param stmt {sqlite3_stmt*} prepared statement
	 * @return {int} ������
	 */
	int sqlite3_column_count(sqlite3_stmt *stmt);

	/**
	 * ���ز�ѯ����Ķ�Ӧ�еĶ����ƽ����Ϣ
	 * @param stmt {sqlite3_stmt*} prepared statement
	 * @param iCol {int} ������
	 * @return {const void*} ����ָ��
	 */
	const void *sqlite3_column_blob(sqlite3_stmt *stmt, int iCol);

	/**
	 * ���ز�ѯ����Ķ�Ӧ�е�int�����Ϣ
	 * @param stmt {sqlite3_stmt*} prepared statement
	 * @param iCol {int} ������
	 * @return {int} ����
	 */
	int sqlite3_column_int(sqlite3_stmt *stmt, int iCol);

	/**
	 * ���ز�ѯ����Ķ�Ӧ�е�int64�����Ϣ
	 * @param stmt {sqlite3_stmt*} prepared statement
	 * @param iCol {int} ������
	 * @return {long long int} ����
	 */
	long long int sqlite3_column_int64(sqlite3_stmt *stmt, int iCol);

	/**
	 * ���ز�ѯ����Ķ�Ӧ�е� utf-8 text �����Ϣ
	 * @param stmt {sqlite3_stmt*} prepared statement
	 * @param iCol {int} ������
	 * @return {const unsigned char *} ����ָ��
	 */
	const unsigned char *sqlite3_column_text(sqlite3_stmt *stmt, int iCol);

	/**
	 * ���ز�ѯ����Ķ�Ӧ�еĽ����Ϣ�����ֽڳ���
	 * @param stmt {sqlite3_stmt*} prepared statement
	 * @param iCol {int} ������
	 * @return {const unsigned char *} ����ָ��
	 */
	int sqlite3_column_bytes(sqlite3_stmt *stmt, int iCol);

	/**
	 * ����select��������ض��е�����
	 * @param stmt {sqlite3_stmt*} prepared statement
	 * @param iCol {int} ������
	 * @return {const char*} ����
	 */
	const char *sqlite3_column_name(sqlite3_stmt *stmt, int iCol);

	/**
	 * ִ�е���sql���
	 * @param sql {const char*} ��ִ�е�sql���
	 * @param callback {int (*)(void*,int,char**,char**)} callback����
	 * @param arg {void*}callback�����ĵ�һ������
	 * @param errmsg {char**} ������Ϣ
	 * @return {int} SQLITE_OK ������������
	 */
	int sqlite3_exec(const char *sql,
		int(*callback)(void*,int,char**,char**), void *arg, char **errmsg);

	/**
	 * Ϊ�ͷ� errmsg ����ӵĽӿ�
	 * @param ptr {void*} ���ͷ�����ָ��
	 */
	void sqlite3_free(void* ptr);

	/********************************************************************/
	/*            ����Ϊ���� db_handle ����ӿ�                         */
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
	 */
	bool begin_transaction(void);

	/**
	 * @override
	 */
	bool commit(void);

	/**
	 * @override
	 */
	bool set_busy_timeout(int nMillisecs);


private:
	// sqlite ����
	sqlite3* db_;

	// ���ݴ洢�ļ�
	string dbfile_;

	// �ַ���ת����
	charset_conv* conv_;

	// �����ַ���
	string charset_;

	// ����ִ��SQL��ѯ�ĺ���
	bool exec_sql(const char* sql, db_rows* result = NULL);
};

} // namespace acl

#endif // !defined(ACL_DB_DISABLE)
