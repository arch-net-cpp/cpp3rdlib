#pragma once
#include "../acl_cpp_define.hpp"
#include <list>
#include "../stdlib/dbuf_pool.hpp"
#include "../http/http_type.hpp"

struct HTTP_HDR_RES;
struct HTTP_HDR_REQ;
struct HTTP_HDR_ENTRY;

namespace acl {

class string;
class HttpCookie;

/**
 * HTTP ͷ�࣬���Թ�������ͷ����Ӧͷ
*/
class ACL_CPP_API http_header : public dbuf_obj
{
public:
	/**
	 * ���캯��
	 * @param dbuf {dbuf_guard*} �ǿ�ʱ����Ϊ�ڴ�����
	 */
	http_header(dbuf_guard* dbuf = NULL);

	/**
	 * HTTP ����ͷ���캯��
	 * @param url {const char*} ����� URL��url ��ʽʾ�����£�
	 *   http://www.test.com/
	 *   /cgi-bin/test.cgi
	 *   http://www.test.com/cgi-bin/test.cgi
	 *   http://www.test.com/cgi-bin/test.cgi?name=value
	 *   /cgi-bin/test.cgi?name=value
	 * ����� url ���������ֶΣ����ڲ��Զ����������
	 * ����� url ���в����ֶΣ����ڲ��Զ����д������� add_param ������
	 * ���øú������û��Կ��Ե��� add_param �Ⱥ����������������
	 * �������ֶ�ֻ�в�����û�в���ֵʱ���ò������ᱻ���ԣ����������
	 * ������Ӳ�������Ӧ�õ��� add_param ���������
	 * @param dbuf {dbuf_guard*} �ǿ�ʱ����Ϊ�ڴ�����
	 * @param encoding {bool} �Ƿ�Դ����� url �еĲ������� url ���룬���Ϊ
	 *  true ������½��� url �����¶� url �еĲ������б��룬������ url ����ԭ��
	 */
	http_header(const char* url, dbuf_guard* dbuf = NULL,
		bool encoding = true);

	/**
	 * HTTP ��Ӧͷ���캯��
	 * @param status {int} ״̬���磺1xx, 2xx, 3xx, 4xx, 5xx
	 * @param dbuf {dbuf_guard*} �ǿ�ʱ����Ϊ�ڴ�����
	 */
	http_header(int status, dbuf_guard* dbuf = NULL);

	/**
	 * ���� C���� �� HTTP ��Ӧͷ���й���
	 * @param hdr_res {const HTTP_HDR_RES&}
	 * @param dbuf {dbuf_guard*} �ǿ�ʱ����Ϊ�ڴ�����
	 */
	http_header(const HTTP_HDR_RES& hdr_res, dbuf_guard* dbuf = NULL);

	/**
	 * ���� C���� �� HTTP ����ͷ���й���
	 * @param hdr_req {const HTTP_HDR_REQ&}
	 * @param dbuf {dbuf_guard*} �ǿ�ʱ����Ϊ�ڴ�����
	 */
	http_header(const HTTP_HDR_REQ& hdr_req, dbuf_guard* dbuf = NULL);

	virtual ~http_header(void);

	/**
	 * ���� HTTP ͷ��Ϣͬʱ���ϴε���ʱ��Դ�ͷ�
	 */
	void reset(void);

	//////////////////////////////////////////////////////////////////////
	//            HTTP ������ HTTP ��Ӧͨ�õķ�������
	//////////////////////////////////////////////////////////////////////

	/**
	 * ���� HTTP Э��汾��
	 * @param version {const char*} HTTP Э��汾�ţ���ʽ��1.0, 1.1
	 * @return {http_header&}
	 */
	http_header& set_proto_version(const char* version);

	/**
	 * ���� HTTP ͷ�ǿͻ��˵�����ͷ���Ƿ���������Ӧͷ
	 * @param onoff {bool} true ��ʾ������ͷ�������ʾ��Ӧͷ
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_request_mode(bool onoff);

	/**
	 * �� HTTP ͷ������ֶ�
	 * @param name {const char*} �ֶ������ǿ�ָ��
	 * @param value {const char*} �ֶ�ֵ���ǿ�ָ��
	 * @param replace {bool} ��������ظ���ʱ�Ƿ��Զ����Ǿ�����
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& add_entry(const char* name, const char* value,
			bool replace = true);
	
	/**
	 * �� HTTP ͷ�л��ָ����ͷ���ֶ�
	 * @param name {const char*} �ֶ������ǿ�ָ��
	 * @return {const char*} ����ֵ NULL ��ʾ������
	 */
	const char* get_entry(const char* name) const;

