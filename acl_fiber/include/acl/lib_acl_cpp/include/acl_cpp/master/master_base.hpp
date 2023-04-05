#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/noncopyable.hpp"
#include "master_conf.hpp"
#include <vector>

#ifndef ACL_CLIENT_ONLY

struct ACL_EVENT;

namespace acl
{

class server_socket;
class event_timer;
class string;

ACL_CPP_API void master_log_enable(bool yes);
ACL_CPP_API bool master_log_enabled(void);

class ACL_CPP_API master_base : public noncopyable
{
public:
	/**
	 * ���� bool ���͵�������
	 * @param table {master_bool_tbl*}
	 * @return {master_base&}
	 */
	master_base& set_cfg_bool(master_bool_tbl* table);

	/**
	 * ���� int ���͵�������
	 * @param table {master_int_tbl*}
	 * @return {master_base&}
	 */
	master_base& set_cfg_int(master_int_tbl* table);

	/**
	 * ���� int64 ���͵�������
	 * @param table {master_int64_tbl*}
	 * @return {master_base&}
	 */
	master_base& set_cfg_int64(master_int64_tbl* table);

	/**
	 * ���� �ַ��� ���͵�������
	 * @param table {master_str_tbl*}
	 * @return {master_base&}
	 */
	master_base& set_cfg_str(master_str_tbl* table);

	/**
	 * �ж��Ƿ����� acl_master ���Ƶ� daemon ģʽ
	 * @return {bool}
	 */
	bool daemon_mode(void) const;

	/////////////////////////////////////////////////////////////////////
	
	/**
	 * ���ý��̼���Ķ�ʱ�����ú���ֻ�������̵߳����пռ� (���ں���
	 * proc_on_init) �б����ã����ö�ʱ������ִ����Ϻ���Զ���
	 * ����(���ڲ����Զ����� master_timer::destroy ����)
	 * @param timer {event_timer*} ��ʱ����
	 * @return {bool} ���ö�ʱ���Ƿ�ɹ�
	 */
	bool proc_set_timer(event_timer* timer);

	/**
	 * ɾ�����̼���ʱ��
	 * @param timer {event_timer*} �� proc_set_timer ���õĶ�ʱ����
	 */
	void proc_del_timer(event_timer* timer);

protected:
	bool daemon_mode_;
	bool proc_inited_;
	std::vector<server_socket*> servers_;

	master_base();
	virtual ~master_base();

	/**
	 * �ڽ�������ʱ���������ÿ�ɹ�����һ�����ص�ַ������ñ�����
	 * @param ss {const server_socket&} ��������
	 */
	virtual void proc_on_listen(server_socket& ss) { (void) ss; }

	/**
	 * �������л��û����ǰ���õĻص������������ڴ˺�������һЩ
	 * �û����Ϊ root ��Ȩ�޲���
	 */
	virtual void proc_pre_jail() {}

	/**
	 * �������л��û���ݺ���õĻص��������˺���������ʱ������
	 * ��Ȩ��Ϊ��ͨ���޼���
	 */
	virtual void proc_on_init() {}

	/**
	 * �������˳�ǰ���õĻص�����
	 */
	virtual void proc_on_exit() {}

	/**
	 * ���յ� SIGHUP �ź�ʱ�Ļص��鷽��
	 */
	virtual bool proc_on_sighup(string&) { return true; }

	// ���ö���
	master_conf conf_;

protected:
	// ���������ñ����������¼�������
	void set_event(ACL_EVENT* event);

	/**
	 * ����¼����湻��
	 * @return {ACL_EVENT*}
	 */
	ACL_EVENT* get_event(void) const
	{
		return event_;
	}

private:
	ACL_EVENT* event_;
};

}  // namespace acl

#endif // ACL_CLIENT_ONLY

