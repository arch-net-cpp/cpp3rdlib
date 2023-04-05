#include "lib_acl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fiber/libfiber.h"

static int __stack_size = 320000;
static int __rw_timeout = 0;
static int __echo_data  = 0;
static int __setsockopt_timeout = 0;

static void echo_client(ACL_FIBER *fiber acl_unused, void *ctx)
{
	ACL_VSTREAM *cstream = (ACL_VSTREAM *) ctx;
	char  buf[8192];
	int   ret, count = 0;

	if (!__setsockopt_timeout) {
		cstream->rw_timeout = __rw_timeout;
	} else if (__rw_timeout > 0) {
		struct timeval tm;
		tm.tv_sec = __rw_timeout;
		tm.tv_usec = 0;

#if defined(__APPLE__) || defined(_WIN32) || defined(_WIN64)
		if (setsockopt(ACL_VSTREAM_SOCK(cstream), SOL_SOCKET,
			SO_RCVTIMEO, &__rw_timeout, sizeof(__rw_timeout)) < 0) {
#else
		if (setsockopt(ACL_VSTREAM_SOCK(cstream), SOL_SOCKET,
			SO_RCVTIMEO, &tm, sizeof(tm)) < 0) {
#endif
			printf("%s: setsockopt error: %s\r\n",
				__FUNCTION__, acl_last_serror());
		}
	}

#define	SOCK ACL_VSTREAM_SOCK

	while (1) {
		ret = acl_vstream_gets(cstream, buf, sizeof(buf) - 1);
		if (ret == ACL_VSTREAM_EOF) {
			printf("gets error: %s, fd: %d, count: %d\r\n",
				acl_last_serror(), SOCK(cstream), count);
			break;
		}
		buf[ret] = 0;
		//printf("gets line: %s", buf);

		if (!__echo_data) {
			count++;
			continue;
		}

		if (acl_vstream_writen(cstream, buf, ret) == ACL_VSTREAM_EOF) {
			printf("write error, fd: %d\r\n", SOCK(cstream));
			break;
		}

		count++;
	}

	acl_vstream_close(cstream);
}

static void fiber_accept(ACL_FIBER *fiber acl_unused, void *ctx)
{
	ACL_VSTREAM *sstream = (ACL_VSTREAM *) ctx;

	for (;;) {
		ACL_VSTREAM *cstream = acl_vstream_accept(sstream, NULL, 0);
		char buf[1024];
		int  ret;

		if (cstream == NULL) {
			printf("acl_vstream_accept error %s\r\n",
				acl_last_serror());
			break;
		}

		ret = acl_vstream_gets(cstream, buf, sizeof(buf) - 1);
		if (ret == ACL_VSTREAM_EOF) {
			printf("get first line error\r\n");
			acl_vstream_close(cstream);
			continue;
		} else if (acl_vstream_writen(cstream, buf, ret) != ret) {
			printf("write error\r\n");
			acl_vstream_close(cstream);
			continue;
		}

		//printf("accept one, fd: %d\r\n", ACL_VSTREAM_SOCK(cstream));
		acl_fiber_create(echo_client, cstream, __stack_size);
		//printf("continue to accept\r\n");
	}

	acl_vstream_close(sstream);
}

static ACL_FIBER *__listen_fiber;

static void fiber_sleep(ACL_FIBER *fiber acl_unused, void *ctx acl_unused)
{
	size_t i;
	ACL_FIBER_STACK *stack;

	while (1) {
		acl_fiber_sleep(1);

		stack = acl_fiber_stacktrace(__listen_fiber, 50);
		if (!stack) {
			continue;
		}

		for (i = 0; i < stack->count; i++) {
			printf("0x%lx:(%s()+0x%lx)\n", stack->frames[i].pc,
				stack->frames[i].func, stack->frames[i].off);
		}
		acl_fiber_stackfree(stack);
		printf("\r\n");
	}
}

static void usage(const char *procname)
{
	printf("usage: %s -h [help]\r\n"
		"  -s listen_addr\r\n"
		"  -r rw_timeout [default: 0]\r\n"
		"  -q listen_queue\r\n"
		"  -z stack_size\r\n"
		"  -T [if using setsockopt to set the timeout option]\r\n"
		"  -w [if echo data, default: no]\r\n", procname);
}

int main(int argc, char *argv[])
{
	char addr[64];
	ACL_VSTREAM *sstream;
	int   ch, qlen = 128;

	acl_msg_stdout_enable(1);
	acl_fiber_msg_stdout_enable(1);

	snprintf(addr, sizeof(addr), "%s", "127.0.0.1:9002");

	while ((ch = getopt(argc, argv, "hs:r:q:wz:T")) > 0) {
		switch (ch) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			snprintf(addr, sizeof(addr), "%s", optarg);
			break;
		case 'r':
			__rw_timeout = atoi(optarg);
			break;
		case 'q':
			qlen = atoi(optarg);
			break;
		case 'w':
			__echo_data = 1;
			break;
		case 'z':
			__stack_size = atoi(optarg);
			break;
		case 'T':
			__setsockopt_timeout = 1;
			break;
		default:
			break;
		}
	}

	sstream = acl_vstream_listen(addr, qlen);
	if (sstream == NULL) {
		printf("acl_vstream_listen error %s\r\n", acl_last_serror());
		return 1;
	}

	printf("listen %s ok\r\n", addr);

	printf("%s: call fiber_creater\r\n", __FUNCTION__);

	__listen_fiber = acl_fiber_create(fiber_accept, sstream, 327680);

	acl_fiber_create(fiber_sleep, NULL, 327680);

	printf("call fiber_schedule\r\n");
	acl_fiber_schedule();

	return 0;
}
