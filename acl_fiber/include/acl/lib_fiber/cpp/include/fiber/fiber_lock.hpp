#pragma once
#include "fiber_cpp_define.hpp"

struct ACL_FIBER_LOCK;
struct ACL_FIBER_RWLOCK;

namespace acl {

/**
 * ��������ͬһ�߳��ڲ���Э��֮����л���Ļ�����
 */
class FIBER_CPP_API fiber_lock
{
public:
	fiber_lock(void);
	~fiber_lock(void);

	/**
	 * �ȴ�������
	 * @return {bool} ���� true ��ʾ�����ɹ��������ʾ�ڲ�����
	 */
	bool lock(void);

	/**
	 * ���Եȴ�������
	 * @return {bool} ���� true ��ʾ�����ɹ��������ʾ�����ڱ�ռ��
	 */
	bool trylock(void);

	/**
	 * ������ӵ�����ͷ�����֪ͨ�ȴ���
	 * @return {bool} ���� true ��ʾ֪ͨ�ɹ��������ʾ�ڲ�����
	 */
	bool unlock(void);

private:
	ACL_FIBER_LOCK* lock_;

	fiber_lock(const fiber_lock&);
	void operator=(const fiber_lock&);
};

/**
 * ��������ͬһ�߳��ڵ�Э��֮����л���Ķ�д��
 */
class FIBER_CPP_API fiber_rwlock
{
public:
	fiber_rwlock(void);
	~fiber_rwlock(void);

	/**
	 * �Ӷ���
	 */
	void rlock(void);

	/**
	 * ���ԼӶ���
	 * @return {bool} ���� true ��ʾ�����ɹ��������ʾ�����ڱ�ռ��
	 */
	bool tryrlock(void);

	/**
	 * �����
	 */
	void runlock(void);

	/**
	 * ��д��
	 */
	void wlock(void);

	/**
	 * ���Լ�д��
	 * @return {bool} ���� true ��ʾ�����ɹ��������ʾ�����ڱ�ռ��
	 */
	bool trywlock(void);

	/**
	 * ��д��
	 */
	void wunlock(void);

private:
	ACL_FIBER_RWLOCK* rwlk_;

	fiber_rwlock(const fiber_rwlock&);
	void operator=(const fiber_rwlock&);
};

} // namespace acl
