#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/string.hpp"
#include "db_cursor.hpp"

#if !defined(ACL_DB_DISABLE)

struct sqlite3_stmt;

namespace acl
{

class db_row;
class db_sqlite;
class dbuf_guard;

class sqlite_cursor : public db_cursor
{
public:
	/**
	 * ���췽��
	 * @param q {query&} SQL ��ѯ�����ڹ��췽���ڻ����Ƚ���תΪ Sql �ַ���
	 */
	sqlite_cursor(query& q);
	~sqlite_cursor(void);

	/**
	 * ��ò�ѯ SQL ���
	 * @return {const string&}
	 */
	const string& get_sql(void) const
	{
		return sql_;
	}

	/**
	 * �ڱ�����ѯ�ṹ��ʱ��ÿ�β�ѯ���ͨ����������ý����
	 * @return {db_row*}
	 */
	db_row* get_row(void) const
	{
		return row_;
	}

	/**
	 * �� db_sqlite ���������ʼ�� names_ �ֶ���
	 * @param name {const char*} ���ݱ�����
	 */
	void add_column_name(const char* name);
	
	/**
	 * �����ֵ
	 * @param n {long long}
	 */
	void add_column_value(long long n);

	/**
	 * �����ֵ
	 * @param n {double}
	 */
	void add_column_value(double n);

	/**
	 * �����ֵ
	 * @param s {cont char*} �ò��������������� stmt_ ����
	 */
	void add_column_value(const char* s);

	/**
	 * �����м�¼����������Ų�ѯ�����
	 */
	void create_row(void);

	/**
	 * �ڱ��������У�db_sqlite::next ���������ȵ��ñ���������ϴεĲ�ѯ���
	 */
	void clear(void);

private:
	friend class db_sqlite;

	typedef int (*free_sqlite3_stmt_fn)(sqlite3_stmt*);

	string sql_;
	sqlite3_stmt* stmt_;
	free_sqlite3_stmt_fn free_callback;

	// ���ݱ��ֶ���
	std::vector<const char*> names_;

	dbuf_guard* dbuf_;	// �ڴ������
	db_row* row_;		// ���ڲ�ѯ�����ԣ������洢���
};

}

#endif // !defined(ACL_DB_DISABLE)
