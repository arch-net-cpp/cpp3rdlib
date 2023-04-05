#pragma once
#include "../acl_cpp_define.hpp"
#include "noncopyable.hpp"

struct ACL_AQUEUE;

namespace acl
{

class ACL_CPP_API thread_qitem : public noncopyable
{
public:
	thread_qitem() {}
	virtual ~thread_qitem() {}
};

class ACL_CPP_API thread_queue : public noncopyable
{
public:
	thread_queue();
	~thread_queue();

	bool push(thread_qitem* item);
	thread_qitem* pop(int wait_ms = -1);
	int qlen() const;

private:
	ACL_AQUEUE* queue_;
};

//////////////////////////////////////////////////////////////////////////////

#if 0

// internal functions being used
void*  tbox_create(void);
void   tbox_free(void*, void (*free_fn)(void*));
bool   tbox_push(void*, void*);
void*  tbox_pop(void*, int);
size_t tbox_size(void*);

/**
 * �����߳�֮�����Ϣͨ�ţ�ͨ���߳������������߳���ʵ��
 *
 * ʾ����
 *
 * class myobj
 * {
 * public:
 *     myobj(void) {}
 *     ~myobj(void) {}
 *
 *     void test(void) { printf("hello world\r\n"); }
 * };
 *
 * acl::tbox<myobj> tbox;
 *
 * void thread_producer(void)
 * {
 *     myobj* o = new myobj;
 *     tbox.push(o);
 * }
 *
 * void thread_consumer(void)
 * {
 *     myobj* o = tbox.pop();
 *     o->test();
 *     delete o;
 * }
 */

template<typename T>
class tbox : noncopyable
{
public:
	tbox(void)
	{
		tbox_ = tbox_create();
	}

	~tbox(void)
	{
		tbox_free(tbox_, tbox_free_fn);
	}

	/**
	 * ������Ϣ����
	 * @param t {T*} �ǿ���Ϣ����
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool push(T* t)
	{
		return tbox_push(tbox_, t);
	}

	/**
	 * ������Ϣ����
	 * @param wait_ms {int} >= 0 ʱ���ö��ȴ���ʱʱ��(���뼶��)������
	 *  ��Զ�ȴ�ֱ��������Ϣ��������
	 * @return {T*} �� NULL ��ʾ���һ����Ϣ����
	 */
	T* pop(int wait_ms = -1)
	{
		return (T*) tbox_pop(tbox_, wait_ms);
	}

	/**
	 * ���ص�ǰ��������Ϣ�����е���Ϣ����
	 * @return {size_t}
	 */
	size_t size(void) const
	{
		return tbox_size(tbox_);
	}

private:
	void* tbox_;

	static void tbox_free_fn(void* o)
	{
		T* t = (T*) o;
		delete t;
	}
};

#endif

} // namespace acl