	/**
	 * ���� HTTP ͷ�е� Content-Length �ֶ�
	 * @param n {int64} ����ֵ
	 * @return {http_header&} ���ر���������ã������û���������
	 */
#if defined(_WIN32) || defined(_WIN64)
	http_header& set_content_length(__int64 n);

	/**
	 * ���ͨ�� set_content_length ���õ� HTTP ͷ�е� Content-Length ֵ
	 * @return {int64}
	 */
	__int64 get_content_length() const
	{
		return content_length_;
	}
#else
	http_header& set_content_length(long long int n);
	long long int get_content_length() const
	{
		return content_length_;
	}
#endif

	/**
	 * ���� HTTP ����ͷ����Ӧͷ���е� Range �ֶΣ����ڷֶ�������Ӧ�����ݣ�
	 * ������֧�ֶϵ������� WEB ��������
	 * @param from {http_off_t} ��ʼƫ��λ�ã��±�� 0 ��ʼ����
	 *  ֵ�� >= 0 ʱ����Ч
	 * @param to {http_off_t} �������ƫ��λ�ã��±�� 0 ��ʼ��
	 *  ������ͷ�е���ֵ���� < 0 ʱ������Ϊ���������ʼλ�ÿ�ʼ�����ճ���λ��
	 * @return {http_header&} ���ر���������ã������û���������
	 */
#if defined(_WIN32) || defined(_WIN64)
	http_header& set_range(__int64 from, __int64 to);
#else
	http_header& set_range(long long from, long long to);
#endif

	/**
	 * ������Ӧͷ�ڷֶδ���ǰ��Ҫ���ô˺��������������ܳ���
	 * @param total {http_off_t} ��������Ӧͷ���ò�����Ҫ��Ϊ�����ܳ���
	 * @return {http_header&}
	 */
#if defined(_WIN32) || defined(_WIN64)
	http_header& set_range_total(__int64 total);
#else
	http_header& set_range_total(long long total);
#endif

	/**
	 * ����� set_range ���õķֶ�����λ��ֵ
	 * @param from {http_off_t*} �ǿ�ʱ�洢��ʼλ��ƫ��
	 * @param to {http_off_t*} �ǿ�ʱ�洢����λ��ƫ��
	 */
#if defined(_WIN32) || defined(_WIN64)
	void get_range(__int64* from, __int64* to);
#else
	void get_range(long long int* from, long long int* to);
#endif

	/**
	 * ���� HTTP ͷ�е� Content-Type �ֶ�
	 * @param value {const char*} ����ֵ
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_content_type(const char* value);

	/**
	 * ���� HTTP ͷ�е� Connection �ֶΣ��Ƿ񱣳ֳ�����
	 * ������Ŀǰ��δ����֧�ֳ����ӣ���ʹ�����˸ñ�־λ��
	 * ��õ���Ӧ���ݺ�Ҳ�������ر�����
	 * @param on {bool} �Ƿ񱣳ֳ�����
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_keep_alive(bool on);

	/**
	 * ��鵱ǰͷ�Ƿ������˱��ֳ�����ѡ��
	 */
	bool get_keep_alive() const
	{
		return keep_alive_;
	}

	http_header& set_upgrade(const char* value = "websocket");
	const char* get_upgrade(void) const
	{
		return upgrade_;
	}

