#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/thread_mutex.hpp"
#include "../stdlib/string.hpp"
#include "../stdlib/token_tree.hpp"
#include "sslbase_conf.hpp"
#include <vector>

typedef struct mbedtls_x509_crt mbedtls_x509_crt;
typedef struct mbedtls_ssl_config  mbedtls_ssl_config;
typedef struct mbedtls_ssl_cache_context mbedtls_ssl_cache_context;
typedef struct mbedtls_ssl_context mbedtls_ssl_context;

namespace acl {

typedef struct MBEDTLS_CERT_KEY {
	mbedtls_x509_crt* cert;
	void* pkey;
} MBEDTLS_CERT_KEY;

/**
 * SSL ֤��У�鼶�����Ͷ���
 */
typedef enum {
	MBEDTLS_VERIFY_NONE,	// ��У��֤��
	MBEDTLS_VERIFY_OPT,	// ѡ����У�飬����������ʱ�����ֺ�У��
	MBEDTLS_VERIFY_REQ	// Ҫ��������ʱУ��
} mbedtls_verify_t;

class mbedtls_io;

/**
 * SSL ���Ӷ���������࣬�������һ���������Ϊȫ�ֶ���������ÿһ�� SSL
 * ���Ӷ������֤�����ã����������ȫ���Ե�֤�顢��Կ����Ϣ��ÿһ�� SSL ����
 * (mbedtls_io) ���ñ������setup_certs ��������ʼ�������֤�顢��Կ����Ϣ
 */
class ACL_CPP_API mbedtls_conf : public sslbase_conf {
public:
	/**
	 * ���캯��
	 * @param server_side {bool} ����ָ���Ƿ���˻��ǿͻ��ˣ���Ϊ true ʱ
	 *  Ϊ�����ģʽ������Ϊ�ͻ���ģʽ
	 * @param verify_mode {mbedtls_verify_t} SSL ֤��У�鼶��
	 */
	mbedtls_conf(bool server_side = false,
		mbedtls_verify_t verify_mode = MBEDTLS_VERIFY_NONE);
	~mbedtls_conf(void);

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
	 * ע: �÷����� mbedtls_conf �н�������, ��ֱ��ʹ�����淽��
	 */
	bool add_cert(const char* /* crt_file */);

	/**
	 * @override
	 * ע: �÷����� mbedtls_conf �н�������, ��ֱ��ʹ�����淽��
	 */
	bool set_key(const char* /*key_file*/, const char* /* key_pass */);

	/**
	 * @override
	 */
	void enable_cache(bool on);

public:
	/**
	 * mbedtls_io::open �ڲ�����ñ�����������װ��ǰ SSL ���Ӷ����֤��
	 * @param ssl {void*} SSL ���Ӷ���Ϊ ssl_context ����
	 * @return {bool} ���� SSL �����Ƿ�ɹ�
	 */
	bool setup_certs(void* ssl);

	/**
	 * �����������������ض���
	 * @return {void*}������ֵΪ entropy_context ����
	 */
	void* get_entropy(void) const {
		return entropy_;
	}

public:
	/**
	 * ��� mbedtls �ֳ������⣬���Ե��ñ���������������̬���ȫ·��
	 * @param libmbedcrypto {const char*} libmbedcrypto ��̬���ȫ·��
	 * @param libmbedx509 {const char*} libmbedx509 ��̬���ȫ·��
	 * @param libmbedtls {const char*} libmbedtls ��̬���ȫ·��
	 */
	static void set_libpath(const char* libmbedcrypto,
		const char* libmbedx509, const char* libmbedtls);

	/**
	 * ��� mbedtls �ϳ�һ���⣬���Ե��ñ���������һ����̬���ȫ·��
	 * @param libmbedtls {const char*} libmbedtls ��̬���ȫ·��
	 */
	static void set_libpath(const char* libmbedtls);

	/**
	 * ��ʽ���ñ���������̬���� mbedtls ��̬��
	 * @return {bool} �����Ƿ�ɹ�
	 */
	static bool load(void);

public:
	// @override sslbase_conf
	sslbase_io* create(bool nblock);

public:
	mbedtls_ssl_config* create_ssl_config(void);

private:
	unsigned status_;
	bool  server_side_;

	int         conf_count_;
	token_tree* conf_table_;
	mbedtls_ssl_config* conf_;
	std::set<mbedtls_ssl_config*> certs_;

	const int* ciphers_;
	void* entropy_;
	void* rnd_;

	mbedtls_x509_crt* cacert_;
	string crt_file_;
	mbedtls_ssl_cache_context* cache_;
	mbedtls_verify_t verify_mode_;

	std::vector<MBEDTLS_CERT_KEY*> cert_keys_;

	bool create_host_key(string& host, string& key, size_t skip = 0);
	void get_hosts(const mbedtls_x509_crt& cert, std::vector<string>& hosts);
	void bind_host(string& host, MBEDTLS_CERT_KEY* ck);

	void map_cert(const mbedtls_x509_crt& cert, MBEDTLS_CERT_KEY* ck);
	MBEDTLS_CERT_KEY* find_ssl_config(const char* host);

private:
	int on_sni_callback(mbedtls_ssl_context* ssl,
		const unsigned char* name, size_t name_len);
	static int sni_callback(void* arg, mbedtls_ssl_context* ssl,
		const unsigned char* name, size_t name_len);

private:
	bool init_rand(void);
	void free_ca(void);
};

} // namespace acl
