#pragma once
#include "../acl_cpp_define.hpp"
#include <stdarg.h>
#include "../stdlib/string.hpp"
#include "aio_handle.hpp"
#include "aio_timer_callback.hpp"
#include "aio_stream.hpp"

struct iovec;

namespace acl {

class aio_ostream;

/**
 * �ӳ��첽д�����࣬����Ϊ aio_timer_callback (see aio_handle.hpp)��
 * ��ν�ӳ��첽д�����ǰ��첽д��(aio_ostream)���ڶ�ʱ���У�������
 * �������첽д���������(���� aio_handle �ļ���н��)����ָ��
 * ʱ�䵽����������첽д����(�� timer_callback �ص��������½��첽
 * �����첽д������)��ͬʱ�ö�ʱ���Զ�����(���� destroy ����)��
 * ��������û��̳��� aio_timer_writer �࣬�����಻���ڶ��Ϸ���ģ�
 * ��������� destroy������ͬʱ������� destroy ��ִ������Դ�ͷŵ�
 * ��ز������������δ���� destroy���򵱶�ʱ���������ڲ����Զ�����
 * ���� aio_timer_writer �� destroy--��������� delete this����ʱ��
 * �ᵼ�·Ƿ��ڴ��ͷŲ���)
 * 
 */
class ACL_CPP_API aio_timer_writer : public aio_timer_callback {
public:
	aio_timer_writer(void);

	/**
	 * �� aio_istream �е��ô˺������ͷ����������Ӧ��ʵ�ָú���
	 */
	virtual void destroy(void)
	{
		delete this;
	}

protected:
	virtual ~aio_timer_writer(void);

	/**
	 * �ӳٶ�����ʱ�Ļص��������� aio_timer_callback ���м̳ж���
	 */
	virtual void timer_callback(unsigned int id);
private:
	friend class aio_ostream;

	aio_ostream* out_;
	//int   write_delayed_;
	acl::string buf_;
};

/**
 * �첽д�������ඨ�壬����ֻ���ڶ��ϱ�ʵ������������ʱ��Ҫ���� close
 * �������ͷŸ������
 */
class ACL_CPP_API aio_ostream : virtual public aio_stream {
public:
	/**
	 * ���캯��
	 * @param handle {aio_handle*} �첽�¼�������
	 */
	aio_ostream(aio_handle* handle);

	/**
	 * ���캯���������첽д�����󣬲� hook д���̼��ر�/��ʱ����
	 * @param handle {aio_handle*} �첽�¼�������
	 * @param fd {int} �����׽ӿھ��
	 */
#if defined(_WIN32) || defined(_WIN64)
	aio_ostream(aio_handle* handle, SOCKET fd);
#else
	aio_ostream(aio_handle* handle, int fd);
#endif

	/**
	 * ������дʱ�Ļص������ָ�룬����ûص�������Ѿ����ڣ���ֻ��
	 * ʹ�ö����ڴ򿪿���״̬
	 * @param callback {aio_callback*} �̳� aio_callback ������ص������
	 *  ���첽��������ʱ���ȵ��ô˻ص�������е� write_callback �ӿ�
	 */
	void add_write_callback(aio_callback* callback);

	/**
	 * ��д�ص����󼯺���ɾ��
	 * @param callback {aio_callback*} ��ɾ����д�ص�����
	 *  ����ֵΪ�գ���ɾ�����еĻص�д����
	 * @return {int} ���ر��ӻص����󼯺���ɾ���Ļص�����ĸ���
	 */
	int del_write_callback(aio_callback* callback = NULL);

	/**
	 * ��ֹ�ص������༯���е�ĳ���ص�����󣬵������ӻص������
	 * ������ɾ����ֻ�ǲ������ö���
	 * @param callback {aio_callback*} �����õ�д�ص�����
	 *  ����ֵΪ�գ���������е�д�ص�����
	 * @return {int} ���ر��ӻص����󼯺��н��õĻص�����ĸ���
	 */
	int disable_write_callback(aio_callback* callback = NULL);

	/**
	 * �������еĻص����󱻵���
	 * @param callback {aio_callback*} ����ָ����д�ص�����
	 *  �����ֵΪ�գ����������е�д�ص�����
	 * @return {int} ���ر����õ�д�ص�����ĸ���
	 */
	int enable_write_callback(aio_callback* callback = NULL);

	/**
	 * �첽д�涨�ֽ��������ݣ�����ȫд�ɹ�������ʱʱ��
	 * �����û�ע��Ļص����������ӳ��첽дʱ������һ������
	 * ���������ô˹���ʱ��ÿ���ӳ��첽д�����ᱻ�����ӳ�д
	 * �Ķ����У��Ա�֤ÿ���ӳ��첽д���������ڸ��ԵĶ�ʱ��
	 * ����ʱ��ִ��
	 * @param data {const void*} ���ݵ�ַ
	 * @param len {int} ���ݳ���
	 * @param delay {int64} �����ֵ > 0 ������ӳٷ��͵�ģʽ(��λΪ΢��)
	 * @param callback {aio_timer_writer*} ��ʱ������ʱ�Ļص����������
	 */
	void write_await(const void* data, int len, long long int delay = 0,
		aio_timer_writer* callback = NULL);

