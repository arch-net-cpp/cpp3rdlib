#pragma once
#include "../acl_cpp_define.hpp"
#include "diff_object.hpp"

namespace acl
{

class diff_string : public diff_object
{
public:
	/**
	 * ���캯��
	 * @param manager {diff_manager&}
	 * @param key {const char*} ���ַ�����ʽ��ʾ�ļ����ǿ��ַ���
	 * @param val {const char*} ���ַ�����ʽ��ʾ��ֵ���ǿ��ַ���
	 */
	diff_string(diff_manager& manager, const char* key, const char* val);

	/**
	 * ���ñ��������ڵ�����ֵ
	 * @param range {long long}
	 */
	void set_range(long long range);

	/**
	 * ��ñ��������ڵ�����ֵ
	 * @return {long long}
	 */
	long long get_range(void) const
	{
		return range_;
	}

public:
	// override: ���ി�麯����ʵ��
	const char* get_key(void) const;

	// override: ���ി�麯����ʵ��
	const char* get_val(void) const;

	// override: ���ി������ʵ��
	bool operator== (const diff_object& obj) const;

	// @override
	bool check_range(long long range_from, long long range_to) const;

private:
	const char* key_;
	const char* val_;
	long long range_;

	// ������������Ϊ˽�еģ��Ӷ�Ҫ��̬�����������
	~diff_string(void);
};

} // namespace acl
