#pragma once
#include "fiber_cpp_define.hpp"

struct ACL_FIBER_EVENT;

namespace acl {

/**
 * ������Э��֮�䡢�߳�֮���Լ�Э�����߳�֮�䣬ͨ���¼��ȴ�/֪ͨ��ʽ����ͬ����
 * ���¼������
 */
class FIBER_CPP_API fiber_event
{
public:
	/**
	 * ���췽��
	 * @param use_mutex {bool} �����ڶ��߳�֮������¼�ͬ��ʱ�����������
	 *  ���߳����϶ࣨ�ɰ���ǧ���̣߳�����˱�־Ӧ��Ϊ true �Ա����ڲ���
	 *  ͬ���ڲ�����ʱʹ���̻߳��������б������Ա����γɾ�Ⱥ�����������
	 *  ���߳����϶൫�ñ�־Ϊ false�����ڲ�ʹ��ԭ��������ͬ��������������
	 *  ��ɾ�Ⱥ���⣻���������߳����ϣ���ʮ�����ң�����˲���������Ϊ false
	 *  �Ը�֮�ڲ�ʹ��ԭ��������ͬ������
	 * @param fatal_on_error {bool} �ڲ���������ʱ�Ƿ�ֱ�ӱ������Ա��ڿ���
	 *  ��Ա���д������
	 */
	fiber_event(bool use_mutex = true, bool fatal_on_error = true);
	~fiber_event(void);

	/**
	 * �ȴ��¼���
	 * @return {bool} ���� true ��ʾ�����ɹ��������ʾ�ڲ�����
	 */
	bool wait(void);

	/**
	 * ���Եȴ��¼���
	 * @return {bool} ���� true ��ʾ�����ɹ��������ʾ�����ڱ�ռ��
	 */
	bool trywait(void);

	/**
	 * �¼���ӵ�����ͷ��¼�����֪ͨ�ȴ���
	 * @return {bool} ���� true ��ʾ֪ͨ�ɹ��������ʾ�ڲ�����
	 */
	bool notify(void);

public:
	/**
	 * ���� C �汾���¼�����
	 * @return {ACL_FIBER_EVENT*}
	 */
	ACL_FIBER_EVENT* get_event(void) const
	{
		return event_;
	}

private:
	ACL_FIBER_EVENT* event_;

	fiber_event(const fiber_event&);
	void operator=(const fiber_event&);
};

} // namespace acl

