#pragma once
#include "../acl_cpp_define.hpp"
#include <vector>
#include <set>
#include "../stdlib/string.hpp"
#include "../stdlib/token_tree.hpp"
#include "sslbase_conf.hpp"

typedef struct ssl_st SSL;
typedef struct ssl_ctx_st SSL_CTX;

namespace acl {

class token_tree;
class openssl_io;

class ACL_CPP_API openssl_conf : public sslbase_conf {
public:
	openssl_conf(bool server_side = false, int timeout = 30);
	~openssl_conf(void);

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
	 * @deprecate use add_cert(const char*, const char*, const char*)
	 */
	bool add_cert(const char* crt_file);

	/**
	 * @override
	 * @deprecate use add_cert(const char*, const char*, const char*)
	 */
	bool set_key(const char* key_file, const char* key_pass);

	/**
	 * @override
	 */
	void enable_cache(bool on);

public:
	/**
	 * ���ñ���������һ����̬���ȫ·��
	 * @param libcrypto {const char*} libcrypto.so ��̬���ȫ·��
	 * @param libssl {const char*} libssl.so ��̬���ȫ·��
	 */
	static void set_libpath(const char* libcrypto, const char* libssl);

	/**
	 * ��ʽ���ñ���������̬���� libssl.so ��̬��
	 * @return {bool} �����Ƿ�ɹ�
	 */
	static bool load(void);

	/**
	 * ���� load() �ɹ����� OpenSSL ��̬��󣬵��ñ���̬������� libssl
	 * ��̬���ؿ������Ӷ����ԴӸþ���л��ָ������ָ��
	 * @return {void*} ���� NULL ��ʾ��δ����
	 */
	static void* get_libssl_handle(void);

	/**
	 * ��� libcrypto ��̬���ؿ���
	 * @return {void*} ���� NULL ��ʾ��δ����
	 */
	static void* get_libcrypto_handle(void);

public:
	// @override sslbase_conf
	sslbase_io* create(bool nblock);

public:
	/**
	 * �Ƿ�Ϊ SSL ����ģʽ
	 * @return {bool}
	 */
	bool is_server_side(void) const {
		return server_side_;
	}

	/**
	 * ���ȱʡ��SSL_CTX����
	 * @return {SSL_CTX*}
	 */
	SSL_CTX* get_ssl_ctx(void) const;

	/**
	 * ������е��Ѿ���ʼ��ɵ� SSL_CTX ����
	 * @param out {std::vector<SSL_CTX*>&}
	 */
	void get_ssl_ctxes(std::vector<SSL_CTX*>& out);

	/**
	 * ����ģʽ��,���� SSL_CTX ����,�ڲ��Զ����� SNI �ص�����,��Ȼ�ڲ�Ҳ��
	 * ͨ������ SSL_CTX_new() API ���� SSL_CTX ����,���ڲ����Զ����ֶ�̬
	 * ���ػ�̬���ص� SSL_CTX_new() API.
	 * @return {SSL_CTX*} ���� NULL ��ʾδ���� OpenSSL ����
	 */
	SSL_CTX* create_ssl_ctx(void);

	/**
	 * ����ģʽ��, ����ⲿ�Ѿ���ʼ��ϵ� SSL_CTX, �ö��������������
	 * create_ssl_ctx() ������,�����䶯̬��̬���� OpenSSL �Ĳ�ͬ��ʽ.
	 * @param {SSL_CTX*} ���û����Լ���ʼ���õ� SSL_CTX ���󣬴����������
	 *  Ȩ���� openssl_conf �ڲ�ͳһ�����ͷ�
	 * @return {bool} ���� false ��ʾ���ʧ�ܣ�ԭ������� ctx Ϊ NULL����
	 *  ��ǰ openssl_conf ����Ϊ�ͻ���ģʽ
	 */
	bool push_ssl_ctx(SSL_CTX* ctx);

private:
	bool         server_side_;
	SSL_CTX*     ssl_ctx_;		// The default SSL_CTX.
	token_tree*  ssl_ctx_table_;	// Holding the map of host/SSL_CTX.
	std::set<SSL_CTX*> ssl_ctxes_;	// Holding all ctx just for freeing.
	int          timeout_;
	string       crt_file_;
	unsigned     status_;

	void map_ssl_ctx(SSL_CTX* ctx);
	SSL_CTX* find_ssl_ctx(const char* host);

	void get_hosts(const SSL_CTX* ctx, std::vector<string>& hosts);
	void bind_host(SSL_CTX* ctx, string& host);
	bool create_host_key(string& host, string& key, size_t skip = 0);

	int on_sni_callback(SSL* ssl, const char*host);
	static int sni_callback(SSL *ssl, int *ad, void *arg);
};

} // namespace acl
