#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/string.hpp"
#include "../stdlib/locker.hpp"
#include "../stdlib/noncopyable.hpp"
#include <vector>
#include <map>

struct ACL_EVENT;

namespace acl
{

class connect_pool;
class connect_monitor;

// �ڲ�ʹ�����ݽṹ
struct conns_pools {
	std::vector<connect_pool*> pools;
	size_t  check_next;			// ���Ӽ��ʱ�ļ����±�
	size_t  conns_next;			// ��һ��Ҫ���ʵĵ��±�ֵ
	conns_pools(void)
	{
		check_next = 0;
		conns_next = 0;
	}
};

struct conn_config {
	string addr;
	size_t count;
	int    conn_timeout;
	int    rw_timeout;

	conn_config(void) {
		count        = 0;
		conn_timeout = 5;
		rw_timeout   = 5;
	}
};

/**
 * connect pool ������������л�ȡ���ӳصȹ���
 */
class ACL_CPP_API connect_manager : public noncopyable
{
public:
	connect_manager(void);
	virtual ~connect_manager(void);

	/**
	 * �Ƿ����ӳ����߳��Զ��󶨣���Ҫ����Э�̻����У��ڲ�ȱʡֵΪ false��
	 * �÷����ڱ����󴴽�����ܵ���һ��
	 * @param yes {bool}
	 */
	void bind_thread(bool yes);

	/**
	 * ��ʼ�����з����������ӳأ��ú������� set �������ÿ����������ӳ�
	 * @param default_addr {const char*} ȱʡ�ķ�������ַ������ǿգ�
	 *  ���ڲ�ѯʱ����ʹ�ô˷�����
	 * @param addr_list {const char*} ���з������б�����Ϊ��
	 *  ��ʽ: IP:PORT:COUNT;IP:PORT:COUNT;IP:PORT;IP:PORT ...
	 *    ��  IP:PORT:COUNT,IP:PORT:COUNT,IP:PORT;IP:PORT ...
	 *  �磺127.0.0.1:7777:50;192.168.1.1:7777:10;127.0.0.1:7778
	 * @param count {size_t} �� addr_list �зָ���ĳ������û��
	 *  COUNT ��Ϣʱ���ô�ֵ������ֵΪ 0 ʱ������������������
	 * @param conn_timeout {int} ��������ʱ��(��)
	 * @param rw_timeout {int} ���� IO ��ʱʱ��(��)
	 *  ע��default_addr �� addr_list ����ͬʱΪ��
	 */
	void init(const char* default_addr, const char* addr_list,
		size_t count, int conn_timeout = 30, int rw_timeout = 30);

	/**
	* ��ӷ������Ŀͻ������ӳأ��ú��������ڳ�������ʱ�����ã��ڲ��Զ�����
	 * @param addr {const char*} ��������ַ����ʽ��ip:port
	 *  ע�⣺���ñ�����ʱÿ�ν������һ����������ַ������ѭ�����ñ�����
	 * @param count {size_t} ���ӳ���������, �����ֵ��Ϊ 0��������
	 *  ���ӳص���������
	 * @param conn_timeout {int} ��������ʱ��(��)
	 * @param rw_timeout {int} ���� IO ��ʱʱ��(��)
	 */
	void set(const char* addr, size_t count,
		int conn_timeout = 30, int rw_timeout = 30);

	/**
	 * ����ָ����ַ��ȡ�õ�ַ��Ӧ�����ӳ����ö���
	 * @param addr {const char*} Ŀ�����ӳص�ַ
	 * @param use_first {bool} ���Ŀ���ַ�����ö��󲻴��ڣ��Ƿ�����ʹ��
	 *  ��һ����ַ���ö���
	 * @return {const conn_config*} ���� NULL ��ʾ������
	 */
	const conn_config* get_config(const char* addr, bool use_first = false);

	/**
	 * �������ӳ�ʧ�ܺ����Ե�ʱ��ʱ������룩���ú��������ڳ�������ʱ��
	 * ���ã��ڲ��Զ�����
	 * @param n {int} ����ֵ <= 0 ʱ�������ӳس��������������������
	 */
	void set_retry_inter(int n);

