#pragma once
#include "master_base.hpp"

#ifndef ACL_CLIENT_ONLY

struct ACL_VSTREAM;
struct ACL_EVENT;
struct ACL_VSTRING;

namespace acl {

class socket_stream;

/**
 * acl_master ����������н��̷�ʽ��ģ���࣬�������ֻ����һ��ʵ������
 */
class ACL_CPP_API master_proc : public master_base
{
public:
	/**
	 * ��ʼ���У����øú�����ָ�÷���������� acl_master ������
	 * ����֮�����У�һ������������״̬
	 * @param argc {int} �� main �д��ݵĵ�һ����������ʾ��������
	 * @param argv {char**} �� main �д��ݵĵڶ�������
	 */
	void run_daemon(int argc, char** argv);

	/**
	 * �ڵ�������ʱ�Ĵ��������û����Ե��ô˺�������һЩ��Ҫ�ĵ��Թ���
	 * @param addrs {const char*} ���������ַ�б���ʽ��IP:PORT, IP:PORT...
	 * @param path {const char*} �����ļ�ȫ·��
	 * @param count {int} ����ֵ > 0 ʱ������յ����Ӵ����ﵽ��ֵ�����
	 *  �󣬸ú��������أ�����һֱѭ������Զ������
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool run_alone(const char* addrs, const char* path = NULL, int count = 1);

	/**
	 * ��������ļ�·��
	 * @return {const char*} ����ֵΪ NULL ��ʾû���������ļ�
	 */
	const char* get_conf_path(void) const;

protected:
	master_proc();
	virtual ~master_proc();

	/**
	 * ���麯���������յ�һ���ͻ�������ʱ���ô˺���
	 * @param stream {aio_socket_stream*} �½��յ��Ŀͻ����첽������
	 * ע���ú������غ������ӽ��ᱻ�رգ��û���Ӧ�����رո���
	 */
	virtual void on_accept(socket_stream* stream) = 0;

private:
	// �����յ�һ���ͻ�������ʱ�ص��˺���
	static void service_main(void*, ACL_VSTREAM *stream);

	// ������һ�������ַʱ�ص��˺���
	static void service_on_listen(void*, ACL_VSTREAM*);

	// �������л��û���ݺ���õĻص�����
	static void service_pre_jail(void*);

	// �������л��û���ݺ���õĻص�����
	static void service_init(void*);

	// �������˳�ʱ���õĻص�����
	static void service_exit(void*);

	// �������յ� SIGHUP �źź��ص�������
	static int service_on_sighup(void*, ACL_VSTRING*);

private:
	// �ڵ������з�ʽ�£��ú����������׽����������ӵ���ʱ������
	static void listen_callback(int event_type, ACL_EVENT*,
		ACL_VSTREAM*, void* context);

private:
	bool stop_;
	int  count_limit_;
	int  count_;
};

}  // namespace acl

#endif // ACL_CLIENT_ONLY
