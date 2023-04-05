#ifndef __LIB_ICMP_INCLUDE_H__
#define __LIB_ICMP_INCLUDE_H__

/* #include "lib_acl.h" */
#include "lib_icmp_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ICMP_LIB
# ifndef ICMP_API
#  define ICMP_API
# endif
#elif defined(ICMP_DLL) /* || defined(_WINDLL) */
# if defined(ICMP_EXPORTS) || defined(protocol_EXPORTS)
#  ifndef ICMP_API
#   define ICMP_API __declspec(dllexport)
#  endif
# elif !defined(ICMP_API)
#  define ICMP_API __declspec(dllimport)
# endif
#elif !defined(ICMP_API)
# define ICMP_API
#endif

/* in icmp_chat.c */
/**
 * ����ICMP�Ự����
 * @param aio {ACL_AIO*} ������Ϊ�գ����ڲ���ͨ�Ź����в��÷�����ģʽ��
 *  �����������ģʽ
 * @param check_tid {int} �Ƿ���У����Ӧ��ʱ��������е��̺߳��ֶ�
 * @return {ICMP_CHAT*} ICMP�Ự������
 */
ICMP_API ICMP_CHAT *icmp_chat_create(ACL_AIO *aio, int check_tid);

/**
 * �ͷ�ICMP�Ự����
 * @param chat {ICMP_CHAT*} ICMP�Ự������
 */
ICMP_API void icmp_chat_free(ICMP_CHAT *chat);

/**
 * ��ʼ��ĳ��Ŀ���������лỰ
 * @param host {ICMP_HOST*} ���� icmp_host_new ���صĶ���
 */
ICMP_API void icmp_chat(ICMP_HOST* host);

/**
 * ��ǰ��ICMP�Ự�����б�̽�����������
 * @param chat {ICMP_CHAT*} �Ự������
 * @return {int} ��̽����������
 */
ICMP_API int icmp_chat_size(ICMP_CHAT *chat);

/**
 * ��ǰ��ICMP�Ự�������Ѿ���ɵ�̽�����������
 * @param chat {ICMP_CHAT*} �Ự������
 * @return {int} ����ɵı�̽����������
 */
ICMP_API int icmp_chat_count(ICMP_CHAT *chat);

/**
 * �жϵ�ǰ��ICMP�Ự����������̽�������Ƿ��Ѿ����
 * @param chat {ICMP_CHAT*} �Ự������
 * @return {int} != 0: ��ʾ���; 0: ��ʾδ���
 */
ICMP_API int icmp_chat_finish(ICMP_CHAT *chat);

/**
 * ȡ�õ�ǰICMP�Ự�����еĵ�ǰ�Ự���к�ֵ
 * @param chat {ICMP_CHAT*} �Ự������
 * @return {unsigned short} �Ự���к�ֵ
 */
ICMP_API unsigned short icmp_chat_seqno(ICMP_CHAT *chat);

/* in icmp_stat.c */
/**
 * �����ǰICMP�ĻỰ״̬
 * @param chat {ICMP_CHAT*} �Ự������
 */
ICMP_API void icmp_stat(ICMP_CHAT *chat);

/**
 * ����ĳ��������ICMP�Ự״̬
 * @param host {ICMP_HOST*} ��̽����������
 * @param show_flag {int} �Ƿ�����������־�ļ�
 */
ICMP_API void icmp_stat_host(ICMP_HOST *host, int show_flag);

/* in icmp_host.c */
/**
 * ����һ���µı�̽�����������
 * @param chat {ICMP_CHAT*} �Ự������
 * @param domain {const char*} ������ʶ�ַ���������Ϊ��
 * @param ip {const char*} ����IP��ַ������Ϊ��
 * @param npkt {size_t} �Ը��������͵����ݰ�����
 * @param dlen {size_t} ÿ��̽�����ݰ��ĳ���
 * @param delay {int} ����̽�����ݰ���ʱ����(����)
 * @param timeout {int} ��̽����������Ӧ����ʱʱ��(����)
 * @return {ICMP_HOST*} ��̽����������, ���Ϊ�����ʾ����
 */
ICMP_API ICMP_HOST* icmp_host_new(ICMP_CHAT *chat, const char *domain,
	const char *ip, size_t npkt, size_t dlen, int delay, int timeout);

/**
 * �ͷ�һ����̽����������
 * @param host {ICMP_HOST*} ��̽����������
 */
ICMP_API void icmp_host_free(ICMP_HOST *host);

/**
 * ����̽�����Ļص�����
 * @param host {ICMP_HOST*} ��̽����������
 * @param arg {void*} �ص������Ĳ���֮һ
 * @param stat_respond {void (*)(ICMP_PKT_STATUS*)} ������Ӧʱ�Ļص�����
 * @param stat_timeout {void (*)(ICMP_PKT_STATUS*)} ��ʱ��Ӧʱ�Ļص�����
 * @param stat_unreach {void (*)(ICMP_PKT_STATUS*}} �������ɴ�ʱ�Ļص�����
 * @param stat_finish {void (*)(ICMP_HOST*)} ��Ը�������̽������ʱ�Ļص�����
 */
