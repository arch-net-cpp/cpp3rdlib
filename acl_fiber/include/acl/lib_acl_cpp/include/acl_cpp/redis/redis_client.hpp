#pragma once
#include "../acl_cpp_define.hpp"
#include <vector>
#include "../stream/socket_stream.hpp"
#include "../connpool/connect_client.hpp"

#if !defined(ACL_CLIENT_ONLY) && !defined(ACL_REDIS_DISABLE)

namespace acl
{

class string;
class dbuf_pool;
class redis_result;
class redis_request;
class redis_command;
class sslbase_conf;

/**
 * redis �ͻ��˶�������ͨ���࣬ͨ�����ཫ��֯�õ� redis ��������� redis
 * ����ˣ�ͬʱ���� redis �������Ӧ���������̳��� connect_client �࣬��Ҫ
 * Ϊ��ʹ�����ӳع��ܡ�
 * redis client network IO class. The redis request is sent to server
 * and the server's respond is handled in this class. The class inherits
 * connect_client, which can use the connection pool function.
 */
class ACL_CPP_API redis_client : public connect_client
{
public:
	/**
	 * ���캯��
	 * constructor
	 * @param addr {const char*} redis-server ������ַ
	 *  the redis-server listening addr
	 * @param conn_timeout {int} ���� redis-server �ĳ�ʱʱ��(��)
	 *  the timeout in seconds to connect the redis-server
	 * @param rw_timeout {int} �� redis-server ����ͨ�ŵ� IO ��ʱʱ��(��)
	 *  the network IO timeout in seconds with the redis-server
	 */
	redis_client(const char* addr, int conn_timeout = 60,
		int rw_timeout = 30, bool retry = true);
	virtual ~redis_client(void);

	/**
	 * ���� SSL ͨ�ŷ�ʽ�µ����þ�����ڲ�ȱʡֵΪ NULL����������� SSL ��
	 * �����ö������ڲ��л��� SSL ͨ�ŷ�ʽ
	 * set SSL communication with Redis-server if ssl_conf not NULL
	 * @param ssl_conf {sslbase_conf*}
	 */
	void set_ssl_conf(sslbase_conf* ssl_conf);

	/**
	 * ���ñ������������� redis �������������
	 * @param pass {const char*}
	 */
	void set_password(const char* pass);

	/**
	 * ���ñ���������Ӧ�� db�������ӽ��������ָ���� db ֵ���� 0�����ڲ��Զ�
	 * ѡ���Ӧ�� db��ע�⣺�ù��ܽ���ԷǼ�Ⱥģʽ
	 * if db > 0 in no cluster mode, select the db when the connection
	 * is created.
	 * @param dbnum {int}
	 */
	void set_db(int dbnum);

	/**
	 * ��ñ�������ѡ��� db
	 * get db for the connection
	 * @return {int}
	 */
	int get_db(void) const
	{
		return dbnum_;
	}

	/**
	 * ��õ�ǰ���ӵķ�������ַ������ redis_client ����ʱ����ĵ�ַ
	 * @return {const char*}
	 */
	const char* get_addr(void) const
	{
		return addr_;
	}

	/**
	 * �ڽ���ÿ�������ǰ���Ƿ�Ҫ���� socket ������ַ��ƥ���������
	 * �򿪸�ѡ��ʱ�������ϸ���ƥ�����������Ӱ��һ�����ܣ���˸����ý�
	 * ���� DEBUG ʱ�����г���
	 * @param on {bool}
	 */
	void set_check_addr(bool on);

	/**
	 * �жϸ��������Ӷ����Ƿ��Ѿ��ر�
	 * check if the connection has been finish
	 * @return {bool}
	 */
	bool eof(void) const;

	/**
	 * �ر���������
	 * close the connection to the redis-server
	 */
	void close(void);

	/**
	 * ��������������������ӹر�ʱ�ڲ����Զ�����һ��
	 * get acl::socket_stream from the connection
	 * @param auto_connect {bool} �ڲ��Ƿ���Ҫ�Զ����ӷ����
	 *  if we should connect the redis server automaticlly
	 * @return {acl::socket_stream*} ��������Ѿ��ر��򷵻� NULL
	 *  NULL will be returned if the connectioin has been closed
	 */
	socket_stream* get_stream(bool auto_connect = true);

