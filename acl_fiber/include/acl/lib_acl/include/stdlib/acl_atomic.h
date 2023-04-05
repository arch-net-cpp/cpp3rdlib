#ifndef ACL_ATOMIC_INCLUDE_H
#define ACL_ATOMIC_INCLUDE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "acl_define.h"

typedef struct ACL_ATOMIC ACL_ATOMIC;

/**
 * ����ԭ�Ӷ���
 * @return {ACL_ATOMIC*} �����´����Ķ���
 */
ACL_API ACL_ATOMIC *acl_atomic_new(void);

/**
 * �ͷ�ԭ�Ӷ���
 * @param self {ACL_ATOMIC*} ԭ�Ӷ���
 */
ACL_API void acl_atomic_free(ACL_ATOMIC *self);

/**
 * ��ָ��������ԭ�Ӷ���󶨣��Ա��ڶԸö������ԭ�Ӳ���
 * @param self {ACL_ATOMIC*} ԭ�Ӷ���
 * @param value {void*} �������Ķ���ͨ��ԭ�Ӷ���ʵ�ֶԸö����ԭ�Ӳ���
 */
ACL_API void acl_atomic_set(ACL_ATOMIC *self, void *value);

/**
 * �Ƚϲ��������󣬵�ԭ�Ӷ���󶨵Ķ���������Ƚ϶�����ͬʱ�������¶�����
 * ����֮ǰ�󶨵Ķ���
 * @param self {ACL_ATOMIC*} ԭ�Ӷ���
 * @param cmp {void*} ���Ƚ϶���ָ��
 * @param value {void*} ��ԭ�Ӷ�������Ƚ϶�����ͬʱ�Ὣ�ö�����ԭ�Ӷ����
 * @return {void*} ����ԭ�Ӷ���֮ǰ�󶨵Ķ���
 */
ACL_API void *acl_atomic_cas(ACL_ATOMIC *self, void *cmp, void *value);

/**
 * ��ԭ�Ӷ������¶�����а󶨣�������֮ǰ�󶨵Ķ���
 * @param self {ACL_ATOMIC*} ԭ�Ӷ���
 * @param value {void*} �����󶨵��¶���
 * @return {void*} ����֮ǰ�󶨵Ķ���
 */
ACL_API void *acl_atomic_xchg(ACL_ATOMIC *self, void *value);

/**
 * ������ acl_atomic_set �󶨵Ķ���Ϊ��ֵ����ʱ�����Ե��ô˺������ñ��󶨶���
 * �ĳ�����ֵ
 * @param self {ACL_ATOMIC*} ԭ�Ӷ���
 * @param n {long long} ��ԭ�Ӷ������󶨵Ķ��󽫱���ֵΪ��ֵ
 */
ACL_API void acl_atomic_int64_set(ACL_ATOMIC *self, long long n);

/**
 * �Ȼ����ֵ�������洢������ֵ��Ȼ��������ָ����ֵ�洢�ڸ���ֵ������
 * @param self {ACL_ATOMIC*} ԭ�Ӷ���
 * @param n {long long} ����ֵ 
 * @return {long long} ��������֮ǰ������ֵ�����ֵ
 */
ACL_API long long acl_atomic_int64_fetch_add(ACL_ATOMIC *self, long long n);

/**
 * �����ݶ���洢��ֵ����ָ����ֵ�������ؽ��ֵ
 * @param self {ACL_ATOMIC*} ԭ�Ӷ���
 * @param n {long long} ����ֵ 
 * @return {long long} ��������֮���ֵ
 */
ACL_API long long acl_atomic_int64_add_fetch(ACL_ATOMIC *self, long long n);

/**
 * �Ƚϲ���������ֵ����ԭ�Ӷ���洢������ֵ������Ƚ�����ֵ��ͬʱ������������
 * ֵ�ҷ���֮ǰ�洢������ֵ
 * @param self {ACL_ATOMIC*} ԭ�Ӷ���
 * @param cmp {long long} ���Ƚ�����ֵ
 * @param n {long long} ��ԭ�Ӷ�������Ƚ�����ֵ��ͬʱ�Ὣԭ�Ӷ�������Ϊ��ֵ
 * @return {long long} ����ԭ�Ӷ���֮ǰ�洢������ֵ
 */
ACL_API long long acl_atomic_int64_cas(ACL_ATOMIC *self, long long cmp, long long n);

/****************************************************************************/

typedef struct ACL_ATOMIC_CLOCK ACL_ATOMIC_CLOCK;

ACL_API ACL_ATOMIC_CLOCK *acl_atomic_clock_alloc(void);
ACL_API void acl_atomic_clock_free(ACL_ATOMIC_CLOCK *clk);
ACL_API long long acl_atomic_clock_count_add(ACL_ATOMIC_CLOCK *clk, int n);
ACL_API long long acl_atomic_clock_users_add(ACL_ATOMIC_CLOCK *clk, int n);
ACL_API void acl_atomic_clock_users_count_inc(ACL_ATOMIC_CLOCK *clk);
ACL_API long long acl_atomic_clock_count(ACL_ATOMIC_CLOCK *clk);
ACL_API long long acl_atomic_clock_atime(ACL_ATOMIC_CLOCK *clk);
ACL_API long long acl_atomic_clock_users(ACL_ATOMIC_CLOCK *clk);

#ifdef __cplusplus
}
#endif

#endif
