#pragma once
#include "../acl_cpp_define.hpp"
#include <list>
#include <vector>
#include "noncopyable.hpp"

namespace acl
{

class diff_object;

/**
 * ���������ϵĲ�Ĺ�����
 */
class diff_manager : public noncopyable
{
public:
	diff_manager(long long range_from = -1, long long range_to = -1);
	~diff_manager(void);

	/**
	 * ����ڲ��������ڴ�ض���
	 * @return {dbuf_guard&} �����ڽ��ڴ�ض���ʹ�ø��ڴ�ش�����
	 *  ��������� diff_manager ��������ǰ���٣���Ϊ diff_manager ����ʱ
	 *  ���ڽ��ڴ�ػ��Զ�����
	 */
	dbuf_guard& get_dbuf(void);

	/**
	 * �Ƚ��������ϵĲ���Ӷ�����������������Ķ��󼯺ϡ�ɾ���Ķ��󼯺�
	 * �Լ��仯�Ķ��󼯺�
	 * @param curr_objs {const std::vector<diff_object*>&} ��ǰ����ļ���
	 * @param old_olds {const std::list<diff_object*>&} ��Ԫ�ؼ��ϣ��ڲ�
	 *  �øü������ɹ�ϣ��ʹ��ǰ������þɼ��Ͻ��в��컯�Ƚ�
	 */
	void diff_changes(const std::vector<diff_object*>& curr_objs,
		const std::vector<diff_object*>& old_olds);

	/**
	 * ��������Ķ��󼯺�
	 * @return {std::vector<diff_object*>&}
	 */
	const std::vector<diff_object*>& get_new(void) const
	{
		return objs_new_;
	}

	/**
	 * �� diff_changes ���в��컯�Ƚϳɹ��󣬱��������ڷ�������ھɼ��ϣ�
	 * �ڵ�ǰ�����в����ڵģ�����ɾ���ģ�Ԫ�ؼ���
	 * @return {std::vector<diff_object*>&}
	 */
	const std::vector<diff_object*>& get_deleted(void) const
	{
		return objs_del_;
	}

	/**
	 * �� diff_changes ���в��컯�Ƚϳɹ��󣬱��������ڷ�������ھɼ��ϣ�
	 * �ڵ�ǰ�����ж���ֵ�����仯�ı仯���϶���
	 * @return {std::vector<std::pair<diff_object*, diff_object*> >&}
	 *  ���ز����仯�Ķ���ļ��ϣ����� pair �е� first Ϊ�¶���second
	 *  Ϊ�ɶ���
	 * @sample
	 *  const std::vector<std::pair<diff_object*, diff_object*> >&
	 *  	results = manager.get_updated();
	 *  std::vector<std::pair<diff_object*, diff_object*> >::const_iterator
	 *  	cit = results.begin();
	 *  for (; cit != results.end(); ++cit)
	 *  	printf(">> key: %s, curr value:%s, old value: %s\r\n",
	 *  		(*cit).first->get_key(),
	 *  		(*cit).first->get_val(),
	 *  		(*cit).second->get_val());
	 */
	const std::vector<std::pair<diff_object*, diff_object*> >&
		get_updated(void) const
	{
		return objs_upd_;
	}

	/**
	 * �� diff_manger ���в��컪�Ƚϳɹ��󣬱��������ڷ�����ͬ����ļ���
	 * @return {std::vector<diff_object*>&}
	 */
	const std::vector<diff_object*>& get_same(void) const
	{
		return objs_equ_;
	}

	/**
	 * ��������Ĳ���ָ�����䷶Χ�ڵĶ��󼯺�
	 * @return {const std::vector<diff_object*>&}
	 */
	const std::vector<diff_object*>& get_extra_added(void) const
	{
		return objs_new_extra_;
	}

	/**
	 * ���ɾ���Ĳ���ָ�����䷶Χ�ڵĶ��󼯺�
	 * @return {const std::vector<diff_object*>&}
	 */
	const std::vector<diff_object*>& get_extra_deleted(void) const
	{
		return objs_del_extra_;
	}

	/**
	 * ����޸ĵĲ���ָ�����䷶Χ�ڵĶ��󼯺�
	 * @return {const std::vector<diff_object*>&}
	 */
	const std::vector<std::pair<diff_object*, diff_object*> >&
		get_extra_updated(void) const
	{
		return objs_upd_extra_;
	}

	/**
	 * ���ظ�ʹ�ñ� diff_manager ���в��컯�Ƚ�ʱ����Ҫ���ñ����������
	 * ��һ�αȽϹ����в�������ʱ�ڴ漰����
	 */
	void reset(void);

private:
	dbuf_guard dbuf_;
	long long  range_from_;
	long long  range_to_;

	// ��ͬ�Ķ��󼯺�
	std::vector<diff_object*> objs_equ_;

	// �仯�Ķ��󼯺�

	// �����Ķ��󼯺�
	std::vector<diff_object*> objs_new_;
	// ɾ���Ķ��󼯺�
	std::vector<diff_object*> objs_del_;
	// �޸ĵĶ��󼯺�
	std::vector<std::pair<diff_object*, diff_object*> > objs_upd_;

	// ����Ķ��󼯺�

	// �����Ķ�����󼯺�
	std::vector<diff_object*> objs_new_extra_;
	// ɾ���Ķ�����󼯺�
	std::vector<diff_object*> objs_del_extra_;
	// �޸ĵĶ�����󼯺�
	std::vector<std::pair<diff_object*, diff_object*> > objs_upd_extra_;
};

} // namespace acl