	/**
	 * �����������ݰ����˺�����������װ�������ݰ���ʱ��ϳ�һ�����ݰ�����
	 * just for request package, setting flag for sending data with
	 * multi data chunks; this is useful when the request data is large
	 * @param on {bool} ��Ϊ true ʱ�򲻻Ὣ�������ݺϳ�һ�����ݰ�����
	 *  if true the request data will not be combined one package
	 */
	void set_slice_request(bool on);

	/**
	 * ������Ӧ���ݰ����˺��������Ƿ� redis-server ��Ӧ�����ݷֲ�ɶ�����ݿ飬
	 * ����ڴ�����ݰ����ô������Բ���һ���Է���һ�������ԵĴ��ڴ�
	 * just for response package, settint flag for receiving data
	 * if split the large response data into multi little chunks
	 * @param on {bool} ��Ϊ true ʱ�����Ӧ���ݰ����в��
	 *  if true the response data will be splitted into multi little
	 *  data, which is useful for large reponse data for avoiding
	 *  malloc large continuously memory from system.
	 */
	void set_slice_respond(bool on);

	/**
	 * ���ڷǷ�Ƭ���ͷ�ʽ���� redis-server �����������ݣ�ͬʱ��ȡ������
	 * ����˷��ص���Ӧ����
	 * send request to redis-server, and read/anlyse response from server,
	 * this function will be used for no-slice request mode.
	 * @param pool {dbuf_pool*} �ڴ�ع���������
	 *  memory pool manager
	 * @param req {const string&} �������ݰ�
	 *  the request package
	 * @param nchildren {size_t} ��Ӧ�����м������ݶ���
	 *  the data object number in the server's response data
	 * @return {const redis_result*} �����ķ�������Ӧ���󣬷��� NULL �����,
	 *  �ö��󲻱��ֹ��ͷţ���Ϊ������ pool �ڴ�ض����϶�̬����ģ����Ե�
	 *  �ͷ� pool ʱ�ý������һͬ���ͷ�
	 *  the result object from server's response, NULL will be returned
	 *  when some error happens; the result object needn't be freed
	 *  manually, which was created in the pool object, and will be freed
	 *  when the pool were freed.
	 *  
	 */
	const redis_result* run(dbuf_pool* pool, const string& req,
		size_t nchildren, int* rw_timeout = NULL);

	/**
	 * ���ڷ�Ƭ��������ʽ
	 * just for sending proccess in slice request mode
	 * @param req {const redis_request&} �������ݶ���
	 *  request object
	 */
	const redis_result* run(dbuf_pool* pool, const redis_request& req,
		size_t nchildren, int* rw_timeout = NULL);

	const redis_result* run(redis_command* cmd, size_t nchildren,
		int* rw_timeout = NULL);

protected:
	// �����麯��
	// @override
	bool open(void);

protected:
	socket_stream conn_;
	bool   check_addr_;
	char*  addr_;
	char*  pass_;
	bool   retry_;
	bool   authing_;
	bool   slice_req_;
	bool   slice_res_;
	int    dbnum_;
	sslbase_conf* ssl_conf_;

public:
	redis_result* get_objects(socket_stream& conn,
	 	dbuf_pool* pool, size_t nobjs);
	redis_result* get_object(socket_stream& conn, dbuf_pool* pool);
	redis_result* get_error(socket_stream& conn, dbuf_pool* pool);
	redis_result* get_status(socket_stream& conn, dbuf_pool* pool);
	redis_result* get_integer(socket_stream& conn, dbuf_pool* pool);
	redis_result* get_string(socket_stream& conn, dbuf_pool* pool);
	redis_result* get_array(socket_stream& conn, dbuf_pool* pool);

private:
	void put_data(dbuf_pool* pool, redis_result* rr,
		const char* data, size_t len);
	bool check_connection(socket_stream& conn);
};

} // end namespace acl

#endif // !defined(ACL_CLIENT_ONLY) && !defined(ACL_REDIS_DISABLE)