ICMP_API void icmp_host_set(ICMP_HOST *host, void *arg,
	void (*stat_respond)(ICMP_PKT_STATUS*, void*),
	void (*stat_timeout)(ICMP_PKT_STATUS*, void*),
	void (*stat_unreach)(ICMP_PKT_STATUS*, void*),
	void (*stat_finish)(ICMP_HOST*, void*));

/* in icmp_ping.c */
/**
 * ping һ̨����(�ڲ�Ĭ��ÿ��̽�������Ϊ64���ֽ�)
 * @param chat {ICMP_CHAT*} �Ự������
 * @param domain {const char*} ������ʶ�ַ���������Ϊ��
 * @param ip {const char*} ����IP��ַ������Ϊ��
 * @param npkt {size_t} �Ը��������͵����ݰ�����
 * @param delay {int} ����̽�����ݰ���ʱ����(����)
 * @param timeout {int} ��̽����������Ӧ����ʱʱ��(����)
 */
ICMP_API void icmp_ping_one(ICMP_CHAT *chat, const char *domain,
	const char *ip, size_t npkt, int delay, int timeout);

/*--------------------------------------------------------------------------*/

/**
 * low level interface for operating ICMP.
 */

ICMP_API ICMP_STREAM* icmp_stream_open(ACL_AIO *aio);
ICMP_API void icmp_stream_close(ICMP_STREAM* is);
ICMP_API ACL_VSTREAM *icmp_vstream(ICMP_STREAM *is);
ICMP_API void icmp_stream_from(ICMP_STREAM *is, struct sockaddr_in *addr);
ICMP_API void icmp_stream_dest(ICMP_STREAM *is, struct sockaddr_in *addr);
ICMP_API void icmp_stream_set_dest(ICMP_STREAM *is, struct sockaddr_in addr);

ICMP_API ICMP_HOST *icmp_host_alloc(ICMP_CHAT *chat, const char *domain,
		const char *ip);
ICMP_API void icmp_host_init(ICMP_HOST *host, unsigned char type,
		unsigned char code, size_t npkt, size_t dlen,
		int delay, int timeout);

ICMP_API ICMP_PKT *icmp_pkt_alloc(void);
ICMP_API void icmp_pkt_free(ICMP_PKT *ipkt);
ICMP_API void icmp_pkt_pack(ICMP_PKT *pkt, unsigned char type,
		unsigned char code, unsigned short id,
		const void *payload, size_t payload_len);
ICMP_API void icmp_pkt_build(ICMP_PKT *pkt, unsigned short seq);
ICMP_API void icmp_pkt_save_status(ICMP_PKT* to, const ICMP_PKT* from);
ICMP_API int  icmp_pkt_unpack(struct sockaddr_in from, const char *buf,
		int bytes, ICMP_PKT *pkt);
ICMP_API ICMP_PKT* icmp_pkt_check(const ICMP_HOST *host, const ICMP_PKT *pkt);

ICMP_API unsigned char  icmp_pkt_type(const ICMP_PKT *pkt);
ICMP_API unsigned char  icmp_pkt_code(const ICMP_PKT *pkt);
ICMP_API unsigned short icmp_pkt_cksum(const ICMP_PKT *pkt);
ICMP_API unsigned short icmp_pkt_id(const ICMP_PKT *pkt);
ICMP_API unsigned short icmp_pkt_seq(const ICMP_PKT *pkt);
ICMP_API unsigned int   icmp_pkt_gid(const ICMP_PKT *pkt);
ICMP_API const ICMP_PKT *icmp_pkt_peer(const ICMP_PKT *pkt);
ICMP_API const ICMP_PKT_STATUS *icmp_pkt_status(const ICMP_PKT *pkt);
ICMP_API size_t icmp_pkt_len(const ICMP_PKT *pkt);
ICMP_API size_t icmp_pkt_wlen(const ICMP_PKT *pkt);
ICMP_API size_t icmp_pkt_payload(const ICMP_PKT *pkt, char *buf, size_t size);

ICMP_API size_t icmp_pkt_set_extra(ICMP_PKT *pkt,
		const void *data, size_t len);
ICMP_API void icmp_pkt_set_type(ICMP_PKT *pkt, unsigned char type);
ICMP_API void icmp_pkt_set_code(ICMP_PKT *pkt, unsigned char code);
ICMP_API void icmp_pkt_set_cksum(ICMP_PKT *pkt, unsigned short cksum);
ICMP_API void icmp_pkt_set_id(ICMP_PKT *pkt, unsigned short id);
ICMP_API void icmp_pkt_set_seq(ICMP_PKT *pkt, unsigned short seq);
ICMP_API void icmp_pkt_set_data(ICMP_PKT *pkt, void *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif
