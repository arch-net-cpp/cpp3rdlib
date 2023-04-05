#ifndef	RING_INCLUDE_H
#define	RING_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef struct RING RING;

/**
 * ���ݻ��ṹ���Ͷ���
 */
struct RING {
	RING   *succ;           /**< successor */
	RING   *pred;           /**< predecessor */

	RING   *parent;         /**< the header of all the rings */
	int     len;            /**< the count in the ring */
};

typedef struct RING_ITER {
	RING *ptr;
} RING_ITER;

/**
 * ��ʼ�����ݻ�
 * @param ring {RING*} ���ݻ�
 */
#ifdef USE_FAST_RING
#define ring_init(__ring) do { \
	RING *_ring   = __ring; \
	_ring->pred   = _ring->succ = _ring; \
	_ring->parent = _ring; \
	_ring->len    = 0; \
} while (0)
#else
void ring_init(RING *ring);
#endif

/**
 * ��õ�ǰ���ݻ���Ԫ�ظ���
 * @param ring {RING*} ���ݻ�
 * @return {int} ���ݻ���Ԫ�ظ���
 */
#ifdef USE_FAST_RING
#define ring_size(r) (((RING*)(r))->len)
#else
int  ring_size(const RING *ring);
#endif

/**
 * ��һ����Ԫ����ӽ�����ͷ�����ڻ��к���ӵ�Ԫ��λ������ӵ�Ԫ��ǰ��
 * @param ring {RING*} ���ݻ�
 * @param entry {RING*} �µ�Ԫ��
 */
#ifdef USE_FAST_RING
#define ring_append(r, e) do { \
	((RING*)(e))->succ       = ((RING*)(r))->succ; \
	((RING*)(e))->pred       = (RING*)(r); \
	((RING*)(e))->parent     = ((RING*)(r))->parent; \
	((RING*)(r))->succ->pred = (RING*)(e); \
	((RING*)(r))->succ       = (RING*)(e); \
	((RING*)(r))->parent->len++; \
} while (0)

#else
void ring_append(RING *ring, RING *entry);
#endif

/**
 * ��һ����Ԫ����ӽ�����β�����ڻ��к���ӵ�Ԫ��λ������ӵ�Ԫ�غ���
 * @param ring {RING*} ���ݻ�
 * @param entry {RING*} �µ�Ԫ��
 */
#ifdef USE_FAST_RING
#define ring_prepend(r, e) do { \
	((RING*)(e))->pred       = ((RING*)(r))->pred; \
	((RING*)(e))->succ       = (RING*)(r); \
	((RING*)(e))->parent     = ((RING*)(r))->parent; \
	((RING*)(r))->pred->succ = (RING*)(e); \
	((RING*)(r))->pred       = (RING*)(e); \
	((RING*)(r))->parent->len++; \
} while (0)
#else
void ring_prepend(RING *ring, RING *entry);
#endif

/**
 * ��һ����Ԫ�ش����ݻ���ɾ��
 * @param entry {RING*} ��Ԫ��
 */
#ifdef USE_FAST_RING
#define ring_detach(e) do { \
	RING *_succ, *_pred; \
	if (((RING*)(e))->parent != (RING*)(e)) { \
		_succ = ((RING*)(e))->succ; \
		_pred = ((RING*)(e))->pred; \
		if (_succ && _pred) { \
			_pred->succ = _succ; \
			_succ->pred = _pred; \
			((RING*)(e))->parent->len--; \
			((RING*)(e))->succ   = (RING*)(e); \
			((RING*)(e))->pred   = (RING*)(e); \
			((RING*)(e))->parent = (RING*)(e); \
			((RING*)(e))->len    = 0; \
		} \
	} \
} while (0)
#else
void ring_detach(RING *entry);
#endif

/**
 * �ӻ��е���ͷ����Ԫ��
 * @param ring {RING*} ���ݻ�
 * @return {RING*} ͷ����Ԫ�أ�������ؿ����ʾ�����ݻ�Ϊ��
 */
