#ifndef ACL_LISTEN_INCLUDE_H
#define ACL_LISTEN_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../stdlib/acl_define.h"
#ifdef	ACL_UNIX
#include <sys/socket.h>
#include <netdb.h>
#endif

#define ACL_INET_FLAG_NONE		0
#define ACL_INET_FLAG_NBLOCK		1
#define ACL_INET_FLAG_REUSEPORT		(1 << 1)
#define ACL_INET_FLAG_FASTOPEN		(1 << 2)
#define ACL_INET_FLAG_EXCLUSIVE		(1 << 3)

/**
 * �����׽��ֽ��������ͻ�������
 * @param sock {ACL_SOCKET} �����׽���
 * @param sa {struct sockaddr*} �洢�ͻ��˵������ַ������Ϊ��
 * @param len {socklen_t*} sa �ڴ�ռ��С������Ϊ��
 * @return {ACL_SOCKET} ������� ACL_SOCKET_INVALID ��ʾ����ʧ��
 */
ACL_API ACL_SOCKET acl_sane_accept(ACL_SOCKET sock, struct sockaddr * sa,
		socklen_t *len);

/**
 * ����ͨ�õļ����׽��ֵĺ������������տͻ�������
 * @param sock {ACL_SOCKET} �����׽���
 * @param buf {char*} ���ɹ�����һ���ͻ������Ӻ������ buf �ǿ����ſͻ���
 *  ��ַ����ʽ��ip:port (��� TCP �׽ӿ�), file_path (��� UNIX ���׽ӿ�)
 * @param size {size_t} buf ��������С
 * @param sock_type {int*} �ǿ�ʱ������ſͻ��� SOCKET ���ͣ�AF_INET/AF_UNIX
 * @return {ACL_SOCKET} �ͻ������Ӿ��, ����ֵ != ACL_SOCKET_INVALID �������
 *  ���յ�һ���ͻ�������
 */
ACL_API ACL_SOCKET acl_accept(ACL_SOCKET sock, char *buf, size_t size,
		int* sock_type);

/* in acl_inet_listen.c */

/**
 * ����ĳ�������ַ
 * @param addr {const char*} �����ַ, ��ʽ�磺127.0.0.1:8080���������ַΪ
 *  ip:0 ʱ���ɲ���ϵͳ�Զ���������˿ںţ������ɹ�����Ե��� acl_getsockname
 *  ������������ĵ�ַ
 * @param backlog {int} �����׽���ϵͳ�������Ķ��д�С
 * @param flag {unsigned} ������־λ���μ���ACL_INET_FLAG_XXX
 * @return {ACL_SOCKET} ���ؼ����׽��֣����Ϊ ACL_SOCKET_INVALID ��ʾ�޷�����
 *  �������ַ
 */
ACL_API ACL_SOCKET acl_inet_listen(const char *addr, int backlog, unsigned flag);

/**
 * ���������ͻ�����������
 * @param listen_fd {ACL_SOCKET} �����׽���
 * @return {ACL_SOCKET} �ͻ������ӣ�������� ACL_SOCKET_INVALID ��ʾ���տͻ���
 *  ���ӳ���
 */
ACL_API ACL_SOCKET acl_inet_accept(ACL_SOCKET listen_fd);

/**
 * ���������ͻ�����������
 * @param listen_fd {ACL_SOCKET} �����׽���
 * @param ipbuf {char*} ָ��ǿ��ҽ��տͻ������ӳɹ�������洢�ͻ��˵������ַ
 * @param size {size_t} ��� ipbuf ��Ϊ�����ʾ ipbuf ���ڴ�ռ��С
 * @return {ACL_SOCKET} �ͻ������ӣ�ACL_SOCKET_INVALID ��ʾ���տͻ������ӳ���
 */
ACL_API ACL_SOCKET acl_inet_accept_ex(ACL_SOCKET listen_fd, char *ipbuf,
		size_t size);

/* in acl_sane_bind.c */

/**
 * �����ַ�󶨺����������� TCP/UDP �׽ӿ�
 * @param res {const struct addrinfo*} ���������õ��ĵ�ַ��Ϣ����
 * @param flag {unsigned int} ��־λ
 * @return {ACL_SOCKET} ���� ACL_SOCKET_INVALID ��ʾ��ʧ��
 */
