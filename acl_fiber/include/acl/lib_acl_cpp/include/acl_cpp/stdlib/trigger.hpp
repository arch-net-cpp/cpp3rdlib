#pragma once
#include "../acl_cpp_define.hpp"
#include <map>
#include <vector>
#include "noncopyable.hpp"
#include "mbox.hpp"
#include "util.hpp"
#include "thread.hpp"
#include "thread_mutex.hpp"

namespace acl {

/**
 * ������ͬʱ��صĶ�ʱ����ļ���
 */
template <typename T>
class trigger_item : public noncopyable
{
public:
	typedef std::map<long long, trigger_item<T>*> trigger_items_t;

	trigger_item(trigger_items_t& items) : items_(items) {}
	~trigger_item(void) {}

	/**
	 * ���һ����ʱ����
	 * @pararm o {T*}
	 */
	void add(T* o)
	{
		objs_.push_back(o);
	}

	/**
	 * ɾ��һ��������ͬʱ��صĶ�ʱ����
	 * @pararm o {T*}
	 * @return {int} ����ֵ >= 0 ��ʾʣ��ľ�����ͬʱ��صĶ�ʱ��������
	 *  ���� -1 ��ʾ�ö�ʱ���񲻴���
	 */
	int del(T* o)
	{
		for (typename std::vector<T*>::iterator it = objs_.begin();
			it != objs_.end(); ++it) {

			if (*it == o) {
				objs_.erase(it);
				return (int) objs_.size();
			}
		}
		return -1;
	}

	/**
	 * ��ȡ������ͬʱ��ص����ж�ʱ���񼯺�
	 * @return {std::vector<T*>&}
	 */
	std::vector<T*>& get_objs(void)
	{
		return objs_;
	}

private:
	std::vector<T*> objs_;
	trigger_items_t& items_;
};

/**
 * ��ʱ���񴥷���������ͨ��������Ӷ�ʱ���񣬸���Ὣ���ڵ�������д���
 * ÿ����ʱ������� T ��Ҫʵ�����·������Ա����ɸô���������
 *
 * bool on_trigger(void);		// ��ʱʱ�䵽��ʱ�Ļص�����������ֵ��ʾ
 * 					// �Ƿ���Ҫ�ٴδ����ö�ʱ����
 * int get_ttl(void) const;		// ��ʱ���񵽴�ʱ��ʱ���������룩
 * void set_key(long long key);		// ������������ö�ʱ��������ļ�
 * long long get_key(void) const;	// ����� set_key ���õļ�
 *
 * ��һ�� T ��ʵ�����������£�
 * class mytask
 * {
 * public:
 *     mytask(void) {}
 *     ~mytask(void) {}
 *
 *     // @override
 *     bool on_trigger(void)
 *     {
 *         return true;
 *     }
 *
 *     // @override
 *     int get_ttl(void) const
 *     {
 *         return 1000;
 *     }
 *
 *     // @override
 *     void set_key(long long key)
 *     {
 *         key_ = key;
 *     }
 *
 *     // @override
 *     long long get_key(void) const
 *     {
 *         return key_;
 *     }
 *
 * private:
 *     long long key_;
 * };
 */
template <typename T>
class timer_trigger : public noncopyable
{
public:
	typedef std::map<long long, trigger_item<T>*> trigger_items_t;
	typedef typename trigger_items_t::iterator trigger_iter_t;

	timer_trigger(void) {}
	~timer_trigger(void) {}

	/**
	 * ���һ���������
	 * @pararm o {T*}
	 */
	void add(T* o)
	{
		int ttl       = o->get_ttl();
		long long key = get_curr_stamp() + ttl;

		trigger_item<T>* item;
		trigger_iter_t it = items_.find(key);
		if (it == items_.end()) {
			item        = new trigger_item<T>(items_);
			items_[key] = item;
		} else
			item = it->second;
		item->add(o);
		o->set_key(key);
	}