	/**
	 * same as write_await();
	 */
	void write(const void* data, int len, long long int delay = 0,
		aio_timer_writer* callback = NULL)
	{
		write_await(data, len, delay, callback);
	}

	/**
	 * �����ñ��ķ�ʽ��������ʱ���ɵ��ñ�������Ŀ���ַ�������ݰ�
	 * @param data {const void*} ���ݵ�ַ
	 * @param len {int} ���ݳ���
	 * @param dest_addr {const char*} Ŀ���ַ����ʽ��ip|port
	 * @param flags {int} ���ͱ�־λ����ο�ϵͳ sendto() api �� flags ˵��
	 * @return {int} ���� -1 ��ʾ����ʧ��
	 */
	int sendto(const void* data, int len, const char* dest_addr, int flags = 0);

	/**
	 * �����ñ��ķ�ʽ��������ʱ���ɵ��ñ�������Ŀ���ַ�������ݰ�
	 * @param data {const void*} ���ݵ�ַ
	 * @param len {int} ���ݳ���
	 * @param dest_addr {const sockaddr*} Ŀ���ַ����ʽ��ip|port
	 * @param addrlen {int} dest_addr ��ַ����
	 * @param flags {int} ���ͱ�־λ����ο�ϵͳ sendto() api �� flags ˵��
	 * @return {int} ���� -1 ��ʾ����ʧ��
	 */
	int sendto(const void* data, int len,
		const struct sockaddr* dest_addr, int addrlen, int flags = 0);

	/**
	 * �첽������д����, ��������д��ʱ��д�ɹ�ʱ�������¼�֪ͨ���̣�
	 * ����ϵͳ�� writev
	 * @param iov {const struct iovec*} ���ݼ�������
	 * @param count {int} iov ����ĳ���
	 */
	void writev_await(const struct iovec *iov, int count);

	/**
	 * same as writev_await()
	 */
	void writev(const struct iovec *iov, int count)
	{
		writev_await(iov, count);
	}

	/**
	 * ��ʽ����ʽ�첽д���ݣ�����ȫд�ɹ�������ʱʱ��
	 * �����û�ע��Ļص�����
	 * @param fmt {const char*} ��ʽ�ַ���
	 */
	void format_await(const char* fmt, ...) ACL_CPP_PRINTF(2, 3);

	/**
	 * please use format_await() instead
	 */
	void format(const char* fmt, ...) ACL_CPP_PRINTF(2, 3)
	{
		va_list ap;
		va_start(ap, fmt);
		vformat_await(fmt, ap);
		va_end(ap);
	}

	/**
	 * ��ʽ����ʽ�첽д���ݣ�����ȫд�ɹ�������ʱʱ��
	 * �����û�ע��Ļص�����
	 * @param fmt {const char*} ��ʽ�ַ���
	 * @param ap {va_list} ����ֵ�б�
	 */
	void vformat_await(const char* fmt, va_list ap);

	/**
	 * same as vformat_await()
	 */
	void vformat(const char* fmt, va_list ap)
	{
		vformat_await(fmt, ap);
	}

	/**
	 * �첽�ȴ���������д���ú��������첽����д����״̬�����п�дʱ��
	 * �ص����������������û��Լ��������ݵĶ�ȡ
	 * @param timeout {int} д��ʱʱ��(��)������ֵΪ <= 0 ʱ����û��д��ʱ
	 */
	void writable_await(int timeout = 0);

	/**
	 * same as writable_await()
	 */
	void write_wait(int timeout = 0)
	{
		writable_await(timeout);
	}

	/**
	 * ��ֹ�첽�����첽д״̬���򽫸��첽�����첽����ļ��
	 * �¼����Ƴ���ֱ���û������κ�һ��д����ʱ���Զ�����
	 * ��д״̬(��ʱ���������±��첽������)
	 */
	void disable_write(void);

	/**
	 * ��÷��Ͷ��������ݳ���
	 * @return {size_t}
	 */
	size_t pending_length(void) const;

protected:
	virtual ~aio_ostream(void);

	/**
	 * �ͷŶ�̬�������麯��
	 */
	virtual void destroy(void);

	/**
	 * ע��д����
	 */
	void enable_write(void);

private:
	friend class aio_timer_writer;
	std::list<aio_timer_writer*>* timer_writers_;
	std::list<AIO_CALLBACK*> write_callbacks_;

	static int write_callback(ACL_ASTREAM*, void*);
	static int write_wakup(ACL_ASTREAM*, void*);
};

}  // namespace acl
