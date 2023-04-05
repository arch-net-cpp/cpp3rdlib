#ifndef	ACL_SANE_INET_INCLUDE_H
#define	ACL_SANE_INET_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "../stdlib/acl_define.h"

#ifdef	ACL_UNIX
#include <netinet/in.h>
#include <sys/un.h>
#endif

typedef union {
	struct sockaddr_storage ss;
#ifdef AF_INET6
	struct sockaddr_in6 in6;
#endif
	struct sockaddr_in in;
#ifdef ACL_UNIX
	struct sockaddr_un un;
#endif
	struct sockaddr sa;
} ACL_SOCKADDR;

/**
 * �� socket ��ַתΪ�ַ�����ʽ��ͬʱ֧�� IPV4 �� IPV6 �� UNIX ���׽ӿ�
 * @param sa {const struct sockaddr*}
 * @param buf {char*} �洢ת�����
 * @param size {size_t} buf �ռ��С
 * @return {size_t} ���� sockaddr ��ַ����Ӧ��ַ���͵�ʵ�ʳ��ȣ������ IPV4 ��
 *  ��Ӧ struct sockaddr_in �Ľṹ�峤�ȣ����� IPV6 ���Ӧ struct sockaddr_in6
 *  �Ľṹ�峤�ȣ�����ֵ 0 ��ʾת������
 */
ACL_API size_t acl_inet_ntop(const struct sockaddr *sa, char *buf, size_t size);

/**
 * ���ַ�����ʾ�ĵ�ַתΪ socket ��ַ��֧�� IPV4 �� IPV6 �� UNIX ���׽ӿ�
 * @param af {int} ��ַ���ͣ�AF_INET��IPV4���� AF_INET6��IPV6��
 * @param src {const char*} �ַ�����ʾ�ĵ�ַ������Ϊ ip��ip#port �� ipv4:port
 * @param dst {struct sockaddr*} �洢ת�����
 * @return {size_t} ���ض�Ӧ  IPV4 �� IPV6 ��ַ�ṹ��Ĵ�С������� IPV4 ��
 *  ��Ӧ struct sockaddr_in �Ľṹ�峤�ȣ����� IPV6 ���Ӧ struct sockaddr_in6
 *  �Ľṹ�峤�ȣ�����ֵ 0 ��ʾת������
 */
ACL_API size_t acl_inet_pton(int af, const char *src, struct sockaddr *dst);

/**
 * ���ַ�����ʾ�ĵ�ַתΪ socket ��ַ��֧�� IPV4 �� IPV6 �� UNIX ���׽ӿڣ�
 * �ڲ����Զ�̽��������ַ�ַ����ĵ�ַ���ͣ����Զ������� IPV4 ���� IPV6
 * @param src {const char*} �ַ�����ʾ�ĵ�ַ������Ϊ ip��ip#port �� ipv4:port
 * @param dst {struct sockaddr*} �洢ת�����
 * @return {size_t} ���ض�Ӧ  IPV4 �� IPV6 ��ַ�ṹ��Ĵ�С������� IPV4 ��
 *  ��Ӧ struct sockaddr_in �Ľṹ�峤�ȣ����� IPV6 ���Ӧ struct sockaddr_in6
 *  �Ľṹ�峤�ȣ�����ֵ 0 ��ʾת������
 */
ACL_API size_t acl_sane_pton(const char *src, struct sockaddr *dst);

/**
 * ��IP��ַת�����ַ�����ʽ
 * @param src {const unsigned char*} struct in_addr in.s_addr �������ڴ��ʾ
 * @param dst {char *} �洢ת�����
 * @param size {size_t} dst �Ŀռ��С
 * @return {const char*} NULL: error; !NULL: ok
 */
ACL_API const char *acl_inet_ntop4(const unsigned char *src, char *dst, size_t size);

/**
 * ��IP��ַת�����ַ�����ʽ
 * @param in {struct in_addr}
 * @param dst {char *} �洢ת�����
 * @param size {size_t} dst �Ŀռ��С
 * @return {const char*} NULL: error; !NULL: ok
 */
ACL_API const char *acl_inet_ntoa(const struct in_addr in, char *dst, size_t size);

#ifdef AF_INET6
ACL_API const char *acl_inet6_ntoa(const struct in6_addr in6, char *buf, size_t size);
#endif

/**
 * �жϸ������ַ����Ƿ�����ȷ�� ip ��ַ
 * @param ip {const char *ip}
 * @return {int} != 0: ��; == 0: ��
 */
ACL_API int acl_is_ip(const char *ip);
ACL_API int acl_is_ipv4(const char *ip);
ACL_API int acl_is_ipv6(const char *ip);

/**
 * �ж������� ip ��ַ�Ƿ���� xxx.xxx.xxx.xxx:port ��ʽ
 * @param addr {const char*} IP:PORT ��ַ
 * @return {int} 1: ����, 0: ������
 */
ACL_API int acl_ipv4_addr_valid(const char *addr);

#ifdef	__cplusplus
}
#endif

#endif

