#include <iostream>
#include <assert.h>
#include "lib_acl.h"
#include "../../util.h"
#include "acl_cpp/lib_acl.hpp"
#include "acl_cpp/stream/openssl_conf.hpp"

typedef struct
{
	acl::aio_handle* handle;
	char  addr[64];
	int   connect_timeout;
	int   read_timeout;
	int   nopen_limit;
	int   nopen_total;
	int   nwrite_limit;
	int   nwrite_total;
	int   nread_total;
	int   id_begin;
	int   dlen;
} IO_CTX;

static bool connect_server(acl::sslbase_conf* ssl_conf, IO_CTX* ctx, int id);

/**
 * �ͻ����첽�������ص�������
 */
class client_io_callback : public acl::aio_open_callback
{
public:
	/**
	 * ���캯��
	 * @param client {aio_socket_stream*} �첽������
	 * @param ssl_conf {acl::sslbase_conf*} �ǿ�ʱָ�� SSL ���ӷ�ʽ
	 * @param ctx {IO_CTX*}
	 * @param id {int} ������ID��
	 */
	client_io_callback(acl::aio_socket_stream* client,
		acl::sslbase_conf* ssl_conf, IO_CTX* ctx, int id)
	: client_(client)
	, ssl_conf_(ssl_conf)
	, ctx_(ctx)
	, nwrite_(0)
	, nread_(0)
	, id_(id)
	{
		dlen_ = ctx->dlen;
		buff_ = (char*) malloc(dlen_);
		memset(buff_, 'x', dlen_);
		buff_[dlen_ - 1] = '\n';
		buff_[dlen_ - 2] = '\r';
	}

	~client_io_callback()
	{
		std::cout << ">>>ID: " << id_
			<< ", io_callback deleted now!" << std::endl;
		free(buff_);
	}

	/**
	 * �����麯��, ���첽��������Ҫ�������ʱ���ô˻ص�����
	 * @param data {char*} ���������ݵ�ַ
	 * @param len {int�� ���������ݳ���
	 * @return {bool} ���ظ������� true ��ʾ�����������ʾ��Ҫ�ر��첽��
	 */
	bool read_callback(char*, int len)
	{
		//printf(">>>>read len=%d\n", len);

		nread_ += len;
		ctx_->nread_total++;

		if (nwrite_ < 100 || nwrite_ % 1000 == 0) {
			char buf[256];

			acl::safe_snprintf(buf, sizeof(buf),
				"current len: %d, total_len: %d, nwrite: %d",
				len, nread_, nwrite_);
			acl::meter_time(__FILE__, __LINE__, buf);
		}

		write_line();
		return true;
	}

	/**
	 * �����麯��, ���첽��д�ɹ�ʱ���ô˻ص�����
	 * @return {bool} ���ظ������� true ��ʾ�����������ʾ��Ҫ�ر��첽��
	 */
	bool write_callback()
	{
		ctx_->nwrite_total++;
		nwrite_++;

		return true;
	}

	/**
	 * �����麯��, �����첽���ر�ʱ���ô˻ص�����
	 */
	void close_callback()
	{
		if (!client_->is_opened()) {
			std::cout << "Id: " << id_ << " connect "
				<< ctx_->addr << " error: "
				<< acl::last_serror();

			// ����ǵ�һ�����Ӿ�ʧ�ܣ����˳�
			if (ctx_->nopen_total == 0) {
				std::cout << ", first connect error, quit";
				/* ����첽��������������Ϊ�˳�״̬ */
				client_->get_handle().stop();
			}
			std::cout << std::endl;
			delete this;
			return;
		}

		/* ����첽�������ܼ�ص��첽������ */
		int nleft = client_->get_handle().length();
		if (ctx_->nopen_total == ctx_->nopen_limit && nleft == 1) {
			std::cout << "Id: " << id_ << " stop now! nstream: "
				<< nleft << std::endl;
			/* ����첽��������������Ϊ�˳�״̬ */
			client_->get_handle().stop();
		}

		// �����ڴ˴�ɾ���ö�̬����Ļص�������Է�ֹ�ڴ�й¶
		delete this;
	}

	/**
	 * �����麯�������첽����ʱʱ���ô˺���
	 * @return {bool} ���ظ������� true ��ʾ�����������ʾ��Ҫ�ر��첽��
	 */
	bool timeout_callback()
	{
		std::cout << "Connect " << ctx_->addr
			<< " Timeout ..." << std::endl;
		client_->close();
		return false;
	}

