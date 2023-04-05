#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/pipe_stream.hpp"
#include "stream.hpp"

struct iovec;

namespace acl {

class string;

/**
 * ��������������࣬��������ȷ��֪��������Ƿ������Ƿ�رգ�
 * Ӧ�õ��� stream->eof() �������ж�
 */

class ACL_CPP_API ostream : virtual public stream , public pipe_stream {
public:
	ostream(void) {}
	virtual ~ostream(void) {}

	/**
	 * д�������������
	 * @param data {const void*} ����ָ���ַ
	 * @param size {size_t} data ���ݳ���(�ֽ�)
	 * @param loop {bool} �Ƿ�֤����ȫ������ŷ��أ����Ϊ true,
	 *  ��ú���ֱ������ȫ����������Ż᷵�أ������дһ�α㷵�أ�
	 *  ��������֤����ȫ��д��
	 * @param buffed {bool} �Ƿ��Ȼ����д������
	 * @return {int} ��ʵд���������, ���� -1 ��ʾ����
	 */
	int write(const void* data, size_t size, bool loop = true,
		bool buffed = false);

	/**
	 * �����ñ��ķ�ʽ��������ʱ���ɵ��ñ�������Ŀ���ַ�������ݰ�
	 * @param data {const void*} ���ݵ�ַ
	 * @param size {size_t} ���ݳ���
	 * @param dest_addr {const char*} Ŀ���ַ����ʽ��ip|port
	 * @param flags {int} ���ͱ�־λ����ο�ϵͳ sendto() api �� flags ˵��
	 * @return {int} ���� -1 ��ʾ����ʧ��
	 */
	int sendto(const void* data, size_t size,
		const char* dest_addr, int flags = 0);

	/**
	 * �����ñ��ķ�ʽ��������ʱ���ɵ��ñ�������Ŀ���ַ�������ݰ�
	 * @param data {const void*} ���ݵ�ַ
	 * @param size {size_t} ���ݳ���
	 * @param dest_addr {const sockaddr*} Ŀ���ַ����ʽ��ip|port
	 * @param addrlen {int} dest_addr ��ַ����
	 * @param flags {int} ���ͱ�־λ����ο�ϵͳ sendto() api �� flags ˵��
	 * @return {int} ���� -1 ��ʾ����ʧ��
	 */
	int sendto(const void* data, size_t size,
		const struct sockaddr* dest_addr, int addrlen, int flags = 0);

	/**
	 * �������д���巽ʽ���������Ҫ���ñ�����ˢд������
	 * @return {bool} ���� false ��ʾдʧ�ܣ��п��������ӹر�
	 */
	bool fflush(void);

	/**
	 * д�������������
	 * @param v {const struct iovec*}
	 * @param count {int} ���� v ��Ԫ�ظ���
	 * @param loop {bool} �Ƿ�֤����ȫ������ŷ��أ����Ϊ true��
	 *  ��ú���ֱ������ȫ����������Ż᷵�أ������дһ�α㷵�أ�
	 *  ��������֤����ȫ��д��
	 * @return {int} ��ʵд���������, ���� -1 ��ʾ����
	 */
	int writev(const struct iovec *v, int count, bool loop = true);

	/**
	 * ����ʽ��ʽд���ݣ������� vfprintf����֤����ȫ��д��
	 * @param fmt {const char*} ��ʽ�ַ���
	 * @param ap {va_list} ����б�
	 * @return {int} ��ʵд������ݳ��ȣ����� -1 ��ʾ����
	 */
	int vformat(const char* fmt, va_list ap);

	/**
	 * ����ʽ��ʽд���ݣ������� fprintf����֤����ȫ��д��
	 * @param fmt {const char*} ��θ�ʽ�ַ���
	 * @return {int} ��ʵд������ݳ��ȣ����� -1 ��ʾ����
	 */
	int format(const char* fmt, ...) ACL_CPP_PRINTF(2, 3);

	/**
	 * д��һ�� 64 λ����
	 * @param n {acl_int64} 64 λ����
	 * @return {int} д������ݳ��ȣ����� -1 ��ʾ����
	 */
#if defined(_WIN32) || defined(_WIN64)
	int write(__int64 n);
#else
	int write(long long int n);
#endif

	/**
	 * д��һ�� 32 λ����
	 * @param n {int} 32 λ����
	 * @return {int} д������ݳ��ȣ����� -1 ��ʾ����
	 */
	int write(int n);

	/**
	 * д��һ�� 16 λ������
	 * @param n {int} 16 λ����
	 * @return {int} д������ݳ��ȣ����� -1 ��ʾ����
	 */
	int write(short n);

	/**
	 * дһ���ֽ�
	 * @param ch {char}
	 * @return {int} д������ݳ��ȣ����� -1 ��ʾ����
	 */
	int write(char ch);

	/**
	 * ����������е�����
	 * @param s {const string&}
	 * @param loop {bool} �Ƿ�Ҫ��ȫ�������ŷ���
	 * @return {int} ������ݵĳ��ȣ����� -1 ��ʾ����
	 */
	int write(const string& s, bool loop = true);

	/**
	 * ���һ���ַ������ݣ��������ַ�������� "\r\n"
	 * @param s {const char*} �ַ���ָ�룬������ '\0' ��β
	 * @return {int} ������ݵĳ��ȣ����� -1 ��ʾ����
	 */
	int puts(const char* s);

	/**
	 * ���¼�������Ϊ������������غ������Ҷ�������������̣�
	 * ������ж�������Ƿ�����ر�Ӧ�õ��� stream->eof()
	 * �������ж�
	 */

	ostream& operator<<(const string& s);
	ostream& operator<<(const char* s);
#if defined(_WIN32) || defined(_WIN64)
	ostream& operator<<(__int64 n);
#else
	ostream& operator<<(long long int n);
#endif
	ostream& operator<<(int n);
	ostream& operator<<(short n);
	ostream& operator<<(char ch);

	// pipe_stream �����麯��
	// ��Ϊ������������Խ�ʵ��һ��
	virtual int push_pop(const char* in, size_t len,
		string* out = NULL, size_t max = 0);
	virtual int pop_end(string* out, size_t max = 0)
	{
		(void) out;
		(void) max;
		return (0);
	}

protected:
private:
};

} // namespace acl
