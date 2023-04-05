#include "acl_stdafx.hpp"
#ifndef ACL_PREPARE_COMPILE
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"
#endif

namespace acl
{

aio_socket_stream::aio_socket_stream(aio_handle* handle,
	ACL_ASTREAM* stream, bool opened /* = false */)
: aio_stream(handle), aio_istream(handle), aio_ostream(handle)
, open_callbacks_(NULL)
{
	acl_assert(handle);
	acl_assert(stream);

	if (opened) {
		status_ |= STATUS_CONN_OPENED;
	}

	stream_ = stream;

	// ���û���� enable_error ���� handle �������첽������,
	// ͬʱע��رռ���ʱ�ص�����
	this->enable_error();

	// ֻ�е������ӳɹ���ſ� hook IO ��д״̬
	if (opened) {
		// ע����ص�����
		this->enable_read();

		// ע��д�ص�����
		this->enable_write();
	}
}

aio_socket_stream::aio_socket_stream(aio_handle* handle, ACL_SOCKET fd)
: aio_stream(handle), aio_istream(handle), aio_ostream(handle)
, open_callbacks_(NULL)
{
	acl_assert(handle);

	status_ |= STATUS_CONN_OPENED;

	ACL_VSTREAM* vstream = acl_vstream_fdopen(fd, O_RDWR, 8192, -1,
					ACL_VSTREAM_TYPE_SOCK);
	stream_ = acl_aio_open(handle->get_handle(), vstream);

	// ���û���� enable_error ���� handle �������첽������,
	// ͬʱע��رռ���ʱ�ص�����
	this->enable_error();

	// ֻ�е������ӳɹ���ſ� hook IO ��״̬
	// ע����ص�����
	this->enable_read();

	// ע��д�ص�����
	this->enable_write();
}

aio_socket_stream::~aio_socket_stream(void)
{
	if (open_callbacks_) {
		std::list<AIO_OPEN_CALLBACK*>::iterator
			it = open_callbacks_->begin();
		for (; it != open_callbacks_->end(); ++it) {
			acl_myfree(*it);
		}
		delete open_callbacks_;
	}
}

void aio_socket_stream::destroy(void)
{
	delete this;
}

void aio_socket_stream::add_open_callback(aio_open_callback* callback)
{
	if (open_callbacks_ == NULL) {
		open_callbacks_ = NEW std::list<AIO_OPEN_CALLBACK*>;
	}

	// �Ȳ�ѯ�ûص������Ѿ�����
	std::list<AIO_OPEN_CALLBACK*>::iterator it = open_callbacks_->begin();
	for (; it != open_callbacks_->end(); ++it) {
		if ((*it)->callback == callback) {
			if ((*it)->enable == false) {
				(*it)->enable = true;
			}
			return;
		}
	}

	// ��һ����λ
	it = open_callbacks_->begin();
	for (; it != open_callbacks_->end(); ++it) {
		if ((*it)->callback == NULL) {
			(*it)->enable = true;
			(*it)->callback = callback;
			return;
		}
	}

	// ����һ���µ�λ��
	AIO_OPEN_CALLBACK* ac = (AIO_OPEN_CALLBACK*)
		acl_mycalloc(1, sizeof(AIO_OPEN_CALLBACK));
	ac->enable   = true;
	ac->callback = callback;

	// ��ӽ��ص����������
	open_callbacks_->push_back(ac);
}

int aio_socket_stream::del_open_callback(aio_open_callback* callback /* = NULL */)
{
	if (open_callbacks_ == NULL) {
		return 0;
	}

	std::list<AIO_OPEN_CALLBACK*>::iterator it = open_callbacks_->begin();
	int   n = 0;

	if (callback == NULL) {
		for (; it != open_callbacks_->end(); ++it) {
			if ((*it)->callback == NULL) {
				continue;
			}
			(*it)->enable = false;
			(*it)->callback = NULL;
			n++;
		}
	} else {
		for (; it != open_callbacks_->end(); ++it) {
			if ((*it)->callback != callback) {
				continue;
			}
			(*it)->enable = false;
			(*it)->callback = NULL;
			n++;
			break;
		}
	}

	return n;
}

int aio_socket_stream::disable_open_callback(aio_open_callback* callback /* = NULL */)
{
	if (open_callbacks_ == NULL) {
		return 0;
	}

	std::list<AIO_OPEN_CALLBACK*>::iterator it = open_callbacks_->begin();
	int   n = 0;

	if (callback == NULL) {
		for (; it != open_callbacks_->end(); ++it) {
			if ((*it)->callback == NULL || !(*it)->enable) {
				continue;
			}
			(*it)->enable = false;
			n++;
		}
	} else {
		for (; it != open_callbacks_->end(); ++it) {
			if ((*it)->callback != callback || !(*it)->enable) {
				continue;
			}
			(*it)->enable = false;
			n++;
			break;
		}
	}

	return n;
}

int aio_socket_stream::enable_open_callback(aio_open_callback* callback /* = NULL */)
{
	if (open_callbacks_ == NULL) {
		return 0;
	}

	std::list<AIO_OPEN_CALLBACK*>::iterator it = open_callbacks_->begin();
	int   n = 0;

	if (callback == NULL) {
		for (; it != open_callbacks_->end(); ++it) {
			if (!(*it)->enable && (*it)->callback != NULL) {
				(*it)->enable = true;
				n++;
			}
		}
	} else {
		for (; it != open_callbacks_->end(); ++it) {
			if (!(*it)->enable && (*it)->callback == callback) {
				(*it)->enable = true;
				n++;
			}
		}
	}

	return n;
}

aio_socket_stream* aio_socket_stream::open(aio_handle* handle,
	const char* addr, int timeout)
{
	acl_assert(handle);

	ACL_ASTREAM* astream =
		acl_aio_connect(handle->get_handle(), addr, timeout);
	if (astream == NULL) {
		return NULL;
	}

	aio_socket_stream* stream = NEW aio_socket_stream(handle, astream, false);

	// ���û���� enable_error ���� handle �������첽������,
	// ͬʱע��رռ���ʱ�ص�����
	stream->enable_error();
	// ע�����ӳɹ��Ļص�����
	stream->enable_open();

	return stream;
}

aio_socket_stream* aio_socket_stream::bind(aio_handle* handle, const char * addr)
{
	acl_assert(handle);
	unsigned flag = ACL_INET_FLAG_NBLOCK | ACL_INET_FLAG_REUSEPORT;

	ACL_VSTREAM* vstream = acl_vstream_bind(addr, 0, flag);
	if (vstream == NULL) {
		logger_error("bind %s error %s", addr, last_serror());
		return NULL;
	}

	ACL_AIO* aio = handle->get_handle();
	ACL_ASTREAM* astream = acl_aio_open(aio, vstream);
	aio_socket_stream* stream = NEW aio_socket_stream(handle, astream, true);
	return stream;
}

bool aio_socket_stream::is_opened(void) const
{
	return (status_ & STATUS_CONN_OPENED) ? true : false;
}

void aio_socket_stream::enable_open(void)
{
	acl_assert(stream_);

	if ((status_ & STATUS_HOOKED_OPEN)) {
		return;
	}
	status_ |= STATUS_HOOKED_OPEN;

	acl_aio_ctl(stream_,
		ACL_AIO_CTL_CONNECT_HOOK_ADD, open_callback, this,
		ACL_AIO_CTL_END);
}

int aio_socket_stream::open_callback(ACL_ASTREAM* stream acl_unused, void* ctx)
{
	aio_socket_stream* ss = (aio_socket_stream*) ctx;

	// ����״̬�������Ѿ����ӳɹ�
	ss->status_ |= STATUS_CONN_OPENED;

	// ע����ص�����
	ss->enable_read();

	// ע��д�ص�����
	ss->enable_write();

	if (ss->open_callbacks_ == NULL) {
		return 0;
	}

	// �������еĴ򿪻ص����󣬲�����֮
	std::list<AIO_OPEN_CALLBACK*>::iterator it = ss->open_callbacks_->begin();
	for (; it != ss->open_callbacks_->end(); ++it) {
		if (!(*it)->enable || (*it)->callback == NULL) {
			continue;
		}

		if ((*it)->callback->open_callback() == false) {
			return -1;
		}
	}
	return 0;
}

}  // namespace acl
