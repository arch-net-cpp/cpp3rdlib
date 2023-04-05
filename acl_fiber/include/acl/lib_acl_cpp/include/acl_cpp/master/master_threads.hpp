#pragma once
#include "master_base.hpp"
#include "../stdlib/thread_mutex.hpp"

#ifndef ACL_CLIENT_ONLY

struct ACL_VSTREAM;
struct ACL_EVENT;
struct ACL_VSTRING;
struct acl_pthread_pool_t;

namespace acl {

class socket_stream;

/**
 * �̳߳ط���������࣬����Ϊ�����࣬������Ҫʵ�����еĴ��麯����
 * ÿһ�����̽�����һ���������ʵ�����������ᱻ��ֹ
 */
class ACL_CPP_API master_threads : public master_base
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
	 * @param addrs {const char*} ������ַ�б���ʽ��IP:PORT, IP:PORT...
	 * @param path {const char*} �����ļ�ȫ·��
	 * @param count {unsigned int} ѭ������Ĵ������ﵽ��ֵ�����Զ����أ�
	 *  ����ֵΪ 0 ���ʾ����һֱѭ���������������������
	 * @param threads_count {int} ����ֵ���� 1 ʱ��ʾ�Զ������̳߳ط�ʽ��
	 *  ��ֵֻ�е� count != 1 ʱ����Ч������ count == 1 �������һ�ξͷ���
	 *  �Ҳ��������̴߳���ͻ�������
	 * @return {bool} �����Ƿ�ɹ�
	 *  ע��count, threads_count ��������������Ч������ʹ�������ļ��е�
	 *  ����ֵ ioctl_use_limit(���ƴ������ӵĸ���) �� ioctl_max_threads(
	 *  ��������������߳���)
	 */
	bool run_alone(const char* addrs, const char* path = NULL,
		unsigned int count = 1, int threads_count = 1);
	
	/**
	 * �����������Ŀɶ�״̬
	 * @param stream {socket_stream*}
	 */
	void thread_enable_read(socket_stream* stream);

	/**
	 * ���ټ����������Ŀɶ�״̬
	 * @param stream {socket_stream*}
	 */
	void thread_disable_read(socket_stream* stream);

	/**
	 * ��������ļ�·��
	 * @return {const char*} ����ֵΪ NULL ��ʾû���������ļ�
	 */
	const char* get_conf_path(void) const;

	/**
	 * ��õ�ǰ�̳߳ض����л�ѹ�Ĵ��������������� API ���Է���Ӧ�þ�����ʱ
	 * ��Ҫ���й��ر�������ѹ�����ʱ�򽫺�����������
	 * @return {size_t}
	 */
	size_t task_qlen(void) const;

public:
	/**
	 * ��� lib_acl C ���е��̳߳ؾ��
	 * @return {acl_pthread_pool_t*}
	 */
	acl_pthread_pool_t* threads_pool(void) const;

protected:
	// ���಻��ֱ�ӱ�ʵ����
	master_threads();
	virtual ~master_threads();

	/**
	 * ���麯������ĳ���ͻ������������ݿɶ���رջ����ʱ���ô˺���
	 * @param stream {socket_stream*}
	 * @return {bool} ���� false ���ʾ���������غ���Ҫ�ر����ӣ�
	 *  �����ʾ��Ҫ���ֳ����ӣ��������������Ӧ��Ӧ�÷��� false
	 */
	virtual bool thread_on_read(socket_stream* stream) = 0;

	/**
	 * ����ڵ��� thread_on_read �����䷵�� true �󣬻��Զ����ñ�����
	 * ���ж��Ƿ����������Ƿ�ɶ�
	 * @param stream {socket_stream*}
	 * @return {bool} ������� false�����ܲ��ټ�ظ�������
	 */
	virtual bool keep_read(socket_stream* stream)
	{
		(void) stream;
		return true;
	}

	/**
	 * ���̳߳��е�ĳ���̻߳��һ������ʱ�Ļص����������������һЩ
	 * ��ʼ���������ú����������̵߳��߳̿ռ�������
	 * @param stream {socket_stream*}
	 * @return {bool} ������� false ���ʾ����Ҫ��ر����ӣ�����
	 *  �ؽ��������ٴ����� thread_main ����
	 */
	virtual bool thread_on_accept(socket_stream* stream)
	{
		(void) stream;
		return true;
	}

