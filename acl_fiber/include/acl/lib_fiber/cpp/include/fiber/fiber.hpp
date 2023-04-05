#pragma once
#include <stddef.h>
#include <string>
#include <vector>
#include "fiber_cpp_define.hpp"

struct ACL_FIBER;

namespace acl {

typedef enum 
{
	FIBER_EVENT_T_KERNEL,	// Linux: epoll, FreeBSD: kquque, Windows: iocp
	FIBER_EVENT_T_POLL,	// Linux, FreeBSD, MacOS, Windows
	FIBER_EVENT_T_SELECT,	// Linux, FreeBSD, MacOS, Windows
	FIBER_EVENT_T_WMSG,	// Windows
	FIBER_EVENT_T_IO_URING,	// Linux
} fiber_event_t;

struct FIBER_CPP_API fiber_frame
{
	fiber_frame(void) : pc(0), off(0) {}
	~fiber_frame(void) {}

	std::string func;
	long pc;
	long off;
};

/**
 * Э���ඨ�壬�����࣬��Ҫ����̳в�ʵ�ִ��鷽��
 */
class FIBER_CPP_API fiber
{
public:
	fiber(void);
	fiber(ACL_FIBER *fb);

	/**
	 * ���캯��
	 * @param running {bool} ��Ϊ true ʱ�����ʾ��ǰЭ������������������
	 *  ��һ��Э�̶�����ѣ��Ա����� ACL_FIBER ����󶨣���ʱ��ֹ���ñ���
	 *  ��� start ����������Э��; ��Ϊ false ʱ������Ҫ���� start ������
	 *  ������Э��
	 */
	fiber(bool running);

	virtual ~fiber(void);

	/**
	 * �ڴ���һ��Э��������ҹ������ running Ϊ false ʱ����Ҫ����������
	 * Э�̣�Ȼ����������ص� run ���������ص������ running Ϊ true ʱ��
	 * ���ֹ���� start ����
	 * @param stack_size {size_t} ������Э�̶����ջ��С
	 * @param share_stack {bool} �Ƿ���ù���ջ��ʽ(��Ҫ���ù���ջ��ʽ��
	 *  �����ڱ��� libfiber.a ʱ�����뿪�� SHARE_STACK ��)
	 */
	void start(size_t stack_size = 320000, bool share_stack = false);

	/**
	 * �ڱ�Э������ʱ���ô˺���֪ͨ��Э���˳�
	 * @return {bool} ���� false ��ʾ��Э��δ�������Ѿ��˳�
	 */
	bool kill(void);

	/**
	 * �жϵ�ǰЭ���Ƿ�֪ͨ�˳�
	 * @return {bool} ��Э���Ƿ�֪ͨ�˳�
	 */
	bool killed(void) const;

	/**
	 * �жϵ�ǰ�������е�Э���Ƿ�֪ͨ�˳����÷����� killed ������Ϊ��
	 * killed ���ȱ����� acl::fiber �������У��Ҹ�Э�̶����п����������У�
	 * Ҳ�п��ܱ����𣬶� self_killed ����Ҫ acl::fiber ����������һ����ʾ
	 * ��ǰ�������е�Э��
	 * @return {bool}
	 */
	static bool self_killed(void);

	/**
	 * ��ñ�Э�̶���� ID ��
	 * @return {unsigned int}
	 */
	unsigned int get_id(void) const;

	/**
	 * ��õ�ǰ���е�Э�̶���� ID ��
	 * @return {unsigned int}
	 */
	static unsigned int self(void);

	/**
	 * ���ָ��Э�̶����ID��
	 * @return {unsigned int}
	 */
	static unsigned int fiber_id(const fiber& fb);

	/**
	 * ��õ�ǰЭ����ִ��ĳ��ϵͳ API ����ʱ�Ĵ����
	 * return {int}
	 */
	int get_errno(void) const;

	/**
	 * ���õ�ǰЭ�̵Ĵ����
	 * @param errnum {int}
	 */
	void set_errno(int errnum);

public:
	/**
	 * ��ñ��β����ĳ�����Ϣ
	 * @return {const char*}
	 */
	static const char* last_serror(void);

	/**
	 * ��ñ��β����ĳ����
	 * @return {int}
	 */
	static int last_error(void);

