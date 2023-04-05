#include "acl_stdafx.hpp"
#ifndef ACL_PREPARE_COMPILE
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"
#include "acl_cpp/stream/aio_listen_stream.hpp"
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/ipc/ipc_server.hpp"
#endif

namespace acl
{

ipc_server::ipc_server(void)
: handle_(NULL)
, sstream_(NULL)
{

}

ipc_server::~ipc_server(void)
{
	if (sstream_) {
		sstream_->close();
	}
}

bool ipc_server::open(aio_handle* handle, const char* addr /* = "127.0.0.1:0" */)
{
	if (handle == NULL) {
		logger_fatal("handle null");
	} else {
		handle_ = handle;
	}

#ifdef ACL_WINDOWS
	// ����¼������ǻ��� ACL_WINDOWS ������Ϣ����ֱ�ӷ���
	if (handle->get_engine_type() == ENGINE_WINMSG) {
		return create_window();
	}
#endif

	if (addr == NULL) {
		addr = "127.0.0.1:0";
	}
	sstream_ = NEW aio_listen_stream(handle);
	if (!sstream_->open(addr)) {
		logger_error("listen %s error(%s)", addr, acl_last_serror());
		sstream_->destroy();
		sstream_ = NULL;
		return false;
	}
	sstream_->add_accept_callback(this);

	// �������ദ�����
	on_open(get_addr());
	return true;
}

bool ipc_server::accept_callback(aio_socket_stream* client)
{
	on_accept(client);
	return true;
}

void ipc_server::close_callback(void)
{
	// �������ദ�����
	on_close();
}

bool ipc_server::timeout_callback(void)
{
	return true;
}

const char* ipc_server::get_addr(void) const
{
	acl_assert(sstream_);
	return sstream_->get_addr();
}

aio_listen_stream* ipc_server::get_stream(void) const
{
	acl_assert(sstream_);
	return sstream_;
}

aio_handle& ipc_server::get_handle(void) const
{
	return *handle_;
}

}  // namespace acl
