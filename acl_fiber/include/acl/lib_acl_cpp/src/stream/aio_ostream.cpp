#include "acl_stdafx.hpp"
#ifndef ACL_PREPARE_COMPILE
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stream/aio_ostream.hpp"
#endif

namespace acl
{

aio_timer_writer::aio_timer_writer(void)
: out_(NULL)
{

}

aio_timer_writer::~aio_timer_writer(void)
{

}

void aio_timer_writer::timer_callback(unsigned int id acl_unused)
{
	if (out_ == NULL) {
		return;
	}

	bool findit = false;

	if (out_->timer_writers_) {
		std::list<aio_timer_writer*>::iterator it =
			out_->timer_writers_->begin();
		for (; it != out_->timer_writers_->end(); ++it) {
			if ((*it) == this) {
				out_->timer_writers_->erase(it);
				findit = true;
				break;
			}
		}
	}

	if (findit == false) {
		logger_warn("Warning: timer_writer is the end!");
	}

	out_->write(buf_.c_str(), (int) buf_.length(), 0, NULL);
}

//////////////////////////////////////////////////////////////////////

aio_ostream::aio_ostream(aio_handle* handle)
: aio_stream(handle)
, timer_writers_(NULL)
{
}

aio_ostream::aio_ostream(aio_handle* handle, ACL_SOCKET fd)
: aio_stream(handle)
{
	acl_assert(handle);

	ACL_VSTREAM* vstream = acl_vstream_fdopen(fd, O_RDWR, 8192, -1,
					ACL_VSTREAM_TYPE_SOCK);
	stream_ = acl_aio_open(handle->get_handle(), vstream);

	// ���û���� enable_error ���� handle �������첽������,
	// ͬʱע��رռ���ʱ�ص�����
	this->enable_error();

	// ֻ�е������ӳɹ���ſ� hook IO д״̬
	// ע��д�ص�����
	enable_write();
}

aio_ostream::~aio_ostream(void)
{
	if (timer_writers_) {
		std::list<aio_timer_writer*>::iterator it =
			timer_writers_->begin();
		for (; it != timer_writers_->end(); ++it) {
			handle_->del_timer(*it);
			(*it)->destroy();
		}
		delete timer_writers_;
	}

	std::list<AIO_CALLBACK*>::iterator it = write_callbacks_.begin();
	for (; it != write_callbacks_.end(); ++it) {
		acl_myfree(*it);
	}
}

void aio_ostream::destroy(void)
{
	delete this;
}

void aio_ostream::add_write_callback(aio_callback* callback)
{
	acl_assert(callback);

	// �Ȳ�ѯ�ûص������Ѿ�����
	std::list<AIO_CALLBACK*>::iterator it = write_callbacks_.begin();
	for (; it != write_callbacks_.end(); ++it) {
		if ((*it)->callback == callback) {
			if ((*it)->enable == false) {
				(*it)->enable = true;
			}
			return;
		}
	}

	// ��һ����λ
	it = write_callbacks_.begin();
	for (; it != write_callbacks_.end(); ++it) {
		if ((*it)->callback == NULL) {
			(*it)->enable   = true;
			(*it)->callback = callback;
			return;
		}
	}

	// ����һ���µ�λ��
	AIO_CALLBACK* ac = (AIO_CALLBACK*) acl_mycalloc(1, sizeof(AIO_CALLBACK));
	ac->enable   = true;
	ac->callback = callback;

	// ��ӽ��ص����������
	write_callbacks_.push_back(ac);
}

int aio_ostream::del_write_callback(aio_callback* callback)
{
	std::list<AIO_CALLBACK*>::iterator it = write_callbacks_.begin();
	int   n = 0;

	if (callback == NULL) {
		for (; it != write_callbacks_.end(); ++it) {
			if ((*it)->callback == NULL) {
				continue;
			}
			(*it)->enable   = false;
			(*it)->callback = NULL;
			n++;
		}
	} else {
		for (; it != write_callbacks_.end(); ++it) {
			if ((*it)->callback != callback) {
				continue;
			}
			(*it)->enable   = false;
			(*it)->callback = NULL;
			n++;
			break;
		}
	}

	return n;
}

int aio_ostream::disable_write_callback(aio_callback* callback)
{
	std::list<AIO_CALLBACK*>::iterator it = write_callbacks_.begin();
	int   n = 0;

	if (callback == NULL) {
		for (; it != write_callbacks_.end(); ++it) {
			if ((*it)->callback == NULL || !(*it)->enable) {
				continue;
			}
			(*it)->enable = false;
			n++;
		}
	} else {
		for (; it != write_callbacks_.end(); ++it) {
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

int aio_ostream::enable_write_callback(aio_callback* callback /* = NULL */)
{
	std::list<AIO_CALLBACK*>::iterator it = write_callbacks_.begin();
	int   n = 0;

	if (callback == NULL) {
		for (; it != write_callbacks_.end(); ++it) {
			if (!(*it)->enable && (*it)->callback != NULL) {
				(*it)->enable = true;
				n++;
			}
		}
	} else {
		for (; it != write_callbacks_.end(); ++it) {
			if (!(*it)->enable && (*it)->callback == callback) {
				(*it)->enable = true;
				n++;
			}
		}
	}

	return n;
}

void aio_ostream::enable_write(void)
{
	acl_assert(stream_);

	if ((status_ & STATUS_HOOKED_WRITE)) {
		return;
	}
	status_ |= STATUS_HOOKED_WRITE;

	acl_aio_add_write_hook(stream_, write_callback, this);
}

void aio_ostream::disable_write(void)
{
	acl_assert(stream_);
	acl_aio_disable_write(stream_);
}

void aio_ostream::write_await(const void* data, int len,
	acl_int64 delay /* = 0 */, aio_timer_writer* callback /* = NULL */)
{
	if (delay > 0) {
		disable_write();

		aio_timer_writer* timer_writer;

		if (callback != NULL) {
			timer_writer= callback;
		} else {
			timer_writer = NEW aio_timer_writer();
		}

		// ���� timer_writer_ ����ĳ�Ա����
		timer_writer->out_ = this;
		timer_writer->buf_.copy(data, len);

		// ����д���������ӳ��첽д�Ķ�����
		if (timer_writers_ == NULL) {
			timer_writers_ = NEW std::list<aio_timer_writer*>;
		}
		timer_writers_->push_back(timer_writer);
		// ���ö�ʱ��
		handle_->set_timer(timer_writer, delay);
		return;
	}

	acl_assert(stream_);
	acl_aio_writen(stream_, (const char*) data, len);
}

int aio_ostream::sendto(const void* data, int len, const char* dest_addr, int flags)
{
	ACL_SOCKADDR sa;
	size_t addrlen = acl_sane_pton(dest_addr, (struct sockaddr*) &sa);
	if (addrlen == 0) {
		logger_error("invalid dest_addr=%s", dest_addr);
		return -1;
	}

	return sendto(data, len, (const struct sockaddr*) &sa, (int) addrlen, flags);
}

int aio_ostream::sendto(const void* data, int len,
	const struct sockaddr* dest_addr, int addrlen, int flags)
{
	acl_assert(stream_);
	ACL_VSTREAM* vs = acl_aio_vstream(stream_);
	ACL_SOCKET fd = ACL_VSTREAM_SOCK(vs);

#if defined(_WIN32) || defined(_WIN64)
	return (int) ::sendto(fd, (char*) data, len, flags, dest_addr, addrlen);
#else
	return (int) ::sendto(fd, data, len, flags, dest_addr, (socklen_t) addrlen);
#endif
}

void aio_ostream::writev_await(const struct iovec *iov, int count)
{
	acl_assert(stream_);
	acl_aio_writev(stream_, iov, count);
}

void aio_ostream::format_await(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	acl_aio_vfprintf(stream_, fmt, ap);
	va_end(ap);
}

void aio_ostream::vformat_await(const char* fmt, va_list ap)
{
	acl_aio_vfprintf(stream_, fmt, ap);
}

size_t aio_ostream::pending_length(void) const
{
	return acl_aio_send_pending(stream_);
}

void aio_ostream::writable_await(int timeout /* = 0 */)
{
	// ���������첽����ʱʱ��
	if (timeout >= 0) {
		ACL_AIO_SET_TIMEOUT(stream_, timeout);
	}
	acl_aio_enable_write(stream_, write_wakup, this);
}

int aio_ostream::write_callback(ACL_ASTREAM* stream acl_unused, void* ctx)
{
	aio_ostream* aos = (aio_ostream*) ctx; 
	std::list<AIO_CALLBACK*>::iterator it = aos->write_callbacks_.begin();
	for (; it != aos->write_callbacks_.end(); ++it) {
		if ((*it)->enable == false || (*it)->callback == NULL) {
			continue;
		}

		if ((*it)->callback->write_callback() == false) {
			return -1;
		}
	}
	return 0;
}

int aio_ostream::write_wakup(ACL_ASTREAM* stream acl_unused, void* ctx)
{
	aio_ostream* out = (aio_ostream*) ctx; 
	std::list<AIO_CALLBACK*>::iterator it = out->write_callbacks_.begin();
	for (; it != out->write_callbacks_.end(); ++it) {
		if ((*it)->enable == false || (*it)->callback == NULL) {
			continue;
		}

		if ((*it)->callback->write_wakeup() == false) {
			return -1;
		}
	}
	return 0;
}

}  // namespace acl
