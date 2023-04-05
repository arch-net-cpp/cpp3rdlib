#pragma once
#include "../acl_cpp_define.hpp"
#include <list>
#include "aio_handle.hpp"
#include "aio_timer_callback.hpp"
#include "aio_stream.hpp"

namespace acl {

class aio_istream;

/**
 * �ӳ��첽�������࣬����Ϊ aio_timer_callback (see aio_handle.hpp)��
 * ��ν�ӳ��첽�������ǰ��첽����(aio_istream)���ڶ�ʱ���У�������
 * �������첽�����������(���� aio_handle �ļ���н��)����ָ��
 * ʱ�䵽����������첽������(�� timer_callback �ص��������½��첽
 * �����첽��������)��ͬʱ�ö�ʱ���Զ�����(���� destroy ����)��
 * ��������û��̳��� aio_timer_reader �࣬�����಻���ڶ��Ϸ���ģ�
 * ��������� destroy������ͬʱ������� destroy ��ִ������Դ�ͷŵ�
 * ��ز������������δ���� destroy���򵱶�ʱ���������ڲ����Զ�����
 * ���� aio_timer_reader �� destroy--��������� delete this����ʱ��
 * �ᵼ�·Ƿ��ڴ��ͷŲ���)
 * 
 */
class ACL_CPP_API aio_timer_reader : public aio_timer_callback {
public:
	aio_timer_reader(void) : in_(NULL) {}

	/**
	 * �� aio_istream �е��ô˺������ͷ����������Ӧ��ʵ�ָú���
	 */
	virtual void destroy(void)
	{
		delete this;
	}
protected:
	virtual ~aio_timer_reader(void) {}
	/**
	 * �ӳٶ�����ʱ�Ļص��������� aio_timer_callback ���м̳ж���
	 */
	virtual void timer_callback(unsigned int id);
private:
	// ���� aio_istream ����ֱ���޸ı����˽�г�Ա����
	friend class aio_istream;

	aio_istream* in_;
	//int   read_delayed_;
	bool  delay_gets_;
	int   delay_timeout_;
	bool  delay_nonl_;
	int   delay_count_;
};

/**
 * �첽���������ඨ�壬����ֻ���ڶ��ϱ�ʵ������������ʱ��Ҫ���� close
 * �������ͷŸ������
 */
class ACL_CPP_API aio_istream : virtual public aio_stream
{
public:
	/**
	 * ���캯��
	 * @param handle {aio_handle*} �첽�¼�������
	 */
	aio_istream(aio_handle* handle);

	/**
	 * ���캯���������첽�������󣬲� hook �����̼��ر�/��ʱ����
	 * @param handle {aio_handle*} �첽�¼�������
	 * @param fd {int} �����׽ӿھ��
	 */
#if defined(_WIN32) || defined(_WIN64)
	aio_istream(aio_handle* handle, SOCKET fd);
#else
	aio_istream(aio_handle* handle, int fd);
#endif

	/**
	 * �����ɶ�ʱ�Ļص������ָ�룬����ûص�������Ѿ����ڣ���ֻ��
	 * ʹ�ö����ڴ򿪿���״̬
	 * @param callback {aio_callback*} �̳� aio_callback ������ص������
	 *  ���첽��������ʱ���ȵ��ô˻ص�������е� read_callback �ӿ�
	 */
	void add_read_callback(aio_callback* callback);

	/**
	 * �Ӷ��ص����󼯺���ɾ��
	 * @param callback {aio_read_callback*} ��ɾ���Ļص�����
	 * ����ֵΪ�գ���ɾ�����еĻص�����
	 * @return {int} ���ر��ӻص����󼯺���ɾ���Ļص�����ĸ���
	 */

	/**
	 * �Ӷ��ص����󼯺���ɾ���ص�����
	 * @param callback {aio_callback*} �� aio_callback �̳е��������ָ�룬
	 *  ����ֵΪ�գ���ɾ�����еĶ��ص�����
	 * @return {int} ���ر��ӻص����󼯺���ɾ���Ļص�����ĸ���
	 */
	int del_read_callback(aio_callback* callback = NULL);

	/**
	 * ��ֹ�ص������༯���е�ĳ���ص�����󣬵������ӻص������
	 * ������ɾ����ֻ�ǲ������ö���
	 * @param callback {aio_callback*} �� aio_callback �̳е��������ָ�룬
	 *  ����ֵΪ�գ����ֹ���еĶ��ص�����
	 * @return {int} ���ر��ӻص����󼯺��н��õĻص�����ĸ���
	 */
	int disable_read_callback(aio_callback* callback = NULL);

	/**
	 * �������еĻص����󱻵���
	 * @param callback {aio_callback*} �� aio_callback �̳е��������ָ�룬
	 *  ����ֵΪ�գ����������еĶ��ص�����
	 * @return {int} ���ر����õĻص�����ĸ���
	 */
	int enable_read_callback(aio_callback* callback = NULL);

