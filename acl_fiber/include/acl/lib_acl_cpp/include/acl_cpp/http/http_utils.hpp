#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/string.hpp"

namespace acl {

class ACL_CPP_API http_utils
{
public:
	http_utils() {}
	~http_utils() {}

	/**
	 * �������� url �л�� WEB ��������ַ����ʽ��domain:port
	 * @param url {const char*} HTTP url���ǿ�
	 * @param addr {char*} �洢������洢��ʽ��domain:port
	 * @param size {size_t} out ��������С
	 * @return {bool} �Ƿ�ɹ����
	 */
	static bool get_addr(const char* url, char* addr, size_t size);

	/**
	 * �������� url �л�� WEB ������ IP ��ַ���˿ں�
	 * @param url {const char*} HTTP url���ǿ�
	 * @param domain {char*} �洢����
	 * @param size {size_t} domain �ڴ��С
	 * @param port {unsigned short*} �洢�˿ںŴ�С
	 * @return {bool} �Ƿ�ɹ����
	 */
	static bool get_addr(const char* url, char* domain, size_t size,
		unsigned short* port);
};

class ACL_CPP_API http_url {
public:
	http_url(void);
	~http_url(void) {}

	bool parse(const char* url);

public:
	/**
	 * ���� URL �е�Э�����ͣ�http �� https
	 * @return {const char*}
	 */
	const char* get_proto(void) const {
		return proto_;
	}

	/**
	 * ���� URL �е������ֶ�
	 * @return {const char*} ���ؿմ����ʾû�и��ֶ�
	 */
	const char* get_domain(void) const {
		return domain_.c_str();
	}

	/**
	 * ���ظ��� URL ��ȡ�� HTTP Э�����˶˿ںţ��ڲ�ȱʡֵΪ 80
	 * @return {unsigned short}
	 */
	unsigned short get_port(void) const {
		return port_;
	}

	/**
	 * ���ظ��� URL ��ȡ�����·�����֣����� ? ����Ĳ�����
	 * @return {const char*}
	 */
	const char* get_url_path(void) const {
		return url_path_.c_str();
	}

	/**
	 * ���ش� URL ����ȡ�Ĳ����ֶ�
	 * @return {const char*}
	 */
	const char* get_url_params(void) const {
		return url_params_.c_str();
	}

	/**
	 * ������������е��м�״̬���Ա��ظ�ʹ�ø�����������һ�� URL
	 */
	void reset(void);

private:
	char proto_[16];
	string domain_;
	unsigned short port_;
	string url_path_;
	string url_params_;

	bool parse_url_part(const char* url);
	const char* parse_domain(const char* url);
};

} // namespace acl
