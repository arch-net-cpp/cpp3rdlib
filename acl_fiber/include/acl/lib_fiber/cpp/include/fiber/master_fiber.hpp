#pragma once
#include "fiber_cpp_define.hpp"
//#include "acl_cpp/master/master_base.hpp"

struct ACL_VSTREAM;

namespace acl {

class socket_stream;

/**
 * ����Э�̷�ʽ�����������
 */
class FIBER_CPP_API master_fiber : public master_base
{
public:
	/**
	 * �� acl_master ��������б�����������
	 * @param argc {int} ����Ĳ��������С
	 * @param argv {char**} ����Ĳ�������
	 */
	void run_daemon(int argc, char** argv);

	/**
	 * �Զ�������ģʽ����������������
	 * @param addrs {const char*} �����ı��������ַ�б���ʽ��
	 *  ip:port, ip:port, ...
	 * @param path {const char*} �� NULL ָ�������ļ�·��
	 */
	bool run_alone(const char* addrs, const char* path = NULL);

	/**
	 * ��������ļ�·��
	 * @return {const char*} ����ֵΪ NULL ��ʾû���������ļ�
	 */
	const char* get_conf_path(void) const;

protected:
	master_fiber();

	virtual ~master_fiber();

	/**
	 * �麯������Э�̷��������յ��ͻ������Ӻ���ñ�����
	 * @param stream {socket_stream&} �ͻ������Ӷ��󣬱��������غ�Э��
	 *  �����ܽ���رո����Ӷ���
	 */
	virtual void on_accept(socket_stream& stream) = 0;

	/**
	 * ���̳߳�ʼ��ʱ���鷽����������
	 */
	virtual void thread_on_init(void) {}

private:
	static void service_on_listen(void*, ACL_VSTREAM*);
	static void service_on_accept(void*, ACL_VSTREAM*);
	static void service_pre_jail(void*);
	static void service_init(void*);
	static void thread_init(void*); 
	static void service_exit(void*);
	static int  service_on_sighup(void*, ACL_VSTRING*);

	void run(int argc, char** argv);
};

} // namespace acl