	/**
	 * �첽��ȡһ�����ݣ����ӳ��첽��ʱ������������ô˹��̣�
	 * ��ֻ�����һ���ӳٶ�������Ч
	 * @param timeout {int} ����ʱʱ��(��)����Ϊ 0 ���ʾ
	 *  ��Զ�ȴ�ֱ����������һ�����ݻ����
	 * @param nonl {bool} �Ƿ��Զ�ȥ��β���Ļس����з�
	 * @param delay {long long int} ����Է��������ݱȽϿ�ʱ���˲���
	 *  ���� 0 ʱ�����ӳٽ��նԷ������ݣ���ֵ�����ӳٶ�����
	 *  ��ʱ��(��λΪ΢��)
	 * @param callback {aio_timer_reader*} ��ʱ������ʱ�Ļص����������
	 *  �� delay > 0�������ֵΪ�գ������ȱʡ�Ķ���
	 */
	void gets_await(int timeout = 0, bool nonl = true,
		long long int delay = 0, aio_timer_reader* callback = NULL);

	/**
	 * same as gets_await();
	 */
	void gets(int timeout = 0, bool nonl = true,
		long long int delay = 0, aio_timer_reader* callback = NULL)
	{
		gets_await(timeout, nonl, delay, callback);
	}

	/**
	 * �첽��ȡ���ݣ����ӳ��첽��ʱ������������ô˹��̣�
	 * ��ֻ�����һ���ӳٶ�������Ч
	 * @param count {int} ��Ҫ������������������Ϊ 0 ��ֻҪ������
	 *  �ɶ��ͷ��أ�����ֱ������ʱ���������������Ҫ����ֽ���
	 * @param timeout {int} ����ʱʱ��(��)����Ϊ 0 ���ʾ
	 *  ��Զ�ȴ�ֱ��������Ҫ������ݻ����
	 * @param delay {long long int} ����Է��������ݱȽϿ�ʱ���˲���
	 *  ���� 0 ʱ�����ӳٽ��նԷ������ݣ���ֵ�����ӳٶ�����
	 *  ��ʱ��(��λΪ΢��)
	 * @param callback {aio_timer_reader*} ��ʱ������ʱ�Ļص����������
	 *  �����ֵΪ�գ������ȱʡ�Ķ���
	 */
	void read_await(int count = 0, int timeout = 0,
		long long int delay = 0, aio_timer_reader* callback = NULL);

	/**
	 * same as read_await()
	 */
	void read(int count = 0, int timeout = 0,
		long long int delay = 0, aio_timer_reader* callback = NULL)
	{
		read_await(count, timeout, delay, callback);
	}

	/**
	 * �첽�ȴ��������ɶ����ú��������첽���Ķ�����״̬���������ݿɶ�
	 * ʱ���ص����������������û��Լ��������ݵĶ�ȡ
	 * @param timeout {int} ����ʱʱ��(��)������ֵΪ 0 ʱ����û�ж���ʱ
	 */
	void readable_await(int timeout = 0);

	/**
	 * same as readable_await()
	 */
	void read_wait(int timeout = 0)
	{
		readable_await(timeout);
	}

	/**
	 * ��ֹ�첽�����첽��״̬�������첽�����첽����ļ����
	 * �Ƴ���ֱ���û������κ�һ���첽������(��ʱ���첽�����
	 * �Զ����¼�ظ����Ŀɶ�״̬)
	 */
	void disable_read(void);

	/**
	 * �������Ƿ�������Ӷ�����
	 * @param onoff {bool}
	 */
	void keep_read(bool onoff);

	/**
	 * ������Ƿ�������������������
	 * @return {bool}
	 */
	bool keep_read(void) const;

	/**
	 * ���ý��ջ���������󳤶ȣ��Ա��⻺���������Ĭ��ֵΪ 0 ��ʾ������
	 * @param max {int}
	 * @return {aio_istream&}
	 */
	aio_istream& set_buf_max(int max);

	/**
	 * ��õ�ǰ���ջ���������󳤶�����
	 * @return {int} ����ֵ  <= 0 ��ʾû������
	 */
	int get_buf_max(void) const;

	/**
	 * ����ڲ��� read_ready flag λ���� IO �����⵽ IO �ɶ�ʱ��������
	 * read_ready ��־λ�����һ��ᶨ�ڼ��ñ�־λ�Ƿ����ã��Ծ����Ƿ���
	 * Ҫ�ٴδ������ص����̣�ͨ��������������ñ�־λ��IO ����Ͳ�����
	 * Ӧ�ò���þ���Ƿ�ɶ������ǽ��� OS ȥ�ж��Ƿ�ɶ�
	 */
	void clear_read_ready(void);

protected:
	virtual ~aio_istream(void);

	/**
	 * �ͷŶ�̬�������麯��
	 */
	virtual void destroy(void);

	/**
	 * ע��ɶ��Ļص�����
	 */
	void enable_read(void);

private:
	friend class aio_timer_reader;
	aio_timer_reader* timer_reader_;
	std::list<AIO_CALLBACK*> read_callbacks_;

	static int read_callback(ACL_ASTREAM*,  void*, char*, int);
	static int read_wakeup(ACL_ASTREAM* stream, void* ctx);
};

}  // namespace acl
