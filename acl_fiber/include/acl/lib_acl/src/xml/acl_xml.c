#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include <string.h>
#include <stdio.h>
#include "stdlib/acl_msg.h"
#include "stdlib/acl_mystring.h"
#include "stdlib/acl_define.h"
#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_dbuf_pool.h"
#include "xml/acl_xml.h"

#endif

#define	LEN	ACL_VSTRING_LEN
#define	STR	acl_vstring_str

ACL_XML_ATTR *acl_xml_attr_alloc(ACL_XML_NODE *node)
{
	ACL_XML_ATTR *attr = (ACL_XML_ATTR*)
		acl_dbuf_pool_calloc(node->xml->dbuf, sizeof(ACL_XML_ATTR));

	attr->node = node;
	attr->name = acl_vstring_dbuf_alloc(node->xml->dbuf, 16);
	attr->value = acl_vstring_dbuf_alloc(node->xml->dbuf, 16);
	attr->quote = 0;
	attr->backslash = 0;

	acl_array_append(node->attr_list, attr);
	node->xml->attr_cnt++;

	return attr;
}

static ACL_XML_NODE *node_iter_head(ACL_ITER *it, ACL_XML_NODE *node)
{
	ACL_RING *ring_ptr;

	it->dlen = -1;
	it->key = NULL;
	it->klen = -1;

	it->i = 0;
	it->size = node->children.len;

	if ((ring_ptr = acl_ring_succ(&node->children)) == &node->children) {
		it->ptr = it->data = NULL;
		return NULL;
	}

	it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
	it->data = it->ptr;

	return it->ptr;
}

static ACL_XML_NODE *node_iter_next(ACL_ITER *it, ACL_XML_NODE *node)
{
	ACL_RING *ring_ptr;
	struct ACL_XML_NODE *child = (struct ACL_XML_NODE*) it->data;

	if ((ring_ptr = acl_ring_succ(&child->node)) == &node->children) {
		it->ptr = it->data = NULL;
		return NULL;
	}

	it->i++;
	it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
	it->data = it->ptr;

	return it->ptr;
}

static ACL_XML_NODE *node_iter_tail(ACL_ITER *it, ACL_XML_NODE *node)
{
	ACL_RING *ring_ptr;

	it->dlen = -1;
	it->key = NULL;
	it->klen = -1;

	it->i = 0;
	it->size = node->children.len;

	if ((ring_ptr = acl_ring_pred(&node->children)) == &node->children) {
		it->ptr = it->data = NULL;
		return NULL;
	}

	it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
	it->data = it->ptr;

	return it->ptr;
}

static ACL_XML_NODE *node_iter_prev(ACL_ITER *it, ACL_XML_NODE *node)
{
	ACL_RING *ring_ptr;
	struct ACL_XML_NODE *child = (struct ACL_XML_NODE*) it->data;

	if ((ring_ptr = acl_ring_pred(&child->node)) == &node->children) {
		it->ptr = it->data = NULL;
		return NULL;
	}

	it->i++;
	it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
	it->data = it->ptr;

	return it->ptr;
}

ACL_XML_NODE *acl_xml_node_alloc(ACL_XML *xml)
{
	ACL_XML_NODE *node = (ACL_XML_NODE*)
		acl_dbuf_pool_calloc(xml->dbuf, sizeof(ACL_XML_NODE));

	acl_ring_init(&node->children);
	acl_ring_init(&node->node);

	node->xml = xml;
	node->status = ACL_XML_S_NXT;
	node->ltag = acl_vstring_dbuf_alloc(xml->dbuf, 16);
	node->rtag = acl_vstring_dbuf_alloc(xml->dbuf, 16);
	node->text = acl_vstring_dbuf_alloc(xml->dbuf, 16);
	node->attr_list = acl_array_dbuf_create(100, xml->dbuf);

	node->iter_head = node_iter_head;
	node->iter_next = node_iter_next;
	node->iter_tail = node_iter_tail;
	node->iter_prev = node_iter_prev;

	return node;
}

