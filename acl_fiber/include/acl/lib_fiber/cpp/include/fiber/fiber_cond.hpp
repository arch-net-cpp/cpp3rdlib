#pragma once
#include "fiber_cpp_define.hpp"

struct ACL_FIBER_COND;

namespace acl {

class fiber_mutex;

/**
 * ������Э��֮�䣬�߳�֮�䣬Э�����߳�֮�����������
 */
class FIBER_CPP_API fiber_cond
{
public:
	fiber_cond(void);
	~fiber_cond(void);

	/**
	 * �ȴ����������¼�������
	 * @param mutex {fiber_mutex&}
	 * @param timeout {int} ��ʱ�ȴ�ʱ�䣨���룩
	 * @return {bool} �ɹ�ʱ���� true�����򷵻� false ��ʾ��ʱ
	 */
	bool wait(fiber_mutex& mutex, int timeout = -1);

	/**
	 * ���������������ϵĵȴ��ߣ����û�еȴ�����ֱ�ӷ��أ�������Ϊ��
	 * �߳�������������
	 * @return {bool} �ɹ����� true�����򷵻� false ��ʾʧ��
	 */
	bool notify(void);

public:
	/**
	 * ���� C �汾��������������
	 * @return {ACL_FIBER_COND*}
	 */
	ACL_FIBER_COND* get_cond(void) const
	{
		return cond_;
	}

private:
	ACL_FIBER_COND* cond_;

	fiber_cond(const fiber_cond&);
	void operator=(const fiber_cond&);
};

}

