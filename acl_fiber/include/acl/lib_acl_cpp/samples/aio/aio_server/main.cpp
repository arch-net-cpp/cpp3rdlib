#include <iostream>
#include <assert.h>
#include "lib_acl.h" // just for getopt on Windows
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/acl_cpp_init.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/aio_istream.hpp"
#include "acl_cpp/stream/aio_listen_stream.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"

static int   __max = 0;
static int   __timeout = 0;

/**
 * �ӳٶ��ص�������
 */
class timer_reader: public acl::aio_timer_reader
{
public:
	timer_reader(int delay)
	{
		delay_ = delay;
		std::cout << "timer_reader init, delay: " << delay << std::endl;
	}

protected:
	~timer_reader(void) {}

	// aio_timer_reader ������������� destroy ����
	// @override
	void destroy(void)
	{
		std::cout << "timer_reader delete, delay: "  << delay_ << std::endl;
		delete this;
	}

	// ���ػ���ص�����
	// @override
	void timer_callback(unsigned int id)
	{
		std::cout << "timer_reader(" << id
			<< "): timer_callback, delay: " << delay_ << std::endl;

		// ���û���Ĵ������
		aio_timer_reader::timer_callback(id);
	}

private:
	int   delay_;
};

/**
 * �ӳ�д�ص�������
 */
class timer_writer: public acl::aio_timer_writer
{
public:
	timer_writer(int delay)
	{
		delay_ = delay;
		std::cout << "timer_writer init, delay: " << delay << std::endl;
	}

protected:
	~timer_writer(void) {}

	// aio_timer_reader ������������� destroy ����
	// @override
	void destroy(void)
	{
		std::cout << "timer_writer delete, delay: " << delay_ << std::endl;
		delete this;
	}

	// ���ػ���ص�����
	// @override
	void timer_callback(unsigned int id)
	{
		std::cout << "timer_writer(" << id << "): timer_callback, delay: "
			<< delay_ << std::endl;

		// ���û���Ĵ������
		acl::aio_timer_writer::timer_callback(id);
	}

private:
	int   delay_;
};

/**
 * �첽�ͻ������Ļص��������
 */
