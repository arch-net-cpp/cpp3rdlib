#pragma once
#include "../acl_cpp_define.hpp"
#include <map>
#include "../stdlib/string.hpp"
#include "../connpool/connect_manager.hpp"

#if !defined(ACL_CLIENT_ONLY) && !defined(ACL_DB_DISABLE)

namespace acl {

class pgsql_conf;

class ACL_CPP_API pgsql_manager : public connect_manager
{
public:
	pgsql_manager(time_t idle_ttl = 120);
	~pgsql_manager();

	/**
	 * ���һ�����ݿ�ʵ��������
	 * @param conf {const pgsql_conf&}
	 * @return {pgsql_manager&}
	 */
	pgsql_manager& add(const pgsql_conf& conf);

protected:
	/**
	 * ���� connect_manager �麯����ʵ��
	 * @param addr {const char*} ������������ַ����ʽ��ip:port
	 * @param count {size_t} ���ӳصĴ�С���ƣ���ֵΪ 0 ʱ��û������
	 * @param idx {size_t} �����ӳض����ڼ����е��±�λ��(�� 0 ��ʼ)
	 * @return {connect_pool*} ���ش��������ӳض���
	 */
	connect_pool* create_pool(const char* addr, size_t count, size_t idx);

private:
	time_t idle_ttl_;       // ���ݿ����ӵĿ��й���ʱ��
	std::map<string, pgsql_conf*> dbs_;
};

} // namespace acl

#endif // !defined(ACL_CLIENT_ONLY) && !defined(ACL_DB_DISABLE)
