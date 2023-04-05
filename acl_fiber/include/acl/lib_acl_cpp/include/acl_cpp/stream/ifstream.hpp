#pragma once
#include "../acl_cpp_define.hpp"
#include "fstream.hpp"

namespace acl {

class string;

class ACL_CPP_API ifstream: public fstream {
public:
	ifstream(void) {}
	virtual ~ifstream(void) {}

	/**
	 * ��ֻ����ʽ���Ѿ����ڵ��ļ�
	 * @param path {const char*} �ļ���
	 * @return {bool} ���ļ��Ƿ�ɹ�
	 */
	bool open_read(const char* path);

	/**
	 * �Ӵ򿪵��ļ����м��ظ��ļ��е��������ݵ��û�ָ����������
	 * @param s {string*} �û�������
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool load(string* s);
	bool load(string& s);

	/**
	 * �����ļ��е��������û�ָ��������, �ú����Ǿ�̬��Ա������
	 * ��ֱ��ʹ��
	 * @param path {const char*} �ļ���
	 * @param s {string*} �û�������
	 * @return {bool} �Ƿ�ɹ�
	 */
	static bool load(const char* path, string* s);
	static bool load(const char* path, string& s);
};

} // namespace acl
