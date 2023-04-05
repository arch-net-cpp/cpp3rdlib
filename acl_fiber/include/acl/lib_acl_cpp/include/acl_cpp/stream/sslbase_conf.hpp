#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/noncopyable.hpp"

namespace acl {

class sslbase_io;

class ACL_CPP_API sslbase_conf : public noncopyable {
public:
	sslbase_conf(void) {}
	virtual ~sslbase_conf(void) {}

	/**
	 * ���鷽�������� SSL IO ����
	 * @param nblock {bool} �Ƿ�Ϊ������ģʽ
	 * @return {sslbase_io*}
	 */
	virtual sslbase_io* create(bool nblock) = 0;

public:
	/**
	 * ���� CA ��֤��(ÿ������ʵ��ֻ�����һ�α�����)
	 * @param ca_file {const char*} CA ֤���ļ�ȫ·��
	 * @param ca_path {const char*} ��� CA ֤���ļ�����Ŀ¼
	 * @return {bool} ����  CA ��֤���Ƿ�ɹ�
	 * ע����� ca_file��ca_path ���ǿգ�������μ�������֤��
	 */
	virtual bool load_ca(const char* ca_file, const char* ca_path)
	{
		(void) ca_file;
		(void) ca_path;
		return false;
	}

	/**
	 * ���һ�������/�ͻ����Լ���֤�飬���Զ�ε��ñ��������ض��֤��
	 * @param crt_file {const char*} ֤���ļ�ȫ·�����ǿ�
	 * @param key_file {const char*} ��Կ�ļ�ȫ·�����ǿ�
	 * @param key_pass {const char*} ��Կ�ļ������룬û����Կ�����д NULL
	 * @return {bool} ���֤���Ƿ�ɹ�
	 */
	virtual bool add_cert(const char* crt_file, const char* key_file,
		const char* key_pass = NULL)
	{
		(void) crt_file;
		(void) key_file;
		(void) key_pass;
		return false;
	}

	/**
	 * ���һ�������/�ͻ����Լ���֤�飬���Զ�ε��ñ��������ض��֤��
	 * @param crt_file {const char*} ֤���ļ�ȫ·�����ǿ�
	 * @return {bool} ���֤���Ƿ�ɹ�
	 * @deprecated use add_cert(const char*, const char*, const char*)
	 */
	virtual bool add_cert(const char* crt_file)
	{
		(void) crt_file;
		return false;
	}

	/**
	 * ��ӷ����/�ͻ��˵���Կ(ÿ������ʵ��ֻ�����һ�α�����)
	 * @param key_file {const char*} ��Կ�ļ�ȫ·�����ǿ�
	 * @param key_pass {const char*} ��Կ�ļ������룬û����Կ�����д NULL
	 * @return {bool} �����Ƿ�ɹ�
	 * @deprecated use add_cert(const char*, const char*, const char*)
	 */
	virtual bool set_key(const char* key_file, const char* key_pass = NULL)
	{
		(void) key_file;
		(void) key_pass;
		return false;
	}

	/**
	 * ��Ϊ�����ģʽʱ�Ƿ����ûỰ���湦�ܣ���������� SSL ����Ч��
	 * @param on {bool}
	 * ע���ú������Է����ģʽ��Ч
	 */
	virtual void enable_cache(bool on)
	{
		(void) on;
	}
};

} // namespace acl
