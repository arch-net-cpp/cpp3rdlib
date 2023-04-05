#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/string.hpp"
#include "../stdlib/noncopyable.hpp"
#if defined(_WIN32) || defined(_WIN64)
#include <WinSock2.h>
#endif

namespace acl {

class socket_stream;

enum {
	OPEN_FLAG_NONE      = 0,
	OPEN_FLAG_NONBLOCK  = 1,	// ������ģʽ
	OPEN_FLAG_REUSEPORT = 1 << 1,	// �˿ڸ��ã�Ҫ�� Linux3.0 ����
	OPEN_FLAG_FASTOPEN  = 1 << 2,	// �Ƿ����� Fast open��ʵ��׶Σ�
	OPEN_FLAG_EXCLUSIVE = 1 << 3,	// �Ƿ��ֹ���õ�ַ
};

/**
 * ����˼����׽ӿ��࣬���տͻ������ӣ��������ͻ��������Ӷ���
 */
class ACL_CPP_API server_socket : public noncopyable {
public:
#if 0
	/**
	 * ���캯�������ñ����캯����������෽�� open ������ָ�������ַ
	 * @param backlog {int} �����׽ӿڶ��г���
	 * @param block {bool} ������ģʽ���Ƿ�����ģʽ
	 */
	server_socket(int backlog, bool block);
#endif

	/**
	 * ���캯��
	 * @param flag {unsigned} ����μ� OPEN_FLAG_XXX
	 * @param backlog {int} �����׽ӿڶ��г���
	 */
	server_socket(unsigned flag, int backlog);

	/**
	 * ���캯�������ñ����캯�����ֹ�ٵ��� open ����
	 * @param sstream {ACL_VSTREAM*} �ⲿ�����ļ��������󣬱����ʹ��
	 *  �������ͷţ���Ӧ�����йرոü�������
	 */
	server_socket(ACL_VSTREAM* sstream);

	/**
	 * ���캯�������ñ����캯�����ֹ�ٵ��� open ����
	 * @param fd {ACL_SOCKET} �ⲿ�����ļ�������������ʹ�õ������ͷţ�
	 *  ��Ӧ�����йرոü������
	 */
#if defined(_WIN32) || defined(_WIN64)
	server_socket(SOCKET fd);
#else
	server_socket(int fd);
#endif

	server_socket(void);
	~server_socket(void);

	/**
	 * ��ʼ������������˵�ַ
	 * @param addr {const char*} ������������ַ����ʽΪ��
	 *  ip:port���� unix �����£������������׽ӿڣ���ʽΪ��/path/xxx����
	 *  Linux ƽ̨�£�������׽ӿڵ�ַΪ��@xxx ��ʽ������һ����ĸΪ @ ��
	 *  �ڲ��Զ����� Linux �µĳ������׽��ַ�ʽ��abstract unix socket��
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool open(const char* addr);

	/**
	 * �жϵ�ǰ�����׽ӿ��Ƿ����
	 * @return {bool}
	 */
	bool opened(void) const;

	/**
	 * �ر��Ѿ��򿪵ļ����׽ӿ�
	 * @return {bool} �Ƿ������ر�
	 */
	bool close(void);

	/**
	 * �������׽ӿڴӷ�����������н��
	 * @return {SOCKET} ���ر����ľ��
	 */
#if defined(_WIN32) || defined(_WIN64)
	SOCKET unbind(void);
#else
	int unbind(void);
#endif

	/**
	 * ���տͻ������Ӳ������ͻ���������
	 * @param timeout {int} ����ֵ >= 0 ʱ�����ó�ʱ��ʽ���տͻ������ӣ�
	 *  ����ָ��ʱ����δ��ÿͻ������ӣ��򷵻� NULL
	 * @param etimed {bool*} ����ָ��� NULL ʱ�������ʱ���¸ú�������
	 *  NULL�����ֵ����Ϊ true
	 * @return {socket_stream*} ���ؿձ�ʾ����ʧ�ܻ�ʱ
	 */
	socket_stream* accept(int timeout = -1, bool* etimed = NULL);

	/**
	 * ��ü����ĵ�ַ
	 * @return {const char*} ����ֵ�ǿ�ָ��
	 */
	const char* get_addr(void) const
	{
		return addr_.c_str();
	}

	/**
	 * ������������������ַ����ñ��������Ի�ü����׽ӿ�
	 * @return {int}
	 */
#if defined(_WIN32) || defined(_WIN64)
	SOCKET sock_handle(void) const
#else
	int sock_handle(void) const
#endif
	{
		return fd_;
	}

	/**
	 * ���ü����׽��ֵ��ӳٽ��չ��ܣ������ͻ���������������ʱ�Ž������ӷ���
	 * ��Ӧ�ã�Ŀǰ�ù��ܽ�֧�� Linux
	 * @param timeout {int} ����ͻ��������ڹ涨��ʱ����δ�������ݣ�
	 *  Ҳ�������ӷ��ظ�Ӧ��
	 */
	void set_tcp_defer_accept(int timeout);

private:
	int      backlog_;
	unsigned open_flag_;
	bool     unix_sock_;
	string   addr_;

#if defined(_WIN32) || defined(_WIN64)
	SOCKET fd_;
	SOCKET fd_local_;
#else
	int   fd_;
	int   fd_local_;
#endif
};

} // namespace acl