	/**
	 * �����������ת��������Ϣ
	 * @param errnum {int} �����
	 * @param buf {char*} �洢���
	 * @param size {size_t} buf �ռ��С
	 * @return {const char*} buf ��ַ
	 */
	static const char* strerror(int errnum, char* buf, size_t size);

	/**
	 * ��������Ϣ�������׼���
	 * @param on {bool} Ϊ true ʱ���ڲ�������Ϣ���������׼���
	 */
	static void stdout_open(bool on);

	/**
	 * ��ʽ����Э�̵����¼��������ͣ�ͬʱ����Э�̵�����Ϊ������ģʽ������
	 * ����Э�̺󲻱���ʽ���� schedule �� schedule_with ������Э�̵�����
	 * @param type {fiber_event_t} �¼��������ͣ��μ���FIBER_EVENT_T_XXX
	 * @param schedule_auto {bool} ��Ϊ true���򴴽�Э�̶�������и�Э��
	 *  ����󲻱���ʽ���� schedule/schedule_with ���������е�Э�̹��̣���
	 *  �����Զ�����Э�̵������������ڴ���������Э�̺󣬱�����ʽ�ص���
	 *  schedule �� schedule_with ��ʽ������Э�̵���������������Э�̹��̣�
	 *  �ڲ�ȱʡ״̬Ϊ false
	 */
	static void init(fiber_event_t type, bool schedule_auto = false);

	/**
	 * ����Э�����еĵ��ȹ���
	 * @param type {fiber_event_t} �¼��������ͣ��μ���FIBER_EVENT_T_XXX
	 */
	static void schedule(fiber_event_t type = FIBER_EVENT_T_KERNEL);

	/**
	 * ����Э�̵���ʱָ���¼��������ͣ����ñ���������ͬʱ������ schedule_init
	 * �� schedule ��������
	 * @param type {fiber_event_t} �¼��������ͣ��μ���FIBER_EVENT_T_XXX
	 */
	static void schedule_with(fiber_event_t type);

	/**
	 * �жϵ�ǰ�߳��Ƿ���Э�̵���״̬
	 * @return {bool}
	 */
	static bool scheduled(void);

	/**
	 *  ֹͣЭ�̵��ȹ���
	 */
	static void schedule_stop(void);

public:
	/**
	 * ����ǰ�������е�Э��(����Э��) ����
	 */
	static void yield(void);

	/**
	 * ����ǰЭ�̣�ִ�еȴ������е���һ��Э��
	 */
	static void switch_to_next(void);

	/**
	 * ��ָ��Э�̶�����������ж�����
	 * @param f {fiber&}
	 */
	static void ready(fiber& f);

	/**
	 * ʹ��ǰ���е�Э������ָ��������
	 * @param milliseconds {unsigned int} ָ��Ҫ���ߵĺ�����
	 * @return {unsigned int} ��Э�����ߺ��ٴα����Ѻ�ʣ��ĺ�����
	 */
	static unsigned int delay(unsigned int milliseconds);

	/**
	 * ��ô��ڴ��״̬��Э������
	 * @return {unsigned}
	 */
	static unsigned alive_number(void);

	/**
	 * ��ô����˳�״̬��Э�̶�������
	 * @return {unsigned}
	 */
	static unsigned dead_number(void);

	/**
	 * ���ñ��߳�������Э�������ӷ����ʱ�������˴���ʱ�ķ�������ʽ������Windows)
	 * @param yes {bool}
	 */
	static void set_non_blocking(bool yes);

	/**
	 * �����ù���ջģʽ�����ù���ջ�Ĵ�С,�ڲ�ȱʡֵΪ 1024000 �ֽ�
	 * @param size {size_t} ����ջ�ڴ��С
	 */
	static void set_shared_stack_size(size_t size);

	/**
	 * �����ù���ջģʽ�»�ù���ջ��С
	 * @return {size_t} ������� 0 ���ʾδ���ù���ջ��ʽ
	 */
	static size_t get_shared_stack_size(void);

	/**
	 * ��ʽ���ñ�����ʹ acl ������� IO ����Э�̻����� UNIX ƽ̨�²�����ʽ
	 * ���ñ���������Ϊ�ڲ����Զ� HOOK IO API
	 */
	static void acl_io_hook(void);

