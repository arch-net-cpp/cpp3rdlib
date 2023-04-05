#ifndef	ACL_CORE_LIMIT_INCLUDE_H
#define	ACL_CORE_LIMIT_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "../acl_define.h"
#ifdef ACL_UNIX

/**
 * ���ô˺������ó������ʱ������ core �ļ������ֵ
 * @param max {long long int} ���� max ��ֵ��Χ��ͬ������ core ����������ͬ��
 *  1)   0����ֹ���� core �ļ�
 *  2) < 0������ core �ļ����Ҳ����� core �ļ����ɴ�С
 *  3) > 0������ core �ļ����� core �ļ���С�� max ����
 */
void acl_set_core_limit(long long int max);

#endif  /* ACL_UNIX */

#ifdef  __cplusplus
}
#endif

#endif