	/**
	 * �������ӳ��п������ӵĿ�����������
	 * @param ttl {time_t} �������ӵ��������ڣ�����ֵ < 0 ���ʾ��������
	 *  �����ڣ�== 0 ʱ��ʾ���̹��ڣ�> 0 ��ʾ���и�ʱ��κ󽫱��ͷ�
	 */
	void set_idle_ttl(time_t ttl);

	/**
	 * �����Զ����������ӵ�ʱ������ȱʡֵΪ 30 ��
	 * @param n {int} ʱ����
	 */
	void set_check_inter(int n);

	/**
	 * �����ӳؼ�Ⱥ��ɾ��ĳ����ַ�����ӳأ��ú��������ڳ������й�����
	 * �����ã���Ϊ�ڲ����Զ�����
	 * @param addr {const char*} ��������ַ(ip:port)
	 */
	void remove(const char* addr);

	/**
	 * ���ݷ���˵�ַ��ø÷����������ӳ�
	 * @param addr {const char*} redis ��������ַ(ip:port)
	 * @param exclusive {bool} �Ƿ���Ҫ����������ӳ����飬����Ҫ��̬
	 *  �������ӳؼ�Ⱥʱ����ֵӦΪ true
	 * @param restore {bool} ���÷����㱻��Ϊ������ʱ���ò��������Ƿ�
	 *  �Զ���֮�ָ�Ϊ����״̬
	 * @return {connect_pool*} ���ؿձ�ʾû�д˷���
	 */
	connect_pool* get(const char* addr, bool exclusive = true,
		bool restore = false);

	/**
	 * �����ӳؼ�Ⱥ�л��һ�����ӳأ��ú���������ѭ��ʽ�����ӳؼ����л�ȡ
	 * һ����˷����������ӳأ��Ӷ���֤����ȫ�ľ����ԣ��ú����ڲ����Զ���
	 * ���ӳع�����м���
	 * ���⣬�ú���Ϊ��ӿڣ���������ʵ���Լ�����ѭ��ʽ
	 * @return {connect_pool*} ����һ�����ӳأ�����ָ����Զ�ǿ�
	 */
	virtual connect_pool* peek(void);

	/**
	 * �����ӳؼ�Ⱥ�л��һ�����ӳأ��ú������ù�ϣ��λ��ʽ�Ӽ����л�ȡһ��
	 * ��˷����������ӳأ�����������ش��麯���������Լ��ļ�Ⱥ��ȡ��ʽ
	 * ���麯���ڲ�ȱʡ���� CRC32 �Ĺ�ϣ�㷨��
	 * @param key {const char*} ��ֵ�ַ����������ֵΪ NULL�����ڲ�
	 *  �Զ��л�����ѭ��ʽ
	 * @param exclusive {bool} �Ƿ���Ҫ����������ӳ����飬����Ҫ��̬
	 *  �������ӳؼ�Ⱥʱ����ֵӦΪ true
	 * @return {connect_pool*} ����һ�����õ����ӳأ�����ָ����Զ�ǿ�
	 */
	virtual connect_pool* peek(const char* key, bool exclusive = true);

	/**
	 * ���û������� peek ����ʱ�����Ե��ô˺��������ӳع�����̼���
	 */
	void lock(void);

	/**
	 * ���û������� peek ����ʱ�����Ե��ô˺��������ӳع�����̼���
	 */
	void unlock(void);

	/**
	 * ������еķ����������ӳأ������ӳ��а���ȱʡ�ķ������ӳ�
	 * @return {std::vector<connect_pool*>&}
	 */
	std::vector<connect_pool*>& get_pools(void);

	/**
	 * ������ӳ��еĿ������ӣ������ڵ������ͷŵ�
	 * @param step {size_t} ÿ�μ�����ӳصĸ���
	 * @param left {size_t*} �ǿ�ʱ�����洢����ʣ�����Ӹ����ܺ�
	 * @return {size_t} ���ͷŵĿ���������
	 */
	size_t check_idle(size_t step, size_t* left = NULL);

	/**
	 * ������ӳؼ��������ӳض���ĸ���
	 * @return {size_t}
	 */
	size_t size(void) const;

	/**
	 * ���ȱʡ�ķ��������ӳ�
	 * @return {connect_pool*} ������ init ������ default_addr Ϊ��ʱ
	 *  �ú������� NULL
	 */
	connect_pool* get_default_pool(void)
	{
		return default_pool_;
	}