	bool read_wakeup()
	{
		// ȡ��֮ǰͨ�� setup_hook ע��� SSL IO���
		acl::sslbase_io* hook = (acl::sslbase_io*) client_->get_hook();

		if (hook == NULL) {
			std::cout << "get hook error"<< std::endl;
			return false;
		}

		printf(">>>%s: begin handshake\n", __FUNCTION__);
		// ���Խ��� SSL ����
		if (!hook->handshake()) {
			logger_error("%s: ssl handshake failed", __FUNCTION__);
			return false;
		}

		printf(">>>%s: end handshake\r\n", __FUNCTION__);
		// SSL ���ֻ�δ��ɣ��ȴ��������ٴα�����
		if (!hook->handshake_ok()) {
			printf(">>>%s: not handshake ok\r\n", __FUNCTION__);
			return true;
		}

		// ��� SSL �����Ѿ��ɹ�����ʼ������
		
		printf(">>>%s: ssl handshake ok\r\n", __FUNCTION__);

		// �� reactor ģʽתΪ proactor ģʽ���Ӷ�ȡ��
		// read_wakeup �ص�����
		client_->disable_read();

		// ��ʼ�����˵Ķ�д����
		return begin_run();
	}

	/**
	 * �����麯��, ���첽���ӳɹ�����ô˺���
	 * @return {bool} ���ظ������� true ��ʾ�����������ʾ��Ҫ�ر��첽��
	 */
	bool open_callback()
	{
		// ���ӳɹ�������IO��д�ص�����
		client_->add_read_callback(this);
		client_->add_write_callback(this);
		ctx_->nopen_total++;

		acl::assert_(id_ > 0);
		if (ctx_->nopen_total < ctx_->nopen_limit) {
			// ��ʼ������һ�����ӹ���
			if (!connect_server(ssl_conf_, ctx_, id_ + 1)) {
				std::cout << "connect error!" << std::endl;
			}
		}

		// ���� SSL ��ʽ
		if (ssl_conf_) {
			return setup_ssl(*ssl_conf_);
		}

		// ��ʼ�����˵Ķ�д����
		else {
			return begin_run();
		}
	}

private:
	acl::aio_socket_stream* client_;
	acl::sslbase_conf* ssl_conf_;
	IO_CTX* ctx_;
	int   nwrite_;
	int   nread_;
	int   id_;
	char *buff_;
	int   dlen_;

	bool setup_ssl(acl::sslbase_conf& ssl_conf)
	{
		acl::sslbase_io* ssl = ssl_conf.create(true);

		// �� SSL IO ����ע�����첽����
		if (client_->setup_hook(ssl) == ssl) {
			std::cout << "open ssl error!" << std::endl;
			ssl->destroy();
			return false;
		}

		// ��ʼ SSL ���ֹ���
		if (!ssl->handshake()) {
			client_->remove_hook();
			ssl->destroy();
			return false;
		}

		if (ssl->handshake_ok()) {
			printf("%s: ssl handshake ok\r\n", __FUNCTION__);
			client_->disable_read();
			return begin_run();
		} else {
			// ��ʼ�첽 SSL ���ֹ��̣�����ɶ�����ʱ������
			// read_wakeup
			client_->read_wait(10);
		}
		return true;
	}

	bool begin_run(void)
	{
		write_line();

		// �첽�ӷ�������ȡ����
		//client_->read();
		client_->gets(10, false);

		return true;
	}

	void write_line(void)
	{
		// �첽���������������
		client_->write(buff_, dlen_);

		if (nwrite_ >= ctx_->nwrite_limit) {
			client_->close();
		}
	}
};

static bool connect_server(acl::sslbase_conf* ssl_conf, IO_CTX* ctx, int id)
{
	// ��ʼ�첽����Զ�̷�����
	acl::aio_socket_stream* stream = acl::aio_socket_stream::open(
			ctx->handle, ctx->addr, ctx->connect_timeout);
	if (stream == NULL)
	{
		std::cout << "connect " << ctx->addr << " error!" << std::endl;
		std::cout << "stoping ..." << std::endl;
		if (id == 0) {
			ctx->handle->stop();
		}
		return false;
	}

	acl_non_blocking(stream->sock_handle(), ACL_BLOCKING);

	// �������Ӻ�Ļص�������
	client_io_callback* callback = new
		client_io_callback(stream, ssl_conf, ctx,  id);

	// ������ӳɹ��Ļص�������
	stream->add_open_callback(callback);

	// �������ʧ�ܺ�ص�������
	stream->add_close_callback(callback);

	// ������ӳ�ʱ�Ļص�������
	stream->add_timeout_callback(callback);

	return true;
}

static void usage(const char* procname)
{
	printf("usage: %s -h[help] -l server_addr \r\n"
		" -d path_to_polarssl\r\n"
		" -c nconnect\r\n"
		" -n io_max\r\n"
		" -k[use kernel event: epoll/kqueue/devpoll\r\n"
		" -I check_fds_inter\r\n"
		" -M delay_ms\r\n"
		" -t connect_timeout\r\n"
		" -S[use_ssl]\r\n"
		" -L data_len[default: 8193]\n", procname);
}

