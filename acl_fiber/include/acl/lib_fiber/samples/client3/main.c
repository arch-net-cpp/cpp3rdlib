#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#include <signal.h>
#endif
#include "lib_acl.h"
#include "fiber/libfiber.h"
#include "stamp.h"

#if defined(_WIN32) || defined(_WIN64)
# define snprintf	_snprintf
#else
# define SOCKET		int
# define INVALID_SOCKET	-1
#endif

static char __server_ip[64];
static int  __server_port = 9001;
static int  __event_mode  = FIBER_EVENT_KERNEL;

static int __fiber_delay  = 0;
static int __conn_timeout = 0;
static int __max_loop     = 10000;
static int __max_fibers   = 100;
static int __read_data    = 1;
static int __stack_size   = 32000;

static struct timeval __begin;
static __thread int __left_fibers           = 100;
static __thread long long int __total_count = 0;
static __thread int __total_clients         = 0;
static __thread int __total_error_clients   = 0;
static __thread double __speed;

static void echo_client(SOCKET fd)
{
#define BUF_SIZE 8192
	char *buf = malloc(BUF_SIZE);
	int   ret, i;
	const char *str = "hello world\r\n";

	for (i = 0; i < __max_loop; i++) {
#if defined(_WIN32) || defined(_WIN64)
		if (acl_fiber_send(fd, str, strlen(str), 0) <= 0) {
#else
		if (write(fd, str, strlen(str)) <= 0) {
#endif
			printf("write error: %s\r\n", acl_last_serror());
			break;
		}

		if (!__read_data) {
			__total_count++;
			if (i % 10000 == 0) {
				printf("fiber-%d: total %lld, curr %d\r\n",
					acl_fiber_self(), __total_count, i);
			}
			if (__total_count % 10000 == 0) {
				acl_fiber_yield();
			}
			continue;
		}

#if defined(_WIN32) || defined(_WIN64)
		ret = acl_fiber_recv(fd, buf, BUF_SIZE, 0);
#else
		ret = read(fd, buf, BUF_SIZE);
#endif
		if (ret <= 0) {
			printf("read error: %s\r\n", acl_last_serror());
			break;
		}

		__total_count++;
	}

	free(buf);
#if defined(_WIN32) || defined(_WIN64)
	acl_fiber_close(fd);
#else
	close(fd);
#endif
}

static void fiber_connect(ACL_FIBER *fiber acl_unused, void *ctx acl_unused)
{
	SOCKET  fd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sa;
	socklen_t len = (socklen_t) sizeof(sa);

	assert(fd != INVALID_SOCKET);

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port   = htons(__server_port);
	sa.sin_addr.s_addr = inet_addr(__server_ip);

#if defined(_WIN32) || defined(_WIN64)
	if (acl_fiber_connect(fd, (const struct sockaddr *) &sa, len) < 0) {
		acl_fiber_close(fd);
#else
	if (connect(fd, (const struct sockaddr *) &sa, len) < 0) {
		close(fd);
#endif

		printf("fiber-%d: connect %s:%d error %s\r\n",
			acl_fiber_self(), __server_ip, __server_port,
			acl_last_serror());

		__total_error_clients++;
	} else {
		__total_clients++;
		printf("fiber-%d: connect %s:%d ok, clients: %d, fd: %d\r\n",
			acl_fiber_self(), __server_ip, __server_port,
			__total_clients, fd);

		echo_client(fd);
	}

	--__left_fibers;
	printf("max_fibers: %d, left: %d\r\n", __max_fibers, __left_fibers);

	if (__left_fibers == 0) {
		double spent;
		struct timeval end;

		gettimeofday(&end, NULL);
		spent = stamp_sub(&end, &__begin);
		__speed = (__total_count * 1000) / (spent > 0 ? spent : 1);

		printf("fibers: %d, clients: %d, error: %d, count: %lld, "
			"spent: %.2f ms, speed: %.2f tps\r\n",
			__max_fibers, __total_clients, __total_error_clients,
			__total_count, spent, __speed);
	}
}

static void fiber_main(ACL_FIBER *fiber acl_unused, void *ctx acl_unused)
{
	int i;

	for (i = 0; i < __max_fibers; i++) {
		acl_fiber_create(fiber_connect, NULL, __stack_size);

		if (__fiber_delay > 0) {
			acl_fiber_delay(__fiber_delay);
		}
	}
}

static void *thread_main(void *ctx acl_unused)
{
	__left_fibers = __max_fibers;

	acl_fiber_create(fiber_main, NULL, 327680);
	acl_fiber_schedule_with(__event_mode);
	return &__speed;
}

static void usage(const char *procname)
{
	printf("usage: %s -h [help]\r\n"
		" -e event_mode [kernel|select|poll]\r\n"
		" -s server_ip\r\n"
		" -p server_port\r\n"
		" -T connt_timeout\r\n"
		" -t nthreads\r\n"
		" -c max_fibers\r\n"
		" -S [if using single IO, dafault: no]\r\n"
		" -d fiber_delay_ms\r\n"
		" -z stack_size\r\n"
		" -n max_loop\r\n", procname);
}

static void test_time(void)
{
	struct timeval begin, end;
	double diff;

	gettimeofday(&begin, NULL);
	//usleep(1000);
	acl_doze(1);
	gettimeofday(&end, NULL);
	diff = stamp_sub(&end, &begin);
	printf("usleep 1000 diff=%.2f\r\n", diff);
}

int main(int argc, char *argv[])
{
	int   ch, i, nthreads = 1;
	double total_speed;
	pthread_t *threads;
       
	acl_lib_init();
	acl_msg_stdout_enable(1);

#if !defined(_WIN32) && !defined(_WIN64)
	signal(SIGPIPE, SIG_IGN);
#endif

	snprintf(__server_ip, sizeof(__server_ip), "%s", "127.0.0.1");

	while ((ch = getopt(argc, argv, "ht:c:n:s:p:t:Sd:z:e:")) > 0) {
		switch (ch) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 't':
			nthreads = atoi(optarg);
			if (nthreads <= 0) {
				nthreads = 1;
			}
			break;
		case 'c':
			__max_fibers = atoi(optarg);
			break;
		case 'T':
			__conn_timeout = atoi(optarg);
			break;
		case 'n':
			__max_loop = atoi(optarg);
			break;
		case 's':
			snprintf(__server_ip, sizeof(__server_ip), "%s", optarg);
			break;
		case 'p':
			__server_port = atoi(optarg);
			break;
		case 'S':
			__read_data = 0;
			break;
		case 'd':
			__fiber_delay = atoi(optarg);
			break;
		case 'z':
			__stack_size = atoi(optarg);
			break;
		case 'e':
			if (strcasecmp(optarg, "select") == 0) {
				__event_mode = FIBER_EVENT_SELECT;
			} else if (strcasecmp(optarg, "poll") == 0) {
				__event_mode = FIBER_EVENT_POLL;
			} else if (strcasecmp(optarg, "kernel") == 0) {
				__event_mode = FIBER_EVENT_KERNEL;
			}
			break;
		default:
			break;
		}
	}

	acl_fiber_msg_stdout_enable(1);
	gettimeofday(&__begin, NULL);

	printf("call fiber_schedule with=%d\r\n", __event_mode);

	threads = (pthread_t*) malloc(nthreads * sizeof(pthread_t));
	for (i = 0; i < nthreads; i++) {
		pthread_create(&threads[i], NULL, thread_main, NULL);
	}

	total_speed = 0;
	for (i = 0; i < nthreads; i++) {
		double *speed;
		pthread_join(threads[i], (void**) &speed);
		total_speed += *speed;
	}

	printf("\r\ntotal speed: %.2f qps\r\n", total_speed);

	free(threads);
	test_time();

	return 0;
}
