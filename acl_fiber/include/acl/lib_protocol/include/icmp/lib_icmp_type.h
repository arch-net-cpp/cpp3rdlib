#ifndef __LIB_ICMP_TYPE_INCLUDE_H__
#define __LIB_ICMP_TYPE_INCLUDE_H__

#define ICMP_TYPE_ECHOREPLY	0
#define ICMP_TYPE_ECHO		8

#define ICMP_CODE_EXTRA		16

typedef struct ICMP_STREAM ICMP_STREAM;
typedef struct ICMP_CHAT ICMP_CHAT;
typedef struct ICMP_STAT ICMP_STAT;
typedef struct ICMP_HOST ICMP_HOST;
typedef struct ICMP_PKT ICMP_PKT;
typedef struct ICMP_PKT_STATUS ICMP_PKT_STATUS;

/**< ICMP ͨ�Ź�����ÿ�������� PING ��Ӧ״̬��Ϣ���� */
struct ICMP_STAT {
	double tmin;			/**< ���ʱ�� */
	double tmax;			/**< �ʱ�� */
	double tsum;			/**< ��ʱ�� */
	double tave;			/**< ƽ��ʱ�� */
	size_t nsent;			/**< �Ѿ����͵İ����� */
	size_t nreceived;		/**< �Ѿ��յ��İ����� */
	double loss;			/**< ��ʧ�İ����� */
};

#define ICMP_MIN_PACKET		32
#define ICMP_MAX_PACKET		1024

/**< ICMP �����͵�ÿ�� PING ��֮�������״̬Ӧ�� */
struct ICMP_PKT_STATUS {
	size_t reply_len;		/**< ���ظ������ݳ��� */
	char   from_ip[64];		/**< Դ��ַ */

	double         rtt;		/**< ����ʱ��(����)(Round Trip Time) */
	unsigned short seq;		/**< ���к�(seq no) */
	unsigned char  ttl;		/**< ����ʱ��(time to live) */
	unsigned int   gid;
	char          *data;
	size_t         dlen;
	char           status;
#define ICMP_STATUS_INIT		0
#define ICMP_STATUS_OK			1
#define ICMP_STATUS_UNREACH		(1<<1) 
#define ICMP_STATUS_TIMEOUT		(1<<2)

	ICMP_PKT      *pkt;
};

/**< Ŀ��������Ϣ�ṹ */
struct ICMP_HOST {
	ICMP_STAT icmp_stat;		/**< ICMP ͨ�Ź����е�״̬ */
	char dest_ip[32];		/**< Ŀ������IP��ַ */
	char domain[64];		/**< Ŀ������IP����Ӧ������ */
	struct sockaddr_in dest;	/**< ���Ͱ�ʱĿ��������ַ */
	struct sockaddr_in from;	/**< ����ʱԴ������ַ */
	int   from_len;			/**< ����ʱ�洢�� from �еĵ�ַ���� */
	int   delay;			/**< ���� PING �ļ������λΪ���� */
	int   timeout;			/**< ��ʱʱ��(����) */
	size_t dlen;			/**< ÿ�����Ͱ��Ĵ�С(�ֽ�) */
	size_t nsent;			/**< �Ѿ����͸���Ŀ���������ĸ��� */

	ICMP_PKT **pkts;		/**< ���а������� */
	size_t npkt;			/**< ���õ����Ŀ���������Ͱ��ĸ��� */
	size_t ipkt;			/**< ��¼��һ��Ҫ���͵İ����±� */

	ACL_RING   host_ring;		/**< ���� ICMP_CHAT->host_head ���� */
	ICMP_CHAT *chat;		/**< ������ͨ�Ŷ��� */
	char  enable_log;		/**< �Ƿ���Ӧ������Ϣ����־ */

	/**< �㱨���Ͱ�����Ӧ��״̬ */
	void (*stat_respond)(ICMP_PKT_STATUS*, void*);

	/**< �÷��Ͱ�����Ӧ����ʱ */
	void (*stat_timeout)(ICMP_PKT_STATUS*, void*);

	/**< ���������ɴ� */
	void (*stat_unreach)(ICMP_PKT_STATUS*, void*);

	/**< �������İ�����ʱ�Ļص����� */
	void (*stat_finish)(ICMP_HOST*, void*);

	/**< Ӧ�ô��ݵ�˽�в�����ַ */
	void *arg;
};

#endif
