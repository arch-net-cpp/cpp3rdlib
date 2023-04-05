#include "acl_stdafx.hpp"
#ifndef ACL_PREPARE_COMPILE
#include <utility>
#include "acl_cpp/stdlib/snprintf.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/escape.hpp"
#include "acl_cpp/session/session.hpp"
#endif

#ifndef ACL_CLIENT_ONLY

namespace acl
{

session::session(time_t ttl /* = 0 */, const char* sid /* = NULL */)
: sid_(64)
, ttl_(ttl)
, dirty_(false)
{
	if (sid == NULL || *sid == 0) {
		struct timeval tv;

		(void) gettimeofday(&tv, NULL);
		sid_.format("acl.%d.%d.%d", (int) tv.tv_sec,
			(int) tv.tv_usec, rand());
		sid_.todo_ = TODO_NUL;
		sid_saved_ = false;
	} else {
		sid_.copy(sid);
		sid_.todo_ = TODO_NUL;
		sid_saved_ = true;
	}
}

session::~session(void)
{
	reset();
}

void session::set_sid(const char* sid)
{
	sid_.copy(sid);
	sid_.todo_ = TODO_NUL;

	// �п����Ѿ��洢�ں�� cache �������
	if (!sid_saved_) {
		sid_saved_ = true;
	}

	// ��������ϴε��м���
	reset();
}

void session::reset(void)
{
	attrs_clear(attrs_);
	attrs_clear(attrs_cache_);
}

void session::attrs_clear(std::map<string, session_string>& attrs)
{
	attrs.clear();
}

bool session::flush(void)
{
	if (!dirty_) {
		return true;
	}
	dirty_ = false;

	// ���ô���ӿڣ����ԭ���� sid ����
	if (get_attrs(attrs_)) {
		if (!sid_saved_) {
			sid_saved_ = true;
		}
	}

	std::map<string, session_string>::iterator it_cache =
		attrs_cache_.begin();
	for (; it_cache != attrs_cache_.end(); ++it_cache) {
		// ����������Ѵ��ڣ�����Ҫ���ͷ�ԭ��������ֵ���������ֵ

		std::map<string, session_string>::iterator it_attr =
			attrs_.find(it_cache->first);
		if (it_attr == attrs_.end()) {
			if (it_cache->second.todo_ == TODO_SET) {
				attrs_.insert(std::make_pair(it_cache->first,
					it_cache->second));
			}
		} else if (it_cache->second.todo_ == TODO_SET) {
			// �����µ�����
			attrs_.insert(std::make_pair(it_cache->first,
				it_cache->second));
		} else if (it_cache->second.todo_ == TODO_DEL) {
			attrs_.erase(it_attr);
		} else {
			logger_warn("unknown todo(%d)",
				(int) it_cache->second.todo_);
		}
	}

	// �����������ݣ���Ϊ�ڲ��������Ѿ�������� attrs_ �У�
	// ����ֻ��Ҫ�� attrs_cache_ �ռ��������
	attrs_cache_.clear();

	// ���ô���ӿڣ��� memcached �����ƻ������������
	if (!this->set_attrs(attrs_)) {
		logger_error("set cache error, sid(%s)", sid_.c_str());
		attrs_clear(attrs_);  // ������Լ�������

		return false;
	}

	attrs_clear(attrs_);  // ������Լ�������

	if (!sid_saved_) {
		sid_saved_ = true;
	}
	return true;
}

bool session::set(const char* name, const char* value)
{
	return set(name, value, strlen(value));
}

bool session::set_delay(const char* name, const void* value, size_t len)
{
	session_string ss(len);
	ss.copy(value, len);
	ss.todo_ = TODO_SET;
	attrs_cache_.insert(std::make_pair(string(name), ss));

	dirty_ = true;
	return true;
}

bool session::set(const char* name, const void* value, size_t len)
{
	// ֱ�Ӳ������ cache ������������(���/�޸�) �����ֶ�

	// ���ô���ӿڣ����ԭ���� sid ����
	if (!this->get_attrs(attrs_)) {
		session_string ss(len);
		ss.copy(value, len);
		ss.todo_ = TODO_SET;
		attrs_cache_.insert(std::make_pair(string(name), ss));
	}
	// ������ڶ�Ӧ sid �����ݣ��������������ԭ��������
	else {
		if (!sid_saved_) {
			sid_saved_ = true;
		}

		// ����������Ѵ��ڣ�����Ҫ���ͷ�ԭ��������ֵ���������ֵ
		session_string ss(len);
		ss.copy(value, len);
		ss.todo_ = TODO_SET;
		attrs_cache_.insert(std::make_pair(string(name), ss));
	}

	// ���ô���ӿڣ��� memcached �����ƻ������������
	if (!this->set_attrs(attrs_)) {
		logger_error("set cache error, sid(%s)", sid_.c_str());
		attrs_clear(attrs_);  // ������Լ�������

		return false;
	}
	attrs_clear(attrs_);  // ������Լ�������

	if (!sid_saved_) {
		sid_saved_ = true;
	}
	return true;
}

const char* session::get(const char* name)
{
	const session_string* bf = get_buf(name);
	if (bf == NULL) {
		return "";
	}
	return bf->c_str();
}

const session_string* session::get_buf(const char* name)
{
	attrs_clear(attrs_);

	if (!get_attrs(attrs_)) {
		return NULL;
	}

	std::map<string, session_string>::const_iterator cit = attrs_.find(name);
	if (cit == attrs_.end()) {
		return NULL;
	}
	return &cit->second;
}

bool session::get_attrs(const std::vector<string>& names,
	std::vector<session_string>& values)
{
	attrs_clear(attrs_);

	if (!get_attrs(attrs_)) {
		return false;
	}

	for (std::vector<string>::const_iterator cit = names.begin();
		cit != names.end(); ++cit) {

		std::map<string, session_string>::const_iterator cit2
			= attrs_.find(*cit);
		if (cit2 != attrs_.end()) {
			values.push_back(cit2->second);
		} else {
			values.push_back("");
		}
	}

	return true;
}

bool session::set_ttl(time_t ttl, bool delay)
{
	if (ttl == ttl_) {
		return true;
	}

	// ������ӳ��޸ģ����������س�Ա���������ͳһ flush
	else if (delay) {
		ttl_   = ttl;
		dirty_ = true;
		return true;
	}

#if 0
	// ����� sid ��û���ں�� cache �ϴ洢��������ڶ����б�������һ��
	else if (!sid_saved_) {
		ttl_ = ttl;
		return true;
	}
#endif

	// �޸ĺ�� cache ����Ը� sid �� ttl
	else if (set_timeout(ttl)) {
		ttl_ = ttl;
		return true;
	} else {
		return false;
	}
}

bool session::del_delay(const char* name)
{
	std::map<string, session_string>::iterator it = attrs_cache_.find(name);
	if (it != attrs_cache_.end()) {
		it->second.todo_ = TODO_DEL;
	}
	dirty_ = true;
	return true;
}

bool session::del(const char* name)
{
	// ֱ�Ӳ������ cache ��������ɾ�������ֶ�

	if (!get_attrs(attrs_)) {
		return true;
	}

	std::map<string, session_string>::iterator it = attrs_.find(name);
	if (it == attrs_.end()) {
		return false;
	}

	// ��ɾ�����ͷŶ�Ӧ�Ķ���
	attrs_.erase(it);

	// ��� sid ���Ѿ�û�������ݣ���Ӧ�ý� sid ����� memcached ��ɾ��
	if (attrs_.empty()) {
		// �����麯����ɾ���� sid ��Ӧ�Ļ�������
		if (!this->remove()) {
			logger_error("del sid(%s) error", sid_.c_str());
			return false;
		}
		return true;
	}

	// �������ʣ�������

	if (!set_attrs(attrs_)) {
		logger_error("set cache error, sid(%s)", sid_.c_str());
		attrs_clear(attrs_);  // ������Լ�������

		return false;
	}

	attrs_clear(attrs_);  // ������Լ�������
	return true;
}

// ���� handlersocket �ı��뷽ʽ

void session::serialize(const std::map<string, session_string>& attrs,
	string& out)
{
	out.clear(); // �����������

	std::map<string, session_string>::const_iterator it = attrs.begin();
	if (it == attrs.end()) {
		return;
	}

	// ��ӵ�һ������
	const char ch = 1;
	escape(it->first.c_str(), it->first.length(), out);
	escape(&ch, 1, out);
	escape(it->second.c_str(), it->second.length(), out);
	++it;

	// ��Ӻ���������
	for (; it != attrs.end(); ++it) {
		// ����һ������������Ķ���Ҫ��ӷָ���
		out << '\t';
		escape(it->first.c_str(), it->first.length(), out);
		escape(&ch, 1, out);
		escape(it->second.c_str(), it->second.length(), out);
	}
}

// ���� handlersocket �Ľ��뷽ʽ

void session::deserialize(string& buf, std::map<string, session_string>& attrs)
{
	attrs_clear(attrs);  // ������ session ǰһ�β�ѯ״̬

	ACL_ARGV* tokens = acl_argv_split(buf.c_str(), "\t");
	ACL_ITER  iter;
	acl_foreach(iter, tokens) {
		char* ptr = (char*) iter.data;

		// �ظ�ʹ��ԭ�����ڴ�������Ϊ tokens ���Ѿ��洢���м�������
		buf.clear();
		if (!unescape(ptr, strlen(ptr), buf)) {
			logger_error("unescape error");
			continue;
		}
		ptr = buf.c_str();
		// ��Ϊ acl::string �϶��ܱ�֤���������ݵ�β���� \0����������
		// strchr ʱ�����뵣��Խ�����⣬�� std::string ������֤����
		char* p1 = strchr(ptr, 1);
		if (p1 == NULL || *(p1 + 1) == 0) {
			continue;
		}
		*p1++ = 0;
		//std::map<string, session_string>::iterator it = attrs.find(ptr);

		size_t len = buf.length() - (p1 - buf.c_str());
		session_string ss(len);
		ss.copy(p1, len);
		ss.todo_ = TODO_SET;
		attrs.insert(std::make_pair(string(ptr), ss));
	}

	acl_argv_free(tokens);
}

} // namespace acl

#endif // ACL_CLIENT_ONLY
