#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/noncopyable.hpp"
#include <list>
#include <string>
#if defined(_WIN32) || defined(_WIN64)
#include <WinSock2.h>
#endif

struct ACL_ASTREAM;
struct ACL_VSTREAM;

namespace acl {

/**
 * �첽���ص���
 */
class ACL_CPP_API aio_callback : public noncopyable {
public:
	aio_callback(void) {}
	virtual ~aio_callback(void) {};

	virtual void close_callback(void) {}
	virtual bool timeout_callback(void)
	{
		return false;
	}

	/**
	 * ���ص��麯�����ûص��������������� aio_istream ʵ���е�
	 * gets/read �Ŀɶ������󱻵��ã����첽����ڲ���������������
	 * �ݶ�����ֱ�Ӵ��ݸ��û�������
	 * @param data {char*} ���������ݵ�ָ���ַ
	 * @param len {int} ���������ݳ���(> 0)
	 * @return {bool} �ú������� false ֪ͨ�첽����رո��첽��
	 */
	virtual bool read_callback(char* data, int len)
	{
		(void) data;
		(void) len;
		return true;
	}

	/**
	 * ���ص��麯�����ûص��������������� aio_istream ʵ���е�
	 * read_wait �Ŀɶ��������첽���������ݿɶ�ʱ�����ã�����ʱʱ��
	 * ���� timeout_callback�����쳣���ر�ʱ����� close_callback
	 */
	virtual bool read_wakeup(void)
	{
		return true;
	}

	/**
	 * д�ɹ���Ļص��麯��
	 * @return {bool} �ú������� false ֪ͨ�첽����رո��첽��
	 */
	virtual bool write_callback(void)
	{
		return true;
	}

	/**
	 * ���ص��麯�����ûص��������������� aio_ostream ʵ���е�
	 * write_wait �Ŀ�д�������첽����дʱ�����ã�����ʱʱ��
	 * ���� timeout_callback�����쳣���ر�ʱ����� close_callback
	 */
	virtual bool write_wakeup(void)
	{
		return true;
	}
};

struct AIO_CALLBACK {
	aio_callback* callback;
	bool enable;
};

class aio_handle;
class stream_hook;

/**
 * �첽�����࣬����Ϊ�����࣬���ܱ�ֱ��ʵ������ֻ�ܱ�����̳�ʹ��
 * ����ֻ���ڶ��Ϸ��䣬������ջ�Ϸ���
 */
class ACL_CPP_API aio_stream : public noncopyable {
public:
	/**
	 * ���캯��
	 * @param handle {aio_handle*}
	 */
	aio_stream(aio_handle* handle);

	/**
	 * �ر��첽��
	 * @param flush_out {bool} ��Ϊ true ʱ������Ҫ�ȷ��ͻ��徴������д��
	 *  �ſ��Թرգ������򲻱�����д�������е����ݱ�ɹر�
	 */
	void close(bool flush_out = false);

	/**
	 * ��ӹر�ʱ�Ļص������ָ�룬����ûص�������Ѿ����ڣ���ֻ��
	 * ʹ�ö����ڴ򿪿���״̬
	 * @param callback {aio_callback*} �̳� aio_callback ������ص������
	 *  ���첽���ر�ǰ���ȵ��ô˻ص�������е� close_callback �ӿ�
	 */
	void add_close_callback(aio_callback* callback);

	/**
	 * ��ӳ�ʱʱ�Ļص������ָ�룬����ûص�������Ѿ����ڣ���ֻ��
	 * ʹ�ö����ڴ򿪿���״̬
	 * @param callback {aio_callback*} �̳� aio_callback ������ص������
	 *  ���첽���ر�ǰ���ȵ��ô˻ص�������е� timeout_callback �ӿ�
	 */
	void add_timeout_callback(aio_callback* callback);

	/**
	 * ɾ���ر�ʱ�Ļص������ָ��
	 * @param callback {aio_callback*} �� aio_callback �̳е��������ָ�룬
	 *  ����ֵΪ�գ���ɾ�����еĹرջص�����
	 * @return {int} ���ر��ӻص����󼯺���ɾ���Ļص�����ĸ���
	 */
	int del_close_callback(aio_callback* callback = NULL);

	/**
	 * ɾ����ʱʱ�Ļص������ָ��
	 * @param callback {aio_callback*} �� aio_callback �̳е��������ָ�룬
	 *  ����ֵΪ�գ���ɾ�����еĳ�ʱ�ص�����
	 * @return {int} ���ر��ӻص����󼯺���ɾ���Ļص�����ĸ���
	 */
	int del_timeout_callback(aio_callback* callback = NULL);

	/**
	 * ��ֹ�رյĻص�����󣬵������ӹرն��󼯺���ɾ��
	 * @param callback {aio_callback*} �� aio_callback �̳е��������ָ�룬
	 *  ����ֵΪ�գ����ֹ���еĹرջص�����
	 * @return {int} ���ر��ӻص����󼯺��н��õĻص�����ĸ���
	 */
	int disable_close_callback(aio_callback* callback = NULL);

	/**
	 * ��ֹ��ʱ�Ļص�����󣬵������ӳ�ʱ���󼯺���ɾ��
	 * @param callback {aio_callback*} �� aio_callback �̳е��������ָ�룬
	 *  ����ֵΪ�գ����ֹ���еĳ�ʱ�ص�����
	 * @return {int} ���ر��ӻص����󼯺��н��õĻص�����ĸ���
	 */
	int disable_timeout_callback(aio_callback* callback = NULL);