	/**
	 * �� HTTP ͷ����� cookie
	 * @param name {const char*} cookie ��
	 * @param value {const char*} cookie ֵ
	 * @param domain {const char*} ������
	 * @param path {const char*} �洢·��
	 * @param expires {time_t} ����ʱ�䣬����ֵΪ 0 ʱ��ʾ�����ڣ�
	 *  > 0 ʱ����������������� expires ��Ϊ����ʱ�䣬��λΪ��
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& add_cookie(const char* name, const char* value,
		const char* domain = NULL, const char* path = NULL,
		time_t expires = 0);

	/**
	 * �� HTTP ͷ����� cookie
	 * @param cookie {const http_cookie*} cookie ����
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& add_cookie(const HttpCookie* cookie);

	/**
	 * �� HTTP ͷ�л�ö�Ӧ���Ƶ� cookie ����
	 * @param name {const char*} cookie ��
	 * @return {const HttpCookie*}
	 */
	const HttpCookie* get_cookie(const char* name) const;

	/**
	 * �����͵�����ת��Ϊ rfc1123 �ַ�����ʽ������
	 */
	static void date_format(char* out, size_t size, time_t t);

	/**
	 * �ж��Ƿ��� HTTP ����ͷ
	 * @return {bool} ���� false ������ HTTP ��Ӧͷ
	 */
	bool is_request(void) const;

	/**
	 * ���ñ�־λ����� HTTP ����� URI �е� ? �ʺű�ת��(����ת�� %3F)�������Ƿ�
	 * �������Դ����ڲ�ȱʡΪ�������Դ���
	 * @param on {bool} Ϊ true ��ʾ�������Դ���
	 */
	static void uri_unsafe_correct(bool on);

	//////////////////////////////////////////////////////////////////////
	//                        HTTP ���󷽷�����
	//////////////////////////////////////////////////////////////////////
	
	/**
	 * ���� HTTP ����ͷ����
	 * @param buf {string&} �洢�������
	 * @return {bool} ��������ͷ�з�ɹ�
	 */
	bool build_request(string& buf) const;

	/**
	 * ��������� URL��url ��ʽʾ�����£�
	 * 1��http://www.test.com/
	 * 2��/cgi-bin/test.cgi
	 * 3��http://www.test.com/cgi-bin/test.cgi
	 * 3��http://www.test.com/cgi-bin/test.cgi?name=value
	 * 4��/cgi-bin/test.cgi?name=value
	 * 5��http://www.test.com
	 * ����� url ���������ֶΣ����ڲ��Զ����������
	 * ����� url ���в����ֶΣ����ڲ��Զ����д������� add_param ������
	 * ���øú������û��Կ��Ե��� add_param �Ⱥ����������������
	 * �������ֶ�ֻ�в�����û�в���ֵʱ���ò������ᱻ���ԣ����������
	 * ������Ӳ�������Ӧ�õ��� add_param ���������
	 * @param url {const char*} ����� url���ǿ�ָ��
	 * @param encoding {bool} �Ƿ�Դ����� url �еĲ������� url ���룬���Ϊ
	 *  true ������½��� url �����¶� url �еĲ������б��룬������ url ����ԭ��
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_url(const char* url, bool encoding = true);

	/**
	 * ���� HTTP ����ͷ�� HOST �ֶ�
	 * @param value {const char*} ����ͷ�� HOST �ֶ�ֵ
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_host(const char* value);

	/**
	 * ������õ� HTTP ����ͷ�е� HOST �ֶ�
	 * @return {const char*} ���ؿ�ָ���ʾû������ HOST �ֶ�
	 */
	const char* get_host() const
	{
		return host_[0] == 0 ? NULL : host_;
	}

	/**
	 * ���� HTTP Э������󷽷�����������ô˺�������Ĭ���� GET ����
	 * @param method {http_method_t} HTTP ���󷽷�
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_method(http_method_t method);

	/**
	 * ���� HTTP Э������󷽷��������������û���չ HTTP ���󷽷���
	 * ͨ���ú������õ����󷽷���Ӱ�� HTTP �������
	 * @param method {const char*} ���󷽷�
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_method(const char* method);

	/**
	 * ����Ϊ����ͷʱ��������ȡ�õ�ǰ�ʼ�ͷ�����󷽷�
	 * @param buf {string*} �洢���ַ�����ʾ�����󷽷�
	 * @return {http_method_t}
	 */
	http_method_t get_method(string* buf = NULL) const;