ACL_API ACL_SOCKET acl_inet_bind(const struct addrinfo *res, unsigned flag);

/**
 * �����ַ�󶨺����������� TCP/UDP �׽ӿ�
 * @param addr {const char*} ���������õ��ĵ�ַ
 * @param flag {unsigned int} ��־λ
 * @param socktype {int} ���󶨵� socket ���ͣ�SOCK_STREAM, SOCK_DGRAM
 * @param family {int*} ����󶨳ɹ��Ҹõ�ַ�ǿ����ŵ�ַ���ͣ������У�
 *  AF_INET, AF_INET6, AF_UNIX
 * @return {ACL_SOCKET} ���� ACL_SOCKET_INVALID ��ʾ��ʧ��
 */
ACL_API ACL_SOCKET acl_sane_bind(const char *addr, unsigned flag,
	int socktype, int *family);

#ifdef ACL_UNIX

/**
 * �� UDP ���ķ�ʽ�󶨱��� UNIX ���׽ӿ�
 * @param addr {const char*} UNIX ���׽ӿڵ�ַ·������ Linux ƽ̨�£��������ĸ
 *  Ϊ '@'������Ϊ�� Linux �µ� abstract unix domain path.
 * @param flag {unsigned} ��־λ
 * @return {ACL_SOCKET} �������׽ӿڣ�������� ACL_SOCKET_INVALID ��ʾʧ��
 */
ACL_API ACL_SOCKET acl_unix_dgram_bind(const char *addr, unsigned flag);
#endif

/**
 * ��ָ��� UDP ��ַ
 * @param addr {const char*} UDP ��ַ����ʽ��IP:PORT �� UNIX ���׽ӿڣ���Ϊ
 *  UNIX ���׽ӿ�ʱ�ĸ�ʽΪ��{domain_path}@udp������ @udp ��ʾΪ UDP ���׽ӿ�
 *  ��׺���ڲ��Զ����������׽ӿں� UNIX ���׽ӿڣ����׽ӿڽ�֧�� UNIX ƽ̨
 * @param flag {unsigned int} ��־λ
 * @param family {int*} ����󶨳ɹ��Ҹõ�ַ�ǿ����ŵ�ַ���ͣ������У�
 *  AF_INET, AF_INET6, AF_UNIX
 * @return {ACL_SOCKET} ���� ACL_SOCKET_INVALID ��ʾ��ʧ��
 */
ACL_API ACL_SOCKET acl_udp_bind3(const char *addr, unsigned flag, int *family);
ACL_API ACL_SOCKET acl_udp_bind(const char *addr, unsigned flag);

#ifdef ACL_UNIX

/* in acl_unix_listen.c */
/**
 * �������׽���
 * @param addr {const char*} �������׽���ʱ���õ�ȫ·��
 * @param backlog {int} �������д�С
 * @param flag {unsigned} ������־λ���μ���ACL_INET_FLAG_XXX
 * @return {ACL_SOCKET} �����׽��֣�ACL_SOCKET_INVALID ��ʾ�޷������������ַ
 */
ACL_API ACL_SOCKET acl_unix_listen(const char *addr, int backlog, unsigned flag);

/**
 * �����׽����Ͻ���һ���ͻ�������
 * @param fd {ACL_SOCKET} �����׽���
 * @return {ACL_SOCKET} �ͻ������ӣ�ACL_SOCKET_INVALID ��ʾ���տͻ������ӳ���
 */
ACL_API ACL_SOCKET acl_unix_accept(ACL_SOCKET fd);

/* in acl_fifo_listen.c */

ACL_API int acl_fifo_listen(const char *path, int permissions, int mode);

#endif

#if defined(_WIN32) || defined(_WIN64)
# ifdef USE_WSASOCK
/* The WSAAccept prototype */
/* */typedef SOCKET (WSAAPI *acl_accept_fn)(SOCKET, struct sockaddr FAR *,
    LPINT, LPCONDITIONPROC, DWORD_PTR);
# else
/* The accept prototype */
typedef SOCKET (WINAPI *acl_accept_fn)(SOCKET, struct sockaddr*, socklen_t*);
# endif
#else
typedef int (*acl_accept_fn)(int, struct sockaddr*, socklen_t*);
#endif

ACL_API void acl_set_accept(acl_accept_fn fn);

#ifdef __cplusplus
}
#endif

#endif
