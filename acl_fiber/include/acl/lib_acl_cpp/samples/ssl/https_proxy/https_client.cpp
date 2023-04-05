#include "stdafx.h"
#include <memory>
#include <iostream>
#include "master_service.h"
#include "https_client.h"

#define	DEBUG	100

https_client::https_client(acl::ostream& out, acl::sslbase_conf* conf)
: out_(out)
, ssl_conf_(conf)
{
}

https_client::~https_client()
{
}

bool https_client::connect_server(const acl::string& server_addr,
	acl::http_client& client)
{
	// �Ȳ鱾��ӳ�������û��ӳ����
	master_service& ms = acl::singleton2<master_service>::get_instance();
	const char* addr = ms.get_addr(server_addr.c_str());
	if (addr == NULL) {
		addr = server_addr.c_str();
	}

	if (!client.open(addr, 60, 60, true)) {
		out_.format("connect server %s error", addr);
		return false;
	} else {
		logger_debug(DEBUG, 1, "connect server ok");
	}

	if (ssl_conf_) {
		logger_debug(DEBUG, 1, "begin open ssl");

		acl::sslbase_io* ssl = ssl_conf_->create(false);
		if (client.get_stream().setup_hook(ssl) == ssl) {
			out_.puts("open ssl client error");
			ssl->destroy();
			return false;
		} else {
			logger_debug(DEBUG, 1, "open ssl ok");
		}
	}

	return true;
}

bool https_client::http_request(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	const char* host = req.getRemoteHost();
	if (host == NULL || *host == 0) {
		host = req.getLocalAddr();
	}

	acl::string server_addr;
	if (ssl_conf_ == NULL) {
		server_addr.format("%s:80", host);
	} else {
		server_addr.format("%s:443", host);
	}

	std::auto_ptr<acl::http_client> backend(new acl::http_client);

	// ���ӷ�����
	if (!connect_server(server_addr, *backend)) {
		return false;
	}

	acl::http_client* front = req.getClient();

	// �����ڴ˴�ǿ���滻 HTTP ����ͷ�е� HOST �ֶ�
	//front->header_update("Host", "www.test.com:443");

	// ȡ��  HTTP ����ͷ����
	acl::string req_hdr;
	front->sprint_header(req_hdr, NULL);

	printf(">>>req_hdr: %s\r\n", req_hdr.c_str());

	// ת�� HTTP ����ͷ��������
	if (backend->get_ostream().write(req_hdr) == -1) {
		out_.puts(">>>>write header error");
		return false;
	}
	if (backend->get_ostream().write("\r\n") == -1) {
		out_.puts(">>>>write CRLF error");
		return false;
	}

	// ������������壬��ת��������������������
	long long int len = req.getContentLength();

	if (len > 0) {
		char req_body[8192];
		int ret;

		while (true) {
			ret = front->read_body(req_body, sizeof(req_body) - 1);
			if (ret < 0) {
				out_.puts(">>> read req body error");
				return false;
			}
			if (ret == 0) {
				break;
			}

			req_body[ret] = 0;
			out_.write(req_body, ret);
			if (backend->get_ostream().write(req_body, ret) == -1) {
				out_.puts(">>> write body to server error");
				return false;
			}
		}
	}
	out_.puts("");

	// ��ʼ�Ӻ�˷�������ȡ��Ӧͷ����Ӧ������

	out_.puts(">>>> begin read res header<<<<<");
	if (!backend->read_head()) {
		out_.puts(">>>>>>>>read header error<<<<<<<<<<");
		return false;
	}

	acl::string res_hdr;
	backend->sprint_header(res_hdr, NULL);
	if (res.getOutputStream().write(res_hdr) == -1) {
		out_.puts(">>>>>write res hdr error<<<<<<");
		return false;
	}

	if (res.getOutputStream().write("\r\n") == -1) {
		out_.puts(">>>write CRLF error");
		return false;
	}

	out_.puts("------------------res hdr----------------");
	out_.write(res_hdr);
	out_.puts("------------------res hdr end------------");

	char buf[8192];

	while (true) {
		int ret = backend->read_body(buf, sizeof(buf) - 1);
		if (ret < 0) {
			logger_error(">>> read body error");
			return false;
		} else if (ret == 0) {
			break;
		}

		buf[ret] = 0;
		out_.write(buf, ret);
		if (res.getOutputStream().write(buf, ret) == -1) {
			out_.puts(">>> write res body error");
			return false;
		}
	}

	const char* ptr = backend->header_value("Transfer-Encoding");
	if (ptr == NULL || *ptr == 0 || strcasecmp(ptr, "chunked") != 0) {
		return backend->keep_alive();
	}

	// ���� http ��Ӧ�壬��Ϊ������ chunk ����ģʽ��������Ҫ�����һ��
	// res.write ������������Ϊ 0 �Ա�ʾ chunk �������ݽ���
	return res.write(NULL, 0);
}
