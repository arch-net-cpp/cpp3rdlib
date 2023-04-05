#ifndef __ARGV_INCLUDE_H__
#define __ARGV_INCLUDE_H__

# ifdef	__cplusplus
extern "C" {
# endif
#include <stdarg.h>
#include "iterator.h"

/**
 * External interface.
 */
typedef struct ARGV {
	int     len;			/**< number of array elements */
	int     argc;			/**< array elements in use */
	char  **argv;			/**< string array */

	/* ��Ӽ����� */

	/* ������β������ַ��� (�ڲ���̬�������ַ���) */
	void  (*push_back)(struct ARGV*, const char*);
	/* ������ͷ����Ӷ�̬���� (�ڲ���̬�������ַ���)*/
	void  (*push_front)(struct ARGV*, const char*);
	/* ��������β���ַ��� (���������� myfree �ͷ�) */
	char *(*pop_back)(struct ARGV*);
	/* ��������ͷ���ַ��� (���������� myfree �ͷ�) */
	char *(*pop_front)(struct ARGV*);

	/* for iterator */

	/* ȡ������ͷ���� */
	void *(*iter_head)(ITER*, struct ARGV*);
	/* ȡ��������һ������ */
	void *(*iter_next)(ITER*, struct ARGV*);
	/* ȡ������β���� */
	void *(*iter_tail)(ITER*, struct ARGV*);
	/* ȡ��������һ������ */
	void *(*iter_prev)(ITER*, struct ARGV*);
} ARGV;

/* in argv.c */
/**
 * ����һ���ַ�����̬����
 * @param size {int} ��̬����ĳ�ʼ��С
 * @return {ARGV*}
 */
ARGV *argv_alloc(int size);

/**
 * ���ַ�����̬���������һ������ַ��������һ��NULL�ַ�����ʾ����
 * @param argvp {ARGV*} �ַ�����̬����ָ��
 * @param ... �ַ����б����һ��ΪNULL, ��ʽ�磺{s1}, {s2}, ..., NULL
 */
void argv_add(ARGV *argvp,...);

/**
 * ���ַ�����̬����������ַ����б�
 * @param argvp {ARGV*} �ַ�����̬����ָ��
 * @param ap {va_list} �ɶ���ַ�����ɵı���б�
 */
void argv_addv(ARGV *argvp, va_list ap);

/**
 * �ͷ��ַ�����̬����
 * @param argvp {ARGV*} �ַ�����̬����ָ��
 */
ARGV *argv_free(ARGV *argvp);

/**
 * �����ַ�����̬����Ľ���λ��
 * @param argvp {ACL_ARGV*} �ַ�����̬����ָ��
 */
void argv_terminate(ARGV *argvp);

/**
 * ���ص�ǰ�ַ�����̬�������Ѿ���ŵ��ַ�������
 * @param argvp {ARGV*} �ַ�����̬����ָ��
 * @return {int}
 */
int argv_size(ARGV *argvp);

/**
 * ����Դ�ַ������ָ��ַ�������һ���ַ�����̬����
 * @param str {const char*} Դ�ַ���
 * @param delim {const char*} �ָ��ַ���
 * @return {ARGV*}
 */
ARGV *argv_split(const char *str, const char *delim);

#define ARGV_END	((char *) 0)

# ifdef	__cplusplus
}
# endif

#endif