int acl_xml_node_delete(ACL_XML_NODE *node)
{
	ACL_RING *next;
	ACL_XML_NODE *node_next;
	int   n = 1;

	while ((next = acl_ring_pop_head(&node->children)) != NULL) {
		node_next = acl_ring_to_appl(next, ACL_XML_NODE, node);
		n += acl_xml_node_delete(node_next);
	}

	if (node->id != NULL)
		acl_htable_delete(node->xml->id_table, STR(node->id), NULL);

	if (node->attr_list != NULL) {
		int k = acl_array_size(node->attr_list);
		if (node->xml->attr_cnt >= k)
			node->xml->attr_cnt -= k;
	}

	acl_ring_detach(&node->node);
	node->xml->node_cnt--;

	return n;
}

void acl_xml_node_append(ACL_XML_NODE *node1, ACL_XML_NODE *node2)
{
	acl_ring_append(&node1->node, &node2->node);
	node2->parent = node1->parent;
	node1->xml->node_cnt++;
}

void acl_xml_node_add_child(ACL_XML_NODE *parent, ACL_XML_NODE *child)
{
	acl_ring_prepend(&parent->children, &child->node);
	child->parent = parent;
	parent->xml->node_cnt++;
}

ACL_XML_NODE *acl_xml_node_parent(ACL_XML_NODE *node)
{
	return node->parent;
}

ACL_XML_NODE *acl_xml_node_next(ACL_XML_NODE *node)
{
	ACL_RING *ring_ptr = acl_ring_succ(&node->node);
	ACL_XML_NODE *parent;

	if (ring_ptr == &node->node)
		return NULL;

	parent = node->parent;
	acl_assert(parent != NULL);
	if (ring_ptr == &parent->children)
		return NULL;

	return acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
}

ACL_XML_NODE *acl_xml_node_prev(ACL_XML_NODE *node)
{
	ACL_RING *ring_ptr = acl_ring_pred(&node->node);
	ACL_XML_NODE *parent;

	if (ring_ptr == &node->node)
		return NULL;
	parent = node->parent;
	acl_assert(parent != NULL);
	if (ring_ptr == &parent->children)
		return NULL;

	return acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
}

static ACL_XML_NODE *xml_iter_head(ACL_ITER *it, ACL_XML *xml)
{
	ACL_RING *ring_ptr;

	it->dlen = -1;
	it->key = NULL;
	it->klen = -1;

	it->i = 0;
	it->size = xml->node_cnt;

	ring_ptr = acl_ring_succ(&xml->root->children);
	if (ring_ptr== &xml->root->children) {
		it->ptr = it->data = NULL;
		return NULL;
	}

	it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
	it->data = it->ptr;

	return it->ptr;
}

static ACL_XML_NODE *xml_iter_next(ACL_ITER *it, ACL_XML *xml)
{
	ACL_RING *ring_ptr;
	struct ACL_XML_NODE *node, *parent;

	node = (struct ACL_XML_NODE*) it->data;

	/* �ȱ�����ǰ�ڵ���ӽڵ� */

	ring_ptr = acl_ring_succ(&node->children);
	if (ring_ptr != &node->children) {
		it->i++;
		it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
		it->data = it->ptr;
		return it->ptr;
	}

	/* ��ǰ�ڵ���ӽڵ������ϣ��ٱ�����ǰ�ڵ���ֵܽڵ� */

	parent = acl_xml_node_parent(node);
	ring_ptr = acl_ring_succ(&node->node);
	if (ring_ptr != &parent->children) {
		it->i++;
		it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
		it->data = it->ptr;
		return it->ptr;
	}

	/* ��ǰ�ڵ���ֵܽڵ������ϣ���������ǰ�ڵ�ĸ��ڵ���ֵܽڵ� */

	do {
		if (parent == xml->root)
			break;

		ring_ptr = acl_ring_succ(&parent->node);
		parent = acl_xml_node_parent(parent);
		if (parent == NULL)
			acl_msg_fatal("%s(%d): parent null",
				__FILE__, __LINE__);

		if (ring_ptr != &parent->children) {
			it->i++;
			it->ptr = acl_ring_to_appl(ring_ptr,
					ACL_XML_NODE, node);
			it->data = it->ptr;
			return it->ptr;
		}
	} while (ring_ptr != &xml->root->children);

	/* ���������нڵ� */

	it->ptr = it->data = NULL;
	return NULL;
}

