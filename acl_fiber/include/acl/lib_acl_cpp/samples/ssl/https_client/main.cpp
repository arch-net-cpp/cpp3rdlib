#include "stdafx.h"
#include "util.h"
#include "https_client.h"
#include "https_request.h"

static void usage(const char* procname)
{
	printf("usage: %s -h [help]\r\n"
		" -f path of mbedtls or polarss\r\n"
		" -s server_addr [default: 127.0.0.1:1443]\r\n"
		" -H host\r\n"
		" -k [keep alive, default: false]\r\n"
		" -L data_length [default: 1024]\r\n"
		" -c cocurrent [default: 1]\r\n"
		" -S [use ssl, default: no]\r\n"
		" -Z [if support gzip, default: no]\r\n"
		" -B [if show response body, default: no]\r\n"
		" -n count [default: 10]\r\n", procname);
}

int main(int argc, char* argv[])
{
	int   ch, cocurrent = 1, count = 10, length = 1024;
	bool  keep_alive = false, use_ssl = false, accept_gzip = false;
	bool  show_body = false;
	acl::string server_addr("127.0.0.1:1443");
	acl::string domain, libpath;

	acl::acl_cpp_init();
	acl::log::stdout_open(true);

	while ((ch = getopt(argc, argv, "hf:s:c:n:kSH:ZB")) > 0) {
		switch (ch) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 'f':
			libpath = optarg;
			break;
		case 'c':
			cocurrent = atoi(optarg);
			break;
		case 'n':
			count = atoi(optarg);
			break;
		case 'k':
			keep_alive = true;
			break;
		case 's':
			server_addr = optarg;
			break;
		case 'S':
			use_ssl = true;
			break;
		case 'H':
			domain = optarg;
			break;
		case 'Z':
			accept_gzip = true;
			break;
		case 'B':
			show_body = true;
			break;
		default:
			break;
		}
	}

	(void) keep_alive;

	acl::sslbase_conf* ssl_conf = NULL;
	if (libpath.find("mbedtls")) {
		const std::vector<acl::string>& libs = libpath.split2(";");
		if (libs.size() != 3) {
			printf("invalid libpath=%s\r\n", libpath.c_str());
			return 1;
		}

		acl::mbedtls_conf::set_libpath(libs[0], libs[1], libs[2]);
		if (!acl::mbedtls_conf::load()) {
			printf("load %s error\r\n", libpath.c_str());
			return 1;
		}

		ssl_conf = new acl::mbedtls_conf(false);
	} else if (libpath.find("polarssl")) {
		acl::polarssl_conf::set_libpath(libpath);
		if (!acl::polarssl_conf::load()) {
			printf("load %s error\r\n", libpath.c_str());
			return 1;
		}

		ssl_conf = new acl::polarssl_conf;
	} else if (libpath.find("libssl")) {
		const std::vector<acl::string>& libs = libpath.split2(";");
		if (libs.size() != 2) {
			printf("please specify libcrypto.so, libssl.so\r\n");
			return 1;
		}

		acl::openssl_conf::set_libpath(libs[0], libs[1]);

		if (!acl::openssl_conf::load()) {
			printf("load %s %s error\r\n", libs[0].c_str(), libs[1].c_str());
			return 1;
		}

		ssl_conf = new acl::openssl_conf(false);
	}

	if (domain.empty()) {
		domain = server_addr;
	}

	struct timeval begin;
	gettimeofday(&begin, NULL);

#if 1
	std::list<https_client*> threads;

	for (int i = 0; i < cocurrent; i++) {
		https_client* thread = new https_client(server_addr, domain,
				keep_alive, count, length);

		thread->accept_gzip(accept_gzip);
		if (use_ssl) {
			thread->set_ssl_conf(ssl_conf);
		}

		thread->set_show_body(show_body);

		thread->set_detachable(false);
		threads.push_back(thread);
		thread->start();
	}

	std::list<https_client*>::iterator it = threads.begin();
	for (; it != threads.end(); ++it) {
		(*it)->wait(NULL);
		delete *it;

	}
#else
	(void) length;

	std::list<https_request*> threads;

	for (int i = 0; i < cocurrent; i++) {
		https_request* thread = new https_request(server_addr,
				use_ssl ? ssl_conf : NULL);

		thread->set_detachable(false);
		threads.push_back(thread);
		thread->start();
	}

//	sleep(2);

	std::list<https_request*>::iterator it = threads.begin();
	for (; it != threads.end(); ++it) {
		if ((*it)->wait(NULL)) {
			printf("wait one thread(%lu) error\r\n",
				(*it)->thread_id());
		} else {
			printf("wait thread(%lu) ok\r\n", (*it)->thread_id());
		}
		delete *it;

	}
#endif

	struct timeval end;
	gettimeofday(&end, NULL);

	double spent = util::stamp_sub(&end, &begin);
	printf("total: %d, spent: %.2f, speed: %.f\r\n", count,
		spent, (count * 1000) / (spent > 1 ? spent : 1));

	printf("enter any key to exit\r\n");
	getchar();

	delete ssl_conf;
	return 0;
}
