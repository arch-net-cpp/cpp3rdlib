#pragma once
#include "../acl_cpp_define.hpp"
#include <vector>
#include "../stdlib/string.hpp"
#include "../stdlib/noncopyable.hpp"
#include "../http/http_header.hpp"
#include "http_ctype.hpp"
#include "http_type.hpp"

#ifndef ACL_CLIENT_ONLY

namespace acl {

class dbuf_guard;
class istream;
class ostream;
class socket_stream;
class http_client;
class http_mime;
class json;
class xml;
class session;
class HttpSession;
class HttpCookie;
class HttpServletResponse;

/**
 * �� HTTP �ͻ���������ص��࣬���಻Ӧ���̳У��û�Ҳ����Ҫ
 * ����򴴽��������
 */
class ACL_CPP_API HttpServletRequest : public noncopyable {
public:
	/**
	 * ���캯��
	 * @param res {HttpServletResponse&}
	 * @param sess {session*} �洢�Ự���ݵĶ���
	 * @param stream {socket_stream&} ���������ڲ����������ر���
	 * @param charset {const char*} �����ַ�������ֵ�ǿ�ʱ��
	 *  �ڲ����Զ��� HTTP ���������ת��Ϊ�����ַ���������ת��
	 * @param body_limit {int} ��� POST ��������������Ϊ�ı�����
	 *  ����ʱ���˲�������������ĳ��ȣ���������Ϊ�������� MIME
	 *  ��ʽ�� on Ϊ false���˲�����Ч
	 */
	HttpServletRequest(HttpServletResponse& res, session* sess,
		socket_stream& stream, const char* charset = NULL,
		int body_limit = 102400);
	~HttpServletRequest(void);

	/**
	 * ��� POST �������÷��������Ƿ���Ҫ���� Form ���������ݣ�Ĭ��Ϊ������
	 * �ú��������� doRun ֮ǰ���ò���Ч����������Ϊ�������� MIME ��ʽ��
	 * ��ʹ���ñ����������˽������ݣ�Ҳ�������������н���
	 * @param yes {bool} �Ƿ���Ҫ����
	 */
	void setParseBody(bool yes);

	/**
	 * ��� HTTP �ͻ������󷽷���GET, POST, PUT, CONNECT, PURGE
	 * @param method_s {string*} �ǿ�ʱ�洢�ַ�����ʽ�����󷽷�
	 * @return {http_method_t}
	 */
	http_method_t getMethod(string* method_s = NULL) const;

	/**
	 * ��� HTTP �ͻ������������ cookie ���󼯺�
	 * @return {const std::vector<HttpCookie*>&}
	 */
	const std::vector<HttpCookie*>& getCookies(void) const;

	/**
	 * ��� HTTP �ͻ��������ĳ�� cookie ֵ
	 * @param name {const char*} cookie ���ƣ�����ǿ�
	 * @return {const char*} cookie ֵ�������� NULL ���ʾ�� cookie
	 *  ������
	 */
	const char* getCookieValue(const char* name) const;

	/**
	 * �� HTTP ���������� cookie ����
	 * @param name {const char*} cookie �����ǿ��ַ���
	 * @param value {const char*} cookie ֵ���ǿ��ַ���
	 */
	void setCookie(const char* name, const char* value);

	/**
	 * ��� HTTP ����ͷ�е�ĳ���ֶ�ֵ
	 * @param name {const char*} HTTP ����ͷ�е��ֶ������ǿ�
	 * @return {const char*} HTTP ����ͷ�е��ֶ�ֵ������ NULL
	 *  ʱ��ʾ������
	 */
	const char* getHeader(const char* name) const;

	/**
	 * ��� HTTP GET ����ʽ URL �еĲ������֣��� ? ����Ĳ���
	 * @return {const char*} û�н���URL ���������������֣�
	 *  ���ؿմ����ʾ URL ��û�в���
	 */
	const char* getQueryString(void) const;

	/**
	 * ���  http://test.com.cn/cgi-bin/test?name=value �е�
	 * /cgi-bin/test ·������
	 * @return {const char*} ���ؿմ���ʾ������
	 */
	const char* getPathInfo(void) const;

	/**
	 * ���  http://test.com.cn/cgi-bin/test?name=value �е�
	 * /cgi-bin/test?name=value ·������
	 * @return {const char*} ���ؿմ���ʾ������
	 */
	const char* getRequestUri(void) const;

