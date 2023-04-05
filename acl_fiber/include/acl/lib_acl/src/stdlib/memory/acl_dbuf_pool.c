#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"
#ifdef	ACL_UNIX
#include <unistd.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stdlib/acl_sys_patch.h"
#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_msg.h"
#include "stdlib/acl_dbuf_pool.h"

#endif

typedef struct ACL_DBUF {
        struct ACL_DBUF *next;
	short  used;
	short  keep;
	size_t size;
        char  *addr;
        char   buf[1];
} ACL_DBUF;

struct ACL_DBUF_POOL {
        size_t block_size;
	size_t off;
	size_t huge;
	size_t count;
        ACL_DBUF *head;
	char  buf[1];
};

ACL_DBUF_POOL *acl_dbuf_pool_create(size_t block_size)
{
	ACL_DBUF_POOL *pool;
	size_t size;
	int    page_size;

#ifdef ACL_UNIX
	page_size = getpagesize();
#elif defined(ACL_WINDOWS)
	SYSTEM_INFO info;

	memset(&info, 0, sizeof(SYSTEM_INFO));
	GetSystemInfo(&info);
	page_size = info.dwPageSize;
	if (page_size <= 0)
		page_size = 4096;
#else
	page_size = 4096;
#endif

	size = (block_size / (size_t) page_size) * (size_t) page_size;
	if (size < (size_t) page_size)
		size = page_size;

	/* xxx: Ϊ�˾�����֤�ڵ��� acl_mymalloc �����ڴ�ʱΪ�ڴ�ҳ����������
	 * ��Ҫ��ȥ sizeof(ACL_DBUF) �� 16 �ֽڣ����� 16 �ֽ��� acl_mymalloc
	 * �ڲ���ÿ���ڴ�������ӵĿ���ͷ���� acl_mymalloc �ڲ� 16 �ֽ�Ϊ��
	 * offsetof(MBLOCK, u.payload[0])
	 */
	size -= 16 + sizeof(ACL_DBUF);

#ifdef	USE_VALLOC
	pool = (ACL_DBUF_POOL*) valloc(sizeof(struct ACL_DBUF_POOL)
			+ sizeof(ACL_DBUF) + size);
#else
	pool = (ACL_DBUF_POOL*) acl_mymalloc(sizeof(struct ACL_DBUF_POOL)
			+ sizeof(ACL_DBUF) + size);
#endif

	pool->block_size = size;
	pool->off        = 0;
	pool->huge       = 0;
	pool->head       = (ACL_DBUF*) pool->buf;
	pool->head->next = NULL;
	pool->head->keep = 1;
	pool->head->used = 0;
	pool->head->size = size;
	pool->head->addr = pool->head->buf;
	pool->count      = 1;

	return pool;
}

void acl_dbuf_pool_destroy(ACL_DBUF_POOL *pool)
{
	ACL_DBUF *iter = pool->head, *tmp;

	while (iter) {
		tmp = iter;
		iter = iter->next;
		if ((char*) tmp == pool->buf)
			break;
		if (tmp->size > pool->block_size)
			pool->huge--;
#ifdef	USE_VALLOC
		free(tmp);
#else
		acl_myfree(tmp);
#endif
	}

#ifdef	USE_VALLOC
	free(pool);
#else
	acl_myfree(pool);
#endif
}

int acl_dbuf_pool_reset(ACL_DBUF_POOL *pool, size_t off)
{
	size_t n;
	ACL_DBUF *iter = pool->head, *tmp;

	if (off > pool->off) {
		acl_msg_warn("warning: %s(%d) off(%ld) > pool->off(%ld)",
			__FUNCTION__, __LINE__, (long) off, (long) pool->off);
		return -1;
	} else if (off == pool->off)
		return 0;

	while (1) {
		/* �����ǰ�ڴ���б����ڴ��������������ڴ�� */
		if (iter->keep)
			break;

		/* ���㵱ǰ�ڴ�鱻ʹ�õ��ڴ��С */
		n = iter->addr - iter->buf;

		/* �� off ���ƫ�����ڵ�ǰ�ڴ��ʱ�����˳�ѭ�� */
		if (pool->off <= off + n) {
			iter->addr -= pool->off - off;
			pool->off  = off;
			break;
		}

		/* ��ͷ���ڵ��⣬�����ٱ���һ�����еĽڵ㣬�Ա��ظ�ʹ�� */
		if (pool->count == 2) {
			ACL_DBUF *first = (ACL_DBUF*) pool->buf;
			pool->head->addr = pool->head->buf;
			pool->head->used = 0;
			pool->off        = first->size;
			break;
		}

		/* ������ǰ�ڴ��ָ���Ա�����������ͷ� */
		tmp = iter;
		/* ָ����һ���ڴ���ַ */
		iter = iter->next;

		pool->head = iter;

		/* off Ϊ��һ���ڴ��� addr ���ڵ����ƫ��λ��  */
		pool->off -=n;

		if (tmp->size > pool->block_size)
			pool->huge--;

#ifdef	USE_VALLOC
		free(tmp);
#else
		acl_myfree(tmp);
#endif
		pool->count--;
	}

	return 0;
}

int acl_dbuf_pool_free(ACL_DBUF_POOL *pool, const void *addr)
{
	const char *ptr = (const char*) addr;
	ACL_DBUF *iter = pool->head, *prev = iter;

	while (iter) {
		if (ptr < iter->addr && ptr >= iter->buf) {
			iter->used--;
			break;
		}

		prev = iter;
		iter = iter->next;
	}

	if (iter == NULL) {
		acl_msg_warn("warning: %s(%d), not found addr: %p",
			__FUNCTION__, __LINE__, addr);
		return -1;
	}

	if (iter->used < 0) {
		acl_msg_warn("warning: %s(%d), used(%d) < 0",
			__FUNCTION__, __LINE__, iter->used);
		return -1;
	}

	if (iter->used > 0 || iter->keep)
		return 0;

	/* should free the ACL_DBUF block */

	if (iter == pool->head)
		pool->head = iter->next;
	else
		prev->next = iter->next;

	pool->off -= iter->addr - iter->buf;

	if (iter->size > pool->block_size)
		pool->huge--;

	acl_myfree(iter);

	return 1;
}

