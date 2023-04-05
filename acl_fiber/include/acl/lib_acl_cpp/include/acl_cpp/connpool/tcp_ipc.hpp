#pragma once
#include "../stdlib/noncopyable.hpp"

namespace acl
{

class tcp_manager;
class tcp_pool;
class string;

/**
 * �����װ�� tcp_manager �����࣬���Զ�̬���Ŀ�����˵�ַ��ͬʱ��̬������
 * ÿһ������˵����ӳ�
 */
class ACL_CPP_API tcp_ipc : public noncopyable
{
public:
	tcp_ipc(void);
	~tcp_ipc(void);

	/**
	 * ������ÿ���������������ӳص������������
	 * @param max {int} ÿ�����ӳص�����������ƣ��� <= 0 ʱ������������
	 * @return {tcp_ipc&}
	 */
	tcp_ipc& set_limit(int max);

	/**
	 * �������ӳ���ÿ�������˿���ʱ�䣬�����ӿ���ʱ�䳬������ֵʱ�����ر�
	 * @param ttl {int} �������ӵ����ʱʱ��
	 * @return {tcp_ipc&}
	 */
	tcp_ipc& set_idle(int ttl);

	/**
	 * ����ÿ�����ӵ��������ӳ�ʱʱ��
	 * @param conn_timeout {int} �������ӳ�ʱʱ�䣨�룩
	 * @return {tcp_ipc&}
	 */
	tcp_ipc& set_conn_timeout(int conn_timeout);

	/**
	 * ����ÿ�����ӵ������д��ʱʱ��
	 * @param timeout {int} ��д��ʱʱ�䣨�룩
	 * @return {tcp_ipc&}
	 */
	tcp_ipc& set_rw_timeout(int timeout);

	/**
	 * ��� TCP ����������
	 * @return {tcp_manager&}
	 */
	tcp_manager& get_manager(void) const;

	/**
	 * ���Ե��ñ�������ʾ���һ����������ַ��ֻ�е���ַ������ʱ�Ż����
	 * @param addr {const char*} ��������ַ����ʽ��IP:PORT
	 * @return {tcp_ipc&}
	 */
	tcp_ipc& add_addr(const char* addr);

	/**
	 * ���ݷ�������ַɾ��ָ�������ӳض��󣬵����ӳض������ڱ�����ʱ���ö���
	 * ���ᱻɾ�������ǲ����ӳ�ɾ����ʽ�������һ�����ӱ��黹������ӳض���
	 * �Żᱻ����ɾ��
	 * @param addr {const char*} ��������ַ����ʽ��IP:PORT
	 * @return {tcp_ipc&}
	 */
	tcp_ipc& del_addr(const char* addr);

	/**
	 * ���ָ���ķ�������ַ�Ƿ�ɹ�
	 * @param addr {const char*} ��������ַ����ʽ��IP:PORT
	 * @return {bool}
	 */
	bool addr_exist(const char* addr);

	/**
	 * ��õ�ǰ���еķ�������ַ����
	 * @param addrs {std::vector<string>&} �洢�����
	 */
	void get_addrs(std::vector<string>& addrs);

	/**
	 * �����������ָ�����ȵ����ݰ�
	 * @param addr {const char*} ָ����Ŀ���������ַ
	 * @param data {const void*} Ҫ���͵����ݰ���ַ
	 * @param len {unsigned int} ���ݳ���
	 * @param out {string*} ���ö���� NULL ʱ������Ҫ�ӷ�������ȡ��Ӧ���ݣ�
	 *  ��Ӧ�����������ڸû������У�����ö���Ϊ NULL�����ʾ�����ȡ
	 *  ����������Ӧ����
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool send(const char* addr, const void* data, unsigned int len,
		string* out = NULL);

	/**
	 * �����з������������ݰ�
	 * @param data {const void*} Ҫ���͵����ݰ���ַ
	 * @param len {unsigned int} ���ݳ���
	 * @param exclusive {bool} ���͹㲥��ʱ���Ƿ���߳����Է�ֹ�����߳�
	 *  �����ڲ����ӳ���Դ
	 * @param check_result {bool} �Ƿ����������Ӧ��֤���������յ�������
	 * @param nerr {unsigned *} �� NULL ʱ���ʧ�ܵķ������ĸ���
	 * @return {size_t} ���ط��͵��ķ�����������
	 */
	size_t broadcast(const void* data, unsigned int len,
		bool exclusive = true, bool check_result = false,
		unsigned* nerr = NULL);

private:
	tcp_manager* manager_;
	int max_;
	int ttl_;
	int conn_timeout_;
	int rw_timeout_;

	bool send(tcp_pool&, const void*, unsigned int, string*);
};

} // namespace acl