	/**
	 * �����յ�һ���ͻ������Ӻ󣬷���˻ص��˺�����ͻ��˽������ֵĲ�����
	 * �ú������� thread_on_accept ֮�󱻵���
	 * @return {bool} ������� false ���ʾ����Ҫ��ر����ӣ�����
	 *  �ؽ��������ٴ����� thread_main ����
	 */
	virtual bool thread_on_handshake(socket_stream *stream)
	{
		(void) stream;
		return true;
	}

	/**
	 * ��ĳ���������ӵ� IO ��д��ʱʱ�Ļص�����������ú������� true ��
	 * ��ʾ�����ȴ���һ�ζ�д��������ϣ���رո�����
	 * @param stream {socket_stream*}
	 * @return {bool} ������� false ���ʾ����Ҫ��ر����ӣ�������Ҫ��
	 *  ��������������
	 */
	virtual bool thread_on_timeout(socket_stream* stream)
	{
		(void) stream;
		return false;
	}

	/**
	 * ����ĳ���̰߳󶨵����ӹر�ʱ�Ļص�����
	 * @param stream {socket_stream*}
	 * ע������ thread_on_accept ���� false �����ر�ʱ�ú���������
	 * ������
	 */
	virtual void thread_on_close(socket_stream* stream) { (void) stream; }

	/**
	 * ���̳߳���һ�����̱߳�����ʱ�Ļص�����
	 */
	virtual void thread_on_init() {}

	/**
	 * ���̳߳���һ���߳��˳�ʱ�Ļص�����
	 */
	virtual void thread_on_exit() {}

	/**
	 * ���ӽ�����Ҫ�˳�ʱ��ܽ��ص��˺�������ܾ����ӽ����Ƿ��˳�ȡ���ڣ�
	 * 1) ����˺������� true ���ӽ��������˳�������
	 * 2) ������ӽ������пͻ������Ӷ��ѹرգ����ӽ��������˳�������
	 * 3) �鿴�����ļ��е�������(ioctl_quick_abort)�������������� 0 ��
	 *    �ӽ��������˳�������
	 * 4) �����пͻ������ӹرպ���˳�
	 * @param nclients {size_t} ��ǰ���ӵĿͻ��˸���
	 * @param nthreads {size_t} ��ǰ�̳߳��з�æ�Ĺ����̸߳���
	 * @return {bool} ���� false ��ʾ��ǰ�ӽ��̻������˳��������ʾ��ǰ
	 *  �ӽ��̿����˳���
	 */
	virtual bool proc_exit_timer(size_t nclients, size_t nthreads)
	{
		(void) nclients;
		(void) nthreads;
		return true;
	}

private:
	thread_mutex lock_;

	void push_back(server_socket* ss);
	void run(int argc, char** argv);

	// �����յ�һ���ͻ�������ʱ�ص��˺���
	static int service_main(void*, ACL_VSTREAM*);

	// ������һ�������ַʱ�ص��˺���
	static void service_on_listen(void*, ACL_VSTREAM*);

	// �����յ�һ���ͻ�����ʱ�Ļص����������Խ���һЩ��ʼ��
	static int service_on_accept(void*, ACL_VSTREAM*);

	// �����յ��ͻ������Ӻ�������Ҫ��ͻ�����һЩ���ȵ����ֶ���ʱ
	// �ص��˺������ú������� service_on_accept ֮�󱻵���
	static int service_on_handshake(void*, ACL_VSTREAM*);

	// ���ͻ������Ӷ�д��ʱʱ�Ļص�����
	static int service_on_timeout(void*, ACL_VSTREAM*);

	// ���ͻ������ӹر�ʱ�Ļص�����
	static void service_on_close(void*, ACL_VSTREAM*);

	// �������л��û���ݺ���õĻص�����
	static void service_pre_jail(void*);

	// �������л��û���ݺ���õĻص�����
	static void service_init(void*);

	// ��������Ҫ�˳�ʱ���ô˺�������Ӧ����ȷ�������Ƿ���Ҫ�˳�
	static int service_exit_timer(void*, size_t, size_t);

	// �������˳�ʱ���õĻص�����
	static void service_exit(void*);

	// ���̴߳�������õĻص�����
	static int thread_init(void*);

	// ���߳��˳�ǰ���õĻص�����
	static void thread_exit(void*);

	// �������յ� SIGHUP �źź��ص�������
	static int service_on_sighup(void*, ACL_VSTRING*);
};

} // namespace acl

#endif // ACL_CLIENT_ONLY
