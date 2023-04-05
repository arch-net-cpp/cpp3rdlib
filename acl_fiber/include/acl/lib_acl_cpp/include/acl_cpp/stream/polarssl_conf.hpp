#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/thread_mutex.hpp"
#include "sslbase_conf.hpp"
#include <vector>

namespace acl {

/**
 * SSL ֤��У�鼶�����Ͷ���
 */
typedef enum {
	POLARSSL_VERIFY_NONE,	// ��У��֤��
	POLARSSL_VERIFY_OPT,	// ѡ����У�飬����������ʱ�����ֺ�У��
	POLARSSL_VERIFY_REQ	// Ҫ��������ʱУ��
} polarssl_verify_t;

class polarssl_io;

/**
 * SSL ���Ӷ���������࣬�������һ���������Ϊȫ�ֶ���������ÿһ�� SSL
 * ���Ӷ������֤�����ã����������ȫ���Ե�֤�顢��Կ����Ϣ��ÿһ�� SSL ����
 * (polarssl_io) ���ñ������setup_certs ��������ʼ�������֤�顢��Կ����Ϣ
 */
class ACL_CPP_API polarssl_conf : public sslbase_conf {
public:
	/**
	 * ���캯��
	 * @param server_side {bool} ����ָ���Ƿ���˻��ǿͻ��ˣ���Ϊ true ʱ
	 *  Ϊ�����ģʽ������Ϊ�ͻ���ģʽ
	 * @param verify_mode {polarssl_verify_t} SSL ֤��У�鼶��
	 */
	polarssl_conf(bool server_side = false,
		polarssl_verify_t verify_mode = POLARSSL_VERIFY_NONE);
	virtual ~polarssl_conf(void);

	/**
	 * @override
	 */
	bool load_ca(const char* ca_file, const char* ca_path);

	/**
	 * @override
	 */
	bool add_cert(const char* crt_file, const char* key_file,
		const char* key_pass = NULL);

	/**
	 * @override
	 */
	bool add_cert(const char* crt_file);

	/**
	 * @override
	 */
	bool set_key(const char* key_file, const char* key_pass = NULL);

	/**
	 * @override
	 */
	void enable_cache(bool on);

public:
	/**
	 * ���� SSL ֤��У�鷽ʽ���ڲ�ȱʡΪ��У��֤��
	 * @param verify_mode {polarssl_verify_t}
	 */
	void set_authmode(polarssl_verify_t verify_mode);

	/**
	 * �����������������ض���
	 * @return {void*}������ֵΪ entropy_context ����
	 */
	void* get_entropy(void)
	{
		return entropy_;
	}

	/**
	 * stream_hook::open �ڲ�����ñ�����������װ��ǰ SSL ���Ӷ����֤��
	 * @param ssl {void*} SSL ���Ӷ���Ϊ ssl_context ����
	 * @param server_side {bool} �Ƿ���˻��ǿͻ���
	 * @return {bool} ���� SSL �����Ƿ�ɹ�
	 */
	bool setup_certs(void* ssl, bool server_side);

public:
	/**
	 * �������ȵ��ô˺������� libpolarssl.so ��ȫ·��
	 * @param path {const char*} libpolarssl.so ��ȫ·��
	 */
	static void set_libpath(const char* path);

	/**
	 * ������ʽ���ñ���������̬���� polarssl ��̬��
	 * @return {bool} �����Ƿ�ɹ�
	 */
	static bool load(void);

public:
	// @override sslbase_conf
	sslbase_io* create(bool nblock);

private:
	friend class polarssl_io;

	bool has_inited_;
	thread_mutex lock_;

	bool  server_side_;
	void* entropy_;
	void* cacert_;
	void* pkey_;
	void* cert_chain_;
	void* cache_;
	polarssl_verify_t verify_mode_;

private:
	void init_once(void);
	void free_ca(void);
};

} // namespace acl