#ifdef USE_FAST_RING
static inline RING *ring_pop_head(RING *ring)
{
	RING *succ;

	succ = ring->succ;
	if (succ == ring) {
		return NULL;
	}

	ring_detach(succ);
	return succ;
}
#else
RING *ring_pop_head(RING *ring);
#endif

/**
 * �ӻ��е���β����Ԫ��
 * @param ring {RING*} ���ݻ�
 * @return {RING*} β����Ԫ�أ�������ؿ����ʾ�����ݻ�Ϊ��
 */
#ifdef USE_FAST_RING
static inline RING *ring_pop_tail(RING *ring)
{
	RING *pred;

	pred = ring->pred;
	if (pred == ring) {
		return NULL;
	}

	ring_detach(pred);
	return pred;
}
#else
RING *ring_pop_tail(RING *ring);
#endif

/*--------------------  һЩ�����ݵĺ���� --------------------------------*/

/**
 * ���ص�ǰ��Ԫ�ص���һ����Ԫ��
 */
#define RING_SUCC(c) ((c)->succ)
#define	ring_succ	RING_SUCC

/**
 * ���ص�ǰ��Ԫ�ص�ǰһ����Ԫ��
 */
#define RING_PRED(c) ((c)->pred)
#define	ring_pred	RING_PRED

/**
 * ����Ԫ��ָ��ת����Ӧ�õ��Զ������͵�ָ���ַ
 * @param ring_ptr {RING*} ��Ԫ��ָ��
 * @param app_type Ӧ���Զ�������
 * @param ring_member {RING*} ��Ԫ����Ӧ���Զ���ṹ�еĳ�Ա����
 * @return {app_type*} Ӧ���Զ���ṹ���͵Ķ����ַ
 */
#define RING_TO_APPL(ring_ptr, app_type, ring_member) \
    ((app_type *) (((char *) (ring_ptr)) - offsetof(app_type,ring_member)))

#define	ring_to_appl	RING_TO_APPL

/**
 * ��ͷ����β���������ݻ��е����л�Ԫ��
 * @param iter {RING_ITER}
 * @param head_ptr {RING*} ���ݻ���ͷָ��
 * @example:
 	typedef struct {
		char  name[32];
		RING entry;
	} DUMMY;

	void test()
	{
		RING head;
		DUMMY *dummy;
		RING_ITER iter;
		int   i;

		ring_init(&head);

		for (i = 0; i < 10; i++) {
			dummy = (DUMMY*) mycalloc(1, sizeof(DUMMY));
			snprintf(dummy->name, sizeof(dummy->name), "dummy:%d", i);
			ring_append(&head, &dummy->entry);
		}

		ring_foreach(iter, &head) {
			dummy = ring_to_appl(iter.ptr, DUMMY, entry);
			printf("name: %s\n", dummy->name);
		}

		while (1) {
			iter.ptr = ring_pop_head(&head);
			if (iter.ptr == NULL)
				break;
			dummy = ring_to_appl(iter.ptr, DUMMY, entry);
			myfree(dummy);
		}
	}
 */
#define	RING_FOREACH(iter, head_ptr) \
        for ((iter).ptr = ring_succ((head_ptr)); (iter).ptr != (head_ptr);  \
             (iter).ptr = ring_succ((iter).ptr))

#define	ring_foreach		RING_FOREACH

/**
 * ��β����ͷ���������ݻ��е����л�Ԫ��
 * @param iter {RING_ITER}
 * @param head_ptr {RING*} ���ݻ���ͷָ��
 */
#define	RING_FOREACH_REVERSE(iter, head_ptr) \
        for ((iter).ptr = ring_pred((head_ptr)); (iter).ptr != (head_ptr);  \
             (iter).ptr = ring_pred((iter).ptr))

#define	ring_foreach_reverse	RING_FOREACH_REVERSE

/**
 * �������ݻ��е�һ����Ԫ��ָ��
 * @param head {RING*} ��ͷָ��
 * @return {RING*} NULL: ��Ϊ��
 */
#define RING_FIRST(head) \
	(ring_succ(head) != (head) ? ring_succ(head) : 0)

