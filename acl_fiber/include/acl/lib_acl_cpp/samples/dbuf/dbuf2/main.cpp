#include "stdafx.h"
#if !defined(_WIN32) && !defined(_WIN64)
#include <sys/time.h>
#endif

/**
 * dbuf_obj ���࣬�� dbuf_pool �϶�̬���䣬�� dbuf_guard ͳһ���й���
 */
class myobj : public acl::dbuf_obj
{
public:
	myobj(acl::dbuf_guard* guard = NULL) : dbuf_obj(guard)
	{
		ptr_ = strdup("hello");
	}

	void run()
	{
		printf("----> run->hello world <-----\r\n");
	}

private:
	char* ptr_;

	// ����������Ϊ˽�ˣ���ǿ��Ҫ��ö��󱻶�̬���䣬��������������
	// dbuf_guard ͳһ���ã����ͷű�������в����Ķ�̬�ڴ�(ptr_)
	~myobj()
	{
		free(ptr_);
	}
};

static void test_dbuf(acl::dbuf_guard& dbuf)
{
	for (int i = 0; i < 102400; i++)
	{
		// ��̬�����ڴ�
		char* ptr = (char*) dbuf.dbuf_alloc(10);
		(void) ptr;
	}

	for (int i = 0; i < 102400; i++)
	{
		// ��̬�����ַ���
		char* str = dbuf.dbuf_strdup("hello world");
		if (i < 5)
			printf(">>str->%s\r\n", str);
	}

	// ��̬�����ڴ�

	(void) dbuf.dbuf_alloc(1024);
	(void) dbuf.dbuf_alloc(1024);
	(void) dbuf.dbuf_alloc(2048);
	(void) dbuf.dbuf_alloc(1024);
	(void) dbuf.dbuf_alloc(1024);
	(void) dbuf.dbuf_alloc(1024);
	(void) dbuf.dbuf_alloc(1024);
	(void) dbuf.dbuf_alloc(1024);
	(void) dbuf.dbuf_alloc(1024);
	(void) dbuf.dbuf_alloc(1024);
	(void) dbuf.dbuf_alloc(1024);
	(void) dbuf.dbuf_alloc(10240);

	for (int i = 0; i < 10000; i++)
	{
		// ��̬���� dbuf_obj ������󣬲�ͨ���� dbuf_guard ������
		// dbuf_obj �Ĺ��캯�����Ӷ���֮�� dbuf_guard ͳһ����

		myobj* obj = dbuf.create<myobj>(&dbuf);

		// ��֤ dbuf_obj ������ dbuf_guard �е�����һ����
		assert(obj == dbuf[obj->pos()]);

		// ���� dbuf_obj ������� myobj �ĺ��� run
		if (i < 10)
			obj->run();
	}

	for (int i = 0; i < 10000; i++)
	{
		myobj* obj = dbuf.create<myobj>();

		assert(dbuf[obj->pos()] == obj);

		if (i < 10)
			obj->run();
	}

	for (int i = 0; i < 10000; i++)
	{
		myobj* obj = dbuf.create<myobj>(&dbuf);

		// ��Ȼ��ν� dbuf_obj �������� dbuf_guard �У���Ϊ dbuf_obj
		// �ڲ������ü��������Կ��Է�ֹ���ظ����
		(void) dbuf.push_back(obj);
		(void) dbuf.push_back(obj);
		(void) dbuf.push_back(obj);

		assert(obj == dbuf[obj->pos()]);

		if (i < 10)
			obj->run();
	}
}

static void wait_pause()
{
	printf("Enter any key to continue ...");
	fflush(stdout);
	getchar();
}

static void test1()
{
	// dbuf_gaurd ���󴴽���ջ�ϣ���������ǰ�ö����Զ�����
	acl::dbuf_guard dbuf;

	test_dbuf(dbuf);
}

static void test2()
{
	// ��̬���� dbuf_guard ������Ҫ�ֶ����ٸö���
	acl::dbuf_guard* dbuf = new acl::dbuf_guard;

	test_dbuf(*dbuf);

	// �ֹ����ٸö���
	delete dbuf;
}

static void test3()
{
	// ���ڴ�ض��� dbuf_pool ��Ϊ dbuf_guard ���캯���������룬��
	// dbuf_guard ��������ʱ��dbuf_pool ����һͬ������
	acl::dbuf_guard dbuf(new acl::dbuf_pool);

	test_dbuf(dbuf);
}

static void test4()
{
	// ��̬���� dbuf_guard ����ͬʱָ���ڴ�����ڴ��ķ��䱶��Ϊ 10��
	// ��ָ���ڲ�ÿ���ڴ���СΪ 4096 * 10 = 40 KB��ͬʱ
	// ָ���ڲ���̬����ĳ�ʼ������С
	acl::dbuf_guard dbuf(10, 100);

	test_dbuf(dbuf);
}

static void test5()
{
	acl::dbuf_pool* dp = new acl::dbuf_pool;

	// ���ڴ�ض����϶�̬���� dbuf_guard �����������Խ��ڴ����Ĵ���
	// ��һ������һ��
	acl::dbuf_guard* dbuf = new (dp->dbuf_alloc(sizeof(acl::dbuf_guard)))
		acl::dbuf_guard(dp);

	test_dbuf(*dbuf);

	// ��Ϊ dbuf_gaurd ����Ҳ���� dbuf_pool �ڴ�ض����϶�̬�����ģ�����
	// ֻ��ͨ��ֱ�ӵ��� dbuf_guard �������������ͷ����е��ڴ����
	// �Ȳ���ֱ�� dbuf_pool->desotry()��Ҳ����ֱ�� delete dbuf_guard ��
	// ���� dbuf_guard ����
	dbuf->~dbuf_guard();
}

class myobj2 : public acl::dbuf_obj
{
public:
	myobj2() {}

	void run()
	{
		printf("hello world\r\n");
	}

private:
	~myobj2() {}
};

class myobj3 : public acl::dbuf_obj
{
public:
	myobj3(int i) : i_(i) {}

	void run()
	{
		printf("hello world: %d\r\n", i_);
	}

private:
	~myobj3() {}

private:
	int i_;
};

class myobj_dummy // : public acl::dbuf_obj
{
public:
	myobj_dummy() {}

	void run()
	{
		printf("can't be compiled\r\n");
	}

private:
	~myobj_dummy() {}
};

static void test6()
{
	acl::dbuf_guard dbuf;

	myobj* o = dbuf.create<myobj>();
	o->run();

	myobj* o1 = dbuf.create<myobj>(&dbuf);
	o1->run();

	myobj2* o2 = dbuf.create<myobj2>();
	o2->run();

	myobj3* o3 = dbuf.create<myobj3>(10);
	o3->run();

	for (int i = 0; i < 10; i++)
	{
		myobj3* o4 = dbuf.create<myobj3>(i);
		o4->run();
	}

	// below codes can't be compiled, because myobj_dummy isn't
	// acl::dbuf_obj's subclass
	// myobj_dummy* dummy = dbuf.create<myobj_dummy>();
	// dummy->run();
}

int main(void)
{
	acl::log::stdout_open(true);

	test1();
	wait_pause();

	test2();
	wait_pause();

	test3();
	wait_pause();

	test4();
	wait_pause();

	test5();
	wait_pause();

	test6();
	return 0;
}
