#include <iostream>
#include <assert.h>
#include "lib_acl.h"
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/stdlib/snprintf.hpp"
#include "acl_cpp/acl_cpp_init.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stream/polarssl_conf.hpp"
#include "acl_cpp/stream/polarssl_io.hpp"
#include "acl_cpp/stream/mbedtls_conf.hpp"
#include "acl_cpp/stream/mbedtls_io.hpp"
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/aio_istream.hpp"
#include "acl_cpp/stream/aio_listen_stream.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"
#include "acl_cpp/stream/openssl_conf.hpp"

static int   __max = 0;
static int   __timeout = 0;
static int   __max_used = 0;
static int   __cur_used = 0;

// SSL ģʽ�µ� SSL ���ö���
static acl::sslbase_conf* __ssl_conf;

/**
 * �첽�ͻ������Ļص��������
 */
class io_callback : public acl::aio_callback
{
public:
	io_callback(acl::aio_socket_stream* client)
	: client_(client)
	, nread_cnt_(0)
	, nread_(0)
	{
	}

	~io_callback()
	{
		printf("delete io_callback now ...\r\n");
		__cur_used++;
	}

	/**
	 * ���ص��麯�����ûص��������������� aio_istream ʵ���е�
	 * gets/read �Ŀɶ������󱻵��ã����첽����ڲ���������������
	 * �ݶ�����ֱ�Ӵ��ݸ��û�������
	 * @param data {char*} ���������ݵ�ָ���ַ
	 * @param len {int} ���������ݳ���(> 0)
	 * @return {bool} �ú������� false ֪ͨ�첽����رո��첽��
	 */
	bool read_wakeup()
	{
		acl::sslbase_io* hook = (acl::sslbase_io*) client_->get_hook();
		if (hook == NULL) {
			// �� SSL ģʽ���첽��ȡ����
			//client_->read(__timeout);
			client_->gets(__timeout, false);
			return true;
		}

		// ���Խ��� SSL ����
		if (!hook->handshake()) {
			printf("%s: ssl handshake failed\r\n", __FUNCTION__);
			return false;
		}

		// ��� SSL �����Ѿ��ɹ�����ʼ���ж�����
		if (hook->handshake_ok()) {
			// �� reactor ģʽתΪ proactor ģʽ���Ӷ�ȡ��
			// read_wakeup �ص�����
			client_->disable_read();

			// �첽��ȡ���ݣ�����ص� read_callback
			//client_->read(__timeout);
			client_->gets(__timeout, false);
			return true;
		}

		// SSL ���ֻ�δ��ɣ��ȴ��������ٴα�����
		return true;
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ������Ķ��ɹ��ص�����
	 * @param data {char*} ���������ݵ�ַ
	 * @param len {int} ���������ݳ���
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	bool read_callback(char* data, int len)
	{
		nread_cnt_++;
		nread_ += len;

		if (nread_cnt_ <= 100 || nread_cnt_ % 1000 == 0) {
			char  buf[256];
			acl::safe_snprintf(buf, sizeof(buf),
				"read len: %d, total read: %d, nread_cnt: %d",
				len, nread_, nread_cnt_);
			acl::meter_time(__FILE__, __LINE__, buf);
		}

		// ���Զ�̿ͻ���ϣ���˳�����ر�֮
		if (strncasecmp(data, "quit", 4) == 0) {
			client_->format("Bye!\r\n");
			client_->close();
			return true;
		}

		// ���Զ�̿ͻ���ϣ�������Ҳ�رգ�����ֹ�첽�¼�����
		if (strncasecmp(data, "stop", 4) == 0) {
			client_->format("Stop now!\r\n");
			client_->close();  // �ر�Զ���첽��

			// ֪ͨ�첽����ر�ѭ������
			client_->get_handle().stop();
		}

		//printf(">>data=%s\n", data);
		// ��Զ�̿ͻ��˻�д�յ�������

		client_->write(data, len);
		return true;
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ�������д�ɹ��ص�����
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	bool write_callback()
	{
		return true;
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ������ĳ�ʱ�ص�����
	 */
	void close_callback()
	{
		// �����ڴ˴�ɾ���ö�̬����Ļص�������Է�ֹ�ڴ�й¶
		delete this;
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ������ĳ�ʱ�ص�����
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	bool timeout_callback()
	{
		std::cout << "Timeout, delete it ..." << std::endl;
		return false;
	}

private:
	acl::aio_socket_stream* client_;
	int   nread_cnt_;
	int   nread_;
};

/**
 * �첽�������Ļص��������
 */
class io_accept_callback : public acl::aio_accept_callback
{
public:
	io_accept_callback() {}
	~io_accept_callback()
	{
		printf(">>io_accept_callback over!\n");
	}

	/**
	 * �����麯�������������ӵ������ô˻ص�����
	 * @param client {aio_socket_stream*} �첽�ͻ�����
	 * @return {bool} ���� true ��֪ͨ��������������
	 */
	bool accept_callback(acl::aio_socket_stream* client)
	{
		//acl_non_blocking(client->sock_handle(), ACL_BLOCKING);

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
		if (__max > 0)
			client->set_buf_max(__max);

		// SSL ģʽ�£��ȴ��ͻ��˷���������Ϣ
		if (__ssl_conf != NULL) {
			acl::sslbase_io* ssl = __ssl_conf->create(true);

			// ע�� SSL IO ���̵Ĺ���
			if (client->setup_hook(ssl) == ssl) {
				std::cout << "setup_hook error" << std::endl;
				ssl->destroy();
				return false;
			}

			// ���ͻ������ڶ�����״̬�Դ��� read_wakeup �ص����̣�
			// SSL ���ֹ��̽��� read_wakeup �����
			printf(">>>begin wait for ssl handshake\r\n");
			client->read_wait(__timeout);
		}

		// �� SSL ģʽ�£����첽����һ������
		else {
			//client->read(__timeout);
			client->gets(__timeout, false);
		}

		return true;
	}
};

static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n"
		"  -d path_to_polarssl_or_mbedtls\r\n"
		"  -l server_addr[ip:port, default: 0.0.0.0:9800]\r\n"
		"  -L line_max_length\r\n"
		"  -t timeout\r\n"
		"  -n conn_used_limit\r\n"
		"  -k[use kernel event: epoll/iocp/kqueue/devpool]\r\n"
		"  -M delay_ms\r\n"
		"  -I check_fds_inter\r\n"
		"  -K ssl_key_file -C ssl_cert_file [in SSL mode]\r\n",
		procname);
}

int main(int argc, char* argv[])
{
	// �¼������Ƿ�����ں��еĸ�Чģʽ
	bool use_kernel = false, use_polarssl = true;
	acl::string key_file, cert_file, libpath;
	acl::string addr("0.0.0.0:9800");
	int  ch, delay_ms = 100, check_fds_inter = 10;

	while ((ch = getopt(argc, argv, "l:d:hkL:t:K:C:n:M:I:")) > 0) {
		switch (ch) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 'd':
			libpath = optarg;
			break;
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
		case 'K':
			key_file = optarg;
			break;
		case 'C':
			cert_file = optarg;
			break;
		case 'n':
			__max_used = atoi(optarg);
			break;
		case 'M':
			delay_ms = atoi(optarg);
			break;
		case 'I':
			check_fds_inter = atoi(optarg);
			break;
		default:
			break;
		}
	}

	acl::log::stdout_open(true);

	if (libpath.find("mbedtls") != NULL) {
		const std::vector<acl::string>& libs = libpath.split2(";, \t");
		if (libs.size() != 3) {
			printf("invalid libpath=%s\r\n", libpath.c_str());
			return 1;
		}
		acl::mbedtls_conf::set_libpath(libs[0], libs[1], libs[2]);

		if (acl::mbedtls_conf::load()) {
			__ssl_conf = new acl::mbedtls_conf(true);
			printf("load %s ok\r\n", libpath.c_str());
		} else {
			key_file.clear();
			cert_file.clear();
			printf("load %s error\r\n", libpath.c_str());
		}
		use_polarssl = false;
	} else if (libpath.find("polarssl") != NULL) {
		acl::polarssl_conf::set_libpath(libpath);

		if (acl::polarssl_conf::load()) {
			__ssl_conf = new acl::polarssl_conf();
			printf("load %s ok\r\n", libpath.c_str());
		} else {
			key_file.clear();
			cert_file.clear();
			printf("load %s error\r\n", libpath.c_str());
		}
	} else if (libpath.find("libssl") != NULL) {
#if 1
		const std::vector<acl::string>& libs = libpath.split2(";, \t");
		if (libs.size() != 2) {
			printf("invalid libpath=%s\r\n", libpath.c_str());
			return 1;
		}

		acl::openssl_conf::set_libpath(libs[0], libs[1]);

		if (acl::openssl_conf::load()) {
			__ssl_conf = new acl::openssl_conf(true);
			printf("use openssl_conf now\r\n");
		} else {
			key_file.clear();
			cert_file.clear();
			printf("load %s error\r\n", libpath.c_str());
		}
#else
		__ssl_conf = new acl::openssl_conf(true);
		printf("use openssl_conf now\r\n");
#endif
	}

	// ��˽Կ��֤�鶼����ʱ�Ų��� SSL ͨ�ŷ�ʽ
	if (!key_file.empty() && !cert_file.empty() && __ssl_conf) {

		// �������˵� SSL �Ự���湦��
		__ssl_conf->enable_cache(true);

		if (use_polarssl) {
			// ��ӱ��ط����֤��
			if (!__ssl_conf->add_cert(cert_file.c_str())) {
				delete __ssl_conf;
				__ssl_conf = NULL;
				std::cout << "add_cert error: " << cert_file.c_str() << std::endl;
			}

			// ��ӱ��ط�����Կ
			else if (!__ssl_conf->set_key(key_file.c_str())) {
				delete __ssl_conf;
				__ssl_conf = NULL;
				std::cout << "set_key error: " << key_file.c_str() << std::endl;
				std::cout << "Load cert&key OK!" << std::endl;
			}
		} else {
			// ��ӱ��ط����֤��
			if (!__ssl_conf->add_cert(cert_file.c_str(), key_file.c_str())) {
				delete __ssl_conf;
				__ssl_conf = NULL;
				std::cout << "add_cert error: " << cert_file.c_str() << std::endl;
			} else {
				std::cout << "Load cert&key OK!" << std::endl;
			}
		}
	}

	// �����첽���������
	acl::aio_handle handle(use_kernel ? acl::ENGINE_KERNEL : acl::ENGINE_SELECT);

	handle.set_check_inter(check_fds_inter);

	int delay_sec = delay_ms / 1000;
	int delay_usec = (delay_ms - delay_sec * 1000) * 1000;
	handle.set_delay_sec(delay_sec);
	handle.set_delay_usec(delay_usec);

	// ���������첽��
	acl::aio_listen_stream* sstream = new acl::aio_listen_stream(&handle);

	// ��ʼ��ACL��(��������WIN32��һ��Ҫ���ô˺�������UNIXƽ̨�¿ɲ�����)
	acl::acl_cpp_init();

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
	sstream->add_accept_callback(&callback);
	std::cout << "Listen: " << addr.c_str() << " ok!" << std::endl;

	while (true) {
		// ������� false ���ʾ���ټ�������Ҫ�˳�
		if (!handle.check()) {
			std::cout << "aio_server stop now ..." << std::endl;
			break;
		}

		if (__max_used > 0 && __cur_used >= __max_used) {
			break;
		}
	}

	// �رռ��������ͷ�������
	sstream->close();

	// XXX: Ϊ�˱�֤�ܹرռ�������Ӧ�ڴ˴��� check һ��
	handle.check();

	// ɾ�� acl::polarssl_conf ��̬����
	delete __ssl_conf;
	return 0;
}
