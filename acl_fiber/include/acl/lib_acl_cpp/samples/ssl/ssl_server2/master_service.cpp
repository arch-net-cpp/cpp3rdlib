#include "stdafx.h"
#include "master_service.h"

////////////////////////////////////////////////////////////////////////////////
// ����������

char *var_cfg_ssl_path;
char *var_cfg_crt_file;
char *var_cfg_key_file;
acl::master_str_tbl var_conf_str_tab[] = {
#ifdef __APPLE__
	{ "ssl_path", "../libmbedcrypto.dylib;../libmbedx509.dylib;../libmbedtls.dylib", &var_cfg_ssl_path },
#else
	{ "ssl_path", "../libmbedcrypto.so;../libmbedx509.so;../libmbedtls.so", &var_cfg_ssl_path },
#endif
	{ "crt_file", "./ssl_crt.pem", &var_cfg_crt_file },
	{ "key_file", "./ssl_key.pem", &var_cfg_key_file },

	{ 0, 0, 0 }
};

int  var_cfg_use_mbedtls;
int  var_cfg_session_cache;
acl::master_bool_tbl var_conf_bool_tab[] = {
	{ "use_mbedtls", 1, &var_cfg_use_mbedtls },
	{ "session_cache", 1, &var_cfg_session_cache },

	{ 0, 0, 0 }
};

int  var_cfg_io_timeout;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "io_timeout", 60, &var_cfg_io_timeout, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

acl::master_int64_tbl var_conf_int64_tab[] = {

	{ 0, 0 , 0 , 0, 0 }
};

////////////////////////////////////////////////////////////////////////////////

master_service::master_service()
: conf_(NULL)
{
}

master_service::~master_service()
{
	if (conf_) {
		delete conf_;
	}
}

static acl::sslbase_io* setup_ssl(acl::socket_stream& conn,
	acl::sslbase_conf& conf)
{
	acl::sslbase_io* hook = (acl::sslbase_io*) conn.get_hook();
	if (hook != NULL) {
		return hook;
	}

	// ����ʹ�� SSL ��ʽ����������Ҫ�� SSL IO ������ע��������
	// �����������У����� ssl io �滻 stream ��Ĭ�ϵĵײ� IO ����

	//logger("begin setup ssl hook...");

	// ���÷����� SSL ���ַ�ʽ
	acl::sslbase_io* ssl = conf.create(true);
	if (conn.setup_hook(ssl) == ssl) {
		logger_error("setup_hook error!");
		ssl->destroy();
		return NULL;
	}

	logger("setup hook ok, tid: %lu", acl::thread::thread_self());
	return ssl;
}

static bool do_run(acl::socket_stream& conn, acl::sslbase_io*)
{
	acl::string* buf =(acl::string*) conn.get_ctx();

	// ������ģʽ��ȡһ�У��÷���������ʽ���� polarssl_io ��ĵײ�
	// IO ���̱��ϵ�
	if (!conn.gets_peek(buf, false)) {
		if (conn.eof()) {
			printf("peek error\n");
			return false;
		} else {
			return true;
		}
	}

	// ����ģʽ��д���ݣ���Ϊ���׽��ֲ�δ����Ϊ������ģʽ������д�Ĺ���
	// ����������
	if (conn.write(*buf) == -1) {
		logger("write error!");
		return false;
	}

	buf->clear();
	return true;
}

bool master_service::thread_on_read(acl::socket_stream* conn)
{
	if (conf_ == NULL) {
		return do_run(*conn, NULL);
	}

	acl::sslbase_io* ssl = setup_ssl(*conn, *conf_);
	if (ssl == NULL) {
		return false;
	}

	if (!ssl->handshake()) {
		logger_error("ssl handshake failed");
		return false;
	}

	if (!ssl->handshake_ok()) {
		logger("handshake trying again...");
		return true;
	}

	//logger("handshake_ok");

	return do_run(*conn, ssl);
}

bool master_service::thread_on_accept(acl::socket_stream* conn)
{
	//logger("connect from %s, fd: %d, timeout: %d", conn->get_peer(true),
	//	conn->sock_handle(), conn->get_rw_timeout());

	acl::string* buf = new acl::string(128);
	conn->set_ctx(buf);
	conn->set_rw_timeout(var_cfg_io_timeout);

	return true;
}

bool master_service::thread_on_timeout(acl::socket_stream* conn)
{
	logger("read timeout from %s, fd: %d", conn->get_peer(),
		conn->sock_handle());
	return false;
}

void master_service::thread_on_close(acl::socket_stream* conn)
{
	//logger("disconnect from %s, fd: %d", conn->get_peer(),
	//	conn->sock_handle());
	acl::string* buf = (acl::string*) conn->get_ctx();
	delete buf;
}

void master_service::thread_on_init()
{
}

void master_service::thread_on_exit()
{
}

void master_service::proc_on_init()
{
	if (var_cfg_crt_file == NULL || *var_cfg_crt_file == 0
		|| var_cfg_key_file == NULL || *var_cfg_key_file == 0) {
		return;
	}

	if (var_cfg_use_mbedtls) {
		acl::string buf(var_cfg_ssl_path);
		const std::vector<acl::string>& libs = buf.split2("; \t\r\n");
		if (libs.size() != 3) {
			logger_error("invalid ssl_path=%s", var_cfg_ssl_path);
			return;
		}
		acl::mbedtls_conf::set_libpath(libs[0], libs[1], libs[2]);
		if (!acl::mbedtls_conf::load()) {
			logger_error("load %s error", var_cfg_ssl_path);
			return;
		}
	} else {
		acl::polarssl_conf::set_libpath(var_cfg_ssl_path);
		if (!acl::polarssl_conf::load()) {
			logger_error("load %s error", var_cfg_ssl_path);
			return;
		}
	}

	if (var_cfg_use_mbedtls) {
		conf_ = new acl::mbedtls_conf(true);
	} else {
		conf_ = new acl::polarssl_conf();
	}

	// �������˵� SSL �Ự���湦��
	conf_->enable_cache(var_cfg_session_cache);

	if (var_cfg_use_mbedtls) {
		if (!conf_->add_cert(var_cfg_crt_file, var_cfg_key_file)) {
			logger_error("add cert failed, crt: %s, key: %s",
				var_cfg_crt_file, var_cfg_key_file);
			delete conf_;
			conf_ = NULL;
			return;
		}
	} else {
		// ��ӱ��ط����֤��
		if (!conf_->add_cert(var_cfg_crt_file)) {
			logger_error("add cert failed, crt: %s, key: %s",
				var_cfg_crt_file, var_cfg_key_file);
			delete conf_;
			conf_ = NULL;
			return;
		}

		// ��ӱ��ط�����Կ
		if (!conf_->set_key(var_cfg_key_file)) {
			logger_error("set private key error");
			delete conf_;
			conf_ = NULL;
		}
	}

	logger("load cert ok, crt: %s, key: %s", var_cfg_crt_file, var_cfg_key_file);
}

void master_service::proc_on_exit()
{
}
