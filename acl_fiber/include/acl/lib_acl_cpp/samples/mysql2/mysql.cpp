// mysql.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

// ��ѯ������
static int tbl_select(acl::db_handle& db)
{
	const char* sql = "select value, category, type from black_white_list";

	if (db.sql_select(sql) == false)
	{
		printf("select sql error\r\n");
		return (-1);
	}

	printf("\r\n---------------------------------------------------\r\n");

	const acl::db_rows* result = db.get_result();
	if (result)
	{
		const std::vector<acl::db_row*>& rows = result->get_rows();
		for (size_t i = 0; i < rows.size(); i++)
		{
			const acl::db_row* row = rows[i];
			for (size_t j = 0; j < row->length(); j++)
				printf("%s, ", (*row)[j]);
			printf("\r\n");
		}
	}

	int  ret = (int) db.length();

	// �ͷŲ�ѯ���
	db.free_result();
	return (ret);
}

int main(void)
{
	// WIN32 ����Ҫ���ô˺��������й� SOCKET �ĳ�ʼ��
	acl::acl_cpp_init();

	// ����������־�������Ļ
	acl::log::stdout_open(true);

	acl::string line;
	acl::stdin_stream in;
	acl::stdout_stream out;

#if	defined(_WIN32) || defined(_WIN64)
	const char* libname = "libmysql.dll";
#else
	const char* libname = "libmysqlclient_r.so";
#endif

	acl::string path;

	// ��Ϊ���ö�̬���صķ�ʽ��������ҪӦ�ø��� mysql �ͻ��˿����ڵ�·��
	out.format("Enter %s load path: ", libname);
	if (in.gets(line) && !line.empty())
#if	defined(_WIN32) || defined(_WIN64)
		path.format("%s\\%s", line.c_str(), libname);
#else
		path.format("%s/%s", line.c_str(), libname);
#endif
	else
		path = libname;

	out.format("%s path: %s\r\n", libname, path.c_str());
	// ���ö�̬����ص�ȫ·��
	acl::db_handle::set_loadpath(path);

	const char* dbaddr = "127.0.0.1:16811";
	const char* dbname = "inc365_antispam_db";
	const char* dbuser = "root", *dbpass = "";
	acl::db_mysql db(dbaddr, dbname, dbuser, dbpass);


	if (db.dbopen(NULL) == false)
	{
		printf("open db(%s) error\r\n", dbname);
		getchar();
		return 1;
	}

	printf("open db %s ok\r\n", dbname);

	int  ret = tbl_select(db);
	if (ret >= 0)
	{
		printf(">>select ok\r\n");
	}
	else
		printf(">>select error\r\n");

	printf("mysqlclient lib's version: %ld, info: %s\r\n",
		db.mysql_libversion(), db.mysql_client_info());
	return 0;
}
