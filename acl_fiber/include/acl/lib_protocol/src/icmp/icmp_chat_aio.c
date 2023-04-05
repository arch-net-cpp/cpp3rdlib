#include "StdAfx.h"
#include "icmp/lib_icmp.h"
#include "icmp_struct.h"
#include "icmp_private.h"

static void delay_send_pkt(ICMP_HOST*, ICMP_PKT*, int);
static int read_close_fn(ACL_ASTREAM*, void*);
static int read_ready_fn(ACL_ASTREAM*, void *, const char*, int);

static void check_timer(int event_type acl_unused,
	ACL_EVENT *event acl_unused, void *context)
{
	const char *myname = "check_timer";
	ICMP_HOST *host = (ICMP_HOST *) context;
	ICMP_CHAT *chat = host->chat; 

	if (chat == NULL)
		acl_msg_fatal("%s(%d): chat null", myname, __LINE__);

	while (1) {
		ICMP_PKT *pkt = (ICMP_PKT*) chat->timer->popup(chat->timer);
		if (pkt == NULL)
			break;

		/* �㱨�������ʱ */
		icmp_stat_timeout(host, pkt);

		if (host->ipkt >= host->npkt) {
			/* ����Ը������İ�������ϣ���ص�״̬�㱨���� */
			if (host->stat_finish)
				host->stat_finish(host, host->arg);

			/* ����������������1 */
			chat->cnt++;
		}

		/* ��ʱ������һ���������ݰ� */
		delay_send_pkt(host, host->pkts[host->ipkt++], 0);
	}

	/* ������һ�ζ�ʱ�� */
	acl_aio_request_timer(chat->aio, check_timer,
		host, chat->check_inter, 0);
}

static void send_pkt(ICMP_HOST *host, ICMP_PKT *pkt)
{
	ICMP_CHAT   *chat    = host->chat;
	ACL_ASTREAM *astream = chat->is->astream;
	ACL_VSTREAM *vstream = acl_aio_vstream(astream);
	int  ret;

	/* �齨�������ݰ� */
	icmp_pkt_build(pkt, chat->seq++);

	/* ָ����ǰ��Ŀ��������ַ */
	chat->is->dest = host->dest;
	/* ����ͬ�����͵�ģʽ */
	ret = acl_vstream_writen(vstream, (const char*) pkt, (int) pkt->wlen);
	host->nsent++;

	if (ret == ACL_VSTREAM_EOF) {
		/* �㱨�������ɴ���Ϣ */
		icmp_stat_unreach(host, pkt);

		if (host->ipkt >= host->npkt) {
			/* ICMP_HOST ����� ICMP ���ѷ���, �� count ֵ�� 1 */
			chat->cnt++;

			/* �㱨�� ICMP_HOST ����� ICMP ���ɴ�״̬ */
			if (host->stat_finish)
				host->stat_finish(host, host->arg);
		} else {
			ICMP_PKT *pkt_next = host->pkts[host->ipkt++];

			/* �ӳٷ�����һ�����ݰ� */
			delay_send_pkt(host, pkt_next, host->delay);
		}

	} else {
		/* ���ð����볬ʱ������ */
		chat->timer->request(chat->timer, pkt, host->timeout);
	}
}

void icmp_chat_aio(ICMP_HOST* host)
{
	if (host->ipkt >= host->npkt)
		return;

	/* ���͵ĵ�һ�����������Ҫ����д��ʱ�� */
	send_pkt(host, host->pkts[host->ipkt++]);
}

/* ĳ�����ķ��Ͷ�ʱ������Ļص����� */
static void delay_send_timer(int event_type acl_unused,
	ACL_EVENT *event acl_unused, void *context)
{
	ICMP_PKT *pkt = (ICMP_PKT*) context;

	/* ���͸� ICMP �� */
	send_pkt(pkt->host, pkt);
}

static void delay_send_pkt(ICMP_HOST *host, ICMP_PKT *pkt, int delay)
{
	const char *myname = "delay_send_pkt";

	if (pkt == NULL)
		acl_msg_fatal("%s(%d): pkt null", myname, __LINE__);

	/* ����д��ʱ������Ϊ����� delay �Ǻ��뼶���� acl_aio_request_timer
	 * ��ʱ�䵥λ��΢��, ������Ҫ�� dely �ɺ���תΪ΢��
	 */
	acl_aio_request_timer(host->chat->aio, delay_send_timer,
		pkt, delay * 1000, 0);
}

