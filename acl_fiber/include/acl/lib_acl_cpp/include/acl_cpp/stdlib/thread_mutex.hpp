#pragma once
#include "../acl_cpp_define.hpp"
#include "noncopyable.hpp"

#if !defined(_WIN32) && !defined(_WIN64)
# include <pthread.h>
# ifndef	acl_pthread_mutex_t
#  define	acl_pthread_mutex_t	pthread_mutex_t
# endif
#else
struct acl_pthread_mutex_t;
#endif

namespace acl {

/**
 * �̻߳�����
 */
class ACL_CPP_API thread_mutex : public noncopyable
{
public:
	/**
	 * ���췽��
	 * @param recursive {bool} �Ƿ����õݹ�����ʽ
	 */
	thread_mutex(bool recursive = true);
	~thread_mutex(void);

	/**
	 * ���߳������м�����һֱ�������ɹ����ڲ�ʧ��(һ�㲻��ʧ�ܣ�������ϵͳ����)
	 * @return {bool} ���� false ˵���߳���������
	 */
	bool lock(void);

	/**
	 * �����Լ��������۳ɹ���񶼻���������
	 * @return {bool} ���� true ��ʾ�����ɹ������� false ��ʾ����ʧ��
	 */
	bool try_lock(void);

	/**
	 * ���߳���
	 * @return {bool} ���� false ��ʾ����ʧ�ܣ��п���֮ǰ��δ�����ɹ�����
	 */
	bool unlock(void);

	/**
	 * ��� acl �� C �汾��ϵͳ���͵��߳���
	 * @return {acl_pthread_mutex_t*}
	 */
	acl_pthread_mutex_t* get_mutex(void) const;

private:
	acl_pthread_mutex_t* mutex_;
#if !defined(_WIN32) && !defined(_WIN64)
	pthread_mutexattr_t  mutex_attr_;
#endif
};

class ACL_CPP_API thread_mutex_guard : public noncopyable
{
public:
	thread_mutex_guard(thread_mutex& mutex);
	~thread_mutex_guard(void);

private:
	thread_mutex& mutex_;
};

} // namespace acl
