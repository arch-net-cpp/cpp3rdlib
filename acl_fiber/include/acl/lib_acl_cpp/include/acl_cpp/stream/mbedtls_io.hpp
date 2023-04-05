#pragma once
#include "../acl_cpp_define.hpp"
#include "sslbase_io.hpp"

struct ACL_VSTREAM;

namespace acl {

class mbedtls_conf;

/**
 * stream/aio_stream ������ײ� IO ������̵Ĵ����࣬��������еĶ�д�Ĺ��̽������
 * stream/aio_stream �������� Ĭ�ϵĵײ� IO ���̣������������Ƕ�̬������(��Ϊ�Ѷ���)��
 * stream/aio_stream ������ͨ�����ñ������� destroy()�������ͷű������
 */
class ACL_CPP_API mbedtls_io : public sslbase_io {
public:
	/**
	 * ���캯��
	 * @param conf {mbedtls_conf&} ��ÿһ�� SSL ���ӽ������õ������
	 * @param server_side {bool} �Ƿ�Ϊ�����ģʽ����Ϊ�ͻ���ģʽ������
	 *  ģʽ�����ַ�����ͬ������ͨ���˲�������������
	 * @param nblock {bool} �Ƿ�Ϊ������ģʽ
	 */
	mbedtls_io(mbedtls_conf& conf, bool server_side, bool nblock = false);

	/**
	 * @override stream_hook
	 * ���� SSL IO ����
	 */
	void destroy(void);

	/**
	 * @override sslbase_io
	 * ���ô˷������� SSL ���֣��ڷ����� IO ģʽ�¸ú�����Ҫ�� handshake_ok()
	 * �������ʹ�����ж� SSL �����Ƿ�ɹ�
	 * @return {bool}
	 *  1������ false ��ʾ����ʧ�ܣ���Ҫ�ر����ӣ�
	 *  2�������� true ʱ��
	 *  2.1�����Ϊ���� IO ģʽ���ʾ SSL ���ֳɹ�
	 *  2.2���ڷ����� IO ģʽ�½����������ֹ����� IO �ǳɹ��ģ�����Ҫ����
	 *       handshake_ok() �����ж� SSL �����Ƿ�ɹ�
	 */
	bool handshake(void);

	/**
	 * ���Է�֤���Ƿ���Ч��һ�㲻�ص��ô˺�����
	 * @return {bool}
	 */
	bool check_peer(void);

protected:
	~mbedtls_io(void);

	// ʵ�� stream_hook ����鷽��

	// @override stream_hook
	bool open(ACL_VSTREAM* s);

	// @override stream_hook
	bool on_close(bool alive);

	// @override stream_hook
	int read(void* buf, size_t len);

	// @override stream_hook
	int send(const void* buf, size_t len);

private:
	mbedtls_conf& conf_;
	void* ssl_;
	void* ssn_;
	char* ebf_;

	const char* ssl_strerror(int err);

private:
	static int sock_read(void *ctx, unsigned char *buf, size_t len);
	static int sock_send(void *ctx, const unsigned char *buf, size_t len);
};

} // namespace acl