	/**
	 * ������ HTTP �Ự��ص� HttpSession ��������
	 * @param create {bool} �� session ������ʱ�Ƿ��ڻ���������Զ�������
	 *  ��ĳ�ͻ��˵� session �������Ҹò���Ϊ false ʱ����ú�����
	 *  �ص� session �������û�б������������޷����ж�д����
	 * @param sid {const char*} �� session �����ڣ��� create �����ǿ�ʱ��
	 *  ��� sid �ǿգ���ʹ�ô�ֵ�����û���Ψһ�Ự��ͬʱ��ӽ��ͻ��˵�
	 *  cookie ��
	 * @return {HttpSession&}
	 *  ע�����ȼ�������� COOKIE > create = true > sid != NULL
	 */
	HttpSession& getSession(bool create = true, const char* sid = NULL);

	/**
	 * ����� HTTP �ͻ������ӹ�������������������
	 * @return {istream&}
	 */
	istream& getInputStream(void) const;

	/**
	 * ��� HTTP ˫���������ɹ��캯���Ĳ�������
	 * @return {socket_stream&}
	 */
	socket_stream& getSocketStream(void) const;

	/**
	 * ��� HTTP �������ݵ����ݳ���
	 * @return {acl_int64} ���� -1 ��ʾ����Ϊ GET ������
	 *  �� HTTP ����ͷ��û�� Content-Length �ֶ�
	 */
#if defined(_WIN32) || defined(_WIN64)
	__int64 getContentLength(void) const;
#else
	long long int getContentLength(void) const;
#endif

	/**
	 * ����ͻ��˵������Ƿֶ����ݣ���ú������������ͷ�еĳ�����ʼ��ַ
	 * ��������ַ
	 * @param range_from {long long int&} ƫ����ʼλ��
	 * @param range_to {long long int&} ƫ�ƽ���λ��
	 * @return {bool} �������Ƿֶ������򷵻�false�����Ƿֶ������򷵻�true
	 *  ע��range_from/range_to �±�� 0 ��ʼ
	 */
#if defined(_WIN32) || defined(_WIN64)
	bool getRange(__int64& range_from, __int64& range_to);
#else
	bool getRange(long long int& range_from, long long int& range_to);
#endif
	/**
	 * ��� HTTP ����ͷ�� Content-Type: text/html; charset=gb2312
	 * Content-Type ���ֶ�ֵ
	 * @param part {bool} ���Ϊ true �򷵻� text�����򷵻�������
	 * ֵ���磺text/html; charset=gb2312
	 * @param ctype {http_ctype*} Ϊ�ǿ�ָ��ʱ�����洢������ http_ctype ��Ϣ
	 * @return {const char*} ���� NULL ��ʾ Content-Type �ֶβ�����
	 */
	const char* getContentType(
		bool part = true, http_ctype* ctype = NULL) const;

	/**
	 * ��� HTTP ����ͷ�е� Content-Type: text/html; charset=gb2312
	 * �е� charset �ֶ�ֵ gb2312
	 * @return {const char*} ���� NULL ��ʾ Content-Type �ֶ� ��
	 *  charset=xxx ������
	 */
	const char* getCharacterEncoding(void) const;

	/**
	 * ���ر��ص��ֶ��ַ���
	 * @ return {const char*} ���� NULL ��ʾû�����ñ����ַ���
	 */
	const char* getLocalCharset(void) const;

	/**
	 * ���� HTTP ���ӵı��� IP ��ַ
	 * @return {const char*} ���ؿգ���ʾ�޷����
	 */
	const char* getLocalAddr(void) const;

	/**
	 * ���� HTTP ���ӵı��� PORT ��
	 * @return {unsigned short} ���� 0 ��ʾ�޷����
	 */
	unsigned short getLocalPort(void) const;

	/**
	 * ���� HTTP ���ӵ�Զ�̿ͻ��� IP ��ַ
	 * @return {const char*} ���ؿգ���ʾ�޷����
	 */
	const char* getRemoteAddr(void) const;

	/**
	 * ���� HTTP ���ӵ�Զ�̿ͻ��� PORT ��
	 * @return {unsigned short} ���� 0 ��ʾ�޷����
	 */
	unsigned short getRemotePort(void) const;

	/**
	 * ��� HTTP ����ͷ�����õ� Host �ֶ�
	 * @return {const char*} ���Ϊ�գ����ʾ������
	 */
	const char* getRemoteHost(void) const;

	/**
	 * ��� HTTP ����ͷ�����õ� User-Agent �ֶ�
	 * @return {const char*} ���Ϊ�գ����ʾ������
	 */
	const char* getUserAgent(void) const;

