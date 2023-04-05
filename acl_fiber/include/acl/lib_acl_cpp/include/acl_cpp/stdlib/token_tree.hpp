#pragma once
#include "../acl_cpp_define.hpp"
#include "noncopyable.hpp"
#include "string.hpp"
#include <list>

struct ACL_TOKEN;
struct ACL_ITER;

namespace acl {

class token_tree;

/**
 * 256 ��ƥ�����еĽڵ����Ϊ��˽����
 */
class ACL_CPP_API token_node : public noncopyable
{
public:
	/**
	 * ��øýڵ��Ӧ�ļ�ֵ
	 * @return {const char*}
	 */
	const char* get_key(void) const;

	/**
	 * ��øýڵ����󶨵Ķ����ַ
	 * @return {void*}
	 */
	void* get_ctx(void) const;

	/**
	 * ��øýڵ�������ƥ��������
	 * @return {token_tree*}
	 */
	token_tree* get_tree(void) const
	{
		return tree_;
	}

	/**
	 * ��� C �汾�Ľڵ����
	 * @return {ACL_TOKEN*}
	 */
	ACL_TOKEN* get_token(void) const
	{
		return me_;
	}

private:
	friend class token_tree;	// ������ token_tree ����/�����������

	token_node(void);
	~token_node(void);

	void set_node(ACL_TOKEN* token, token_tree* tree);

private:
	ACL_TOKEN*  me_;
	token_tree* tree_;
	string      key_;
	bool        dirty_;
};

/**
 * 256 �������ƥ������㷨�����㷨����ͨ���Լ��ǳ��ߵ�����(�ȹ�ϣ���ܻ���)��
 * ͨ�����ַ���ӳ�䵽 256 �����Ͻ���ƥ�����
 */
class ACL_CPP_API token_tree : public noncopyable
{
public:
	token_tree(void);
	~token_tree(void);

	/**
	 * ���һ���µ���
	 * @param key {const char*} ��ֵ
	 * @param ctx {void*} �� key ���󶨵Ķ��󣬿���Ϊ��
	 * @return {bool} ����Ƿ�ɹ������� false ������ͬ key �Ѵ���
	 */
	bool insert(const char* key, void* ctx = NULL);

	/**
	 * ��ƥ������ɾ��ָ���� key ��
	 * @param key {const char*} ��ֵ
	 * @return {void*} �������ʱ�󶨵Ķ����ַ
	 */
	void* remove(const char* key);

	/**
	 * ���ݼ�ֵ��ȷ����ƥ��Ľڵ�
	 * @param key {const char*} ��ֵ
	 * @return {const token_node*} ���� NULL ��ʾδ�ҵ�ƥ����
	 */
	const token_node* find(const char* key);

	/**
	 * ���ַ������ƥ��ģʽ��ƥ���в����������ı��ַ�����ƥ��Ľڵ㣬ͬʱ
	 * �ƶ��ı��ַ�����ָ��λ��
	 * @param text {const char**} Ҫƥ����ҵ��ı��ַ�������ƥ������У���
	 *  ��ַָ��ᱻ�ƶ�����һλ��
	 * @param delimiters {const char*} �� NULL ʱָ���Ľ�ֹ���ַ���������
	 *  �ҹ�����ֻҪ�������ַ��ڸý�ֹ�ַ����У��򷵻ر��β��ҵĽ��
	 * @param delimiters_tab {const char*} �� NULL ʱָ���Ľ�ֹ���ַ����飬
	 *  �����ҹ�����ֻҪ�������ַ��ڸý�ֹ�ַ������У��򷵻ر��β��ҵĽ����������
	 *  ������ create_delimiters_tab �������� free_delimiters_tab �ͷ�
	 * @return {token_node*} ���� NULL ��ʾ���β���δ�ҵ�ƥ���ͨ�����
	 *  *text �Ƿ�Ϊ '\0' ��ʾ�Ƿ�ƥ�����Ŀ���ı��ַ���
	 *  ע���� delimiters �ǿ�ʱ����ʹ�� delimiters ��Ϊ��ֹ���������ټ��
	 *  delimiters_tab �Ƿ�ǿգ�����ǿ���ʹ������Ϊ��ֹ��
	 */
	const token_node* search(const char** text, const char* delimiters = NULL,
		const char* delimiters_tab = NULL);

	/**
	 * ������ֹ������
	 * @param delimiters {const char*} ��ֹ���ַ���
	 * @return {char*} ���ݽ�ֹ���ַ��������Ľ�ֹ������
	 */
	static char* create_delimiters_tab(const char* delimiters);

	/**
	 * �ͷ��� create_delimiters_tab �����Ľ�ֹ������
	 * @param delimiters_tab {char*}
	 */
	static void free_delimiters_tab(char* delimiters_tab);

	/**
	 * ���� 256 ƥ����ʱ���ȵ��ñ�������õ�һ���ڵ����
	 * @return {token_node*}
	 */
	const token_node* first_node(void);

	/**
	 * ���� 256 ƥ����ʱ���ȵ��ñ����������һ���ڵ����
	 * @return {token_node*}
	 */
	const token_node* next_node(void);

	/**
	 * ��� C �汾�� 256 ��������
	 * @return {ACL_TOKEN*}
	 */
	ACL_TOKEN* get_tree(void) const
	{
		return tree_;
	}

private:
	ACL_TOKEN* tree_;
	ACL_ITER*  iter_;
	token_node node_;
};

} // namespace acl
