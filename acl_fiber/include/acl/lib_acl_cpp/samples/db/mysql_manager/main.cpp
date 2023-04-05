// mysql.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "acl_cpp/lib_acl.hpp"

const char* CREATE_TBL =
	"create table group_tbl\r\n"
	"(\r\n"
	"group_name varchar(128) not null,\r\n"
	"uvip_tbl varchar(32) not null default 'uvip_tbl',\r\n"
	"access_tbl varchar(32) not null default 'access_tbl',\r\n"
	"access_week_tbl varchar(32) not null default 'access_week_tbl',\r\n"
	"access_month_tbl varchar(32) not null default 'access_month_tbl',\r\n"
	"update_date date not null default '1970-1-1',\r\n"
	"disable integer not null default 0,\r\n"
	"add_by_hand integer not null default 0,\r\n"
	"class_level integer not null default 0,\r\n"
	"primary key(group_name, class_level)\r\n"
	")";

static bool db_create(const char* dbaddr, const char* dbname,
	const char* dbuser, const char* dbpass)
{
	acl::db_mysql db(dbaddr, "mysql", dbuser, dbpass);
	if (db.open() == false)
	{
		printf("open %s@mysql error, dbuser: %s, dbpass: %s\r\n",
			dbaddr, dbuser, dbpass);
		return false;
	}

	printf("mysqlclient lib's version: %ld, info: %s\r\n",
		db.mysql_libversion(), db.mysql_client_info());

	acl::string sql;
	sql.format("use mysql");
	if (db.sql_update(sql.c_str()) == false)
	{
		printf("'%s' error: %s\r\n", sql.c_str(), db.get_error());
		return false;
	}
	db.free_result();

	sql.format("create database %s character set utf8;\r\n", dbname);
	if (db.sql_update(sql.c_str()) == false)
	{
		printf("'%s' error: %s\r\n", sql.c_str(), db.get_error());
		return false;
	}
	db.free_result();
	
	sql.format("grant CREATE,DROP,INSERT,DELETE,UPDATE,SELECT on %s.* to %s",
		dbname, dbuser);
	if (db.sql_update(sql.c_str()) == false)
	{
		printf("'%s' error: %s\r\n", sql.c_str(), db.get_error());
		return false;
	}
	db.free_result();
		
	sql = "flush privileges";
	if (db.sql_update(sql.c_str()) == false)
	{
		printf("'%s' error: %s\r\n", sql.c_str(), db.get_error());
		return false;
	}
	db.free_result();

	printf("create db(%s) ok\r\n", dbname);
	return true;
}

static bool tbl_create(const char* dbaddr, const char* dbname,
	const char* dbuser, const char* dbpass)
{
	acl::db_mysql db(dbaddr, dbname, dbuser, dbpass);
	if (db.open() == false)
	{
		printf("open %s@mysql error, dbuser: %s, dbpass: %s\r\n",
			dbaddr, dbuser, dbpass);
		return false;
	}

	printf("mysqlclient lib's version: %ld, info: %s\r\n",
		db.mysql_libversion(), db.mysql_client_info());

	if (db.tbl_exists("group_tbl"))
	{
		printf("table exist\r\n");
		return (true);
	}
	else if (db.sql_update(CREATE_TBL) == false)
	{
		printf("sql error\r\n");
		return (false);
	}
	else
	{
		printf("create table ok\r\n");
		return (true);
	}
}

// ��ӱ�����
static bool tbl_insert(acl::db_handle& db, int n)
{
	acl::query query;
	query.create_sql("insert into group_tbl(group_name, uvip_tbl,"
		" update_date) values(:group, :test, :date)")
		.set_format("group", "group:%d", n)
		.set_parameter("test", "test")
		.set_date("date", time(NULL), "%Y-%m-%d");

	if (db.exec_update(query) == false)
		return (false);

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

	db.free_result();
	return (true);
}

