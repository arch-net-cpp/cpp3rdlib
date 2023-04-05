#pragma once
#include "../acl_cpp_define.hpp"
#include <list>
#include <stdlib.h>
#include "thread_mutex.hpp"
#include "thread_cond.hpp"
#include "noncopyable.hpp"
#include "box.hpp"

namespace acl {

/**
 * �����߳�֮�����Ϣͨ�ţ�ͨ���߳������������߳���ʵ��
 *
 * ʾ����
 *
 * class myobj {
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
class tbox : public box<T> {
public:
	/**
	 * ���췽��
	 * @param free_obj {bool} �� tbox ����ʱ���Ƿ��Զ���鲢�ͷ�
	 *  δ�����ѵĶ�̬����
	 */
	tbox(bool free_obj = true)
	: size_(0), free_obj_(free_obj), cond_(&lock_) {}

	~tbox(void)
	{
		clear(free_obj_);
	}

	/**
	 * ������Ϣ������δ�����ѵ���Ϣ����
	 * @param free_obj {bool} �ͷŵ��� delete ����ɾ����Ϣ����
	 */
	void clear(bool free_obj = false)
	{
		if (free_obj) {
			for (typename std::list<T*>::iterator it =
				tbox_.begin(); it != tbox_.end(); ++it) {

				delete *it;
			}
		}
		tbox_.clear();
	}

	/**
	 * ������Ϣ����
	 * @param t {T*} �ǿ���Ϣ����
	 * @param notify_first {bool} ���Ϊ true������֪ͨ������������Ƚ���
	 *  ��֪ͨ��ע����ߵ�����
	 * @return {bool}
	 * @override
	 */
	bool push(T* t, bool notify_first = true)
	{
		if (lock_.lock() == false) {
			abort();
		}
		tbox_.push_back(t);
		size_++;

		if (notify_first) {
			if (cond_.notify() == false) {
				abort();
			}
			if (lock_.unlock() == false) {
				abort();
			}
		} else {
			if (lock_.unlock() == false) {
				abort();
			}
			if (cond_.notify() == false) {
				abort();
			}
		}

		return true;
	}

	/**
	 * ������Ϣ����
	 * @param wait_ms {int} >= 0 ʱ���õȴ���ʱʱ��(���뼶��)��
	 *  ������Զ�ȴ�ֱ��������Ϣ��������
	 * @param found {bool*} �ǿ�ʱ��������Ƿ�����һ����Ϣ������Ҫ����
	 *  �������ݿն���ʱ�ļ��
	 * @return {T*} �� NULL ��ʾ���һ����Ϣ���󣬷��� NULL ʱ����Ҫ����һ
	 *  ����飬��������� push ��һ���ն���NULL������������Ҳ���� NULL��
	 *  ����ʱ��Ȼ��Ϊ�����һ����Ϣ����ֻ����Ϊ�ն������ wait_ms ����
	 *  Ϊ -1 ʱ���� NULL ��Ȼ��Ϊ�����һ������Ϣ������� wait_ms ����
	 *  ���� 0 ʱ���� NULL����Ӧ�ü�� found ������ֵΪ true ���� false ��
	 *  �ж��Ƿ�����һ������Ϣ����
	 * @override
	 */
	T* pop(int wait_ms = -1, bool* found = NULL)
	{
		long long n = ((long long) wait_ms) * 1000;
		bool found_flag;
		if (lock_.lock() == false) {
			abort();
		}
		while (true) {
			T* t = peek(found_flag);
			if (found_flag) {
				if (lock_.unlock() == false) {
					abort();
				}
				if (found) {
					*found = found_flag;
				}
				return t;
			}

			// ע�����˳�򣬱����ȵ��� wait ���ж� wait_ms
			if (!cond_.wait(n, true) && wait_ms >= 0) {
				if (lock_.unlock() == false) {
					abort();
				}
				if (found) {
					*found = false;
				}
				return NULL;
			}
		}
	}

	/**
	 * tbox �����п���Ϣ
	 * @return {bool}
	 * @override
	 */
	bool has_null(void) const {
		return true;
	}

	/**
	 * ���ص�ǰ��������Ϣ�����е���Ϣ����
	 * @return {size_t}
	 */
	size_t size(void) const
	{
		return size_;
	}

public:
	void lock(void)
	{
		if (lock_.lock() == false) {
			abort();
		}
	}

	void unlock(void)
	{
		if (lock_.unlock() == false) {
			abort();
		}
	}

private:
	std::list<T*> tbox_;
	size_t        size_;
	bool          free_obj_;
	thread_mutex lock_;
	thread_cond  cond_;

	T* peek(bool& found_flag)
	{
		typename std::list<T*>::iterator it = tbox_.begin();
		if (it == tbox_.end()) {
			found_flag = false;
			return NULL;
		}
		found_flag = true;
		size_--;
		T* t = *it;
		tbox_.erase(it);
		return t;
	}
};

} // namespace acl