	/**
	 * ���ñ�����ȡ�� acl�������е� IO Э�̻�
	 */
	static void acl_io_unlock(void);

	/**
	 * Windows ƽ̨�¿�����ʽ�ص��ô˺��� Hook һЩ������Э����ص�ϵͳ API
	 * @return {bool}
	 */
	static bool winapi_hook(void);

	/**
	 * ��õ�ǰϵͳ�������
	 * @return {int}
	 */
	static int  get_sys_errno(void);

	/**
	 * ���õ�ǰϵͳ�������
	 * @param errnum {int}
	 */
	static void set_sys_errno(int errnum);

public:
	/**
	 * ���ر�Э�̶����Ӧ�� C ���Ե�Э�̶���
	 * @return {ACL_FIBER* }
	 */
	ACL_FIBER* get_fiber(void) const;

	/**
	 * �ײ���� C API ����Э��
	 * @param fn {void (*)(ACL_FIBER*, void*)} Э�̺���ִ�����
	 * @param ctx {void*} ���ݸ�Э��ִ�к����Ĳ���
	 * @param size {size_t} Э��ջ��С
	 */
	static void fiber_create(void (*fn)(ACL_FIBER*, void*),
			void* ctx, size_t size);

	/**
	 * ���ָ��Э�̵Ķ�ջ
	 * @param fb {const fiber&}
	 * @param out {std::vector<stack_frame>&} ��Ž������
	 * @param max {size_t} ָ����ȡջ��������
	 */
	static void stacktrace(const fiber& fb, std::vector<fiber_frame>& out,
			size_t max = 50);

	/**
	 * ���ָ��Э�̵�ջ����׼���
	 * @param fb {const fiber&}
	 * @param max {size_t} ������ʾջ��������
	 */
	static void stackshow(const fiber& fb, size_t max = 50);

protected:
	/**
	 * �麯����������ʵ�ֱ���������ͨ������ start ��������Э�̺󣬱�
	 * �麯�����ᱻ���ã��Ӷ�֪ͨ����Э��������; ����ڹ��캯���еĲ���
	 * running Ϊ true ���� start ������ֹ���ã��ʱ��鷽��Ҳ���ᱻ����
	 */
	virtual void run(void);

private:
	ACL_FIBER* f_;

	fiber(const fiber&);
	void operator = (const fiber&);

	static void fiber_callback(ACL_FIBER* f, void* ctx);
};

/**
 * ��������ʱ����Э����
 */
class FIBER_CPP_API fiber_timer
{
public:
	fiber_timer(void);
	virtual ~fiber_timer(void) {}

	/**
	 * ����һ��Э�̶�ʱ��
	 * @param milliseconds {unsigned int} ���뼶ʱ��
	 * @param stack_size {size_t} Э�̵�ջ�ռ��С
	 */
	void start(unsigned int milliseconds, size_t stack_size = 320000);

protected:
	/**
	 * �������ʵ�ָô��鷽��������ʱ������ʱ��ص��÷���
	 */
	virtual void run(void) = 0;

private:
	ACL_FIBER* f_;

	fiber_timer(const fiber_timer&);
	void operator = (const fiber_timer&);

	static void timer_callback(ACL_FIBER* f, void* ctx);
};

#if defined(ACL_CPP_API)

/**
 * ��ʱ������Э��
 */
template <typename T>
class fiber_trigger : public fiber
{
public:
	fiber_trigger(timer_trigger<T>& timer)
	: delay_(100)
	, stop_(false)
	, timer_(timer)
	{
	}

	virtual ~fiber_trigger(void) {}

	void add(T* o)
	{
		mbox_.push(o);
	}

	void del(T* o)
	{
		timer_.del(o);
	}

	timer_trigger<T>& get_trigger(void)
	{
		return timer_;
	}

	// @override
	void run(void)
	{
		while (!stop_) {
			T* o = mbox_.pop(delay_);
			if (o)
				timer_.add(o);

			long long next = timer_.trigger();
			long long curr = get_curr_stamp();
			if (next == -1)
				delay_ = 100;
			else {
				delay_ = next - curr;
				if (delay_ <= 0)
					delay_ = 1;
			}
		}
	}

private:
	long long delay_;
	bool stop_;

	timer_trigger<T>& timer_;
	mbox<T> mbox_;
};

#endif // ACL_CPP_API

} // namespace acl
