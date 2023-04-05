#ifndef	__ARRAY_INCLUDE_H__
#define	__ARRAY_INCLUDE_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include "iterator.h"

/**
 * ��̬�������Ͷ���
 */
typedef	struct ARRAY ARRAY;
struct ARRAY{
	int     capacity;	/**< items ����ռ��С */
	int     count;		/**< items �к���Ԫ�صĸ��� */
	void  **items;		/**< ��̬���� */
	unsigned flags;
#define	ARRAY_F_UNORDER		(1 << 0)	/* �Ƿ����˳���� */

	/* ��Ӽ����� */

	/* ������β����Ӷ�̬���� */
	void  (*push_back)(struct ARRAY*, void*);
	/* ������ͷ����Ӷ�̬���� */
	void  (*push_front)(struct ARRAY*, void*);
	/* ��������β����̬���� */
	void *(*pop_back)(struct ARRAY*);
	/* ��������ͷ����̬���� */
	void *(*pop_front)(struct ARRAY*);

	/* for iterator */

	/* ȡ������ͷ���� */
	void *(*iter_head)(ITER*, struct ARRAY*);
	/* ȡ��������һ������ */
	void *(*iter_next)(ITER*, struct ARRAY*);
	/* ȡ������β���� */
	void *(*iter_tail)(ITER*, struct ARRAY*);
	/* ȡ��������һ������ */
	void *(*iter_prev)(ITER*, struct ARRAY*);
};

/**
 * ����һ����̬����
 * @param init_size {int} ��̬����ĳ�ʼ��С
 * @param flags {unsigned} ��������̬��������Ա�־
 * @return {ARRAY*} ��̬����ָ��
 */
ARRAY *array_create(int init_size, unsigned flags);

/**
 * �ͷŵ���̬�����ڵĳ�Ա�������������ͷŶ�̬�������
 * @param a {ARRAY*} ��̬����ָ��
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ��
 */
void array_clean(ARRAY *a, void (*free_fn)(void *));

/**
 * �ͷŵ���̬�����ڵĳ�Ա���������ͷŶ�̬������󣬵�������󴴽� dbuf ����
 * ʱ��������������ͷŽ������ͷ� dbuf ʱ���ͷ�
 * @param a {ARRAY*} ��̬����ָ��
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ��
 */
void array_free(ARRAY *a, void (*free_fn)(void *));
#define array_destroy array_free

/**
 * ��̬����β����Ӷ�̬��Ա����
 * @param a {ARRAY*} ��̬����ָ��
 * @param obj {void*} ��̬��Ա����
 * @return {int} >=0: �ɹ�, ����ֵΪ��Ԫ���������е��±�λ�ã�-1: ʧ��
 */
int array_append(ARRAY *a, void *obj);

/**
 * ��̬����ͷ����Ӷ�̬��Ա����
 * @param a {ARRAY*} ��̬����ָ��
 * @param obj {void*} ��̬��Ա����
 * @return {int} >=0: �ɹ�, ����ֵΪ��Ԫ���������е��±�λ�ã�-1: ʧ��
 */
int array_prepend(ARRAY *a, void *obj);

/**
 * ��̬������ָ��λ��ǰ��Ӷ�̬��Ա����(�ý�㼰�Ժ����н�㶼����һ��λ��)
 * @param a {ARRAY*} ��̬����ָ��
 * @param position {int} ĳ��λ�ã�����Խ��
 * @param obj {void*} ��̬��Ա����
 * @return {int} 0: �ɹ���-1: ʧ��
 */
int array_pred_insert(ARRAY *a, int position, void *obj);

/**
 * ��̬������ָ��λ�ú���Ӷ�̬��Ա����(�ý���Ժ����н�㶼����һ��λ��)
 * @param a {ARRAY*} ��̬����ָ��
 * @param position {int} ĳ��λ�ã�����Խ��
 * @param obj {void*} ��̬��Ա����
 * @return {int} 0: �ɹ���-1: ʧ��
 */
int array_succ_insert(ARRAY *a, int position, void *obj);
#define array_insert array_succ_insert