static ACL_DBUF *acl_dbuf_alloc(ACL_DBUF_POOL *pool, size_t length)
{
#ifdef	USE_VALLOC
	ACL_DBUF *dbuf = (ACL_DBUF*) valloc(sizeof(ACL_DBUF) + length);
#else
	ACL_DBUF *dbuf = (ACL_DBUF*) acl_mymalloc(sizeof(ACL_DBUF) + length);
#endif
	dbuf->addr = (char*) dbuf->buf;

	dbuf->next = pool->head;
	dbuf->used = 0;
	dbuf->keep = 0;
	dbuf->size = length;
	dbuf->addr = dbuf->buf;

	pool->head = dbuf;
	if (length > pool->block_size)
		pool->huge++;

	pool->count++;
	return dbuf;
}

void *acl_dbuf_pool_alloc(ACL_DBUF_POOL *pool, size_t length)
{
	void *ptr;
	ACL_DBUF *dbuf;

	length += 4 - length % 4;

	if (length > pool->block_size)
		dbuf = acl_dbuf_alloc(pool, length);
	else if (pool->head == NULL)
		dbuf = acl_dbuf_alloc(pool, pool->block_size);
	else if (pool->block_size < ((char*) pool->head->addr
		- (char*) pool->head->buf) + length)
	{
		dbuf = acl_dbuf_alloc(pool, pool->block_size);
	}
	else
		dbuf = pool->head;

	ptr = dbuf->addr;
	dbuf->addr = (char*) dbuf->addr + length;
	pool->off += length;
	dbuf->used++;

	return ptr;
}

void *acl_dbuf_pool_calloc(ACL_DBUF_POOL *pool, size_t length)
{
	void *ptr;

	ptr = acl_dbuf_pool_alloc(pool, length);
	if (ptr)
		memset(ptr, 0, length);
	return ptr;
}

char *acl_dbuf_pool_strdup(ACL_DBUF_POOL *pool, const char *s)
{
	size_t  len = strlen(s);
	char *ptr = (char*) acl_dbuf_pool_alloc(pool, len + 1);

	memcpy(ptr, s, len);
	ptr[len] = 0;
	return ptr;
}

char *acl_dbuf_pool_strndup(ACL_DBUF_POOL *pool, const char *s, size_t len)
{
	char *ptr;
	size_t n = strlen(s);

	if (n > len)
		n = len;
	ptr = (char*) acl_dbuf_pool_alloc(pool, n + 1);
	memcpy(ptr, s, n);
	ptr[n] = 0;
	return ptr;
}

void *acl_dbuf_pool_memdup(ACL_DBUF_POOL *pool, const void *addr, size_t len)
{
	void *ptr = acl_dbuf_pool_alloc(pool, len);

	memcpy(ptr, addr, len);
	return ptr;
}

int acl_dbuf_pool_keep(ACL_DBUF_POOL *pool, const void *addr)
{
	const char *ptr = (const char*) addr;
	ACL_DBUF *iter = pool->head;

	while (iter) {
		if (ptr < iter->addr && ptr >= iter->buf) {
			iter->keep++;
			if (iter->keep <= iter->used)
				return 0;

			acl_msg_warn("warning: %s(%d), keep(%d) > used(%d)",
				__FUNCTION__, __LINE__,
				iter->keep, iter->used);
			return -1;
		}

		iter = iter->next;
	}

	acl_msg_warn("warning: %s(%d), not found addr: %p",
		__FUNCTION__, __LINE__, addr);
	return -1;
}

int acl_dbuf_pool_unkeep(ACL_DBUF_POOL *pool, const void *addr)
{
	const char *ptr = (const char*) addr;
	ACL_DBUF *iter = pool->head;

	while (iter) {
		if (ptr < iter->addr && ptr >= iter->buf) {
			iter->keep--;
			if (iter->keep >= 0)
				return 0;

			acl_msg_warn("warning: %s(%d), keep(%d) < 0",
				__FUNCTION__, __LINE__, iter->keep);
			return -1;
		}

		iter = iter->next;
	}

	acl_msg_warn("warning: %s(%d), not found addr: %p",
		__FUNCTION__, __LINE__, addr);
	return -1;
}

void acl_dbuf_pool_test(size_t max)
{
	ACL_DBUF_POOL *pool;
	size_t   i, n = 1000000, len, j, k;

	for (j = 0; j < max; j++) {
		printf("begin alloc, max: %d\n", (int) n);
		pool = acl_dbuf_pool_create(0);
		for (i = 0; i < n; i++) {
			k = i % 10;
			switch (k) {
			case 0:
				len = 1024;
				break;
			case 1:
				len = 1999;
				break;
			case 2:
				len = 999;
				break;
			case 3:
				len = 230;
				break;
			case 4:
				len = 199;
				break;
			case 5:
				len = 99;
				break;
			case 6:
				len = 19;
				break;
			case 7:
				len = 29;
				break;
			case 8:
				len = 9;
				break;
			case 9:
				len = 399;
				break;
			default:
				len = 88;
				break;
			}
			(void) acl_dbuf_pool_alloc(pool, len);
		}
		printf("alloc over now, sleep(10)\n");
		sleep(10);
		acl_dbuf_pool_destroy(pool);
	}
}

