#ifndef FIFO_INCLUDE_H
#define FIFO_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "iterator.h"

typedef struct FIFO_INFO FIFO_INFO;
typedef struct FIFO_ITER FIFO_ITER;
typedef struct FIFO FIFO;

struct FIFO_INFO {
	void *data;     
	FIFO_INFO *prev;    
	FIFO_INFO *next;
};

struct FIFO_ITER {
	FIFO_INFO *ptr;
};

struct FIFO {
	FIFO_INFO *head;
	FIFO_INFO *tail;
	int   cnt;

	/* ��Ӽ����� */

	/* �����β����Ӷ�̬���� */
	void  (*push_back)(struct FIFO*, void*);
	/* �����ͷ����Ӷ�̬���� */
	void  (*push_front)(struct FIFO*, void*);
	/* ��������β����̬���� */
	void *(*pop_back)(struct FIFO*);
	/* ��������ͷ����̬���� */
	void *(*pop_front)(struct FIFO*);

	/* for iterator */

	/* ȡ������ͷ���� */
	void *(*iter_head)(ITER*, struct FIFO*);
	/* ȡ��������һ������ */
	void *(*iter_next)(ITER*, struct FIFO*);
	/* ȡ������β���� */
	void *(*iter_tail)(ITER*, struct FIFO*);
	/* ȡ��������һ������ */
	void *(*iter_prev)(ITER*, struct FIFO*);
	/* ȡ�����������ĵ�ǰ������Ա�ṹ���� */
	FIFO_INFO *(*iter_info)(ITER*, struct FIFO*);
};

/**
 * ��ʼ��һ���������У�Ӧ�ÿ�����ջ�Ϸ�����У�������øú������г�ʼ��
 * @param fifo {FIFO *}
 * @example:
 *   void test(void) {
 	FIFO fifo;

	fifo_init(&fifo);
 *   }
 */
void fifo_init(FIFO *fifo);

/**
 * ���ڴ���з���һ�����ж���
 * @return {FIFO*}
 */
FIFO *fifo_new(void);

/**
 * ���ڴ���з���һ�����ж��󲢴��ڴ�ض�����Ϊ������
 * @return {FIFO*}
 */
FIFO *fifo_new(void);

/**
 * �Ӷ�����ɾ��������ֵ��ͬ�Ķ���
 * @param fifo {FIFO*}
 * @param data {const void*}
 */
int fifo_delete(FIFO *fifo, const void *data);
void fifo_delete_info(FIFO *fifo, FIFO_INFO *info);

/**
 * �ͷ��Զѷ���Ķ��ж���
 * @param fifo {FIFO*}
 * @param free_fn {void (*)(void*)}, ����ú���ָ�벻Ϊ����
 *  �����ͷŶ����ж�̬����Ķ���
 */
void fifo_free(FIFO *fifo, void (*free_fn)(void *));
void fifo_free2(FIFO *fifo, void (*free_fn)(FIFO_INFO *));

/**
 * ����������һ����̬�Ѷ���
 * @param fifo {FIFO*}
 * @param data {void*} ��̬����
 * @return {FIFO_INFO*} ��� data �ǿ��򷵻ض����е�����Ӷ���, ���򷵻� NULL
 */
FIFO_INFO *fifo_push_back(FIFO *fifo, void *data);
#define fifo_push	fifo_push_back
void fifo_push_info_back(FIFO *fifo, FIFO_INFO *info);
#define fifo_push_info	fifo_push_info_back
FIFO_INFO *fifo_push_front(FIFO *fifo, void *data);

/**
 * �Ӷ��������Ƚ��ȳ���ʽ����һ����̬����, ͬʱ���ö���Ӷ�����ɾ��
 * @param fifo {FIFO*}
 * @return {void*}, ���Ϊ�գ����ʾ����Ϊ��
 */
void *fifo_pop_front(FIFO *fifo);
#define fifo_pop	fifo_pop_front
FIFO_INFO *fifo_pop_info(FIFO *fifo);

/**
 * �Ӷ������Ժ���ȳ���ʽ����һ����̬���� ͬʱ�ö���Ӷ�����ɾ��
 * @param fifo {FIFO*}
 * @return {void*}, ���Ϊ�գ����ʾ����Ϊ��
 */
void *fifo_pop_back(FIFO *fifo);

/**
 * ���ض�����ͷ���Ķ�̬����
 * @param fifo {FIFO*}
 * @return {void*}, ���Ϊ�գ����ʾ����Ϊ��
 */
void *fifo_head(FIFO *fifo);
FIFO_INFO *fifo_head_info(FIFO *fifo);

/**
 * ���ض�����β���Ķ�̬����
 * @param fifo {FIFO*}
 * @return {void*}, ���Ϊ�գ����ʾ����Ϊ��
 */
void *fifo_tail(FIFO *fifo);
FIFO_INFO *fifo_tail_info(FIFO *fifo);

/**
 * ���ض����ж�̬������ܸ���
 * @param fifo {FIFO*}
 * @return {int}, >= 0
 */
int fifo_size(FIFO *fifo);

#ifdef __cplusplus
}
#endif

#endif