class io_callback : public acl::aio_callback
{
public:
	io_callback(acl::aio_socket_stream* client)
	: client_(client)
	, i_(0) {}

protected:
	~io_callback(void)
	{
		std::cout << "delete io_callback now ..." << std::endl;
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ������Ķ��ɹ��ص�����
	 * @param data {char*} ���������ݵ�ַ
	 * @param len {int} ���������ݳ���
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	bool read_callback(char* data, int len)
	{
		i_++;
		if (i_ < 5) {
			std::cout << ">>gets(i:" << i_ << "): "
				<< data << std::endl;
		}

		// ���Զ�̿ͻ���ϣ���˳�����ر�֮
		if (strncasecmp(data, "quit", 4) == 0) {
			client_->format("Bye!\r\n");
			client_->close();
			return false;
		}

		// ���Զ�̿ͻ���ϣ�������Ҳ�رգ�����ֹ�첽�¼�����
		else if (strncasecmp(data, "stop", 4) == 0) {
			client_->format("Stop now!\r\n");
			client_->close();  // �ر�Զ���첽��

			// ֪ͨ�첽����ر�ѭ������
			client_->get_handle().stop();
		}

		// ��Զ�̿ͻ��˻�д�յ�������

		int   delay = 0;

		if (!strncasecmp(data, "write_delay", strlen("write_delay"))) {
			// �ӳ�д����

			const char* ptr = data + strlen("write_delay");
			delay = atoi(ptr);
			if (delay > 0) {
				std::cout << ">> write delay " << delay
					<< " second ..." << std::endl;
				timer_writer* timer = new timer_writer(delay);
				client_->write(data, len, delay * 1000000, timer);
				client_->gets(10, false);
				return true;
			}
		} else if (!strncasecmp(data, "read_delay", strlen("read_delay"))) {
			// �ӳٶ�����

			const char* ptr = data + strlen("read_delay");
			delay = atoi(ptr);
			if (delay > 0) {
				client_->write(data, len);
				std::cout << ">> read delay " << delay
					<< " second ..." << std::endl;
				timer_reader* timer = new timer_reader(delay);
				client_->gets(10, false, delay * 1000000, timer);
				return true;
			}
		}

		client_->write(data, len);
		//client_->gets(10, false);
		return true;
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ�������д�ɹ��ص�����
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	bool write_callback(void)
	{
		return true;
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ������ĳ�ʱ�ص�����
	 */
	void close_callback(void)
	{
		// �����ڴ˴�ɾ���ö�̬����Ļص�������Է�ֹ�ڴ�й¶
		delete this;
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ������ĳ�ʱ�ص�����
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	bool timeout_callback(void)
	{
		std::cout << "Timeout, delete it ..." << std::endl;
		return (false);
	}

private:
	acl::aio_socket_stream* client_;
	int  i_;
};

/**
 * �첽�������Ļص��������
 */
class io_accept_callback : public acl::aio_accept_callback
			 , public acl::aio_listen_callback
{
public:
	io_accept_callback(void) {}
	~io_accept_callback(void)
	{
		printf(">>io_accept_callback over!\n");
	}

	/**
	 * ���� aio_accept_callback �麯�������������ӵ������ô˻ص�����
	 * @param client {aio_socket_stream*} �첽�ͻ�����
	 * @return {bool} ���� true ��֪ͨ��������������
	 */
	bool accept_callback(acl::aio_socket_stream* client)
	{
		printf("proactor accept one\r\n");
		return handle_client(client);
	}

	/**
	 * ���� aio_listen_callback �麯�������������ӵ������ô˻ص�����
	 * @param server {acl::aio_listen_stream&} �첽������
	 * @return {bool}
	 */
	bool listen_callback(acl::aio_listen_stream& server)
	{
		// reactor ģʽ����Ҫ�û��Լ����� accept ����
		acl::aio_socket_stream* client = server.accept();
		if (client == NULL) {
			printf("accept error %s\r\n", acl::last_serror());
			return false;
		}

		printf("reactor accept one\r\n");
		return handle_client(client);
	}

private:
	bool handle_client(acl::aio_socket_stream* client)
	{
		// �����첽�ͻ������Ļص���������첽�����а�
		io_callback* callback = new io_callback(client);

		// ע���첽���Ķ��ص�����
		client->add_read_callback(callback);

		// ע���첽����д�ص�����
		client->add_write_callback(callback);

		// ע���첽���Ĺرջص�����
		client->add_close_callback(callback);

		// ע���첽���ĳ�ʱ�ص�����
		client->add_timeout_callback(callback);

		// ���޶�����������󳤶�ʱ
		if (__max > 0) {
			client->set_buf_max(__max);
		}

		// ���첽����һ������
		client->gets(__timeout, false);
		return true;
	}
};

class mytimer : public acl::aio_timer_callback {
public:
	mytimer(long long delay) : id_(0), last_(time(NULL)), delay_(delay) {}
	~mytimer(void) {}

protected:
	// @override
	void destroy(void) {
		delete this;
	}

	// @override
	void timer_callback(unsigned int id) {
		time_t now = time(NULL);
		delay_ += 1000000;
		this->set_task(id_, delay_);

		printf("timer id=%u, delay=%ld, next delay=%lld\r\n",
			id, (long) (now - last_), delay_);

		last_ = now;
		id_ = id;
	}

private:
	unsigned int id_;
	time_t last_;
	long long delay_;
};

static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n"
		"	-l ip:port\r\n"
		"	-L line_max_length\r\n"
		"	-t timeout\r\n"
		"	-r [use reactor mode other proactor mode, default: proactor mode]\r\n"
		"	-k[use kernel event: epoll/iocp/kqueue/devpool]\r\n",
		procname);
}

int main(int argc, char* argv[])
{
	bool use_kernel = false, use_reactor = false;
	int  ch;
	acl::string addr(":9001");

	while ((ch = getopt(argc, argv, "l:hkL:t:r")) > 0) {
		switch (ch) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 'l':
			addr = optarg;
			break;
		case 'k':
			use_kernel = true;
			break;
		case 'L':
			__max = atoi(optarg);
			break;
		case 't':
			__timeout = atoi(optarg);
			break;
		case 'r':
			use_reactor = true;
			break;
		default:
			break;
		}
	}

	// ��ʼ��ACL��(��������WIN32��һ��Ҫ���ô˺�������UNIXƽ̨�¿ɲ�����)
	acl::acl_cpp_init();

	acl::log::stdout_open(true);

	// �����첽���������
	acl::aio_handle handle(use_kernel ? acl::ENGINE_KERNEL : acl::ENGINE_SELECT);

	long long delay = 1000000;
	mytimer* timer = new mytimer(delay);
	timer->keep_timer(true);
	handle.set_timer(timer, delay);

	// ���������첽��
	acl::aio_listen_stream* sstream = new acl::aio_listen_stream(&handle);

	// ����ָ���ĵ�ַ
	if (!sstream->open(addr.c_str())) {
		std::cout << "open " << addr.c_str() << " error!" << std::endl;
		sstream->close();
		// XXX: Ϊ�˱�֤�ܹرռ�������Ӧ�ڴ˴��� check һ��
		handle.check();

		getchar();
		return 1;
	}

	// �����ص�����󣬵��������ӵ���ʱ�Զ����ô������Ļص�����
	io_accept_callback callback;

	if (use_reactor) {
		sstream->add_listen_callback(&callback);
	} else {
		sstream->add_accept_callback(&callback);
	}

	std::cout << "Listen: " << addr.c_str() << " ok!" << std::endl;

	while (true) {
		// ������� false ���ʾ���ټ�������Ҫ�˳�
		if (!handle.check()) {
			std::cout << "aio_server stop now ..." << std::endl;
			break;
		}
	}

	// �رռ��������ͷ�������
	sstream->close();

	// XXX: Ϊ�˱�֤�ܹرռ�������Ӧ�ڴ˴��� check һ��
	handle.check();

	return 0;
}
