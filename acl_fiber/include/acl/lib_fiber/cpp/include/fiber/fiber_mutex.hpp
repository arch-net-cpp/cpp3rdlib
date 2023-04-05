#pragma once
#include <vector>
#include "fiber_cpp_define.hpp"
#include "fiber_mutex_stat.hpp"

struct ACL_FIBER_MUTEX;

namespace acl {

/**
 * ������ͬһ�߳��ڵ�Э��֮���Լ���ͬ�߳�֮���Э��֮��Ļ�����, ͬʱ����������
 * �߳�֮���Լ�Э��������߳�֮��Ļ���.
 */
class FIBER_CPP_API fiber_mutex
{
public:
	/**
	 * ���캯��
	 * @param mutex {ACL_FIBER_MUTEX*} �ǿ�ʱ,���� C �������󴴽� C++ ��
	 *  ����,�����ڲ��Զ����� C ������;����ǿ�ʱ,�ڱ���������ʱ�ô����
	 *  C ����������Ӧ�ò������ͷ�.
	 */
	fiber_mutex(ACL_FIBER_MUTEX* mutex = NULL);
	~fiber_mutex(void);

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

public:
	/**
	 * ���� C �汾�Ļ���������
	 * @return {ACL_FIBER_MUTEX*}
	 */
	ACL_FIBER_MUTEX* get_mutex(void) const
	{
		return mutex_;
	}

	/**
	 * ����ȫ���������
	 * @param out {fiber_mutex_stats&} �洢�����
	 * @return {bool} ���� true ��ʾ������������, ������Ϣ����� out ��
	 */
	static bool deadlock(fiber_mutex_stats& out);

	/**
	 * �������, �������н�������״̬��Э��ջ��ӡ����׼���
	 */
	static void deadlock_show(void);

private:
	ACL_FIBER_MUTEX* mutex_;
	ACL_FIBER_MUTEX* mutex_internal_;

	fiber_mutex(const fiber_mutex&);
	void operator=(const fiber_mutex&);
};

class FIBER_CPP_API fiber_mutex_guard
{
public:
	fiber_mutex_guard(fiber_mutex& mutex) : mutex_(mutex) {
		mutex_.lock();
	}

	~fiber_mutex_guard(void) {
		mutex_.unlock();
	}

private:
	fiber_mutex& mutex_;

};

} // namespace acl
