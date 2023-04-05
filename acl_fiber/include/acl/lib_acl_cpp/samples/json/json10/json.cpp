#include "stdafx.h"

int main(void)
{
	acl::json json;
	acl::json_node& root = json.get_root();


	// ���� json ����{"1": {"1": {"1": "aa"}}}
	root.add_child("1", true).add_child("1", true).add_text("1", "aa");
	printf("%s\r\n", json.to_string().c_str());

	json.reset();

	//////////////////////////////////////////////////////////////////////
	// �������ַ�������������ͬ�� json �����£�
	// {"cmd": "add", "Para": {"xxx": "111", "yyy": "222", "zzz": true, "eee": 100}, "status": true, "length": 100}
	
	//////////////////////////////////////////////////////////////////////
	// ����һ��

	root = json.get_root();

	root.add_text("cmd", "add")	// ��� root �ڵ���ӽڵ㲢���� root
	  .add_child("Para", true)	// ��� root ���ӽڵ�(Para)������ Para
	    .add_text("xxx", "111")	// ��� Para ���ӽڵ㲢���� Para
	    .add_text("yyy", "222")	// ��� Para ���ӽڵ㲢���� Para
	    .add_bool("zzz", true)	// ��� Para ���ӽڵ㲢���� Para
	    .add_number("eee", 100)	// ��� Para ���ӽڵ㲢���� Para
	  .get_parent()			// ���� Para �ĸ��ڵ�(root�ڵ�)
	  .add_bool("status", true)	// ��� root �ڵ���ӽڵ� status
	  .add_number("length", 100);	// ��� root �ڵ���ӽڵ� length

	printf("%s\r\n", json.to_string().c_str());
	acl::string buf1;
	json.build_json(buf1);

	acl::json_node* para = json.getFirstElementByTagName("Para");
	if (para) {
		root.add_text("root-name", "root-value");
		para->get_obj()->add_text("test", "111111");
		printf("%s\r\n", json.to_string().c_str());
	}

	//////////////////////////////////////////////////////////////////////
	// ��������

	// ������ʹ��ǰ��Ҫ���� json ������״̬
	json.reset();

	root = json.get_root();

	root.add_text("cmd", "add")
		.add_child("Para",
			json.create_node()
				.add_text("xxx", "111")
				.add_text("yyy", "222")
				.add_bool("zzz", true)
				.add_number("eee", 100))
		.add_bool("status", true)
		.add_number("length", 100);

	printf("%s\r\n", json.to_string().c_str());
	acl::string buf2;
	json.build_json(buf2);

	//////////////////////////////////////////////////////////////////////
	// ��������

	// ������ʹ��ǰ��Ҫ���� json ������״̬
	json.reset();

	acl::json_node& cmd = json.create_node("cmd", "add");
	root.add_child(cmd);

	acl::json_node& node1 = json.create_node();
	root.add_child("Para", node1);

	acl::json_node& node11 = json.create_node("xxx", "111");
	node1.add_child(node11);

	acl::json_node& node12 = json.create_node("yyy", "222");
	node1.add_child(node12);

	acl::json_node& node13 = json.create_node("zzz", true);
	node1.add_child(node13);

	acl::json_node& node14 = json.create_node("eee", (long long int) 100);
	node1.add_child(node14);

	acl::json_node& status = json.create_node("status", true);
	root.add_child(status);

	acl::json_node& length = json.create_node("length", (long long int) 100);
	root.add_child(length);

	printf("%s\r\n", json.to_string().c_str());
	acl::string buf3;
	json.build_json(buf3);

	//////////////////////////////////////////////////////////////////////
	// �Ƚ����ַ����Ľ���Ƿ����

	if (buf2 == buf1 && buf3 == buf2) {
		printf("OK\r\n");
	} else {
		printf("ERROR\r\n");
	}
	return 0;
}
