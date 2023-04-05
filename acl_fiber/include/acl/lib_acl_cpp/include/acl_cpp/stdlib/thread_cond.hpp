#pragma once
#include "../acl_cpp_define.hpp"
#include "noncopyable.hpp"

#if !defined(_WIN32) && !defined(_WIN64)
# include <pthread.h>
# ifndef	acl_pthread_cond_t
#  define	acl_pthread_cond_t	pthread_cond_t
# endif
#else
struct acl_pthread_cond_t;
#endif

namespace acl {

class thread_mutex;

/**
 * �߳���������
 */
class ACL_CPP_API thread_cond : public noncopyable
{
public:
	/**
	 * ���췽��
	 * @param mutex {thread_mutex*} ���ò����� NULL ʱ���ڲ��Զ�����
	 *  ���߳����������ڲ������߳���
	 */
	thread_cond(thread_mutex* mutex = NULL);
	~thread_cond(void);

	/**
	 * �ȴ��߳�������������
	 * @param microseconds {long long} �ȴ��������������ĳ�ʱʱ��(΢�뼶)
	 *   > 0 ʱ��ʾ�ȴ���ʱ��ʱ��
	 *   == 0�����ȴ�
	 *   < 0 ��һֱ�ȴ�ֱ��������������
	 * @param locked {bool} �ò��������Ƿ��Ѿ����������������δ��������
	 *  �ڲ������Զ���������������ǰ�ٽ���������ⲿ�Ѿ����������ڲ�����
	 *  ������������/��������
	 * @return {bool} ���� true ��ʾ�������������������ʾ��ʱ��û��֪ͨ
	 */
	bool wait(long long microseconds = -1, bool locked = false);

	/**
	 * ֪ͨһ���򼸸��ȴ����߳����������ϵ��̣߳���ʾ���������ͽ�
	 * @return {bool} ���� false ��ʾ֪ͨʧ��
	 */
	bool notify(void);

	/**
	 * ֪ͨ���еȴ����߳����������ϵ��̣߳���ʾ���������ͽ�
	 * @return {bool} ���� false ��ʾ֪ͨʧ��
	 */
	bool notify_all(void);

	/**
	 * �������߳����������󶨵��̻߳�����
	 * @return {thread_mutex&}
	 */
	thread_mutex& get_mutex(void) const;

	/**
	 * ���ϵͳ���͵��߳�������������
	 * @return {acl_pthread_cond_t*}
	 */
	acl_pthread_cond_t* get_cond(void) const;

private:
	thread_mutex* mutex_;
	thread_mutex* mutex_internal_;
	acl_pthread_cond_t* cond_;

	bool block_wait(bool locked);
	bool timed_wait(long long microseconds, bool locked);
};

} // namespace acl
