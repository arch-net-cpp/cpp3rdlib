#include "acl_stdafx.hpp"
#ifndef ACL_PREPARE_COMPILE
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/zlib_stream.hpp"
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"
#include "acl_cpp/stream/sslbase_conf.hpp"
#include "acl_cpp/stream/sslbase_io.hpp"
#include "acl_cpp/http/http_header.hpp"
#include "acl_cpp/http/websocket.hpp"
#include "acl_cpp/http/http_aclient.hpp"
#endif

namespace acl
{

enum {
	HTTP_ACLIENT_STATUS_NONE,
	HTTP_ACLIENT_STATUS_SSL_HANDSHAKE,
	HTTP_ACLIENT_STATUS_WS_HANDSHAKE,
	HTTP_ACLIENT_STATUS_WS_READING,
};

http_aclient::http_aclient(aio_handle& handle, sslbase_conf* ssl_conf /* NULL */)
: status_(HTTP_ACLIENT_STATUS_NONE)
, rw_timeout_(0)
, gzip_header_left_(0)
, keep_alive_(false)
, unzip_(false)
, handle_(handle)
, ssl_conf_(ssl_conf)
, conn_(NULL)
, stream_(NULL)
, hdr_res_(NULL)
, http_res_(NULL)
, ws_in_(NULL)
, ws_out_(NULL)
, buff_(NULL)
, zstream_(NULL)
{
	header_ = NEW http_header;
	memset(&ns_addr_, 0, sizeof(ns_addr_));
	memset(&serv_addr_, 0, sizeof(serv_addr_));
	ssl_enable_ = true;
}

http_aclient::~http_aclient(void)
{
	if (http_res_) {
		http_res_free(http_res_);
	} else if (hdr_res_) {
		http_hdr_res_free(hdr_res_);
	}
	delete header_;

	if (stream_) {
		stream_->unbind();
		delete stream_;
	}

	delete ws_in_;
	delete ws_out_;
	delete buff_;
	delete zstream_;
}

http_header& http_aclient::request_header(void)
{
	return *header_;
}

http_aclient& http_aclient::unzip_body(bool on)
{
	unzip_ = on;
	return *this;
}

http_aclient& http_aclient::set_ssl_conf(sslbase_conf* ssl_conf)
{
	ssl_conf_ = ssl_conf;
	return *this;
}

http_aclient& http_aclient::enable_ssl(bool yes)
{
	ssl_enable_ = yes;
	return *this;
}

bool http_aclient::open(const char* addr, int conn_timeout, int rw_timeout)
{
	ACL_AIO* aio = handle_.get_handle();
	if (acl_aio_connect_addr(aio, addr, conn_timeout,
		connect_callback, this) == -1) {

		logger_error("connect %s error %s", addr, last_serror());
		return false;
	}
	rw_timeout_ = rw_timeout;
	return true;
}

void http_aclient::close(void)
{
	if (conn_) {
		conn_->close();
	}
}

bool http_aclient::get_ns_addr(string& out) const
{
	char buf[256];
	const struct sockaddr* sa = (const struct sockaddr*) &ns_addr_;
	size_t ret = acl_inet_ntop(sa, buf, sizeof(buf));
	if (ret == 0) {
		return false;
	}
	out = buf;
	return true;
}

bool http_aclient::get_server_addr(string& out) const
{
	char buf[256];
	const struct sockaddr* sa = (const struct sockaddr*) &serv_addr_;
	size_t ret = acl_inet_ntop(sa, buf, sizeof(buf));
	if (ret == 0) {
		return false;
	}
	out = buf;
	return true;
}

bool http_aclient::handle_connect(const ACL_ASTREAM_CTX *ctx)
{
	const ACL_SOCKADDR *ns_addr = acl_astream_get_ns_addr(ctx);
	if (ns_addr) {
		memcpy(&ns_addr_, &ns_addr->ss, sizeof(ns_addr_));
	}
	const ACL_SOCKADDR* serv_addr = acl_astream_get_serv_addr(ctx);
	if (serv_addr) {
		memcpy(&serv_addr_, &serv_addr->ss, sizeof(serv_addr_));
	}

	ACL_ASTREAM* astream = acl_astream_get_conn(ctx);
	if (astream == NULL) {
		int status = acl_astream_get_status(ctx);
		switch (status) {
		case ACL_ASTREAM_STATUS_NS_ERROR:
			this->on_ns_failed();
			this->destroy();
			break;
		case ACL_ASTREAM_STATUS_CONNECT_TIMEOUT:
			this->on_connect_timeout();
			this->destroy();
			break;
		default:
			this->on_connect_failed();
			this->destroy();
			break;
		}
		return false;
	}

	// ���ӳɹ������� C++ AIO ���Ӷ���
	conn_ = NEW aio_socket_stream(&handle_, astream, true);

	// ע�����ӹرջص��������
	conn_->add_close_callback(this);

	// ע�� IO ��ʱ�ص��������
	conn_->add_timeout_callback(this);

	if (!ssl_conf_ || !ssl_enable_) {
		return this->on_connect();
	}

	// ��Ϊ������ SSL ͨ�ŷ�ʽ��������Ҫ���� SSL IO ���̣���ʼ SSL ����
	sslbase_io* ssl_io = ssl_conf_->create(true);

	const char* host = header_->get_host();
	if (host && *host) {
		ssl_io->set_sni_host(host);
	}

	if (conn_->setup_hook(ssl_io) == ssl_io || !ssl_io->handshake()) {
		logger_error("open ssl failed");
		conn_->remove_hook();
		ssl_io->destroy();
		this->on_connect_failed();
		return false;
	}

	status_ = HTTP_ACLIENT_STATUS_SSL_HANDSHAKE;

	// ��ʼ SSL ���ֹ��̣�read_wait ��Ӧ�Ļص�����Ϊ read_wakeup
	conn_->add_read_callback(this);
	conn_->readable_await(rw_timeout_);
	return true;
}

int http_aclient::connect_callback(const ACL_ASTREAM_CTX *ctx)
{
	assert(ctx);
	http_aclient* me = (http_aclient*) acl_astream_get_ctx(ctx);
	assert(me);
	return me->handle_connect(ctx) ? 0 : -1;
}

bool http_aclient::timeout_callback(void)
{
	return this->on_read_timeout();
}

void http_aclient::close_callback(void)
{
	// ����ر�ʱ�ص��������ط���
	this->on_disconnect();
	// ��������
	this->destroy();
}

bool http_aclient::handle_ws_ping(void)
{
	if (buff_ == NULL) {
		buff_ = NEW string(1024);
	}

	while (true) {
		int  ret = ws_in_->peek_frame_data(*buff_, 1024);
		bool res;
		switch (ret) {
		case -1:
			if (ws_in_->eof()) {
				buff_->clear();
				return false;
			}
			return true;
		case 0:
			this->on_ws_frame_ping(*buff_);
			if (buff_->empty()) {
				return true;
			}

			// �첽���� pong ����
			res = ws_out_->send_frame_pong(*conn_,
				(void*) buff_->c_str(), buff_->size());
			buff_->clear();
			return res;
		default:
			break;
		}
	}
}

bool http_aclient::handle_ws_pong(void)
{
	if (buff_ == NULL) {
		buff_ = NEW string(1024);
	}

	while (true) {
		int ret = ws_in_->peek_frame_data(*buff_, 1024);
		switch (ret) {
		case -1:
			if (ws_in_->eof()) {
				buff_->clear();
				return false;
			}
			return true;
		case 0:
			this->on_ws_frame_pong(*buff_);
			buff_->clear();
			return true;
		default:
			break;
		}
	}
}

bool http_aclient::handle_ws_other(void)
{
	if (buff_ == NULL) {
		buff_ = NEW string(1024);
	}

	while (true) {
		int ret = ws_in_->peek_frame_data(*buff_, 1024);
		switch (ret) {
		case -1:
			if (ws_in_->eof()) {
				buff_->clear();
				return false;
			}
			return true;
		case 0:
			buff_->clear();
			return true;
		default:
			break;
		}
	}
}

bool http_aclient::handle_ws_data(void)
{
	char buf[8192];
	size_t size = sizeof(buf) - 1;

	while (true) {
		int ret = ws_in_->peek_frame_data(buf, size);
		switch (ret) {
		case -1:
			if (ws_in_->eof()) {
				return false;
			}
			return true;
		case 0:
			return this->on_ws_frame_finish();
		default:
			if (!this->on_ws_frame_data(buf, ret)) {
				return false;
			}
			break;
		}
	}
}

bool http_aclient::handle_websocket(void)
{
	acl_assert(ws_in_);

	if (!ws_in_->is_head_finish()) {
		if (!ws_in_->peek_frame_head()) {
			if (ws_in_->eof()) {
				return false;
			}
			return true;
		}

		// ����������֡ͷʱ�����ݲ�ͬ֡���ͻص���ͬ����
		unsigned char opcode = ws_in_->get_frame_opcode();
		switch (opcode) {
		case FRAME_TEXT:
			if (!this->on_ws_frame_text()) {
				return false;
			}
			break;
		case FRAME_BINARY:
			if (!this->on_ws_frame_binary()) {
				return false;
			}
			break;
		case FRAME_CLOSE:
			this->on_ws_frame_closed();
			return false;
		case FRAME_PING:
			return true;
		case FRAME_PONG:
			return true;
		default:
			return true;
		}
	}

	unsigned char opcode = ws_in_->get_frame_opcode();
	switch (opcode) {
	case FRAME_TEXT:
	case FRAME_BINARY:
		return handle_ws_data();
	case FRAME_PING:
		return handle_ws_ping();
	case FRAME_PONG:
		return handle_ws_pong();
	default:
		return handle_ws_other();
	}
}

// SSL ���ֺͶ� Websocket ����֡�׶Σ��÷������ᱻ��ε��á�
// ������ SSL ģʽʱ���� SSL ���ֽ׶Σ��÷����ᱻ��ε��ã�ֱ�� SSL ���ֳɹ�
// ��ʧ�ܣ��ڶ�ȡ Websocket ����֡�׶Σ��÷���Ҳ�ᱻ��ε��ã�����һֱ��ȡ����֡
bool http_aclient::read_wakeup(void)
{
	// ��� websocket �� NULL����˵�����뵽 websocket ͨ�ŷ�ʽ��
	// �ô��������� http_res_hdr_cllback ��ע��
	switch (status_) {
	case HTTP_ACLIENT_STATUS_WS_READING:
		acl_assert(ws_in_);
		return handle_websocket();
	case HTTP_ACLIENT_STATUS_SSL_HANDSHAKE:
		return handle_ssl_handshake();
	default:
		logger_error("invalid status=%u", status_);
		return false;
	}
}

bool http_aclient::handle_ssl_handshake(void)
{
	// �������ǵ�һ�ν��� SSL ���ֽ׶ε� IO ����
	sslbase_io* ssl_io = (sslbase_io*) conn_->get_hook();
	if (ssl_io == NULL) {
		logger_error("no ssl_io hooked!");
		return false;
	}

	if (!ssl_io->handshake()) {
		logger_error("ssl handshake error!");
		return false;
	}

	// SSL ���ֳɹ��󣬻ص����ӳɹ�������֪ͨ������Է�����������
	if (ssl_io->handshake_ok()) {
		conn_->del_read_callback(this);
		conn_->disable_read();
		return this->on_connect();
	}

	// ���� SSL ���ֹ���
	return true;
}

bool http_aclient::read_callback(char* data, int len)
{
	(void) data;
	(void) len;
	return true;
}

bool http_aclient::res_plain_finish(char* data, int dlen)
{
	if (data == NULL || dlen <= 0) {
		// ���� HTTP ��Ӧ���ݣ��ص���ɷ���
		return this->on_http_res_finish(true) && keep_alive_;
	}

	// �������� HTTP ��Ӧ�����ݴ��ݸ�����
	if (!this->on_http_res_body(data, (size_t) dlen)) {
		return false;
	}

	// ���� HTTP ��Ӧ���ݣ��ص���ɷ���
	return this->on_http_res_finish(true) && keep_alive_;
}

bool http_aclient::res_unzip_finish(zlib_stream& zstream, char* data, int dlen)
{
	if (data == NULL || dlen == 0) {
		string buf(1024);
		if (!zstream.unzip_finish(&buf)) {
			logger_error("unzip_finish error");
			return false;
		}

		if (!buf.empty() && !this->on_http_res_body(
			buf.c_str(), buf.size())) {

			return false;
		}

		// ���� HTTP ��Ӧ���ݣ��ص���ɷ���
		return this->on_http_res_finish(true) && keep_alive_;
	}

	if (gzip_header_left_ >= dlen) {
		logger_error("data tool small, dlen=%d gzip_header=%d",
			dlen, (int) gzip_header_left_);
		return false;
	}

	dlen -= gzip_header_left_;
	data += gzip_header_left_;
	gzip_header_left_ = 0;

	string buf(4096);
	if (!zstream.unzip_update(data, dlen, &buf)) {
		logger_error("unzip_update error");
		return false;
	}
	if (!zstream.unzip_finish(&buf)) {
		logger_error("unzip_finish error");
		return false;
	}

	if (!buf.empty()) {
		if (!this->on_http_res_body(buf.c_str(), buf.size())) {
			return false;
		}
	}

	// ���� HTTP ��Ӧ���ݣ��ص���ɷ���
	return this->on_http_res_finish(true) && keep_alive_;
}

bool http_aclient::handle_res_body_finish(char* data, int dlen)
{
	if (zstream_) {
		bool ret = res_unzip_finish(*zstream_, data, dlen);
		delete zstream_;
		zstream_ = NULL;
		return ret;
	} else {
		return res_plain_finish(data, dlen);
	}
}

bool http_aclient::res_plain(char* data, int dlen)
{
	return this->on_http_res_body(data, (size_t) dlen);
}

bool http_aclient::res_unzip(zlib_stream& zstream, char* data, int dlen)
{
	if (gzip_header_left_ >= dlen) {
		gzip_header_left_ -= dlen;
		return true;
	}

	dlen -= gzip_header_left_;
	data += gzip_header_left_;
	gzip_header_left_ = 0;

	string buf(4096);
	if (!zstream.unzip_update(data, dlen, &buf)) {
		logger_error("unzip_update error, dlen=%d", dlen);
		return false;
	}

	if (!buf.empty()) {
		if (!this->on_http_res_body(buf.c_str(), buf.size())) {
			return false;
		}
	}

	return true;
}

bool http_aclient::handle_res_body(char* data, int dlen)
{
	if (zstream_) {
		return res_unzip(*zstream_, data, dlen);
	} else {
		return res_plain(data, dlen);
	}
}

int http_aclient::http_res_callback(int status, char* data, int dlen, void* ctx)
{
	http_aclient* me = (http_aclient*) ctx;
	switch (status) {
	case HTTP_CHAT_CHUNK_HDR:
	case HTTP_CHAT_CHUNK_DATA_ENDL:
	case HTTP_CHAT_CHUNK_TRAILER:
		return 0;
	case HTTP_CHAT_OK:
		return me->handle_res_body_finish(data, dlen) ? 0 : -1;
	case HTTP_CHAT_ERR_IO:
	case HTTP_CHAT_ERR_PROTO:
		(void) me->on_http_res_finish(false);
		return -1;
	case HTTP_CHAT_DATA:
		// �������� HTTP ��Ӧ�����ݴ��ݸ�����
		return me->handle_res_body(data, dlen) ? 0 : -1;
	default:
		return 0;
	}
}

bool http_aclient::handle_res_hdr(int status)
{
	if (status != HTTP_CHAT_OK) {
		logger_error("http response head error, status=%d", status);
		return false;
	}

	// ���� HTTP ��Ӧͷ
	http_hdr_res_parse(hdr_res_);

	// �� C HTTP ��Ӧͷת���� C++ HTTP ��Ӧͷ�����ص����෽��
	http_header header(*hdr_res_);
	if (!this->on_http_res_hdr(header)) {
		return false;
	}

	keep_alive_ = header.get_keep_alive();

	// ����� websocket ͨ�ŷ�ʽ����ת�� websocket �������
	if (status_ == HTTP_ACLIENT_STATUS_WS_HANDSHAKE) {
		acl_assert(ws_in_ && ws_out_);

		// HTTP ��Ӧ״̬������ 101��������� websocket ����ʧ��
		if (hdr_res_->reply_status != 101) {
			logger_error("invalid status=%d for websocket",
				hdr_res_->reply_status);
			this->on_ws_handshake_failed(hdr_res_->reply_status);
			return false;
		}

		// �ص����෽����֪ͨ WS �������
		if (!this->on_ws_handshake()) {
			return false;
		}
		return true;
	}

	// ������������ HTTP �������

	if (http_res_) {
		http_res_free(http_res_);
	}
	http_res_ = http_res_new(hdr_res_);

	// �����Ӧ����Ϊ GZIP ѹ�����ݣ����û��������Զ���ѹ���ܣ�����Ҫ����
	// ��ѹ�����������Ӧ���ݽ��н�ѹ
	if (unzip_ && header.is_transfer_gzip() && zlib_stream::zlib_load_once()) {
		zstream_ = NEW zlib_stream();
		if (!zstream_->unzip_begin(false)) {
			logger_error("unzip_begin error");
			delete zstream_;
			zstream_ = NULL;
		} else {
			// gzip ��Ӧ������ǰ���� 10 �ֽڵ�ͷ���ֶ�
			gzip_header_left_ = 10;
		}
	}

	// �����Ӧ�����峤��Ϊ 0�����ʾ�� HTTP ��Ӧ���
	if (header.get_content_length() == 0) {
		if (this->on_http_res_finish(true) && keep_alive_) {
			return true;
		} else {
			return false;
		}
	}

	// ��ʼ�첽��ȡ HTTP ��Ӧ������
	http_res_body_get_async(http_res_, conn_->get_astream(),
		http_res_callback, this, rw_timeout_);
	return true;
}

int http_aclient::http_res_hdr_cllback(int status, void* ctx)
{
	http_aclient* me = (http_aclient*) ctx;

	return me->handle_res_hdr(status) ? 0 : -1;
}

void http_aclient::send_request(const void* body, size_t len)
{
	http_method_t method = header_->get_method();
	if (body && len > 0 && method != HTTP_METHOD_POST
		&& method != HTTP_METHOD_PUT && method != HTTP_METHOD_PATCH) {

		header_->set_content_length(len);
		header_->set_method(HTTP_METHOD_POST);
	}

	// ���� HTTP ����ͷ������
	string buf;
	header_->build_request(buf);
	conn_->write(buf.c_str(), (int) buf.size());

	if (body && len > 0) {
		// ���� HTTP ������
		conn_->write(body, (int) len);
	}

	// �� HTTP �����ӱ�����ʱ����Ҫ��ǰһ�δ�������ʱ�����ͷţ���������
	// �ڴ�й¶��ͬʱҪ�� http_res_ �����еĳ�Ա hdr_res ��NULL��������
	// �����ͷ� http_res_ ʱ���Ա��� hdr_res ���ظ��ͷ�
	if (hdr_res_) {
		http_hdr_res_free(hdr_res_);
		if (http_res_) {
			http_res_->hdr_res = NULL;
		}
	}

	// ��ʼ��ȡ HTTP ��Ӧͷ
	hdr_res_ = http_hdr_res_new();
	http_hdr_res_get_async(hdr_res_, conn_->get_astream(),
		http_res_hdr_cllback, this, rw_timeout_);
}

void http_aclient::ws_handshake(const char* key)
{
	acl_assert(key && *key);
	return ws_handshake(key, strlen(key));
}

void http_aclient::ws_handshake(const void* key, size_t len)
{
	acl_assert(key && len > 0);
	acl_assert(stream_ == NULL);

	ACL_VSTREAM* vs = conn_->get_vstream();
	stream_ = NEW socket_stream;
	(void) stream_->open(vs);

	http_header& hdr = request_header();
	hdr.set_ws_key(key, len)
		.set_ws_version(13)
		.set_upgrade("websocket")
		.set_keep_alive(true);

	this->ws_handshake_before(hdr);

	// ���� websocket ������������󣬲��Դ���Ϊ�� HTTP Э���л���
	// websocket ������
	ws_in_  = NEW websocket(*stream_);
	ws_out_ = NEW websocket(*stream_);

	unsigned mask = ~0;
	ws_out_->set_frame_masking_key(mask);

	status_ = HTTP_ACLIENT_STATUS_WS_HANDSHAKE;
	send_request(NULL, 0);
}

void http_aclient::ws_read_wait(int timeout /* = 0 */)
{
	acl_assert(conn_);

	status_ = HTTP_ACLIENT_STATUS_WS_READING;

	// ע�� websocket ���ص�����
	conn_->add_read_callback(this);
	conn_->readable_await(timeout);
}

bool http_aclient::ws_send_text(char* data, size_t len)
{
	acl_assert(ws_out_ && conn_);
	return ws_out_->send_frame_text(*conn_, data, len);
}

bool http_aclient::ws_send_binary(void* data, size_t len)
{
	acl_assert(ws_out_ && conn_);
	return ws_out_->send_frame_binary(*conn_, data, len);
}

bool http_aclient::ws_send_ping(void* data, size_t len)
{
	acl_assert(ws_out_ && conn_);
	return ws_out_->send_frame_ping(*conn_, data, len);
}

bool http_aclient::ws_send_pong(void* data, size_t len)
{
	acl_assert(ws_out_ && conn_);
	return ws_out_->send_frame_pong(*conn_, data, len);
}

} // namespace acl
