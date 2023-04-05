#pragma once
#include "../acl_cpp_define.hpp"
#include "fstream.hpp"

namespace acl {

class ACL_CPP_API ofstream : public fstream
{
public:
	ofstream(void);
	virtual ~ofstream(void);

	/**
	 * ��ֻд��ʽ���ļ�������ļ��������򴴽����ļ�
	 * @param path {const char*} �ļ���
	 * @param otrunc {bool} ���ļ����ڣ�����ļ�ʱ�Ƿ���Ҫ����ո��ļ�
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool open_write(const char* path, bool otrunc = true);

	/**
	 * ��β����ӷ�ʽ���ļ�������ļ��������򴴽����ļ�
	 * @param path {const char*} �ļ���
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool open_append(const char* path);
};

} // namespace acl