/**
 * �Ӷ�̬�����е�ָ��λ��ɾ��ĳ������ɾ����������Ԫ�ص��Ⱥ�˳���п��ܷ����˸ı�,
 * ��Ϊɾ������Զ�������������Ԫ��������λ�ô�
 * @param a {ARRAY*} ��̬����ָ��
 * @param pos {int} ĳ��λ�ã�����Խ��
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ�룬�����
 *  ָ��Ϊ�գ����ͷţ������ô˺��������ͷŶ�̬����
 * @return {int} 0: �ɹ���-1: ʧ��
 */
int array_delete(ARRAY *a, int pos, void (*free_fn)(void*));

/**
 * �Ӷ�̬������ɾ��ָ��ָ���ַ�Ķ�̬����, ɾ����������Ԫ�ص��Ⱥ�˳�򱣳ֲ���
 * �����ɾ��λ�����м�ĳ��λ�ã�Ϊ�˱�֤Ԫ�ص�˳�����ڲ�������ɾ��Ԫ�غ������Ԫ��
 * ��ǰ��һ��λ��
 * @param a {ARRAY*} ��̬����ָ��
 * @param obj {void*} ��̬����ָ���ַ
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ�룬�����
 *  ָ��Ϊ�գ����ͷţ������ô˺��������ͷŶ�̬����
 * @return {int} 0: �ɹ���-1: ʧ��
 */
int array_delete_obj(ARRAY *a, void *obj, void (*free_fn)(void *));

/**
 * �Ӷ�̬������ɾ��ĳ���±귶Χ�Ķ�̬����
 * @param a {ARRAY*} ��̬����ָ��
 * @param ibegin {int} ��ʼ�±�λ��
 * @param iend {int} �����±�λ��
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ�룬�����
 *  ָ��Ϊ�գ����ͷţ������ô˺��������ͷŶ�̬����
 * @return {int} 0: �ɹ���-1: ʧ��
 */
int array_delete_range(ARRAY *a, int ibegin, int iend, void (*free_fn)(void*));

/**
 * �ƶ���̬�����еĶ���
 * @param a {ARRAY*} ��̬����ָ��
 * @param ito {int} �ƶ���Ŀ���±�λ��
 * @param ifrom {int} �Ӵ��±�λ�ÿ�ʼ�ƶ�
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ�룬�����
 *  ָ��Ϊ�գ����ͷţ������ô˺��������ͷŶ�̬�����ͷŵĶ�̬��������Ϊ
 *  [idx_obj_begin, idx_src_begin), Ϊһ�뿪�������
 * @return {int} 0: �ɹ���-1: ʧ��
 */
int array_mv_idx(ARRAY *a, int ito, int ifrom, void (*free_fn)(void *) );

/**
 * Ԥ�ȱ�֤��̬����Ŀռ䳤��
 * @param a {ARRAY*} ��̬����ָ��
 * @param app_count {int} ��Ҫ��̬���������� app_count ������λ��
 */
void array_pre_append(ARRAY *a, int app_count);

/**
 * �Ӷ�̬�����е�ĳ���±�λ��ȡ����̬����
 * @param a {ARRAY*} ��̬����ָ��
 * @param idx {int} �±�λ�ã�����Խ�磬���򷵻�-1
 * @return {void*} != NULL: �ɹ���== NULL: �����ڻ�ʧ��
 */
void *array_index(const ARRAY *a, int idx);

/**
 * ��õ�ǰ��̬�����ж�̬����ĸ���
 * @param a {ARRAY*} ��̬����ָ��
 * @return {int} ��̬�����ж�̬����ĸ���
 */
int array_size(const ARRAY *a);

void *array_head(ARRAY *a);
void *array_tail(ARRAY *a);
void array_push_back(struct ARRAY *a, void *obj);
void array_push_front(struct ARRAY *a, void *obj);
void *array_pop_back(struct ARRAY *a);
void *array_pop_front(struct ARRAY *a);

#ifdef  __cplusplus
}
#endif

#endif

