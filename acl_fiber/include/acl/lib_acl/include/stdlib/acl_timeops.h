/*
 * Name: misc.h
 * Author: zsx
 * Date: 2003-12-16
 * Version: 1.0
 */

#ifndef	ACL_MISC_INCLUDE_H
#define	ACL_MISC_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include <time.h>

/* acl_str2time.c */
/**
 * ��ʱ���ַ���ת��Ϊ time_t ����
 * @param str ʱ���ַ�����ʽΪ: year-month-mday(��: 2004-1-1)
 * @return time_t ���͵�ֵ
 */
ACL_API time_t acl_str2time_t(const char *str);

/* acl_localtime.c */
/**
 * �����͵�ʱ��ת��Ϊָ����ʱ��ṹ����,������glibc�е� localtime_r(), �� glibc
 * ʵ�ֵ� localtime_r() �ڲ�����, ��Ӱ��Ч��, ͬʱ���ܻ����fork()�����������,
 * ���Խ� Redis �Ĵ����е�ʵ�ַ��ڴ˴�.
 * @param t {const time_t*} 1970.1.1������ʱ��ֵ(�뼶)
 * @param result {struct tm*} ���ת���������
 * @return {struct tm *} ����ֵ��NULL��ʾת���ɹ�,�����ʾʧ��
 */
ACL_API struct tm *acl_localtime_r(const time_t *t, struct tm *result);

#ifdef  __cplusplus
}
#endif

#endif

