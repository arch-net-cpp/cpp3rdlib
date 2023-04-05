#include "acl_stdafx.hpp"
#ifndef ACL_PREPARE_COMPILE
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/charset_conv.hpp"
#include "acl_cpp/stream/ofstream.hpp"
#include "acl_cpp/stdlib/xml.hpp"
#include "acl_cpp/stdlib/xml1.hpp"
#include "acl_cpp/mime/mime_define.hpp"
#include "acl_cpp/mime/mime_body.hpp"
#endif

#if !defined(ACL_MIME_DISABLE)

namespace acl {

bool mime_body::save_body(pipe_manager& out, const char* src /* = NULL */,
	int len /* = 0 */)
{
	// �Ƿ���Ҫ�� HTML ��������ȡ�����ı�����

	xml* pXml;
	if (m_htmlFirst == false && m_ctype == MIME_CTYPE_TEXT
		&& m_stype == MIME_STYPE_HTML) {
		pXml = NEW xml1();
		out.push_front(pXml);
	} else {
		pXml = NULL;
	}

	// ����Ҫ�����ַ���ת��ʱ�����ַ���ת����

	charset_conv* conv;
	if (m_toCharset[0] != 0) {
		conv = charset_conv::create(m_charset, m_toCharset);
		if (conv) {
			out.push_front(conv);
		}
	} else {
		conv = NULL;
	}

	bool ret = save(out, src, len); // ���� mime_node::save

	if (pXml) {
		delete pXml;
	}
	if (conv) {
		delete conv;
	}

	return ret;
}

bool mime_body::save_body(ostream& out, const char* src /* = NULL */,
	int len /* = 0 */)
{
	pipe_manager manager;
	manager.push_front(&out);

	return save_body(manager, src, len);
}

bool mime_body::save_body(const char* file_path, const char* src /* = NULL */,
	int len /* = 0 */)
{
	ofstream out;

	if (!out.open_trunc(file_path)) {
		logger_error("open file %s error(%s)",
			file_path, last_serror());
		return false;
	}

	if (!save_body(out, src, len)) {
#ifdef ACL_WINDOWS
		_unlink(file_path);
#else
		unlink(file_path);
#endif
		return false;
	}
	return true;
}

bool mime_body::save_body(pipe_string& out, const char* src /* = NULL */,
	int len /* = 0 */)
{
	pipe_manager manager;
	manager.push_front(&out);
	return save_body(manager, src, len);
}

bool mime_body::save_body(string& out, const char* src /* = NULL */,
	int len /* = 0 */)
{
	pipe_string ps(out);
	return save_body(ps, src, len);
}

bool mime_body::html_stype(void) const
{
	if (m_stype == MIME_STYPE_HTML) {
		return true;
	} else {
		return false;
	}
}

} // namespace acl

#endif // !defined(ACL_MIME_DISABLE)
