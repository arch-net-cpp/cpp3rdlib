#pragma once

////////////////////////////////////////////////////////////////////////////////
// ����������

extern char *var_cfg_str;
extern acl::master_str_tbl var_conf_str_tab[];

extern int  var_cfg_bool;
extern acl::master_bool_tbl var_conf_bool_tab[];

extern int  var_cfg_int;
extern acl::master_int_tbl var_conf_int_tab[];

extern long long int  var_cfg_int64;
extern acl::master_int64_tbl var_conf_int64_tab[];

////////////////////////////////////////////////////////////////////////////////

//class acl::socket_stream;

class master_service : public acl::master_threads
{
public:
	master_service();
	~master_service();

protected:
	/**
	 * @override
	 * ��ĳ���ͻ������������ݿɶ���رջ����ʱ���ô˺���
	 * @param stream {socket_stream*}
	 * @return {bool} ���� false ���ʾ���������غ���Ҫ�ر����ӣ�
	 *  �����ʾ��Ҫ���ֳ����ӣ��������������Ӧ��Ӧ�÷��� false
	 */
	bool thread_on_read(acl::socket_stream* stream);

	/**
	 * @override
	 * ���̳߳��е�ĳ���̻߳��һ������ʱ�Ļص�������
	 * ���������һЩ��ʼ������
	 * @param stream {socket_stream*}
	 * @return {bool} ������� false ���ʾ����Ҫ��ر����ӣ�����
	 *  �ؽ��������ٴ����� thread_main ����
	 */
	bool thread_on_accept(acl::socket_stream* stream);

	/**
	 * @override
	 * ��ĳ���������ӵ� IO ��д��ʱʱ�Ļص�����������ú������� true ���ʾ�����ȴ���һ��
	 * ��д��������ϣ���رո�����
	 * @param stream {socket_stream*}
	 * @return {bool} ������� false ���ʾ����Ҫ��ر����ӣ�����
	 *  �ؽ��������ٴ����� thread_main ����
	 */
	bool thread_on_timeout(acl::socket_stream* stream);

	/**
	 * @override
	 * ����ĳ���̰߳󶨵����ӹر�ʱ�Ļص�����
	 * @param stream {socket_stream*}
	 */
	void thread_on_close(acl::socket_stream* stream);

	/**
	 * @override
	 * ���̳߳���һ�����̱߳�����ʱ�Ļص�����
	 */
	void thread_on_init();

	/**
	 * @override
	 * ���̳߳���һ���߳��˳�ʱ�Ļص�����
	 */
	void thread_on_exit();

	/**
	 * @override
	 * �������л��û���ݺ���õĻص��������˺���������ʱ������
	 * ��Ȩ��Ϊ��ͨ���޼���
	 */
	void proc_on_init();

	/**
	 * @override
	 * �������˳�ǰ���õĻص�����
	 */
	void proc_on_exit();

private:
	acl::sslbase_conf* conf_;
};
