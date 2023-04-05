#pragma once
#include "../acl_cpp_define.hpp"
#include "mime_code.hpp"

#if !defined(ACL_MIME_DISABLE)

namespace acl {

class ACL_CPP_API mime_base64 : public mime_code
{
public:
	/**
	 * ���캯��
	 * @param addCrlf {bool} ����ʽ����ʱ�Ƿ���ĩβ��� "\r\n"
	 * @param addInvalid {bool} ��ʽ����ʱ�Ƿ������Ƿ��ַ��Ƿ�ԭ������
	 */
	mime_base64(bool addCrlf = false, bool addInvalid = false);
	~mime_base64(void);

	/**
	 * ��̬���뺯����ֱ�ӽ��������ݽ��б���ͬʱ�����û�������
	 * �û�������
	 * @param in {const char*} �������ݵ�ַ
	 * @param n {int} �������ݳ���
	 * @param out {string*} �洢����Ļ�����
	 */
	static void encode(const char* in, int n, string* out);

	/**
	 * ��̬���뺯����ֱ�ӽ��������ݽ��н����������û�������
	 * @param in {const char*} �������ݵ�ַ
	 * @param n {int} ���ݳ���
	 * @param out {string*} �洢�������
	 */
	static void decode(const char* in, int n, string* out);
};

} // namespace acl

#endif // !defined(ACL_MIME_DISABLE)
