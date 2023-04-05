#ifndef HTABLE_INCLUDE_H
#define HTABLE_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "iterator.h"

/*--------------------------------------------------------------------------*/
typedef struct HTABLE		HTABLE;
typedef struct HTABLE_INFO 	HTABLE_INFO;

/**
 * ��ϣ�����ṹ���
 */
struct HTABLE {
	int     size;                   /* length of entries array */
	int     init_size;              /* length of initial entryies array */
	int     used;                   /* number of entries in table */
	HTABLE_INFO **data;             /* entries array, auto-resized */
	int     status;                 /* the operator's status on the htable */

	/* for iterator */

	/* ȡ������ͷ���� */
	void *(*iter_head)(ITER*, struct HTABLE*);
	/* ȡ��������һ������ */
	void *(*iter_next)(ITER*, struct HTABLE*);
	/* ȡ������β���� */
	void *(*iter_tail)(ITER*, struct HTABLE*);
	/* ȡ��������һ������ */
	void *(*iter_prev)(ITER*, struct HTABLE*);
	/* ȡ�����������ĵ�ǰ������Ա�ṹ���� */
	HTABLE_INFO *(*iter_info)(ITER*, struct HTABLE*);
};

/**
 * ��ϣ����ÿһ����ϣ��Ĵ洢��Ϣ����
 */
struct HTABLE_INFO {
	char *key;
	void *value;			/**< associated value */
	unsigned hash;			/**< store the key's hash value */
	struct HTABLE_INFO *next;	/**< colliding entry */
	struct HTABLE_INFO *prev;	/**< colliding entry */
};

/**
 * ������ϣ��
 * @param size ��ϣ����
 * @return ������ϣ���ͷָ���Ϊ��(��ʱ��ʾ�������صĴ���, ��Ҫ���ڴ��������)
 */
HTABLE *htable_create(int size);

/**
 * �����һ�ι�ϣ��������ϣ���״̬
 * @param table ��ϣ��ָ��
 * @return {int} ������ϣ����״̬, �μ����µ� HTABLE_STAT_XXX
 */
int htable_errno(HTABLE *table);
#define	HTABLE_STAT_OK          0  /**< ״̬���� */
#define	HTABLE_STAT_INVAL       1  /**< ��Ч���� */
#define	HTABLE_STAT_DUPLEX_KEY  2  /**< �ظ��� */

/**
 * ���ù�ϣ��ĵ�ǰ״̬, error ȡֵ HTABLE_STAT_XXX
 * @param table ��ϣ��ָ��
 * @param error ���ù�ϣ��Ĵ���״̬
 */
void htable_set_errno(HTABLE *table, int error);

/**
 * ����ϣ��������µ���
 * @param table ��ϣ��ָ��
 * @param key ��, �ں����ڲ��Ḵ�ƴ� key ��
 * @param value �û��Լ����ض�������(����������Ӳת������, ���Ǵ����������
 *  ���ܶ�ջ����)
 * @return ������Ĺ�ϣ�����ָ��, == NULL: ��ʾ�ڲ������ڴ����, Ϊ���صĴ���
 *  ע����������ʱ�ù�ϣ�������ڣ��򷵻��Ѿ����ڵĹ�ϣ�ʹ����Ӧ��ͨ������
 *  htable_last_errno() ���鿴�Ƿ��ظ����ͬһ����ֵ(HTABLE_STAT_DUPLEX_KEY)
 */
HTABLE_INFO *htable_enter(HTABLE *table, const char *key, void *value);

/**
 * �������� key ����Ѱĳһ�ض���ϣ��
 * @param table ��ϣ��ָ��
 * @param key ��
 * @return ��Ϊ��ָ��: ��ʾ�鵽�˶�Ӧ�� key ���Ĺ�ϣ��
 *         Ϊ��: ��ʾδ�鵽��Ӧ�� key ���Ĺ�ϣ��
 */
HTABLE_INFO *htable_locate(HTABLE *table, const char *key);

