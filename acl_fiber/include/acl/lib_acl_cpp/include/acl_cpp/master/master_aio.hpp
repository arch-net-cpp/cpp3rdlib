#pragma once
#include "../stdlib/thread_mutex.hpp"
#include "../stream/aio_handle.hpp"
#include "../stream/aio_listen_stream.hpp"
#include "master_base.hpp"

#ifndef ACL_CLIENT_ONLY

struct ACL_VSTREAM;
struct ACL_VSTRING;

namespace acl {

class aio_handle;
class aio_socket_stream;

/**
 * acl_master ����������е��̷߳�������ʽ��ģ���࣬�������ֻ����һ��ʵ������
 */
class ACL_CPP_API master_aio : public master_base, public aio_accept_callback
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
	 * @param ht {aio_handle_type} �¼����������
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool run_alone(const char* addrs, const char* path = NULL,
		aio_handle_type ht = ENGINE_SELECT);

	/**
	 * ����첽IO���¼���������ͨ���˾�����û��������ö�ʱ���ȹ���
	 * @return {aio_handle*}
	 */
	aio_handle* get_handle() const;

	/**
	 * �� run_alone ģʽ�£�֪ͨ��������ܹر����棬�˳�����
	 */
	void stop();

	/**
	 * ��������ļ�·��
	 * @return {const char*} ����ֵΪ NULL ��ʾû���������ļ�
	 */
	const char* get_conf_path(void) const;

protected:
	master_aio();
	virtual ~master_aio();

	/**
	 * ���麯���������յ�һ���ͻ�������ʱ���ô˺���
	 * @param stream {aio_socket_stream*} �½��յ��Ŀͻ����첽������
	 * @return {bool} �ú���������� false ��֪ͨ��������ܲ��ٽ���
	 *  Զ�̿ͻ������ӣ�����������տͻ�������
	 */
	virtual bool on_accept(aio_socket_stream* stream) = 0;

private:
	aio_handle* handle_;
	/**
	 * ���� aio_accept_callback ���麯��ʵ��
	 * @param client {aio_socket_stream*} �첽�ͻ�����
	 * @return {bool} ���� true ��֪ͨ��������������
	 */
	virtual bool accept_callback(aio_socket_stream* client);

private:
	thread_mutex lock_;
	void push_back(server_socket* ss);

private:
#if defined(_WIN32) || defined(_WIN64)
	// �����յ�һ���ͻ�������ʱ�ص��˺���
	static void service_main(SOCKET, void*);
#else
	static void service_main(int, void*);
#endif

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
};

}  // namespace acl

#endif // ACL_CLIENT_ONLY
