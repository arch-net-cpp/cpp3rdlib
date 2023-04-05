#pragma once
#include "noncopyable.hpp"

namespace acl {

class bitmap : public noncopyable
{
public:
	/**
	 * ���캯��
	 * @param buf {const void*} Դ�����ڴ�λӳ����
	 * @param len {size_} buf λӳ���ڴ����б���λ������
	 */
	bitmap(const void* buf, size_t len);

	/**
	 * ���캯��
	 * @param len {size_t} ������ɵ�λӳ�������
	 */
	bitmap(size_t len);

	~bitmap(void);

	/**
	 * ��������ֵӳ����λ������
	 * @param n {size_t}
	 * @return {bool} ���� true ��ʾ��ӳɹ��������ʾ��ֵԽ����Ѿ�����
	 */
	bool bit_set(size_t n);

	/**
	 * �ж����������Ƿ��Ѿ���������λӳ����
	 * @param n {size_t}
	 * @return {bool} �ж�ָ����ֵ�Ƿ������λӳ�伯����
	 */
	bool bit_isset(size_t n) const;

	/**
	 * ��ָ����ֵ��λ������ȥ��
	 * @param n {size_t}
	 * @return {bool} ���� false ��ʾ��ֵԽ��򲻴�����λ������
	 */
	bool bit_unset(size_t n);

	/**
	 * ��bitmap��Ϣ������buf��
	 * @param buf {void*}��ſ������
	 * @param len {size_t} buf����󳤶�
	 * @return {size_t} ���سɹ��������ڴ泤�ȣ����� 0 ��ʾ buf ̫С
	 */
	size_t tobuf(void* buf, size_t len) const;

	/**
	 * ��buf�����õ�ǰbitmap��Ϣ
	 * @param buf {const void*} Ҫ����bitmap��Ϣ
	 * @param len {size_t} buf�ĳ���
	 * @return true �ɹ���falseʧ��
	 */
	bool frombuf(const void* buf, size_t len);

	/**
	 * ���õ�ǰ��bitmapΪ 0
	 */
	void reset(void);

	/**
	 * ��ȡ��ǰλӳ��洢�ռ���Դ洢��λ�ĸ���
	 * @return {size_t}
	 */
	size_t size(void) const;

	/**
	 * ����ڲ��洢�ռ��С���ֽڣ�
	 */
	size_t space(void) const;

	/**
	 * ��ȡ��ǰ�Ѿ����õĸ���
	 * @return {size_t}
	 */
	size_t count(void) const;

	/**
	 * ��ǰbitmap�Ƿ�����
	 * @return {bool}
	 */
	bool full(void) const;

public:
	const unsigned char* get_bmp(void) const {
		return bmp_;
	}

	unsigned char* get_bmp(void) {
		return bmp_;
	}

private:
	unsigned char *bmp_;
	size_t size_;
	size_t count_;

	//����ͳ��count����
	void recount(void);
};

} // namespace acl
