#pragma once
#include "../acl_cpp_define.hpp"
#include "dbuf_pool.hpp"

namespace acl
{

class diff_manager;

/**
 * ��Ƚϴ����࣬�������̳и��࣬��ʵ�����еĴ��鷽��
 * ����̳��� dbuf_obj �࣬������ dbuf_guard ͳһ����ͳһ����
 */
class diff_object : public dbuf_obj
{
public:
	/**
	 * ���캯��
	 * @param manager {diff_manager&}
	 */
	diff_object(diff_manager& manager);

	virtual ~diff_object(void) {}

	/**
	 * ����ӿڣ���øö���ļ��ַ���
	 * @return {const char*} ���뷵�طǿ��ַ���
	 */
	virtual const char* get_key(void) const = 0;

	/**
	 * ����ӿڣ���øö����ֵ�ַ���
	 * @return {const char*} ���뷵�طǿ��ַ���
	 */
	virtual const char* get_val(void) const = 0;

	/**
	 * ����ӿڣ������Ƚ���������
	 * @param obj {const diff_object&}
	 * @return {bool} ���������Ƿ����
	 */
	virtual bool operator== (const diff_object& obj) const = 0;

	/**
	 * �Ƿ��ǲ��ڸ������䷶Χ�������䣩�Ķ�������
	 * @param range_from {long long} ��ʼλ��
	 * @param range_to {long long} ����λ��
	 * @return {bool} �Ƿ��ǳ����������䷶Χ�Ķ������ݶ���
	 */
	virtual bool check_range(long long range_from, long long range_to) const
	{
		(void) range_from;
		(void) range_to;
		return false;
	}

protected:
	diff_manager& manager_;
};

} // namespace acl