// ��ѯ������
static int tbl_select(acl::db_handle& db, int n)
{
	acl::query query;
	query.create_sql("select * from group_tbl where group_name=:group"
		" and uvip_tbl=:test")
		.set_format("group", "group:%d", n)
		.set_format("test", "test");

	if (db.exec_select(query) == false)
	{
		printf("select sql error\r\n");
		return (-1);
	}

	printf("\r\n---------------------------------------------------\r\n");

	// �г���ѯ�������һ
	const acl::db_rows* result = db.get_result();
	if (result)
	{
		const std::vector<acl::db_row*>& rows = result->get_rows();
		for (size_t i = 0; i < rows.size(); i++)
		{
			if (n > 100)
				continue;
			const acl::db_row* row = rows[i];
			for (size_t j = 0; j < row->length(); j++)
				printf("%s, ", (*row)[j]);
			printf("\r\n");
		}
	}

	// �г���ѯ���������
	for (size_t i = 0; i < db.length(); i++)
	{
		if (n > 100)
			continue;
		const acl::db_row* row = db[i];

		// ȡ�����м�¼��ĳ���ֶε�ֵ
		const char* ptr = (*row)["group_name"];
		if (ptr == NULL)
		{
			printf("error, no group name\r\n");
			continue;
		}
		printf("group_name=%s: ", ptr);
		for (size_t j = 0; j < row->length(); j++)
			printf("%s, ", (*row)[j]);
		printf("\r\n");
	}

	// �г���ѯ���������
	const std::vector<acl::db_row*>* rows = db.get_rows();
	if (rows)
	{
		std::vector<acl::db_row*>::const_iterator cit = rows->begin();
		for (; cit != rows->end(); cit++)
		{
			if (n > 100)
				continue;
			const acl::db_row* row = *cit;
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

// ɾ��������
static bool tbl_delete(acl::db_handle& db, int n)
{
	acl::query query;
	query.create_sql("delete from group_tbl where group_name=:group")
		.set_format("group", "group-%d", n);

	if (db.exec_update(query) == false)
	{
		printf("delete sql error\r\n");
		return (false);
	}

	for (size_t i = 0; i < db.length(); i++)
	{
		const acl::db_row* row = db[i];
		for (size_t j = 0; j < row->length(); j++)
			printf("%s, ", (*row)[j]);
		printf("\r\n");
	}
	// �ͷŲ�ѯ���
	db.free_result();

	return (true);
}

//////////////////////////////////////////////////////////////////////////////

class db_thread : public acl::thread
{
public:
	db_thread(acl::connect_manager& manager, int max, const char* dbkey)
		: manager_(manager), max_(max), addr_(dbkey) {}
	~db_thread() {}

protected:
	void* run()
	{
		int n = 0;

		for (int i = 0; i < max_; i++)
		{
			acl::db_pool* pool = (acl::db_pool*)
				manager_.get(addr_.c_str(), false, true);
			if (pool == NULL)
			{
				printf("peek db_pool failed\r\n");
				break;
			}

			acl::db_handle* db = pool->peek_open();
			if (db == NULL)
			{
				printf("peek db connection error\r\n");
				break;
			}

			bool ret = tbl_insert(*db, i);
			if (ret)
			{
				printf(">>insert ok: i=%d, affected: %d\r",
					i, db->affect_count());
				n++;
			}
			else
				printf(">>insert error: i = %d\r\n", i);

			pool->put(db);
		}
		printf("\r\n");
		printf(">>insert total: %d\r\n", n);

		n = 0;
		// ������ѯ����
		for (int i = 0; i < max_; i++)
		{
			acl::db_pool* pool = (acl::db_pool*)
				manager_.get(addr_.c_str(), false, true);
			if (pool == NULL)
			{
				printf("peek db_pool failed\r\n");
				break;
			}
			acl::db_handle* db = pool->peek_open();
			if (db == NULL)
			{
				printf("peek db connection error\r\n");
				break;
			}

			int  ret = tbl_select(*db, i);
			if (ret >= 0)
			{
				n += ret;
				printf(">>select ok: i=%d, ret=%d\r", i, ret);
			}
			else
				printf(">>select error: i = %d\r\n", i);

			pool->put(db);
		}
		printf("\r\n");
		printf(">>select total: %d\r\n", n);

		// ����ɾ������
		for (int i = 0; i < max_; i++)
		{
			acl::db_pool* pool = (acl::db_pool*)
				manager_.get(addr_.c_str(), false, true);
			if (pool == NULL)
			{
				printf("peek db_pool failed\r\n");
				break;
			}
			acl::db_handle* db = pool->peek_open();
			if (db == NULL)
			{
				printf("peek db connection error\r\n");
				break;
			}

			bool ret = tbl_delete(*db, i);
			if (ret)
				printf(">>delete ok: %d, affected: %d\r",
					i, (int) db->affect_count());
			else
				printf(">>delete error: i = %d\r\n", i);

			pool->put(db);
		}
		printf("\r\n");

		return NULL;
	}

private:
	acl::connect_manager& manager_;
	int   max_;
	acl::string addr_;
};

//////////////////////////////////////////////////////////////////////////////

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

	acl::string dbaddr("127.0.0.1:3306");
	acl::string dbname("acl_db"), dbuser("root"), dbpass;

	out.format("Enter dbaddr [default: %s]: ", dbaddr.c_str());
	if (in.gets(line) && !line.empty())
		dbaddr = line;

	out.format("Enter dbname [default: %s]: ", dbname.c_str());
	if (in.gets(line) && !line.empty())
		dbname = line;

	out.format("Enter dbuser [default: %s]: ", dbuser.c_str());
	if (in.gets(line) && !line.empty())
		dbuser = line;

	out.format("Enter dbpass [default: %s]: ", dbpass.c_str());
	if (in.gets(line) && !line.empty())
		dbpass = line;

	out.format("dbname: %s, dbuser: %s, dbpass: %s\r\n",
		dbname.c_str(), dbuser.c_str(), dbpass.c_str());

	// �����Ҫ�������ݿ⣬����Ҫ�� root ��ݽ��д���
	out.format("Do you want to create %s? yes|no: ", dbname.c_str());
	if (in.gets(line) && (line == "yes" || line == "y"))
	{
		if (dbuser != "root")
		{
			printf("dbuser must be root for create db\r\n");
			dbuser = "root";
		}

		// �������ݿ�
		if (db_create(dbaddr, dbname, dbuser, dbpass) == false)
		{
			printf("create db failed, enter any key to exit.\r\n");
			getchar();
			return 1;
		}
		printf("create db %s ok, enter any key to continue\r\n",
			dbname.c_str());
		(void) in.gets(line);
	}

	// �����ݱ�����ʱ������
	if (tbl_create(dbaddr, dbname, dbuser, dbpass) == false)
	{
		printf("create table error\r\n");
		getchar();
		return 1;
	}

	//////////////////////////////////////////////////////////////////////

	out.puts("Enter any key to continue ...");
	(void) in.gets(line);

	// ���ӳ�����������������ƣ���ֵӦ���̳߳��е�����߳�����ȣ�
	// �Ա�֤ÿ���̶߳����Ի��һ������
	size_t  dblimit = 10;

	// �������ݿ����ӳض���
	acl::mysql_manager* manager = new acl::mysql_manager;
	manager->add(dbaddr, dbname, dbuser, dbpass, dblimit);

	// �������ӳ���ÿ�����ӵĿ���ʱ��(��)
	//dp->set_idle(120);

	// ÿ�������߳��ڲ����ÿ������ִ�еĴ���
	int  max = 10;

	acl::string dbkey;
	dbkey.format("%s@%s", dbname.c_str(), dbaddr.c_str());

	// ��������߳�
	std::vector<db_thread*> threads;
	for (size_t i = 0; i < dblimit; i++)
	{
		db_thread* thread = new db_thread(*manager, max, dbkey);
		threads.push_back(thread);
		thread->set_detachable(false);
		thread->start();
	}

	// �ȴ����й����߳��˳�
	std::vector<db_thread*>::iterator it;
	for (it = threads.begin(); it != threads.end(); ++it)
	{
		(*it)->wait(NULL);
		delete *it;
	}

//#ifndef WIN32
//	mysql_server_end();
//	mysql_thread_end();
//#endif

	delete manager;
	printf("Enter any key to exit.\r\n");
	getchar();
	return 0;
}