	/**
	 * �������еĻص����󱻵���
	 * @param callback {aio_callback*} ����ָ���Ļص����������ֵΪ�գ�
	 *  ���������еĹرջص�����
	 * @return {int} ���ر����õĻص�����ĸ���
	 */
	int enable_close_callback(aio_callback* callback = NULL);

	/**
	 * �������еĻص����󱻵���
	 * @param callback {aio_callback*} ����ָ���Ļص����������ֵΪ�գ�
	 *  ���������еĳ�ʱ�ص�����
	 * @return {int} ���ر����õĻص�����ĸ���
	 */
	int enable_timeout_callback(aio_callback* callback = NULL);

	/**
	 * ����첽������ ACL_ASTREAM
	 * @return {ACL_ASTREAM*}
	 */
	ACL_ASTREAM* get_astream(void) const;

	/**
	 * ����첽�������е�ͬ�������� ACL_VSTREAM
	 * @return {ACL_VSTREAM*}
	 */
	ACL_VSTREAM* get_vstream(void) const;

	/**
	 * ����첽���е� SOCKET ������
	 * @return {ACL_SOCKET} ���������򷵻� -1(UNIX) �� INVALID_SOCKET(win32)
	 */
#if defined(_WIN32) || defined(_WIN64)
	SOCKET get_socket(void) const;
	SOCKET sock_handle(void) const
#else
	int get_socket(void) const;
	int sock_handle(void) const
#endif
	{
		return get_socket();
	}

	/**
	 * ���Զ�����ӵĵ�ַ
	 * @param full {bool} �Ƿ���������ַ������IP:PORT������ò���
	 *  Ϊ false��������� IP�����򷵻� IP:PORT
	 * @return {const char*} Զ�����ӵ�ַ��������ֵ == '\0' ���ʾ
	 *  �޷����Զ�����ӵ�ַ
	 */
	const char* get_peer(bool full = false) const;

	/**
	 * ������ӵı��ص�ַ
	 * @param full {bool} �Ƿ���������ַ������IP:PORT������ò���
	 *  Ϊ false��������� IP�����򷵻� IP:PORT
	 * @return {const char*} �����ӵı��ص�ַ��������ֵ == "" ���ʾ
	 *  �޷���ñ��ص�ַ
	 */
	const char* get_local(bool full = false) const;

	/**
	 * ����첽���¼����
	 * @return {aio_handle&}
	 */
	aio_handle& get_handle(void) const;

	/**
	 * ���°��첽�¼����
	 * @param handle {aio_handle&}
	 * ע���÷��������ڶ��󴴽������һ�Σ�һ������ IO �������ֹ����
	 */
	void set_handle(aio_handle& handle);

	/**
	 * ע���д�������ڲ��Զ����� hook->open ���̣�����ɹ����򷵻�֮ǰ
	 * ע��Ķ���(����ΪNULL)����ʧ���򷵻������������ͬ��ָ�룬Ӧ�ÿ���
	 * ͨ���жϷ���ֵ������ֵ�Ƿ���ͬ���ж�ע���������Ƿ�ɹ�
	 * xxx: �ڵ��ô˷���ǰ���뱣֤�������Ѿ�����
	 * @param hook {stream_hook*} �ǿն���ָ��
	 * @return {stream_hook*} ����ֵ������ֵ��ͬ���ʾ�ɹ�
	 */
	stream_hook* setup_hook(stream_hook* hook);

	/**
	 * ��õ�ǰע�������д����
	 * @return {stream_hook*}
	 */
	stream_hook* get_hook(void) const;

	/**
	 * ɾ����ǰע�������д���󲢷��ظö��󣬻ָ�ȱʡ�Ķ�д����
	 * @return {stream_hook*}
	 */
	stream_hook* remove_hook(void);

protected:
	aio_handle*  handle_;
	ACL_ASTREAM* stream_;
	stream_hook* hook_;

	virtual ~aio_stream(void);

	/**
	 * ͨ���˺�������̬�ͷ�ֻ���ڶ��Ϸ�����첽�������
	 */
	virtual void destroy(void);

	/**
	 * ����Ӧ�ڴ����ɹ�����øú���֪ͨ���������첽�������,
	 * ͬʱע�����رռ�����ʱʱ�Ļص�����
	 */
	void enable_error(void);

protected:
	enum {
		// �Ƿ������ hook_xxx ������Ӧ�ı�־λ
		STATUS_HOOKED_ERROR = 1,
		STATUS_HOOKED_READ  = 1 << 1,
		STATUS_HOOKED_WRITE = 1 << 2,
		STATUS_HOOKED_OPEN  = 1 << 3,

		// ���� aio_socket_stream ����ʾ�Ƿ������ѽ���
		STATUS_CONN_OPENED  = 1 << 4,
	};
	unsigned status_;
private:
	std::list<AIO_CALLBACK*>* close_callbacks_;
	std::list<AIO_CALLBACK*>* timeout_callbacks_;

	static int close_callback(ACL_ASTREAM*, void*);
	static int timeout_callback(ACL_ASTREAM*, void*);

private:
	std::string ip_peer_;
	std::string ip_local_;

	const char* get_ip(const char* addr, std::string& out);

private:
#if defined(_WIN32) || defined(_WIN64)
	static int read_hook(SOCKET fd, void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
	static int send_hook(SOCKET fd, const void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);

	static int fread_hook(HANDLE fd, void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
	static int fsend_hook(HANDLE fd, const void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
#else
	static int read_hook(int fd, void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
	static int send_hook(int fd, const void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);

	static int fread_hook(int fd, void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
	static int fsend_hook(int fd, const void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
#endif
};

}  // namespace acl
