#ifndef	QUEUE_INCLUDE_H
#define	QUEUE_INCLUDE_H

#ifdef __linux

#ifdef __cplusplus
extern "C" {
#endif

#define	QUEUE_ERR_UNKNOWN      -1
#define	QUEUE_OK               0
#define	QUEUE_ERR_LOCK         1
#define	QUEUE_ERR_UNLOCK       2
#define	QUEUE_ERR_TIMEOUT      3
#define	QUEUE_ERR_COND_WAIT    4
#define	QUEUE_ERR_COND_SIGNALE 5

typedef struct QUEUE_ITEM QUEUE_ITEM;
typedef struct QUEUE QUEUE;

typedef void (*QUEUE_FREE_FN)(void *);

/**
 * ����һ���¶��ж�����
 * @return QUEUE �ṹָ��
 */
QUEUE *queue_new(void);

/**
 * �����Ƿ��ϸ�����е������ߣ�Ĭ��Ϊ����Ҫ���иü����� queue_free
 * @param que QUEUE �ṹָ��
 * @param flag �����
 */
void queue_check_owner(QUEUE *que, char flag);

/**
 * ���ö��е�������, ֻ�������߲���Ȩ�ͷŶ���, ������ queue_free()
 * @param que QUEUE �ṹָ��
 * @param owner ���̺߳ű�ʶ�������ߵ�ID��
 */
void queue_set_owner(QUEUE *que, unsigned int owner);

/**
 * �ͷŶ��ж�����
 * @param que QUEUE �ṹָ��
 * @param free_fn ���ͷŶ���ʱ, ����ú�����Ϊ��, ���ڲ�ͨ���˺����������е�
 *        �û�ע������ݶ��н����ͷ�
 */
void queue_free(QUEUE *que, QUEUE_FREE_FN free_fn);

/**
 * �Ӷ�������ȡһ��Ԫ��, ������ʱ, һֱ�ȵ���Ԫ�ؿ��û����
 * @param que QUEUE �ṹָ��
 * @return �û�ͨ�� queue_push �����Ԫ��ָ��
 */
void *queue_pop(QUEUE *que);

/**
 * �Ӷ�������ȡһ��Ԫ��, ����ʱ, һֱ�ȵ���Ԫ�ؿ��û�ʱ�����
 * @param que QUEUE �ṹָ��
 * @param tmo_sec �Ӷ�������ȡԪ�صĳ�ʱʱ��, ��λΪ��
 * @param tmo_usec �Ӷ�������ȡԪ�صĳ�ʱʱ��, ��λΪ΢��
 * @return �û�ͨ�� queue_push �����Ԫ��ָ��
 */
 void *queue_pop_timedwait(QUEUE *que, int tmo_sec, int tmo_usec);

/**
 * ����������һ��Ԫ��
 * @param que QUEUE �ṹָ��
 * @param data �û�������ָ��
 * @return {int} ��Ӷ���Ԫ���Ƿ�ɹ�, 0: ok; < 0: error
 */
 int queue_push(QUEUE *que, void *data);

/**
 * �����һ�ζ��в����Ĵ����, define as: QUEUE_XXX
 * @param que QUEUE �ṹָ��
 * @return �����
 */
 int queue_last_error(const QUEUE *que);

/**
 * ���ö���Ϊ�˳�״̬
 * @param que QUEUE �ṹָ��
 */
 void queue_set_quit(QUEUE *que);

/**
 * ��õ�ǰ�����ж���Ԫ�صĸ���
 * @param que {QUEUE*}
 * @return {int} ������Ԫ�ظ�����< 0 ��ʾ����
 */
 int queue_qlen(QUEUE* que);

#ifdef __cplusplus
}
#endif

#endif // __linux__

#endif