#define	ring_first		RING_FIRST

/**
 * �������ݻ���ͷ��һ����Ԫ��ָ��ͬʱ����ת��Ӧ���Զ���ṹ���͵Ķ����ַ
 * @param head {RING*} ��ͷָ��
 * @param app_type Ӧ���Զ���ṹ����
 * @param ring_member {RING*} ��Ԫ����Ӧ���Զ���ṹ�еĳ�Ա����
 * @return {app_type*} Ӧ���Զ���ṹ���͵Ķ����ַ
 */
#define RING_FIRST_APPL(head, app_type, ring_member) \
	(ring_succ(head) != (head) ? \
	 RING_TO_APPL(ring_succ(head), app_type, ring_member) : 0)

#define	ring_first_appl	RING_FIRST_APPL

/**
 * �������ݻ������һ����Ԫ��ָ��
 * @param head {RING*} ��ͷָ��
 * @return {RING*} NULL: ��Ϊ��
 */
#define RING_LAST(head) \
       (ring_pred(head) != (head) ? ring_pred(head) : 0)

#define	ring_last		RING_LAST

/**
 * �������ݻ������һ����Ԫ��ָ��ͬʱ����ת��Ӧ���Զ���ṹ���͵Ķ����ַ
 * @param head {RING*} ��ͷָ��
 * @param app_type Ӧ���Զ���ṹ����
 * @param ring_member {RING*} ��Ԫ����Ӧ���Զ���ṹ�еĳ�Ա����
 * @return {app_type*} Ӧ���Զ���ṹ���͵Ķ����ַ
 */
#define RING_LAST_APPL(head, app_type, ring_member) \
       (ring_pred(head) != (head) ? \
	RING_TO_APPL(ring_pred(head), app_type, ring_member) : 0)

#define	ring_last_appl	RING_LAST_APPL

/**
 * ��һ����Ԫ����ӽ�����ͷ�����ڻ��к���ӵ�Ԫ��λ������ӵ�Ԫ��ǰ��
 * @param ring {RING*} ���ݻ�
 * @param entry {RING*} �µ�Ԫ��
 */
#define	RING_APPEND(ring_in, entry_in) do {  \
	RING *ring_ptr = (ring_in), *entry_ptr = (entry_in);  \
        entry_ptr->succ      = ring_ptr->succ;  \
        entry_ptr->pred      = ring_ptr;  \
        entry_ptr->parent    = ring_ptr->parent;  \
        ring_ptr->succ->pred = entry_ptr;  \
        ring_ptr->succ       = entry_ptr;  \
        ring_ptr->parent->len++;  \
} while (0)

/**
 * ��һ����Ԫ����ӽ�����β�����ڻ��к���ӵ�Ԫ��λ������ӵ�Ԫ�غ���
 * @param ring {RING*} ���ݻ�
 * @param entry {RING*} �µ�Ԫ��
 */
#define	RING_PREPEND(ring_in, entry_in) do {  \
	RING *ring_ptr = (ring_in), *entry_ptr = (entry_in);  \
	entry_ptr->pred      = ring_ptr->pred;  \
	entry_ptr->succ      = ring_ptr;  \
	entry_ptr->parent    = ring_ptr->parent;  \
	ring_ptr->pred->succ = entry_ptr;  \
	ring_ptr->pred       = entry_ptr;  \
	ring_ptr->parent->len++;  \
} while (0)

/**
 * ��һ����Ԫ�ش����ݻ���ɾ��
 * @param entry {RING*} ��Ԫ��
 */
#define	RING_DETACH(entry_in) do {  \
	RING   *succ, *pred, *entry_ptr = (entry_in);  \
	succ = entry_ptr->succ;  \
	pred = entry_ptr->pred;  \
	if (succ != NULL && pred != NULL) {  \
		pred->succ = succ;  \
		succ->pred = pred;  \
		entry_ptr->parent->len--;  \
		entry_ptr->succ = entry_ptr->pred = NULL;  \
	}  \
} while (0)

#ifdef  __cplusplus
}
#endif

#endif