	/**
	 * ��� HTTP �����еĲ���ֵ����ֵ�Ѿ��� URL ������
	 * ת���ɱ���Ҫ����ַ�������� GET ���������ǻ��
	 * URL �� ? ����Ĳ���ֵ����� POST ����������Ի��
	 * URL �� ? ����Ĳ���ֵ���������еĲ���ֵ
	 * @param name {const char*} ������
	 * @param case_sensitive {bool} �Ƚ�ʱ��Բ������Ƿ����ִ�Сд
	 * @return {const char*} ���ز���ֵ��������������ʱ���� NULL
	 */
	const char* getParameter(const char* name,
		bool case_sensitive = false) const;

	/**
	 * �� HTTP ����ͷ�е� Content-Type Ϊ
	 * multipart/form-data; boundary=xxx ��ʽʱ��˵��Ϊ�ļ��ϴ��������ͣ�
	 * �����ͨ���˺������ http_mime ����
	 * @return {const http_mime*} ���� NULL ��˵��û�� MIME ����
	 *  ���ص�ֵ�û������ֹ��ͷţ���Ϊ�� HttpServletRequest ����
	 *  ���л��Զ��ͷ�
	 */
	http_mime* getHttpMime(void);

	/**
	 * ��������Ϊ text/json �� application/json ��ʽʱ�ɵ��ô˷�����ȡ json
	 * �����岢���н������ɹ��󷵻� json ���󣬸ö������ڲ�������������
	 * �� HttpServletRequest �����ͷ�ʱ�� json ����һ���ͷ�
	 * @param body_limit {size_t} �޶������峤���Է�ֹ�ڴ����������������
	 *  �峬����ֵ���򷵻ش��������ֵ��Ϊ 0�������Ƴ���
	 * @return {json*} ���ؽ����õ� json ���������� NULL �������¼���ԭ��
	 *  1�������ݳ���
	 *  2���� json ���ݸ�ʽ
	 *  3�����������
	 */
	json* getJson(size_t body_limit = 1024000);

	/**
	 * �ù��������淽�����ƣ�Ψһ�����ǽ�������������û�����Ķ�����
	 * @param out {json&}
	 * @param body_limit {size_t} �޶������峤���Է�ֹ�ڴ����������������
	 *  �峬����ֵ���򷵻ش��������ֵ��Ϊ 0�������Ƴ���
	 * @return {bool} ���� false ԭ�����£�
	 *  1�������ݳ���
	 *  2���� json ���ݸ�ʽ
	 *  3�����������
	 */
	bool getJson(json& out, size_t body_limit = 1024000);

	/**
	 * ��������Ϊ text/xml �� application/xml ��ʽʱ�ɵ��ô˷�����ȡ xml
	 * �����岢���н������ɹ��󷵻� mxl ���󣬸ö������ڲ�������������
	 * �� HttpServletRequest �����ͷ�ʱ�� xml ����һ���ͷ�
	 * @param body_limit {size_t} �޶������峤���Է�ֹ�ڴ����������������
	 *  �峬����ֵ���򷵻ش��������ֵ��Ϊ 0�������Ƴ���
	 * @return {xml*} ���ؽ����õ� xml ���������� NULL �������¼���ԭ��
	 *  1�������ݳ���
	 *  2���� xml ���ݸ�ʽ
	 */
	xml* getXml(size_t body_limit = 1024000);

	/**
	 * �ù��������淽�����ƣ�Ψһ�����ǽ�������������û�����Ķ�����
	 * @param out {xml&}
	 * @param body_limit {size_t} �޶������峤���Է�ֹ�ڴ����������������
	 *  �峬����ֵ���򷵻ش��������ֵ��Ϊ 0�������Ƴ���
	 * @return {bool} ���� false ԭ�����£�
	 *  1�������ݳ���
	 *  2���� xml ���ݸ�ʽ
	 *  3�����������
	 */
	bool getXml(xml& out, size_t body_limit = 1024000);

	/**
	 * ��� POST �෽���������������������Σ�������ֱ�ӵ��ô˷����������
	 * �����������
	 * @param body_limit {size_t} �޶������峤���Է�ֹ�ڴ����������������
	 *  �峬����ֵ���򷵻ش��������ֵ��Ϊ 0�������Ƴ���
	 * @return {string*} ���ش��������Ķ��󣬷��� NULL ������ԭ��
	 *  1�������ݳ���
	 *  2��û��������
	 *  3�����������
	 */
	string* getBody(size_t body_limit = 1024000);

