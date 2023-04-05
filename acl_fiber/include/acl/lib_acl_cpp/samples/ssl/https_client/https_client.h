#pragma once

class https_client : public acl::thread
{
public:
	https_client(const char* server_addr, const char* domain,
		bool keep_alive, int count, int length);
	~https_client();

	void accept_gzip(bool yes);
	void set_ssl_conf(acl::sslbase_conf* conf);
	void set_show_body(bool yes);

protected:
	virtual void* run();		// �����麯���������߳��б�����

private:
	acl::string server_addr_;	// ��������ַ
	acl::string domain_;		// ����
	bool  keep_alive_;		// �Ƿ���ó����ӷ�ʽ
	bool  accept_gzip_;		// �Ƿ�֧��ѹ��
	int   count_;			// IO �Ự����
	int   length_;			// ÿ�� IO �����ݳ���
	bool  show_body_;		// �Ƿ���ʾ�յ���Ӧ������
	acl::sslbase_conf* ssl_conf_;

	bool connect_server(acl::http_client& client);
	int http_request(int count);
};
