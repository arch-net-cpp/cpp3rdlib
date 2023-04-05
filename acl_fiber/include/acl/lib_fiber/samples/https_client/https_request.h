#pragma once

class https_request
{
public:
	https_request(const char* addr, acl::polarssl_conf* ssl_conf);
	~https_request(void);

	void* run(void);

private:
	acl::http_request request_;
	acl::string to_charset_;

	// 处理 text/plain 类型数据
	bool do_plain(acl::http_request& req);

	// 处理 text/xml 类型数据
	bool do_xml(acl::http_request& req);

	// 处理 text/json 类型数据
	bool do_json(acl::http_request& req);
};