	/**
	 * �ù��������淽�����ƣ�Ψһ�����ǽ���������û�����Ķ�����
	 * @param out {string&}
	 * @param body_limit {size_t}
	 * @return {bool} ���� false ԭ�����£�
	 *  1�������ݳ���
	 *  2��û��������
	 *  3�����������
	 */
	bool getBody(string& out, size_t body_limit = 1024000);

	/**
	 * ��� HTTP �������ݵ�����
	 * @return {http_request_t}��һ��� POST �����е��ϴ��ļ�Ӧ�ã���Ҫ����
	 *  �ú�������Ƿ����ϴ��������ͣ����ú������� HTTP_REQUEST_OTHER ʱ��
	 *  �û�����ͨ������ getContentType ��þ���������ַ���
	 */
	http_request_t getRequestType(void) const;

	/**
	 * ��� HTTP ����ҳ��� referer URL
	 * @return {const char*} Ϊ NULL ��˵���û�ֱ�ӷ��ʱ� URL
	 */
	const char* getRequestReferer(void) const;

	/**
	 * ��ø��� HTTP ����ͷ��õ� http_ctype ����
	 * @return {const http_ctype&}
	 */
	const http_ctype& getHttpCtype(void) const;

	/**
	 * �ж� HTTP �ͻ����Ƿ�Ҫ�󱣳ֳ�����
	 * @return {bool}
	 */
	bool isKeepAlive(void) const;

	/**
	 * ���ͻ���Ҫ�󱣳ֳ�����ʱ���� HTTP ����ͷ�л�ñ��ֵ�ʱ��
	 * @return {int} ����ֵ < 0 ��ʾ������ Keep-Alive �ֶ�
	 */
	int getKeepAlive(void) const;

	/**
	 * ��� HTTP �ͻ�������İ汾��
	 * @param major {unsigned&} ��������汾��
	 * @param minor {unsigned&} ����Ŵΰ汾��
	 * @return {bool} �Ƿ�ɹ�ȡ���˿ͻ�������İ汾��
	 */
	bool getVersion(unsigned& major, unsigned& minor) const;

	/**
	 * ��� HTTP �ͻ���֧�ֵ�����ѹ���㷨����
	 * @param out {std::vector<string>&} �洢�����
	 */
	void getAcceptEncoding(std::vector<string>& out) const;

	/*
	 * �� HTTP ����Ϊ POST ������ͨ�����������ö� HTTP �������
	 * IO ��ʱʱ��ֵ(��)
	 * @param rw_timeout {int} ��������ʱ�ĳ�ʱʱ��(��)
	 */
	void setRwTimeout(int rw_timeout);

	/**
	 * ����ϴγ���Ĵ����
	 * @return {http_request_error_t}
	 */
	http_request_error_t getLastError(void) const;

	/**
	 * �� HttpServlet ���Է���ģʽ(���� CGI ��ʽ)����ʱ�����Ե��ô�
	 * ������ÿͻ������ӵ� HTTP ����󣬴Ӷ���ø���Ĳ���
	 * @return {http_client*} ���Է���ģʽ����ʱ���˺������� HTTP �ͻ���
	 *  ���ӷǿն��󣻵��� CGI ��ʽ����ʱ���򷵻ؿ�ָ��
	 */
	http_client* getClient(void) const;

	/**
	 * �� HTTP ����ͷ��������У��ļ�������������
	 * @param out {ostream&}
	 * @param prompt {const char*} ��ʾ����
	 */	 
	void fprint_header(ostream& out, const char* prompt);

	/**
	 * �� HTTP ����ͷ�����������������
	 * @param out {string&}
	 * @param prompt {const char*} ��ʾ����
	 */
	void sprint_header(string& out, const char* prompt);

private:
	dbuf_guard* dbuf_internal_;
	dbuf_guard* dbuf_;
	http_request_error_t req_error_;
	char* cookie_name_;
	HttpServletResponse& res_;
	session* sess_;
	HttpSession* http_session_;
	socket_stream& stream_;
	int  body_limit_;
	bool body_parsed_;

	std::vector<HttpCookie*> cookies_;
	bool cookies_inited_;
	http_client* client_;
	http_method_t method_;
	bool cgi_mode_;
	http_ctype content_type_;
	char* localAddr_;
	char* remoteAddr_;
	char* localCharset_;
	int  rw_timeout_;
	std::vector<HTTP_PARAM*> params_;
	http_request_t request_type_;
	bool parse_body_;
	http_mime* mime_;
	string* body_;
	json* json_;
	xml* xml_;

	bool readHeaderCalled_;
	bool readHeader(string* method_s);

	void add_cookie(char* data);
	void parseParameters(const char* str);
};

} // namespace acl

#endif // ACL_CLIENT_ONLY
