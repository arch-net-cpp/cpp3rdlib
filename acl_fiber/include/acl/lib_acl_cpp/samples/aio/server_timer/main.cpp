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

class io_callback;

class read_timer : public acl::aio_timer_callback
{
public:
	read_timer(io_callback& io) : io_(io) {};

protected:
	~read_timer(void) {}

	// @override
	void destroy(void);

	// @override
	void timer_callback(unsigned int id);

private:
	io_callback& io_;
};

/**
 * �첽�ͻ������Ļص��������
 */
class io_callback : public acl::aio_callback
{
public:
	io_callback(acl::aio_socket_stream* client)
	: client_(client)
	, timer_(NULL)
	, i_(0) {}

	void close(void) {
		if (timer_) {
			client_->get_handle().del_timer(timer_);
			timer_ = NULL;
		}
		//client_->close();
	}

protected:
	~io_callback(void) {
		if (timer_) {
			client_->get_handle().del_timer(timer_);
		}
		std::cout << "delete io_callback now ..." << std::endl;
	}

	/**
	 * @override
	 * @param data {char*} ���������ݵ�ַ
	 * @param len {int} ���������ݳ���
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	bool read_callback(char* data, int len) {
		if (timer_) {
			client_->get_handle().del_timer(timer_);
		}
		timer_ = new read_timer(*this);
		long long delay = 1000;
		client_->get_handle().set_timer(timer_, delay, 0);

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

		client_->write(data, len);
		return true;
	}

	/**
	 * @override
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	bool write_callback(void) {
		return true;
	}

	/**
	 * @override
	 */
	void close_callback(void) {
		// �����ڴ˴�ɾ���ö�̬����Ļص�������Է�ֹ�ڴ�й¶
		delete this;
	}

	/**
	 * @override
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	bool timeout_callback(void) {
		std::cout << "Timeout, delete it ..." << std::endl;
		return (false);
	}

private:
	acl::aio_socket_stream* client_;
	read_timer* timer_;
	int  i_;
};

void read_timer::destroy(void) {
	//printf("timer %p deleted\r\n", this);
	delete this;
}

void read_timer::timer_callback(unsigned int) {
	io_.close();
}

/**
 * �첽�������Ļص��������
 */
class io_accept_callback : public acl::aio_accept_callback
			 , public acl::aio_listen_callback
{
public:
	io_accept_callback(void) {}
	~io_accept_callback(void) {
		printf(">>io_accept_callback over!\n");
	}

	/**
	 * @override
	 * @param client {aio_socket_stream*} �첽�ͻ�����
	 * @return {bool} ���� true ��֪ͨ��������������
	 */
	bool accept_callback(acl::aio_socket_stream* client) {
		printf("proactor accept one\r\n");
		return handle_client(client);
	}

	/**
	 * @override
	 * @param server {acl::aio_listen_stream&} �첽������
	 * @return {bool}
	 */
	bool listen_callback(acl::aio_listen_stream& server) {
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
	bool handle_client(acl::aio_socket_stream* client) {
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

class mytimer : public acl::aio_timer_callback
{
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
	//handle.set_timer(timer, delay);

	// ���������첽��
	acl::aio_listen_stream* sstream = new acl::aio_listen_stream(&handle);

	// ����ָ���ĵ�ַ
	if (!sstream->open(addr.c_str())) {
		std::cout << "open " << addr.c_str() << " error!" << std::endl;
		sstream->destroy();
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
