#include "stdafx.h"
#include "http_servlet.h"

http_servlet::http_servlet(acl::redis_client_cluster& cluster, size_t max_conns)
{
	// ���� session �洢����
	session_ = new acl::redis_session(cluster, max_conns);
}

http_servlet::~http_servlet(void)
{
	delete session_;
}

bool http_servlet::doUnknown(acl::HttpServletRequest&,
	acl::HttpServletResponse& res)
{
	res.setStatus(400);
	res.setContentType("text/html; charset=");
	// ���� http ��Ӧͷ
	if (res.sendHeader() == false)
		return false;
	// ���� http ��Ӧ��
	acl::string buf("<root error='unkown request method' />\r\n");
	(void) res.getOutputStream().write(buf);
	return false;
}

bool http_servlet::doGet(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	return doPost(req, res);
}

bool http_servlet::doPost(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
#if 0
	const char* session_name = req.getSession().getAttribute("session_name");
	if (*session_name == 0)
	{
		req.getSession().setAttribute("session_name", "name");
		req.getSession().setMaxAge(100);
	}
	session_name = req.getSession().getAttribute("session_name");

	const char* session_user = req.getSession().getAttribute("session_user");
	if (*session_user == 0)
		req.getSession().setAttribute("session_user", "user");
	session_user = req.getSession().getAttribute("session_user");
#else
	const char* session_name = "name", *session_user = "user";
#endif

	// ȡ������� cookie
	const char* cookie_name = req.getCookieValue("cookie_name");

	bool keep_alive = req.isKeepAlive();

	const char* param1 = req.getParameter("name1");
	const char* param2 = req.getParameter("name2");

	// ���� xml ��ʽ��������
	acl::xml1 body;
	body.get_root()
		.add_child("root", true)
			.add_child("session", true)
				.add_child("session_name", true)
					.set_text(session_name)
				.get_parent()
				.add_child("session_user", true)
					.set_text(session_user)
				.get_parent()
			.get_parent()
			.add_child("cookie", true)
				.add_child("cookie_name", true)
					.set_text(cookie_name ? cookie_name : "")
				.get_parent()
			.get_parent()
			.add_child("params", true)
				.add_child("param", true)
					.add_attr("name1", param1 ? param1 : "null")
				.get_parent()
				.add_child("param", true)
					.add_attr("name2", param2 ? param2 : "null");
	acl::string buf;
	body.build_xml(buf);

#if 0
	res.setContentType("text/xml; charset=utf-8")	// ������Ӧ�ַ���
		.setKeepAlive(keep_alive)		// �����Ƿ񱣳ֳ�����
		//.setContentLength(buf.length());
#else
	res.setContentType("text/xml; charset=utf-8")	// ������Ӧ�ַ���
		.setKeepAlive(keep_alive)		// �����Ƿ񱣳ֳ�����
		.setContentEncoding(true)		// �����Ƿ�ѹ������
		.setChunkedTransferEncoding(true);	// ���� chunk ���䷽ʽ
#endif

	//logger("access http://%s%s", req.getRemoteAddr(), req.getRequestUri());

	// ���� http ��Ӧ�壬��Ϊ������ chunk ����ģʽ��������Ҫ�����һ��
	// res.write ������������Ϊ 0 �Ա�ʾ chunk �������ݽ���
	return res.write(buf) && res.write(NULL, 0);
}