static ACL_XML_NODE *xml_iter_tail(ACL_ITER *it, ACL_XML *xml)
{
	ACL_RING *ring_ptr;

	it->dlen = -1;
	it->key = NULL;
	it->klen = -1;

	it->i = 0;
	it->size = xml->node_cnt;

	ring_ptr = acl_ring_pred(&xml->root->children);
	if (ring_ptr== &xml->root->children) {
		it->ptr = it->data = NULL;
		return NULL;
	}
	it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
	it->data = it->ptr;
	return it->ptr;
}

static ACL_XML_NODE *xml_iter_prev(ACL_ITER *it, ACL_XML *xml)
{
	ACL_RING *ring_ptr;
	struct ACL_XML_NODE *node, *parent;

	node = (struct ACL_XML_NODE*) it->data;

	/* �ȱ�����ǰ�ڵ���ӽڵ� */

	ring_ptr = acl_ring_pred(&node->children);
	if (ring_ptr != &node->children) {
		it->i++;
		it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
		it->data = it->ptr;
		return it->ptr;
	}

	/* ��ǰ�ڵ���ӽڵ������ϣ��ٱ�����ǰ�ڵ���ֵܽڵ� */

	parent = acl_xml_node_parent(node);
	ring_ptr = acl_ring_pred(&node->node);
	if (ring_ptr != &parent->children) {
		it->i++;
		it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
		it->data = it->ptr;
		return it->ptr;
	}

	/* ��ǰ�ڵ���ֵܽڵ������ϣ���������ǰ�ڵ�ĸ��ڵ���ֵܽڵ� */

	do {
		if (parent == xml->root)
			break;
		ring_ptr = acl_ring_pred(&parent->node);
		parent = acl_xml_node_parent(parent);
		if (parent == NULL)
			acl_msg_fatal("%s(%d): parent null",
				__FILE__, __LINE__);

		if (ring_ptr != &parent->children) {
			it->i++;
			it->ptr = acl_ring_to_appl(ring_ptr,
					ACL_XML_NODE, node);
			it->data = it->ptr;
			return it->ptr;
		}
	} while (ring_ptr != &xml->root->children);

	/* ���������нڵ� */

	it->ptr = it->data = NULL;
	return NULL;
}

void acl_xml_multi_root(ACL_XML *xml, int on)
{
	if (on)
		xml->flag |= ACL_XML_FLAG_MULTI_ROOT;
	else
		xml->flag &= ~ACL_XML_FLAG_MULTI_ROOT;
}

void acl_xml_slash(ACL_XML *xml, int ignore)
{
	if (ignore)
		xml->flag |= ACL_XML_FLAG_IGNORE_SLASH;
	else
		xml->flag &=~ACL_XML_FLAG_IGNORE_SLASH;
}

void acl_xml_decode_enable(ACL_XML *xml, int on)
{
	if (on) {
		if (xml->decode_buf == NULL)
			xml->decode_buf = acl_vstring_alloc(256);
		else
			ACL_VSTRING_RESET(xml->decode_buf);
		xml->flag |= ACL_XML_FLAG_XML_DECODE;
	} else {
		if (xml->decode_buf != NULL) {
			acl_vstring_free(xml->decode_buf);
			xml->decode_buf = NULL;
		}
		xml->flag &= ~ACL_XML_FLAG_XML_DECODE;
	}
}

void acl_xml_encode_enable(ACL_XML *xml, int on)
{
	if (on)
		xml->flag |= ACL_XML_FLAG_XML_ENCODE;
	else
		xml->flag &=~ACL_XML_FLAG_XML_ENCODE;
}

ACL_XML *acl_xml_alloc()
{
	return acl_xml_dbuf_alloc(NULL);
}