	/**
	 * ��ӡ��ǰ���� redis ���ӳصķ�����
	 */
	void statistics(void);

	/**
	 * ������̨����������̼߳���������ӳ�����״̬
	 * @param monitor {connect_monitor*} ���Ӽ�����
	 * @return {bool} �Ƿ��������������Ӽ������������ false ˵����ǰ����
	 *  �������е����Ӽ�����������ٴ����������ʱ��Ҫ�ȵ��� stop_monitor
	 */
	bool start_monitor(connect_monitor* monitor);

	/**
	 * ֹͣ��̨����߳�
	 * @param graceful {bool} �Ƿ��ڹرռ���߳�ʱ��Ҫ�ȴ����еļ������
	 *  �رպ�ŷ��أ������ӳؼ�Ⱥ����Ϊ���̿ռ��ڲ����η������ͷ�ʱ��
	 *  ���ֵ������Ϊ false �Ӷ�ʹ����߳̿����˳�������Ӧ�õȴ����м��
	 *  ���ӹرպ���ʹ����߳��˳�
	 * @return {connect_monitor*} ���� start_monitor ���õļ������ͬʱ
	 *  �ڲ��� monitor_ ��Ա�Զ��� NULL
	 */
	connect_monitor* stop_monitor(bool graceful = true);

	/**
	 * ����ĳ�����ӳط���Ĵ��״̬���ڲ����Զ�����
	 * @param addr {const char*} ��������ַ����ʽ��ip:port
	 * @param alive {bool} �÷������Ƿ�����
	 */
	void set_pools_status(const char* addr, bool alive);

protected:
	/**
	 * ���麯�����������ʵ�ִ˺��������������ӳض���
	 * @param addr {const char*} ������������ַ����ʽ��ip:port
	 * @param count {size_t} ���ӳصĴ�С���ƣ�Ϊ 0 ʱ�������ӳ�û������
	 * @param idx {size_t} �����ӳض����ڼ����е��±�λ��(�� 0 ��ʼ)
	 * @return {connect_pool*} ���ش��������ӳض���
	 */
	virtual connect_pool* create_pool(const char* addr,
		size_t count, size_t idx) = 0;

protected:
	typedef std::vector<connect_pool*> pools_t;
	typedef pools_t::iterator          pools_it;
	typedef pools_t::const_iterator    pools_cit;

	typedef std::map<unsigned long, conns_pools*> manager_t;
	typedef manager_t::iterator                   manager_it;
	typedef manager_t::const_iterator             manager_cit;

	bool thread_binding_;			// ����Э�̻�������ÿ���̰߳�
	string default_addr_;			// ȱʡ�ķ����ַ
	connect_pool* default_pool_;		// ȱʡ�ķ������ӳ�

	std::map<string, conn_config> addrs_;	// ���еķ���˵�ַ
	manager_t  manager_;

	locker lock_;				// ���� pools_ ʱ�Ļ�����
	int  stat_inter_;			// ͳ�Ʒ������Ķ�ʱ�����
	int  retry_inter_;			// ���ӳ�ʧ�ܺ����Ե�ʱ����
	time_t idle_ttl_;			// �������ӵ���������
	int  check_inter_;			// ���������ӵ�ʱ����
	connect_monitor* monitor_;		// ��̨����߳̾��

	// ���ó�ȱʡ����֮��ķ�������Ⱥ
	void set_service_list(const char* addr_list, int count,
		int conn_timeout, int rw_timeout);
	conns_pools& get_pools_by_id(unsigned long id);
	connect_pool* create_pool(const conn_config& cf, size_t idx);
	void create_pools_for(pools_t& pools);

	void remove(pools_t& pools, const char* addr);
	void set_status(pools_t& pools, const char* addr, bool alive);

	unsigned long get_id(void) const;
	void get_key(const char* addr, string& key);
	void get_addr(const char* key, string& addr);
	connect_pool* add_pool(const char* addr);

	// �ֲ߳̾�������ʼ��ʱ�Ļص�����
	static void thread_oninit(void);
	// �߳��˳�ǰ��Ҫ�ص��˷����������ͷ��ڲ��������ֲ߳̾�����
	static void thread_onexit(void* ctx);
};

} // namespace acl