	/**
	 * ���� HTTP ����ͷ���Ƿ��������ѹ�����ݣ���Ӧ�� HTTP ͷ�ֶ�Ϊ��
	 * Accept-Encoding: gzip, deflate����Ŀǰ��֧�� gzip ��ʽ
	 * @param on {bool} ���Ϊ true ���Զ���� HTTP ѹ��ͷ����
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& accept_gzip(bool on);

	/**
	 * �ڵ�������� add_param/add_int/add_format ʱ���Ƿ�������ͬ��������
	 * �ڲ�ȱʡֵΪ�񣬼Ȳ�����ͬ������
	 * @param yes {bool}
	 * @return {http_header&}
	 */
	http_header& set_param_override(bool yes);

	/**
	 * ������� URL ����Ӳ����ԣ���ֻ�в�����û�в���ֵʱ��
	 * 1���������ǿմ���������ֵΪ��ָ�룬�� URL ������ֻ�У�{name}
	 * 2���������ǿմ���������ֵΪ�մ����� URL������Ϊ��{name}=
	 * @param name {const char*} ������������Ϊ��ָ��
	 * @param value {const char*} ����ֵ����Ϊ��ָ��ʱ������Ӳ�������
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& add_param(const char* name, const char* value);
	http_header& add_int(const char* name, short value);
	http_header& add_int(const char* name, int value);
	http_header& add_int(const char* name, long value);
	http_header& add_int(const char* name, unsigned short value);
	http_header& add_int(const char* name, unsigned int value);
	http_header& add_int(const char* name, unsigned long value);
	http_header& add_format(const char* name, const char* fmt, ...)
		ACL_CPP_PRINTF(3, 4);
#if defined(_WIN32) || defined(_WIN64)
	http_header& add_int(const char* name, __int64 vlaue);
	http_header& add_int(const char* name, unsigned __int64 vlaue);
#else
	http_header& add_int(const char* name, long long int value);
	http_header& add_int(const char* name, unsigned long long int value);
#endif

	http_header& set_ws_origin(const char* url);
	http_header& set_ws_key(const void* key, size_t len);
	http_header& set_ws_key(const char* key);
	http_header& set_ws_protocol(const char* proto);
	http_header& set_ws_version(int ver);

	const char* get_ws_origin(void) const
	{
		return ws_origin_;
	}

	const char* get_ws_key(void) const
	{
		return ws_sec_key_;
	}

	const char* get_ws_protocol(void) const
	{
		return ws_sec_proto_;
	}

	int get_ws_version(void) const
	{
		return ws_sec_ver_;
	}

	http_header& set_ws_accept(const char* key);
	const char* get_ws_accept(void) const
	{
		return ws_sec_accept_;
	}

	/**
	 * url �ض���
	 * @param url {const char*} �ض���� URL����ʽΪ��
	 *  http://xxx.xxx.xxx/xxx �� /xxx
	 *  �����ǰ�ߣ����Զ�����ȡ�� HOST �ֶΣ�����Ǻ��ߣ���
	 *  ����֮ǰ�� HOST
	 */
	bool redirect(const char* url);

	/**
	 * �����ض�������������ֵ == 0 �����������ض��򣬷���
	 * �����ض������ض���Ĵ����ɸ�ֵ����
	 * @param n {int} �����ض���Ĵ���
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_redirect(unsigned int n = 5);

	/**
	 * ��ȡͨ�� set_redirect ���õ����������ض������
	 * @return {unsigned int}
	 */
	unsigned int get_redirect(void) const;

	/**
	 * ����Ҫ�ض���ʱ�����������ô˺�������������һЩ���ù���
	 */
	virtual void redicrect_reset(void) {}

	//////////////////////////////////////////////////////////////////////
	//                       HTTP ��Ӧ��������
	//////////////////////////////////////////////////////////////////////

	/**
	 * ���� HTTP ��Ӧͷ����
	 * @param buf {string&} �洢�������
	 * @return {bool} ������Ӧͷ�з�ɹ�
	 */
	bool build_response(string& buf) const;

	/**
	 * ���� HTTP ��Ӧͷ�е���Ӧ״̬��
	 * @param status {int} ״̬���磺1xx, 2xx, 3xx, 4xx, 5xx
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_status(int status);

	/**
	 * �����Ӧͷ�е� HTTP ״̬��
	 * @return {int} HTTP ��Ӧ״̬�룺1xx, 2xx, 3xx, 4xx, 5xx
	 */
	int get_status(void) const
	{
		return status_;
	}