	/**
	 * ɾ��һ����������ڲ����� o->get_key() ������ø��������ļ�
	 * @pararm o {T*} ָ������ɾ�����������
	 * @return {int} >= 0 ʱ��ʾʣ����������-1 ��ʾ��������󲻴���
	 */
	int del(T* o)
	{
		long long key     = o->get_key();
		trigger_iter_t it = items_.find(key);

		if (it == items_.end())
			return -1;
		if (it->second->del(o) == 0) {
			delete it->second;
			items_.erase(it);
		}
		return (int) items_.size();
	}

	/**
	 * �������е��ڵĶ�ʱ����
	 * @return {long long} ������һ�����������Ķ�ʱ�����ʱ��أ����� -1
	 *  ��ʾû�ж�ʱ����
	 */
	long long trigger(void)
	{
		long long key = get_curr_stamp();
		std::vector<trigger_item<T>*> items;
		trigger_iter_t iter;
		for (iter = items_.begin(); iter != items_.end();) {
			if (iter->first > key)
				break;

			items.push_back(iter->second);
			items_.erase(iter++);
		}

		for (typename std::vector<trigger_item<T>*>::iterator
			it = items.begin(); it != items.end(); ++it) {

			trigger(*it);
			delete *it;
		}

		iter = items_.begin();
		if (iter == items_.end())
			return -1;
		return iter->first;
	}

private:
	trigger_items_t items_;

	/**
	 * ����������ͬ��ʱʱ��ص���������
	 * @pararm item {trigger_item<T>*}
	 */
	void trigger(trigger_item<T>* item)
	{
		std::vector<T*>& objs = item->get_objs();
		for (typename std::vector<T*>::iterator it = objs.begin();
			it != objs.end(); ++it) {

			if (!(*it)->on_trigger())
				continue;

			int ttl       = (*it)->get_ttl();
			long long key = get_curr_stamp() + ttl;

			trigger_iter_t iter = items_.find(key);
			if (iter == items_.end()) {
				item        = new trigger_item<T>(items_);
				items_[key] = item;
			} else
				item = iter->second;

			item->add(*it);
			(*it)->set_key(key);
		}
	}
};

/**
 * ��ʱ�������̣߳����̴߳� mbox �л�ö�ʱ���񣬲����붨ʱ���񴥷����У�Ȼ��
 * ��ʱ�Ӵ���������ȡ���ڵ����񲢴���
 */
template <typename T>
class thread_trigger : public thread
{
public:
	thread_trigger(void)
	: delay_(100)  // ��ʼ��ʱ�ĳ�ʱ�ȴ�ʱ�䣨���룩
	, stop_(false) // �Ƿ�ֹͣ�߳�
	{
	}

	virtual ~thread_trigger(void) {}

	/**
	 * ���һ����ʱ�������
	 * @pararm o {T*}
	 */
	void add(T* o)
	{
		mbox_.push(o);
	}

	/**
	 * ���Ҫɾ���Ķ�ʱ���������ʱ�����У�Ȼ��Ӷ�ʱ����ɾ��֮
	 * @pararm o {T*}
	 */
	void del(T* o)
	{
		lock_.lock();
		timer_del_.push_back(o);
		lock_.unlock();
	}

	timer_trigger<T>& get_trigger(void)
	{
		return timer_;
	}

private:
	// @override
	void* run(void)
	{
		while (!stop_) {
			T* o = mbox_.pop(delay_);
			if (o)
				timer_.add(o);

			long long next = timer_.trigger();
			long long curr = get_curr_stamp();
			if (next == -1)
				delay_ = 100;
			else {
				delay_ = next - curr;
				if (delay_ < 0)
					delay_ = 1;
			}

			lock_.lock();
			typename std::vector<T*>::iterator it;
			for (it = timer_del_.begin();
				it != timer_del_.end(); ++it) {

				timer_.del(*it);
			}
			timer_del_.clear();
			lock_.unlock();
		}
		return NULL;
	}

private:
	long long delay_;
	bool stop_;

	timer_trigger<T> timer_;
	mbox<T> mbox_;

	std::vector<T*> timer_del_;
	thread_mutex lock_;
};

} // namespace acl