static int read_close_fn(ACL_ASTREAM *astream acl_unused, void *arg)
{
	const char *myname = "read_close_fn";
	ICMP_CHAT *chat = (ICMP_CHAT*) arg;

	/* ���ú���������ʱ�����Ѿ��������׽��������⣬������Ҫ����һ���� */

	acl_msg_warn("%s(%d): sock read error", myname, __LINE__);
	icmp_stream_reopen(chat->aio, chat->is);

	/* ���������첽�����̣�ע����ʱchat->is->astream �Ѿ���ͬ��
	 * �ú����Ĳ��� astream ��!
	 */
	acl_aio_read(chat->is->astream);

	/* ����-1��ʹ���첽�����Զ��ر� */
	return (-1);
}

static int read_ready_fn(ACL_ASTREAM *astream, void *arg,
	const char *data, int dlen)
{
	const char *myname = "read_ready_fn";
	ICMP_HOST *host = (ICMP_HOST*) arg;
	ICMP_CHAT *chat = host->chat;
	ICMP_PKT  *pkt_src, pkt;

#define	READ_RETURN(_x_) do { \
	acl_aio_read(astream); \
	return(_x_); \
} while (0)

	if (chat == NULL)
		acl_msg_fatal("%s(%d): chat null", __FILE__, __LINE__);

	if (icmp_pkt_unpack(chat->is->from, data, dlen, &pkt) < 0)
		READ_RETURN(0);
	if (pkt.hdr.id != chat->pid)
		READ_RETURN(0);
	if (pkt.hdr.type != ICMP_TYPE_ECHOREPLY)
		READ_RETURN(0);
	if (chat->check_id && pkt.body.gid != chat->gid)
		READ_RETURN(0);

	/* �����������ݣ�ȡ���÷��Ͱ��Ķ���ʱ��ʱ�� */
	pkt_src = chat->timer->find_delete(chat->timer, pkt.hdr.seq);

	if (pkt_src == NULL) {
		acl_msg_warn("%s(%d): seq(%d) not found",
			myname, __LINE__, pkt.hdr.seq);
		READ_RETURN(0);
	}

	icmp_pkt_save_status(pkt_src, &pkt);

	/* �㱨״̬ */
	icmp_stat_report(host, pkt_src);

	if (host->ipkt >= host->npkt) {
		if (host->stat_finish)
			host->stat_finish(host, host->arg);
		chat->cnt++;
		READ_RETURN(0);
	}

	/* ��ʱ������һ���������ݰ� */
	delay_send_pkt(host, host->pkts[host->ipkt++], host->delay);
	READ_RETURN(0);
}

static int timeout_fn(ACL_ASTREAM *astream acl_unused, void *arg acl_unused)
{
	const char *myname = "timeout_fn";

	acl_msg_fatal("%s(%d): be called", myname, __LINE__);

	/* not reached */
	return (-1);
}

static int write_ready_fn(ACL_ASTREAM *astream acl_unused, void *arg acl_unused)
{
	const char *myname = "write_ready_fn";

	acl_msg_fatal("%s(%d): be called", myname, __LINE__);
	return (-1);
}

void icmp_chat_aio_add(ICMP_CHAT *chat, ICMP_HOST *host)
{
	acl_aio_set_delay_sec(chat->aio, 0);
	acl_aio_set_delay_usec(chat->aio, 50);

	acl_aio_ctl(chat->is->astream,
		ACL_AIO_CTL_READ_HOOK_ADD, read_ready_fn, host,
		ACL_AIO_CTL_WRITE_HOOK_ADD, write_ready_fn, host,
		ACL_AIO_CTL_CLOSE_HOOK_ADD, read_close_fn, host,
		ACL_AIO_CTL_TIMEO_HOOK_ADD, timeout_fn, host,
		ACL_AIO_CTL_TIMEOUT, 0,  /* ��ʼ��ʱ�����ö���ʱΪ 0 */
		ACL_AIO_CTL_END);

	chat->timer = icmp_timer_new();

	chat->check_inter = 2000000;	/* one second check timer */
	acl_aio_request_timer(chat->aio, check_timer, host,
		chat->check_inter, 0);

	/* ��ʼ�첽���ð�����Ӧ���� */
	acl_aio_read(chat->is->astream);
}

static void icmp_rset(ICMP_CHAT *chat)
{
	ACL_RING *ring_ptr;
	ICMP_HOST *host;

	while ((ring_ptr = acl_ring_pop_head(&chat->host_head)) != NULL) {
		host = RING_TO_HOST(ring_ptr);
		icmp_host_free(host);
	}

	chat->cnt = 0;
	acl_ring_init(&chat->host_head);
}

void icmp_chat_aio_free(ICMP_CHAT *chat)
{
	icmp_rset(chat);
	icmp_timer_free(chat->timer);
	icmp_stream_close(chat->is);
	acl_myfree(chat);
}
