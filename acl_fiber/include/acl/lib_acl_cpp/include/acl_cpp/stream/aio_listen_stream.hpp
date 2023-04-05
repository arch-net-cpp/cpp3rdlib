#pragma once
#include "../acl_cpp_define.hpp"
#include "aio_stream.hpp"

namespace acl {

class aio_socket_stream;
class aio_listen_stream;

/**
 * ���첽���������յ��µĿͻ�����ʱ���ô˻ص����еĻص�����������Ϊ�����࣬
 * Ҫ���������ʵ�� accept_callback �ص�����
 */
class ACL_CPP_API aio_accept_callback : public aio_callback {
public:
	aio_accept_callback(void) {}
	virtual ~aio_accept_callback(void) {}

	/**
	 * �����յ��µĿͻ�����ʱ�Ļص�����
	 * @param client {aio_socket_stream*} �ͻ����첽��������
	 *  ���ԶԴ������ж�д����
	 * @return {bool} ���ϣ���رո��첽�����������Է��� false��
	 *  һ�㲻Ӧ���� false
	 */
	virtual bool accept_callback(aio_socket_stream* client) = 0;
};

/**
 * ���첽�������յ��������ӵ����¼�ʱ���ô����е��麯������������ʵ�ָ��麯��
 * �е��� accept() ϵͳ API ���տͻ������ӣ������������ aio_accept_callback
 * ������ͬ���� aio_accept_callback::accept_callback() ������ʱ���ͻ������Ӷ�
 * ���Ѿ������������� listen_callback() �У�����ҪӦ���Լ��������Ӷ���
 */
class ACL_CPP_API aio_listen_callback : public aio_callback {
public:
	aio_listen_callback(void) {}
	virtual ~aio_listen_callback(void) {}

	virtual bool listen_callback(aio_listen_stream& ss) = 0;
};

/**
 * �첽������������������������ڿͻ��˵��������ӣ�ͬʱ����ֻ��
 * �ڶ��Ϸ��䣬������ջ���䣬Ӧ�ÿ��Ե��� close �����ر��������ر�
 * ����첽�������Զ��ͷţ�������� delete ɾ���������
 *
 */
class ACL_CPP_API aio_listen_stream : public aio_stream {
public:
	/**
	 * ���캯�������Թ����첽������
	 * @param handle {aio_handle*} �첽������
	 */
	aio_listen_stream(aio_handle* handle);

	/**
	 * ����첽���������յ��¿ͻ�����ʱ�Ļص�����
	 * @param callback {aio_accept_callback*}
	 */
	void add_accept_callback(aio_accept_callback* callback);

	/**
	 * ����첽�������пͻ������ӵ���ʱ�Ļص�����
	 * @param callback {aio_listen_stream*}
	 *  ע�⣺������������ add_accept_callback �����𣬱������� reactor
	 *  ģʽ���� add_accept_callback ���� proactor ģʽ
	 */
	void add_listen_callback(aio_listen_callback* callback);

	/**
	 * ������ add_listen_callback ��ʽʱ���� aio_listen_callback ����
	 * �еĺ��� listen_callback ����Ե��ñ����������һ���첽���Ӷ���
	 * @return {aio_socket_stream*} ���� NULL ��ʾ�������ʧ��
	 */
	aio_socket_stream* accept(void);

	/**
	 * ��ʼ����ĳ��ָ����ַ������Ϊ�����׽ӿڣ�Ҳ����Ϊ���׽ӿڣ�
	 * @param addr {const char*} ������ַ��TCP������ַ���������ַ
	 * ��ʽ��
	 *   ���TCP���ӣ�IP:PORT���磺127.0.0.1:9001
	 *   ������׽ӿڣ�{path}���磺/tmp/my.sock���� Linux ƽ̨��������֧��
	 *   Linux abstract unix domain socket����Ҫ��ַ���ֽ�Ϊ'@'���� Linux
	 *   ƽ̨�£�acl �ڲ������⵽·�����ֽ�Ϊ '@'�����ڲ��Զ��е� Linux
	 *   abstract unix domain socket ����ģʽ�����е� @ ��ֻ��������ǣ���
	 *   ���ļ�����ַ���Զ�ȥ����
	 * @param flag {unsigned} ���������׽ӿ�ʱ�Ĵ򿪱�־λ���� server_socket.hpp
	 * @param qlen {int} ָ�������׽��ּ������еĳ���
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool open(const char* addr, unsigned flag = 0, int qlen = 128);

	/**
	 * ʹ���׽��ִ����������󣬸��׽��־�������Ѿ������� bind/listen ����
	 * @param fd {int}
	 * @return {bool} �Ƿ�ɹ�
	 */
#if defined(_WIN32) || defined(_WIN64)
	bool open(SOCKET fd);
#else
	bool open(int fd);
#endif

	/**
	 * ʹ��ͬ�������󴴽���������������
	 * @param vstream {ACL_VSTREAM*} �ǿն���
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool open(ACL_VSTREAM* vstream);

	/**
	 * ʹ�÷����������󴴽���������������
	 * @param astream {ACL_ASTREAM*} �ǿն���
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool open(ACL_ASTREAM* astream);

	/**
	 * ��÷�����������ַ
	 * @return {const char*}
	 */
	const char* get_addr(void) const;

	/**
	 * ���ػ��෽�������첽����������ʱ�ص��˷���
	 */
	virtual void destroy(void);

protected:
	virtual ~aio_listen_stream(void);

private:
	bool listen_hooked_;
	char addr_[256];
	std::list<aio_accept_callback*> accept_callbacks_;
	std::list<aio_listen_callback*> listen_callbacks_;

	void enable_listen(void);
	int accept_callback(aio_socket_stream* conn);
	static int listen_callback(ACL_ASTREAM*, void*);
};

}  // namespace acl
