#ifndef ATOMIC_INCLUDE_H
#define ATOMIC_INCLUDE_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct ATOMIC ATOMIC;

/**
 * ����ԭ�Ӷ���
 * @return {ATOMIC*} �����´����Ķ���
 */
ATOMIC *atomic_new(void);

/**
 * �ͷ�ԭ�Ӷ���
 * @param self {ATOMIC*} ԭ�Ӷ���
 */
void atomic_free(ATOMIC *self);

/**
 * ��ָ��������ԭ�Ӷ���󶨣��Ա��ڶԸö������ԭ�Ӳ���
 * @param self {ATOMIC*} ԭ�Ӷ���
 * @param value {void*} �������Ķ���ͨ��ԭ�Ӷ���ʵ�ֶԸö����ԭ�Ӳ���
 */
void atomic_set(ATOMIC *self, void *value);

/**
 * �Ƚϲ��������󣬵�ԭ�Ӷ���󶨵Ķ���������Ƚ϶�����ͬʱ�������¶�����
 * ����֮ǰ�󶨵Ķ���
 * @param self {ATOMIC*} ԭ�Ӷ���
 * @param cmp {void*} ���Ƚ϶���ָ��
 * @param value {void*} ��ԭ�Ӷ�������Ƚ϶�����ͬʱ�Ὣ�ö�����ԭ�Ӷ����
 * @return {void*} ����ԭ�Ӷ���֮ǰ�󶨵Ķ���
 */
void *atomic_cas(ATOMIC *self, void *cmp, void *value);

/**
 * ��ԭ�Ӷ������¶�����а󶨣�������֮ǰ�󶨵Ķ���
 * @param self {ATOMIC*} ԭ�Ӷ���
 * @param value {void*} �����󶨵��¶���
 * @return {void*} ����֮ǰ�󶨵Ķ���
 */
void *atomic_xchg(ATOMIC *self, void *value);

/**
 * ������ atomic_set �󶨵Ķ���Ϊ��ֵ����ʱ�����Ե��ô˺������ñ��󶨶���
 * �ĳ�����ֵ
 * @param self {ATOMIC*} ԭ�Ӷ���
 * @param n {long long} ��ԭ�Ӷ������󶨵Ķ��󽫱���ֵΪ��ֵ
 */
void atomic_int64_set(ATOMIC *self, long long n);

/**
 * �Ȼ����ֵ�������洢������ֵ��Ȼ��������ָ����ֵ�洢�ڸ���ֵ������
 * @param self {ATOMIC*} ԭ�Ӷ���
 * @param n {long long} ����ֵ 
 * @return {long long} ��������֮ǰ������ֵ�����ֵ
 */
long long atomic_int64_fetch_add(ATOMIC *self, long long n);

/**
 * �����ݶ���洢��ֵ����ָ����ֵ�������ؽ��ֵ
 * @param self {ATOMIC*} ԭ�Ӷ���
 * @param n {long long} ����ֵ 
 * @return {long long} ��������֮���ֵ
 */
long long atomic_int64_add_fetch(ATOMIC *self, long long n);

/**
 * �Ƚϲ���������ֵ����ԭ�Ӷ���洢������ֵ������Ƚ�����ֵ��ͬʱ������������
 * ֵ�ҷ���֮ǰ�洢������ֵ
 * @param self {ATOMIC*} ԭ�Ӷ���
 * @param cmp {long long} ���Ƚ�����ֵ
 * @param n {long long} ��ԭ�Ӷ�������Ƚ�����ֵ��ͬʱ�Ὣԭ�Ӷ�������Ϊ��ֵ
 * @return {long long} ����ԭ�Ӷ���֮ǰ�洢������ֵ
 */
long long atomic_int64_cas(ATOMIC *self, long long cmp, long long n);

#ifdef __cplusplus
}
#endif

#endif
