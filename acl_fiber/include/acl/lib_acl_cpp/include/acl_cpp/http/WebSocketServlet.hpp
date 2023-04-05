#pragma once
#include "../acl_cpp_define.hpp"
#include "HttpServlet.hpp"

#ifndef ACL_CLIENT_ONLY

namespace acl
{

class websocket;
class session;
class HttpServletRequest;
class HttpServletResponse;

class ACL_CPP_API WebSocketServlet : public HttpServlet
{
public:
	WebSocketServlet(void);

	/**
	 * ���캯��
	 * @param stream {socket_stream*} ���� acl_master ��������ܿ�����
	 *  ����ʱ���ò�������ǿգ����� apache ���� CGI ��ʽ����ʱ���ò���
	 *  ��Ϊ NULL�����⣬�ú����ڲ�����ر������ӣ�Ӧ��Ӧ���д���������
	 *  �Ĺر�������������Է����� acl_master �ܹ����
	 * @param session {session*} ÿһ�� HttpServlet ����һ�� session ����
	 */
	WebSocketServlet(socket_stream* stream, session* session);

	/**
	 * ���캯��
	 * @param stream {socket_stream*} ���� acl_master ��������ܿ�����
	 *  ����ʱ���ò�������ǿգ����� apache ���� CGI ��ʽ����ʱ���ò���
	 *  ��Ϊ NULL�����⣬�ú����ڲ�����ر������ӣ�Ӧ��Ӧ���д���������
	 *  �Ĺر�������������Է����� acl_master �ܹ����
	 * @param memcache_addr {const char*}
	 */
	WebSocketServlet(socket_stream* stream,
		const char* memcache_addr = "127.0.0.1:11211");

	/**
	 * HttpServlet ����ʼ���У����� HTTP ���󣬲��ص����� doXXX �麯����
	 * �ú������Ȼ���� start ���̣�Ȼ����� start �ķ��ؽ��������/��Ӧ
	 * �����Ƿ�Ҫ�󱣳ֳ������������Ƿ���Ҫ��ͻ��˱��ֳ�����.
	 */

	virtual ~WebSocketServlet(void);

	// @override
	bool doRun(void);

	// @override
	bool doRun(session& session, socket_stream* stream = NULL);

	// @override
	bool doRun(const char* memcached_addr, socket_stream* stream);

	/**
	 * ���Ͷ���������.
	 * @param buf {const char *} ���͵�����
	 * @param len {int} buf ���ݳ���
	 * @return {bool} ���� false, ���� true
	 */
	bool sendBinary(const char *buf, int len);

	/**
	 * �����ı�����.
	 * @param text {const char *} ���͵�����
	 * @return {bool} ���� false, ���� true
	 */
	bool sendText(const char *text);

	/**
	 * ����pong ��Ϣ.
	 * @param buffer {const char *} ���͵�����
	 * @return {bool} ���� false, ���� true
	 */
	bool sendPong(const char *buffer = NULL);

	/**
	 * ����pong ��Ϣ.
	 * @param buffer {const char *} ���͵�����
	 * @return {bool} ���� false, ���� true
	 */
	bool sendPing(const char *buffer = NULL);

protected:
	/**
	 * websocket �ر���Ϣ�ص�
	 */
	virtual void onClose(void) {}

	/**
	 * websocket ping ��Ϣ�ص�.
	 * @param payload_len {unsigned long long} ��Ϣ�����ܳ���
	 * @param finish {bool} �����ݰ��Ƿ����һ��
	 * @return {bool} false �Ͽ����ӡ�
	 */
	virtual bool onPing(unsigned long long payload_len, bool finish) = 0;

	/**
	 * websocket pong ��Ϣ�ص�.
	 * @param payload_len {unsigned long long} ��Ϣ�����ܳ���
	 * @param finish {bool} �����ݰ��Ƿ����һ��
	 * @return {bool} false �Ͽ����ӡ�
	 */
	virtual bool onPong(unsigned long long payload_len, bool finish) = 0;

	/**
	 * websocket ping ��Ϣ�ص�.
	 * @param payload_len {unsigned long long} ��Ϣ�����ܳ���
	 * @param text {bool } true ��ʾΪ�ı�����, ������ ���������ݡ�
	 * @param finish {bool} �����ݰ��Ƿ����һ��
	 * @return {bool} false �Ͽ����ӡ�
	 */
	virtual bool onMessage(unsigned long long payload_len,
			bool text, bool finish) = 0;

	/**
	 * �������ѭ�����ô˷����������֡�������壬ֱ������ <= 0 Ϊֹ
	 * @param buf {size_t*} ���ݻ�����������Ž������
	 * @param size {size_t} buf ��������С
	 * @return {int} ���������ݳ��ȣ��������������Σ�
	 *   0: ��ʾ����֡��������
	 *  -1: ��ʾ������
	 *  >0: ��ʾ���������ݣ�Ӧ�ٴε��ñ������Ա�����µ�����
	 */
	int readPayload(void* buf, size_t size);

	/**
	 * ���� websocket ����������� NULL ��ʾ��δ���� websocket ����
	 * @return {websocket*}
	 */
	websocket* get_websocket(void) const
	{
		return ws_;
	}

private:
	// @override
	bool doWebSocket(HttpServletRequest&, HttpServletResponse&);

private:
	websocket *ws_;
	int   opcode_;
};

} // namespace acl

#endif // ACL_CLIENT_ONLY