ACL_XML *acl_xml_dbuf_alloc(ACL_DBUF_POOL *dbuf)
{
	ACL_XML *xml;

	if (dbuf == NULL) {
		dbuf = acl_dbuf_pool_create(8192);
		xml = (ACL_XML*) acl_dbuf_pool_calloc(dbuf, sizeof(ACL_XML));
		xml->dbuf_inner = dbuf;
	} else {
		xml = (ACL_XML*) acl_dbuf_pool_calloc(dbuf, sizeof(ACL_XML));
		xml->dbuf_inner = NULL;
	}

	xml->dbuf       = dbuf;
	xml->dbuf_keep  = sizeof(ACL_XML);
	xml->space      = 0;
	xml->flag       = ACL_XML_FLAG_MULTI_ROOT |
			  ACL_XML_FLAG_XML_DECODE |
			  ACL_XML_FLAG_XML_ENCODE;
	xml->decode_buf = acl_vstring_alloc(256);
	xml->id_table   = acl_htable_create(100, 0);
	xml->root       = acl_xml_node_alloc(xml);
	xml->node_cnt   = 1;
	xml->attr_cnt   = 0;

	xml->iter_head  = xml_iter_head;
	xml->iter_next  = xml_iter_next;
	xml->iter_tail  = xml_iter_tail;
	xml->iter_prev  = xml_iter_prev;

	return xml;
}

size_t acl_xml_space(ACL_XML *xml)
{
	return xml->space;
}

void acl_xml_space_clear(ACL_XML *xml)
{
	xml->space = 0;
}

int acl_xml_free(ACL_XML *xml)
{
	int  node_cnt = xml->node_cnt;

	acl_htable_free(xml->id_table, NULL);

	if (xml->decode_buf)
		acl_vstring_free(xml->decode_buf);

	if (xml->dbuf_inner != NULL)
		acl_dbuf_pool_destroy(xml->dbuf_inner);

	return node_cnt - 1;
}

void acl_xml_reset(ACL_XML *xml)
{
	acl_htable_reset(xml->id_table, NULL);

	if (xml->dbuf_inner != NULL)
		acl_dbuf_pool_reset(xml->dbuf_inner, xml->dbuf_keep);

	xml->root      = acl_xml_node_alloc(xml);
	xml->node_cnt  = 1;
	xml->root_cnt  = 0;
	xml->attr_cnt  = 0;
	xml->curr_node = NULL;
	xml->space     = 0;
}

void acl_xml_foreach_init(ACL_XML *xml, ACL_XML_NODE *node)
{
	xml->root = node;
	xml->iter_head = xml_iter_head;
	xml->iter_next = xml_iter_next;
	xml->iter_tail = xml_iter_tail;
	xml->iter_prev = xml_iter_prev;
}

int acl_xml_is_closure(ACL_XML *xml)
{
	ACL_RING *ring_ptr;
	ACL_XML_NODE *node;

	/* ��� xml->root �ڵ�����һ��һ���ӽڵ� */
	ring_ptr = acl_ring_succ(&xml->root->children);

	if (ring_ptr == &xml->root->children) {
		/* ˵��û����ʵ�ӽڵ� */
		return 0;
	}

	node = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);

	if ((node->flag & ACL_XML_F_SELF_CL)) {
		/* ˵���ýڵ����ԱպϽڵ� */
		return 1;
	}

	if (node->status == ACL_XML_S_RGT) {
		return 1;
	}

	/* ˵�����һ��һ���ӽڵ㻹δ������� */
	return 0;
}

int acl_xml_is_complete(ACL_XML *xml, const char *tag)
{
	ACL_XML_NODE *last_node = NULL;
	ACL_ITER iter;

	if (!(xml->flag & ACL_XML_FLAG_MULTI_ROOT) && xml->root_cnt > 0)
		return 1;

	acl_foreach_reverse(iter, xml->root) {
		ACL_XML_NODE *node = (ACL_XML_NODE*) iter.data;
		if (!(node->flag & ACL_XML_F_META)) {
			last_node = node;
			break;
		}
	}

	if (last_node == NULL)
		/* ˵��û����ʵ�ӽڵ� */
		return 0;

	if ((last_node->flag & ACL_XML_F_SELF_CL))
		/* ˵���ýڵ����ԱպϽڵ� */
		return 1;

	if (last_node->status != ACL_XML_S_RGT)
		/* ˵�����һ��һ���ӽڵ㻹δ������� */
		return 0;

	if (strcasecmp(STR(last_node->rtag), tag) == 0)
		return 1;

	/* ˵�� xml �е����һ���ڵ���������ǩ��ƥ�� */
	return 0;
}
