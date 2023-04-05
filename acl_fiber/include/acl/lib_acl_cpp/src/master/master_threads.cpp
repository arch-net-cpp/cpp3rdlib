#include "acl_stdafx.hpp"
#ifndef ACL_PREPARE_COMPILE
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/stream/server_socket.hpp"
#include "acl_cpp/master/master_threads.hpp"
#endif

#ifndef ACL_CLIENT_ONLY

namespace acl
{

master_threads::master_threads(void) {}

master_threads::~master_threads(void) {}

//////////////////////////////////////////////////////////////////////////

static bool __has_called = false;

void master_threads::run(int argc, char** argv)
{
	// ���� acl ��������ܵĶ��߳�ģ��
	acl_threads_server_main(argc, argv, service_main, this,
		ACL_MASTER_SERVER_ON_LISTEN, service_on_listen,
		ACL_MASTER_SERVER_ON_ACCEPT, service_on_accept,
		ACL_MASTER_SERVER_ON_HANDSHAKE, service_on_handshake,
		ACL_MASTER_SERVER_ON_TIMEOUT, service_on_timeout,
		ACL_MASTER_SERVER_ON_CLOSE, service_on_close,
		ACL_MASTER_SERVER_PRE_INIT, service_pre_jail,
		ACL_MASTER_SERVER_POST_INIT, service_init,
		ACL_MASTER_SERVER_EXIT_TIMER, service_exit_timer,
		ACL_MASTER_SERVER_EXIT, service_exit,
		ACL_MASTER_SERVER_THREAD_INIT, thread_init,
		ACL_MASTER_SERVER_THREAD_EXIT, thread_exit,
		ACL_MASTER_SERVER_THREAD_INIT_CTX, this,
		ACL_MASTER_SERVER_THREAD_EXIT_CTX, this,
		ACL_MASTER_SERVER_SIGHUP, service_on_sighup,
		ACL_MASTER_SERVER_BOOL_TABLE, conf_.get_bool_cfg(),
		ACL_MASTER_SERVER_INT64_TABLE, conf_.get_int64_cfg(),
		ACL_MASTER_SERVER_INT_TABLE, conf_.get_int_cfg(),
		ACL_MASTER_SERVER_STR_TABLE, conf_.get_str_cfg(),
		ACL_MASTER_SERVER_END);
}

const char* master_threads::get_conf_path(void) const
{
	if (daemon_mode_) {
		const char* ptr = acl_threads_server_conf();
		return ptr && *ptr ? ptr : NULL;
	} else {
		return conf_.get_path();
	}
}

void master_threads::run_daemon(int argc, char** argv)
{
#ifndef ACL_WINDOWS
	// ÿ������ֻ����һ��ʵ��������
	acl_assert(__has_called == false);
	__has_called = true;
	daemon_mode_ = true;

	run(argc, argv);
#else
	logger_fatal("no support win32 yet!");
#endif
}

bool master_threads::run_alone(const char* addrs, const char* path /* = NULL */,
	unsigned int, int)
{
#ifdef ACL_WINDOWS
	acl_cpp_init();
#endif

	// ÿ������ֻ����һ��ʵ��������
	acl_assert(__has_called == false);
	__has_called = true;
	daemon_mode_ = false;
	if (addrs == NULL || *addrs == 0) {
		if (path == NULL || *path == 0) {
			printf("%s: addrs NULL and path NULL\r\n", __FUNCTION__);
			return false;
		}
		printf("%s: addrs NULL and try to use master_service in %s\r\n",
			__FUNCTION__, path);
	}

	//acl_assert(addrs && *addrs);

	int  argc = 0;
	const char *argv[5];

	const char* proc = acl_process_path();
	argv[argc++] = proc ? proc : "demo";
	argv[argc++] = "-L";
	argv[argc++] = addrs;
	if (path && *path) {
		argv[argc++] = "-f";
		argv[argc++] = path;
	}

	run(argc, (char**) argv);
	return true;
}

//////////////////////////////////////////////////////////////////////////

void master_threads::thread_disable_read(socket_stream* stream)
{
	ACL_EVENT* event = get_event();

	if (event == NULL) {
		logger_error("event NULL");
	} else {
		acl_event_disable_readwrite(event, stream->get_vstream());
	}
}

void master_threads::thread_enable_read(socket_stream* stream)
{
	ACL_EVENT* event = get_event();
	if (event == NULL) {
		logger_error("event NULL");
		return;
	}

	acl_pthread_pool_t* threads = acl_threads_server_threads();
	if (threads != NULL) {
		acl_threads_server_enable_read(event, threads,
			stream->get_vstream());
	} else {
		logger_error("threads NULL!");
	}
}

void master_threads::push_back(server_socket* ss)
{
	thread_mutex_guard guard(lock_);
	servers_.push_back(ss);
}

acl_pthread_pool_t* master_threads::threads_pool(void) const
{
	return acl_threads_server_threads();
}

size_t master_threads::task_qlen(void) const
{
	acl_pthread_pool_t* threads = threads_pool();
	return threads ? acl_pthread_pool_qlen(threads) : 0;
}

//////////////////////////////////////////////////////////////////////////

void master_threads::service_pre_jail(void* ctx)
{
	master_threads* mt = (master_threads *) ctx;
	acl_assert(mt != NULL);

	ACL_EVENT* eventp = acl_threads_server_event();
	mt->set_event(eventp);

	mt->proc_pre_jail();
}

void master_threads::service_init(void* ctx)
{
	master_threads* mt = (master_threads *) ctx;
	acl_assert(mt != NULL);

	mt->proc_inited_ = true;
	mt->proc_on_init();
}

int master_threads::service_exit_timer(void* ctx, size_t nclients,
	size_t nthreads)
{
	master_threads* mt = (master_threads *) ctx;
	acl_assert(mt != NULL);
	return mt->proc_exit_timer(nclients, nthreads) == true ? 1 : 0;
}

void master_threads::service_exit(void* ctx)
{
	master_threads* mt = (master_threads *) ctx;
	acl_assert(mt != NULL);
	mt->proc_on_exit();
}

int master_threads::thread_init(void* ctx)
{
	master_threads* mt = (master_threads *) ctx;
	acl_assert(mt != NULL);
	mt->thread_on_init();
	return 0;
}

void master_threads::thread_exit(void* ctx)
{
	master_threads* mt = (master_threads *) ctx;
	acl_assert(mt != NULL);
	mt->thread_on_exit();
}

int master_threads::service_on_accept(void* ctx, ACL_VSTREAM* client)
{
	master_threads* mt = (master_threads *) ctx;
	acl_assert(mt);

	// client->context ��Ӧ��ռ��
	if (client->context != NULL) {
		logger_fatal("client->context not null!");
	}

	socket_stream* stream = NEW socket_stream();

	// ���� client->context Ϊ�����󣬸�������ͳһ��
	// service_on_close �б��ͷ�
	client->context = stream;

	if (!stream->open(client)) {
		logger_error("open stream error(%s)", acl_last_serror());
		// ���� -1 ���ϲ��ܵ��� service_on_close ���̣�������
		// �ͷ� stream ����
		return -1;
	}

	// �������� thread_on_accept �������� false����ֱ�ӷ��ظ��ϲ�
	// ��� -1�����ϲ����ٵ��� service_on_close ���̣��Ӷ��ڸù���
	// �н� stream �����ͷ�
	if (!mt->thread_on_accept(stream)) {
		return -1;
	}

	// �������� thread_on_handshake �������� false����ֱ�ӷ��ظ��ϲ�
	// ��� -1�����ϲ����ٵ��� service_on_close ���̣��Ӷ��ڸù���
	// �н� stream �����ͷ�
	//if (mt->thread_on_handshake(stream) == false)
	//	return -1;

	// ���� 0 ��ʾ���Լ�������ÿͻ������ӣ��Ӷ����ϲ��ܽ�������
	// ����ؼ�����
	return 0;
}

int master_threads::service_on_handshake(void* ctx, ACL_VSTREAM *client)
{
	master_threads* mt = (master_threads *) ctx;
	acl_assert(mt != NULL);

	// client->context �� service_on_accept �б�����
	socket_stream* stream = (socket_stream*) client->context;
	if (stream == NULL) {
		logger_fatal("client->context is null!");
	}

	// �������� thread_on_handshake �������� false����ֱ�ӷ��ظ��ϲ�
	// ��� -1�����ϲ����ٵ��� service_on_close ���̣��Ӷ��ڸù���
	// �н� stream �����ͷ�
	if (mt->thread_on_handshake(stream)) {
		return 0;
	}
	return -1;
}

int master_threads::service_main(void* ctx, ACL_VSTREAM *client)
{
	master_threads* mt = (master_threads *) ctx;
	acl_assert(mt != NULL);

	// client->context �� service_on_accept �б�����
	socket_stream* stream = (socket_stream*) client->context;
	if (stream == NULL) {
		logger_fatal("client->context is null!");
	}

	// ����������麯��ʵ�֣�������� true ��ʾ�ÿ�ܼ�����ظ���������
	// ������Ҫ�رո���
	// ���ϲ��ܷ���ֵ�������£�
	// 0 ��ʾ��������ӱ��ֳ����ӣ�����Ҫ������ظ������Ƿ�ɶ�
	// -1 ��ʾ��Ҫ�رո�����
	// 1 ��ʾ���ټ�ظ�����

	if (mt->thread_on_read(stream)) {
		// ��������ڷ��� true ��ϣ����ܼ������������ֱ�ӷ��ظ���� 1
		if (!mt->keep_read(stream)) {
			return 1;
		}

		// ������Ҫ�������Ƿ��Ѿ��رգ�����رգ�����뷵�� -1
		if (stream->eof()) {
			logger_error("DISCONNECTED, CLOSING, FD: %d",
				(int) stream->sock_handle());
			return -1;
		}

		// ���� 0 ��ʾ������ظ����Ŀɶ�״̬
		return 0;
	}

	// ���� -1 ��ʾ���ϲ��������ر������ϲ����������ر���ǰ
	// ����ص� service_on_close ���̽������ر�ǰ���ƺ�������
	// stream ������ service_on_close �б��ͷ�
	return -1;
}

void master_threads::service_on_listen(void* ctx, ACL_VSTREAM* sstream)
{
	master_threads* mt = (master_threads *) ctx;
	acl_assert(mt != NULL);
	server_socket* ss = NEW server_socket(sstream);
	mt->push_back(ss);
	mt->proc_on_listen(*ss);
}

int master_threads::service_on_timeout(void* ctx, ACL_VSTREAM* client)
{
	master_threads* mt = (master_threads *) ctx;
	acl_assert(mt);

	socket_stream* stream = (socket_stream*) client->context;
	if (stream == NULL) {
		logger_fatal("client->context is null!");
	}

	acl_assert(mt != NULL);

	return mt->thread_on_timeout(stream) == true ? 0 : -1;
}

void master_threads::service_on_close(void* ctx, ACL_VSTREAM* client)
{
	master_threads* mt = (master_threads *) ctx;
	acl_assert(mt != NULL);

	socket_stream* stream = (socket_stream*) client->context;
	if (stream == NULL) {
		logger_fatal("client->context is null!");
	}

	// �������ຯ���Խ�Ҫ�رյ��������ƺ���
	mt->thread_on_close(stream);

	// �������������İ󶨹�ϵ���������Է�ֹ��ɾ��������ʱ
	// �����ر�������������Ϊ����������Ҫ�ڱ��������غ���
	// ����Զ��ر�
	(void) stream->unbind();
	delete stream;
}

int master_threads::service_on_sighup(void* ctx, ACL_VSTRING* buf)
{
	master_threads* mt = (master_threads *) ctx;
	acl_assert(mt);
	string s;
	bool ret = mt->proc_on_sighup(s);
	if (buf) {
		acl_vstring_strcpy(buf, s.c_str());
	}
	return ret ? 0 : -1;
}

}  // namespace acl

#endif // ACL_CLIENT_ONLY
