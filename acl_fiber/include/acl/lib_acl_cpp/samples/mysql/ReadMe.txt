========================================================================
    ����̨Ӧ�ó��� : mysql ��Ŀ�ſ�
========================================================================

Ӧ�ó�������Ϊ����������� mysql Ӧ�ó���
���ļ�������� mysql Ӧ�ó���
��ÿ���ļ�������ժҪ��


mysql.vcproj
    ������Ӧ�ó��������ɵ� VC++ ��Ŀ������Ŀ�ļ���
    �������й����ɴ��ļ��� Visual C++ �汾����Ϣ���Լ�
    �й���Ӧ�ó�����ѡ���
    ƽ̨�����ú���Ŀ���ܵ���Ϣ��

mysql.cpp
    ������Ӧ�ó���Դ�ļ���

/////////////////////////////////////////////////////////////////////////////
������׼�ļ�:

StdAfx.h��StdAfx.cpp
    ��Щ�ļ�����������Ϊ mysql.pch
    ��Ԥ����ͷ(PCH)�ļ��Լ���Ϊ StdAfx.obj ��Ԥ���������ļ���

/////////////////////////////////////////////////////////////////////////////
����ע��:

Ӧ�ó�����ʹ�� "TODO:" ע��ָʾӦ��ӻ��Զ����Դ���벿�֡�

/////////////////////////////////////////////////////////////////////////////

# For old Mysql
use mysql;
create database acl_test_db;
insert into user (User, Host, Password) values ('acl_user', 'localhost', PASSWORD('111111'));
insert into user (User, Host, Password) values ('acl_user', '192.168.1.%', PASSWORD('111111'));

grant CREATE,DROP,INSERT,DELETE,UPDATE,SELECT on acl_test_db.* to 'acl_user';
flush privileges;

# For new Mysql(version > 5.1)
use mysql
UPDATE User SET authentication_string=PASSWORD('1qazxsw2') WHERE user='root';
flush privileges;
exit;

use mysql
ALTER User user() identified by '111111';
create database acl_test_db;

use acl_test_db;
create user acl_user identified by '111111';

#INSERT INTO User (User, Host, authentication_string, ssl_cipher, x509_issuer, x509_subject) VALUES ('acl_user', '10.%', PASSWORD('111111'), '','','');
#INSERT INTO User (User, Host, authentication_string, ssl_cipher, x509_issuer, x509_subject) VALUES ('acl_user', 'localhost', PASSWORD('111111'), '', '', '');

grant CREATE,DROP,INSERT,DELETE,UPDATE,SELECT on acl_test_db.* to 'acl_user';
flush privileges;