/**
 * �������� key ����Ѱ�û���������
 * @param table ��ϣ��ָ��
 * @param key ��
 * @return ��Ϊ��: ��ʾ�鵽�˶�Ӧ�� key ����������, �û����Ը����û��Լ���
 *  �������ͽ���ת��; Ϊ��: ��ʾδ�鵽��Ӧ�� key ����������
 */
void *htable_find(HTABLE *table, const char *key);

/**
 * ���������� key ��ɾ��ĳһ��ϣ��
 * @param table ��ϣ��ָ��
 * @param key ��
 * @param free_fn ����ú���ָ�벻Ϊ�ղ����ҵ��˶�Ӧ�� key ����������, ����
 *  �����û����ṩ������������һЩ��β����, Ȼ�����ͷŸù�ϣ��
 * @return 0: �ɹ�;  -1: δ�ҵ��� key ��
 */
int htable_delete(HTABLE *table, const char *key, void (*free_fn) (void *));

/**
 * ֱ�Ӹ��� htable_locate ���صķǿն���ӹ�ϣ����ɾ���ö���
 * @param table ��ϣ��ָ��
 * @param ht {HTABLE_INFO*} �洢�ڹ�ϣ���е��ڲ��ṹ����
 * @param free_fn ����ú���ָ�벻Ϊ�ղ����ҵ��˶�Ӧ�� key ����������, ����
 *  �����û����ṩ������������һЩ��β����, Ȼ�����ͷŸù�ϣ��
 */
void htable_delete_entry(HTABLE *table, HTABLE_INFO *ht, void (*free_fn) (void *));

/**
 * �ͷ�������ϣ��
 * @param table ��ϣ��ָ��
 * @param free_fn �����ָ�벻Ϊ����Թ�ϣ���е�ÿһ���ϣ�����øú�����
 *  ��β����, Ȼ�����ͷ�
 */
void htable_free(HTABLE *table, void (*free_fn) (void *));

/**
 * ���ù�ϣ��, �ú������ͷŹ�ϣ���е�����������, �����³�ʼ��
 * @param table ��ϣ��ָ��
 * @param free_fn �����ָ�벻Ϊ����Թ�ϣ���е�ÿһ���ϣ�����øú�����
 *  ��β����, Ȼ�����ͷ�
 * @return �Ƿ����óɹ�. 0: OK; < 0: error.
 */
int htable_reset(HTABLE *table, void (*free_fn) (void *));

/**
 * �Թ�ϣ���е�ÿһ���ϣ����д���
 * @param table ��ϣ��ָ��
 * @param walk_fn ����ÿһ���ϣ��ĺ���ָ��, ����Ϊ��
 * @param arg �û��Լ����͵�����
 */
void htable_walk(HTABLE *table, void (*walk_fn) (HTABLE_INFO *, void *), void *arg);

/**
 * ���ع�ϣ��ǰ�������ռ��С
 * @param table ��ϣ��ָ��
 * @return ��ϣ��������ռ��С
 */
int htable_size(const HTABLE *table);

/**
 * ���ع�ϣ��ǰ�Ĵ���������Ԫ�ظ���
 * @param table ��ϣ��ָ��
 * @return ��ϣ����Ԫ�ظ���
 */
int htable_used(const HTABLE *table);

/**
 * ����ϣ�������������ϳ�һ������
 * @param table ��ϣ��
 * @return ��Ϊ��: ����ָ��; Ϊ��: ��ʾ�ù�ϣ����û�й�ϣ��
 */
HTABLE_INFO **htable_list(const HTABLE *table);

/**
 * ��ʾ��ϣ���� key ���ķֲ�״̬
 * @param table ��ϣ��ָ��
 */
void htable_stat(const HTABLE *table);
#define	htable_stat_r	htable_stat

/*--------------------  һЩ�����ݵĺ���� --------------------------------*/

#define	HTABLE_ITER_KEY(iter)	((iter).ptr->key.c_key)
#define	htable_iter_key		HTABLE_ITER_KEY

#define	HTABLE_ITER_VALUE(iter)	((iter).ptr->value)
#define	htable_iter_value	HTABLE_ITER_VALUE

#ifdef  __cplusplus
}
#endif

#endif
