#ifndef ACL_MBOX_INCLUDE_H
#define ACL_MBOX_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "acl_define.h"

typedef struct ACL_MBOX ACL_MBOX;

/**
 * ����������Ϣ���ж���
 * @return {ACL_MBOX}
 */
ACL_API ACL_MBOX *acl_mbox_create(void);

#define	ACL_MBOX_T_SPSC		0	/* �������ߵ������� */
#define	ACL_MBOX_T_MPSC		1	/* �������ߵ������� */

ACL_API ACL_MBOX *acl_mbox_create2(unsigned type);

/**
 * �ͷ�������Ϣ���ж���
 * @param mbox {ACL_MBOX*} ��Ϣ���ж���
 * @param free_fn {void (*)(void*)} �ǿ�ʱ�����ͷŵ�ǰ��������Ϣ�����еĶ���
 */
ACL_API void acl_mbox_free(ACL_MBOX *mbox, void (*free_fn)(void*));

/**
 * ����Ϣ��������Ӷ�̬��Ϣ����
 * @param mbox {ACL_MBOX*} ��Ϣ���ж���
 * @param msg {void*}
 * @return {int} ���ͳɹ����� 0�����򷵻� -1
 */
ACL_API int acl_mbox_send(ACL_MBOX *mbox, void *msg);

/**
 * ����Ϣ�����ж�ȡ��Ϣ
 * @param mbox {ACL_MBOX*} ��Ϣ���ж���
 * @param timeout {int} �ȴ���ʱʱ��(���뼶��)����� < 0 ��һֱ�ȴ�ֱ��������
 * @param success {int*} �洢�����Ƿ�ɹ��Ľ���� 0 ��ʾ������ 0 ��ʾ�ɹ�
 * @return {void*} ���ض�������Ϣ����������� NULL ʱ�����ж� success ��ֵ���Դ���
 *  �ж϶������Ƿ�ɹ���������ط� NULL ��ʾ�ɹ�����һ����Ϣ
 */
ACL_API void *acl_mbox_read(ACL_MBOX *mbox, int timeout, int *success);

/**
 * ��õ�ǰ��Ϣ�����Ѿ��ɹ����͵���Ϣ��
 * @param mbox {ACL_MBOX*} ��Ϣ���ж���
 * @return {size_t}
 */
ACL_API size_t acl_mbox_nsend(ACL_MBOX *mbox);

/**
 * ��õ�ǰ��Ϣ�����Ѿ��ɹ����յ�����Ϣ��
 * @param mbox {ACL_MBOX*} ��Ϣ���ж���
 * @return {size_t}
 */
ACL_API size_t acl_mbox_nread(ACL_MBOX *mbox);

#ifdef __cplusplus
}
#endif

#endif
