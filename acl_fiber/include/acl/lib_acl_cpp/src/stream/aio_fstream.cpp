#include "acl_stdafx.hpp"
#ifndef ACL_PREPARE_COMPILE
#include "acl_cpp/stream/fstream.hpp"
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/aio_fstream.hpp"
#endif

namespace acl {

aio_fstream::aio_fstream(aio_handle* handle)
: aio_stream(handle), aio_istream(handle), aio_ostream(handle)
{
	acl_assert(handle);
}

aio_fstream::aio_fstream(aio_handle* handle, ACL_FILE_HANDLE fd,
	unsigned int oflags /* = 600 */)
: aio_stream(handle), aio_istream(handle), aio_ostream(handle)
{
	acl_assert(handle);
	acl_assert(fd != ACL_FILE_INVALID);

	ACL_VSTREAM* vstream = acl_vstream_fhopen(fd, oflags);
	stream_ = acl_aio_open(handle->get_handle(), vstream);

	// ���û���� enable_error ���� handle �������첽������,
	// ͬʱע��رռ���ʱ�ص�����
	this->enable_error();

	// ֻ�е������ӳɹ���ſ� hook IO ��д״̬
	// ע����ص�����
	this->enable_read();

	// ע��д�ص�����
	this->enable_write();
}

aio_fstream::~aio_fstream(void)
{
}

void aio_fstream::destroy(void)
{
	delete this;
}

bool aio_fstream::open(const char* path, unsigned int oflags, unsigned int mode)
{
	ACL_VSTREAM* fp = acl_vstream_fopen(path, oflags, mode, 8192);
	if (fp == NULL) {
		return false;
	}
	stream_ = acl_aio_open(handle_->get_handle(), fp);

	// ���û���� enable_error ���� handle �������첽������,
	// ͬʱע��رռ���ʱ�ص�����
	this->enable_error();

	// ֻ�е������ӳɹ���ſ� hook IO ��д״̬
	// hook ���ص�����
	if ((oflags & (O_RDONLY | O_RDWR | O_APPEND | O_CREAT | O_TRUNC))) {
		this->enable_read();
	}

	// hook д�ص�����
	if ((oflags & (O_WRONLY | O_RDWR | O_APPEND | O_CREAT | O_TRUNC))) {
		this->enable_write();
	}

	return true;
}

bool aio_fstream::open_trunc(const char* path, unsigned int mode /* = 0600 */)
{
	return open(path, O_RDWR | O_CREAT | O_TRUNC, mode);
}

bool aio_fstream::create(const char* path, unsigned int mode /* = 0600 */)
{
	return open(path, O_RDWR | O_CREAT, mode);
}

bool aio_fstream::open_read(const char* path)
{
	return open(path, O_RDONLY, 0200);
}

bool aio_fstream::open_write(const char* path)
{
	return open(path, O_WRONLY | O_TRUNC | O_CREAT, 0600);
}

bool aio_fstream::open_append(const char* path)
{
	return open(path, O_WRONLY | O_APPEND | O_CREAT, 0600);
}

}  // namespace