	/**
	 * ���� HTTP ��Ӧͷ�е� chunked �����־
	 * @param on {bool}
	 * @return {http_header&}
	 */
	http_header& set_chunked(bool on);

	/**
	 * �жϵ�ǰ HTTP �����Ƿ���� chunked ���䷽ʽ
	 * @return {bool}
	 */
	bool chunked_transfer(void) const
	{
		return chunked_transfer_;
	}

	/**
	 * �����Ƿ��������� CGI ��ʽ����Ӧͷ
	 * @param on {bool} �Ƿ� CGI ��ʽ��Ӧͷ
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_cgi_mode(bool on);

	/**
	 * �Ƿ������� CGI ģʽ
	 * @return {bool}
	 */
	bool is_cgi_mode() const
	{
		return cgi_mode_;
	}

	/**
	 * ���ô���������Ƿ���� gzip ��ʽ����ѹ��
	 * @param on {bool}
	 * @return {http_header&}
	 */
	http_header& set_transfer_gzip(bool on);

	/**
	 * ��õ�ǰ�����ݴ����Ƿ������˲��� gzip ѹ����ʽ
	 * @return {bool}
	 */
	bool is_transfer_gzip() const
	{
		return transfer_gzip_;
	}

private:
	dbuf_guard* dbuf_internal_;
	dbuf_guard* dbuf_;
	bool fixed_;                          // HTTP �Ƿ��Ѿ�������
	//char* domain_;  // HTTP ����������
	//unsigned short port_;               // HTTP �������˿�
	char* url_;                           // HTTP ����� URL
	std::list<HTTP_PARAM*> params_;       // �����������
	bool param_override_;                 // ����Ӳ���ʱ�Ƿ񸲸�ͬ������
	std::list<HttpCookie*> cookies_;      // cookies ����
	std::list<HTTP_HDR_ENTRY*> entries_;  // HTTP ����ͷ�и��ֶμ���
	http_method_t method_;                // HTTP ����ķ���
	char  version_[8];                    // HTTP Э��汾��
	char  method_s_[64];                  // HTTP ���󷽷����ַ�����ʾ
	char  host_[256];                     // HTTP ����ͷ�е� HOST �ֶ�
	bool keep_alive_;                     // �Ƿ񱣳ֳ�����
	unsigned int nredirect_;              // ����ض���Ĵ�������
	bool accept_compress_;                // �Ƿ����ѹ������
	int  status_;                         // ��Ӧͷ��״̬��
	bool is_request_;                     // ������ͷ������Ӧͷ
	bool cgi_mode_;                       // �Ƿ� CGI ��Ӧͷ
#if defined(_WIN32) || defined(_WIN64)
	__int64 range_from_;                  // ����ͷ�У�range ��ʼλ��
	__int64 range_to_;                    // ����ͷ�У�range ����λ��
	__int64 range_total_;                 // range ����ģʽ�¼�¼�����ܳ���
	__int64 content_length_;              // HTTP �����峤��
#else
	long long int range_from_;            // ����ͷ�У�range ��ʼλ��
	long long int range_to_;              // ����ͷ�У�range ����λ��
	long long int range_total_;           // range ����ģʽ�¼�¼�����ܳ���
	long long int content_length_;        // HTTP �����峤��
#endif
	bool chunked_transfer_;               // �Ƿ�Ϊ chunked ����ģʽ
	bool transfer_gzip_;                  // �����Ƿ���� gzip ѹ��

	char* upgrade_;
	// just for websocket
	char* ws_origin_;
	char* ws_sec_key_;
	char* ws_sec_proto_;
	int   ws_sec_ver_;
	char* ws_sec_accept_;

	void init(void);                      // ��ʼ��
	void clear(void);
	void build_common(string& buf) const; // ����ͨ��ͷ

	void add_res_cookie(const HTTP_HDR_ENTRY& entry);
	void append_accept_key(const char* sec_key, string& out) const;
	unsigned char* create_ws_key(const void* key, size_t len) const;
};

}  // namespace acl end