int main(int argc, char* argv[])
{
	bool use_kernel = false, use_ssl = false;
	IO_CTX ctx;
	acl::string libpath;
	acl::sslbase_conf* ssl_conf = NULL;
	int   ch;
	int   check_fds_inter = 10, delay_ms = 100;

	memset(&ctx, 0, sizeof(ctx));
	ctx.connect_timeout = 5;
	ctx.nopen_limit = 1;
	ctx.id_begin = 1;
	ctx.nwrite_limit = 1;
	ctx.dlen = 8193;
	acl::safe_snprintf(ctx.addr, sizeof(ctx.addr), "127.0.0.1:9800");

	acl::log::stdout_open(true);

	while ((ch = getopt(argc, argv, "hd:c:n:kl:t:SL:I:M:")) > 0) {
		switch (ch) {
		case 'c':
			ctx.nopen_limit = atoi(optarg);
			if (ctx.nopen_limit <= 0) {
				ctx.nopen_limit = 10;
			}
			break;
		case 'd':
			libpath = optarg;
			break;
		case 'n':
			ctx.nwrite_limit = atoi(optarg);
			if (ctx.nwrite_limit <= 0) {
				ctx.nwrite_limit = 10;
			}
			break;
		case 'h':
			usage(argv[0]);
			return 0;
		case 'k':
			use_kernel = true;
			break;
		case 'l':
			acl::safe_snprintf(ctx.addr, sizeof(ctx.addr), "%s", optarg);
			break;
		case 't':
			ctx.connect_timeout = atoi(optarg);
			break;
		case 'S':
			use_ssl = true;
			break;
		case 'L':
			ctx.dlen = atoi(optarg);
			break;
		case 'I':
			check_fds_inter = atoi(optarg);
			break;
		case 'M':
			delay_ms = atoi(optarg);
			break;
		default:
			break;
		}
	}

	acl::meter_time(__FUNCTION__, __LINE__, "-----BEGIN-----");
	acl::acl_cpp_init();
	acl::log::stdout_open(true);

	acl::aio_handle handle(use_kernel ? acl::ENGINE_KERNEL : acl::ENGINE_SELECT);

	handle.set_check_inter(check_fds_inter);

	int delay_sec = delay_ms / 1000;
	int delay_usec = (delay_ms - delay_sec * 1000) * 1000;
	handle.set_delay_sec(delay_sec);
	handle.set_delay_usec(delay_usec);

	printf(">>>delay_sec: %d, delay_usec: %d, check_fds_inter: %d\r\n",
		delay_sec, delay_usec, check_fds_inter);
	printf("Enter any key to continue ...\r\n");
	getchar();

	if (use_ssl) {
		if (libpath.find("mbedtls") != NULL) {
			const std::vector<acl::string>& libs = libpath.split2(";");
			if (libs.size() != 3) {
				printf("invalid libpath=%s\r\n", libpath.c_str());
				return 1;
			}
			acl::mbedtls_conf::set_libpath(libs[0], libs[1], libs[2]);
			if (acl::mbedtls_conf::load()) {
				ssl_conf = new acl::mbedtls_conf(false);
			} else {
				printf("load %s error\r\n", libpath.c_str());
			}
		} else if (libpath.find("polarssl") != NULL) {
			acl::polarssl_conf::set_libpath(libpath);
			if (acl::polarssl_conf::load()) {
				ssl_conf = new acl::polarssl_conf;
			} else {
				printf("load %s error\r\n", libpath.c_str());
			}
		} else if (libpath.find("libssl") != NULL) {
			const std::vector<acl::string>& libs = libpath.split2(";, \t");
			if (libs.size() != 2) {
				printf("invalid libpath=%s\r\n", libpath.c_str());
				return 1;
			}

			acl::openssl_conf::set_libpath(libs[0], libs[1]);

			if (acl::openssl_conf::load()) {
				ssl_conf = new acl::openssl_conf(false);
			} else {
				printf("load %s error\r\n", libpath.c_str());
			}
		}
	}

	ctx.handle = &handle;

	if (!connect_server(ssl_conf, &ctx, ctx.id_begin)) {
		std::cout << "enter any key to exit." << std::endl;
		getchar();
		return 1;
	}

	std::cout << "Connect " << ctx.addr << " ..." << std::endl;

	struct timeval begin;
	gettimeofday(&begin, NULL);

	while (true) {
		// ������� false ���ʾ���ټ�������Ҫ�˳�
		if (!handle.check()) {
			break;
		}
	}

	struct timeval end;
	gettimeofday(&end, NULL);

	double spent = util::stamp_sub(&end, &begin);

	printf("total open: %d, total write: %d, total read: %d,"
		" spent: %.2f ms, speed: %.2f\r\n",
		ctx.nopen_total, ctx.nwrite_total, ctx.nread_total,
		spent, (ctx.nread_total * 1000) / (spent > 1 ? spent : 1));

	delete ssl_conf;
	return 0;
}
