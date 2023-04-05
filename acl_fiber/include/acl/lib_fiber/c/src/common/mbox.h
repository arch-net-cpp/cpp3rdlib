#ifndef _MBOX_INCLUDE_H
#define _MBOX_INCLUDE_H

typedef struct MBOX MBOX;

#define	MBOX_T_SPSC		0	/* �������ߵ������� */
#define	MBOX_T_MPSC		1	/* �������ߵ������� */

/**
 * ����������Ϣ���ж���
 * @param type {unsigned}
 * @return {MBOX}
 */
MBOX *mbox_create(unsigned type);

/**
 * �ͷ�������Ϣ���ж���
 * @param mbox {MBOX*} ��Ϣ���ж���
 * @param free_fn {void (*)(void*)} �ǿ�ʱ�����ͷŵ�ǰ��������Ϣ�����еĶ���
 */
void mbox_free(MBOX *mbox, void (*free_fn)(void*));

/**
 * ����Ϣ��������Ӷ�̬��Ϣ����
 * @param mbox {MBOX*} ��Ϣ���ж���
 * @param msg {void*}
 * @return {int} ���ͳɹ����� 0�����򷵻� -1
 */
int mbox_send(MBOX *mbox, void *msg);

/**
 * ����Ϣ�����ж�ȡ��Ϣ
 * @param mbox {MBOX*} ��Ϣ���ж���
 * @param timeout {int} �ȴ���ʱʱ��(���뼶��)����� < 0 ��һֱ�ȴ�ֱ��������
 * @param success {int*} �洢�����Ƿ�ɹ��Ľ���� 0 ��ʾ������ 0 ��ʾ�ɹ�
 * @return {void*} ���ض�������Ϣ����������� NULL ʱ�����ж� success ��ֵ��
 *  �Դ����ж϶������Ƿ�ɹ���������ط� NULL ��ʾ�ɹ�����һ����Ϣ
 */
void *mbox_read(MBOX *mbox, int timeout, int *success);

/**
 * ��õ�ǰ��Ϣ�����Ѿ��ɹ����͵���Ϣ��
 * @param mbox {MBOX*} ��Ϣ���ж���
 * @return {size_t}
 */
size_t mbox_nsend(MBOX *mbox);

/**
 * ��õ�ǰ��Ϣ�����Ѿ��ɹ����յ�����Ϣ��
 * @param mbox {MBOX*} ��Ϣ���ж���
 * @return {size_t}
 */
size_t mbox_nread(MBOX *mbox);

socket_t mbox_in(MBOX *mbox);
socket_t mbox_out(MBOX *mbox);

#endif
