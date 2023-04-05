#include "acl_stdafx.hpp"
#ifndef ACL_PREPARE_COMPILE
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/locker.hpp"
#include "acl_cpp/connpool/connect_client.hpp"
#include "acl_cpp/connpool/connect_pool.hpp"
#endif

namespace acl
{

connect_pool::connect_pool(const char* addr, size_t max, size_t idx /* = 0 */)
: alive_(true)
, delay_destroy_(false)
, last_dead_(0)
, conn_timeout_(30)
, rw_timeout_(30)
, idx_(idx)
, max_(max)
, count_(0)
, idle_ttl_(-1)
, last_check_(0)
, check_inter_(30)
, total_used_(0)
, current_used_(0)
, last_(0)
{
	retry_inter_ = 1;
	ACL_SAFE_STRNCPY(addr_, addr, sizeof(addr_));
	ACL_SAFE_STRNCPY(key_, addr_, sizeof(key_));
	acl_lowercase(key_);
}

connect_pool::~connect_pool(void)
{
	std::list<connect_client*>::iterator it = pool_.begin();
	for (; it != pool_.end(); ++it) {
		delete *it;
	}
}

void connect_pool::set_key(const char* key)
{
	ACL_SAFE_STRNCPY(key_, key, sizeof(key_));
	acl_lowercase(key_);
}

connect_pool& connect_pool::set_timeout(int conn_timeout, int rw_timeout)
{
	conn_timeout_ = conn_timeout;
	rw_timeout_ = rw_timeout;
	return *this;
}

connect_pool& connect_pool::set_idle_ttl(time_t ttl)
{
	idle_ttl_ = ttl;
	return *this;
}

connect_pool& connect_pool::set_retry_inter(int retry_inter)
{
	lock_.lock();
	retry_inter_ = retry_inter;
	lock_.unlock();

	return *this;
}

connect_pool& connect_pool::set_check_inter(int n)
{
	check_inter_ = n;
	return *this;
}

void connect_pool::reset_statistics(int inter)
{
	time_t now = time(NULL);
	lock_.lock();
	if (now - last_ >= inter) {
		last_ = now;
		current_used_ = 0;
	}
	lock_.unlock();
}

bool connect_pool::aliving()
{
	// XXX����Ȼ�˴�δ��������ҲӦ�ò��������⣬��Ϊ����� peek() ���̻��ٴ�
	// �� alive_ �������Է�ֹ���̲߳���ʱ�ĳ�ͻ
	if (alive_) {
		return true;
	}

	time_t now = time(NULL);

	lock_.lock();
	if (retry_inter_ > 0 && now - last_dead_ >= retry_inter_) {
		alive_ = true;
		lock_.unlock();

		// ���÷��������״̬���Ա�����
		logger("reset server: %s", get_addr());
		return true;
	}

	lock_.unlock();
	return false;
}

connect_client* connect_pool::peek(bool on /* = true */)
{
	lock_.lock();
	if (alive_ == false) {
		time_t now = time(NULL);
		if (retry_inter_ <= 0 || now - last_dead_ < retry_inter_) {
			lock_.unlock();
			return NULL;
		}
		alive_ = true;

		// ���÷��������״̬���Ա�����
		logger("reset server: %s", get_addr());
	}

	connect_client* conn;

	std::list<connect_client*>::iterator it = pool_.begin();
	if (it != pool_.end()) {
		conn = *it;
		pool_.erase(it);
		total_used_++;
		current_used_++;

		lock_.unlock();
		return conn;
	} else if (max_ > 0 && count_ >= max_) {
		logger_error("too many connections, max: %d, curr: %d,"
			" server: %s", (int) max_, (int) count_, addr_);
		lock_.unlock();
		return NULL;
	}

	if (!on) {
		lock_.unlock();
		return NULL;
	}

	// ����������ֵԤ +1
	count_++;
	total_used_++;
	current_used_++;

	lock_.unlock();

	// �����麯��������ʵ�ַ��������������Ӷ��󣬲�������
	conn = create_connect();
	// �ڵ��� open ֮ǰ�����ó�ʱʱ��
	conn->set_timeout(conn_timeout_, rw_timeout_);
	// �������෽��������
	if (conn->open() == false) {
		lock_.lock();

		// ��Ϊ������ʧ�ܣ����Ի��轫����Ԥ +1 ��������Ա�� -1
		count_--;
		total_used_--;
		current_used_--;
#ifdef AUTO_SET_ALIVE
		alive_ = false;
		(void) time(&last_dead_);
#endif

		lock_.unlock();
		delete conn;
		return NULL;
	}

	conn->set_pool(this);
	return conn;
}

void connect_pool::bind_one(connect_client* conn)
{
	lock_.lock();
	if (conn->get_pool() != this) {
		conn->set_pool(this);
		count_++;
	}
	lock_.unlock();
}

void connect_pool::put(connect_client* conn, bool keep /* = true */)
{
	time_t now = time(NULL);

	lock_.lock();

	// ����Ƿ������������ٱ�־λ
	if (delay_destroy_) {
		if (conn->get_pool() == this) {
			count_--;
		}
		delete conn;

		if (count_ <= 0) {
			// ������ü���Ϊ 0 ��������
			lock_.unlock();
			delete this;
		} else {
			lock_.unlock();
		}
		return;
	}

	if (keep && alive_) {
		conn->set_when(now);

		// ���黹�����ӷ��������ײ��������ڵ����ͷŹ�������
		// ʱ�ȽϷ��㣬�����ھ��콫��æ�����ݿ����ӹر�
		pool_.push_front(conn);
	} else {
		acl_assert(count_ > 0);
		if (conn->get_pool() == this) {
			count_--;
		}
		delete conn;
	}

	if (idle_ttl_ >= 0 && now - last_check_ >= check_inter_) {
		(void) check_idle(idle_ttl_, false);
		(void) time(&last_check_);
	}
	lock_.unlock();
}

void connect_pool::set_delay_destroy()
{
	lock_.lock();
	delay_destroy_ = true;
	lock_.unlock();
}

void connect_pool::set_alive(bool yes /* true | false */)
{
	lock_.lock();
	alive_ = yes;
	if (yes == false) {
		time(&last_dead_);
	}
	lock_.unlock();
}

int connect_pool::check_idle(time_t ttl, bool exclusive /* = true */)
{
	if (ttl < 0) {
		return 0;
	}
	if (exclusive) {
		lock_.lock();
	}
	if (pool_.empty()) {
		if (exclusive) {
			lock_.unlock();
		}
		return 0;
	}

	if (ttl == 0) {
		int   n = 0;
		std::list<connect_client*>::iterator it = pool_.begin();
		for (; it != pool_.end(); ++it) {
			delete *it;
			n++;
		}
		pool_.clear();
		count_ = 0;
		if (exclusive) {
			lock_.unlock();
		}
		return n;
	}

	int n = 0;
	time_t now = time(NULL), when;

	std::list<connect_client*>::iterator it, next;
	std::list<connect_client*>::reverse_iterator rit = pool_.rbegin();

	for (; rit != pool_.rend();) {
		it = --rit.base();
		when = (*it)->get_when();
		if (when <= 0) {
			++rit;
			continue;
		}

		if (now - when < ttl) {
			break;
		}

		if ((*it)->get_pool() == this) {
			count_--;
		}
		delete *it;

		next = pool_.erase(it);
		rit = std::list<connect_client*>::reverse_iterator(next);

		n++;
	}

	if (exclusive) {
		lock_.unlock();
	}
	return n;
}

} // namespace acl
