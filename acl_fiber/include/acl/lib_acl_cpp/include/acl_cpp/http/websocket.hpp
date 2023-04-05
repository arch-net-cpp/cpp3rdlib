/**
 * Copyright (C) 2015-2018
 * All rights reserved.
 *
 * AUTHOR(S)
 *   niukey@qq.com
 *   shuxin.zheng@qq.com
 * 
 * VERSION
 *   Sun 18 Sep 2016 05:15:52 PM CST
 */

#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/noncopyable.hpp"

namespace acl
{

class socket_stream;
class aio_socket_stream;

enum
{
	FRAME_CONTINUATION = 0x00,
	FRAME_TEXT         = 0x01,
	FRAME_BINARY       = 0x02,
	FRAME_RSV3         = 0x03,
	FRAME_RSV4         = 0x04,
	FRAME_RSV5         = 0x05,
	FRAME_RSV6         = 0x06,
	FRAME_RSV7         = 0x07,
	FRAME_CLOSE        = 0x08,
	FRAME_PING         = 0x09,
	FRAME_PONG         = 0x0A,
	FRAME_CTL_RSVB     = 0x0B,
	FRAME_CTL_RSVC     = 0x0C,
	FRAME_CTL_RSVD     = 0x0D,
	FRAME_CTL_RSVE     = 0x0E,
	FRAME_CTL_RSVF     = 0x0F,
};

struct frame_header
{
	bool fin;
	bool rsv1;
	bool rsv2;
	bool rsv3;
	unsigned char opcode:4;
	bool mask;
	unsigned long long payload_len;
	unsigned int masking_key;

	frame_header(void) {
		fin         = false;
		rsv1        = false;
		rsv2        = false;
		rsv3        = false;
		opcode      = FRAME_TEXT;
		mask        = false;
		payload_len = 0;
		masking_key = 0;
	}
};

class string;

/**
 * websocket ������
 */
class ACL_CPP_API websocket : public noncopyable
{
public:
	/**
	 * ���췽��
	 * @param client {socket_stream&}
	 */
	websocket(socket_stream& client);
	~websocket(void);

	/**
	 * ��������ظ�ʹ��ʱ����Ҫͨ����������״̬����
	 */
	websocket& reset(void);

	/**
	 * ��ñ���������󶨵� socket_stream ����
	 * @return {socket_stream&}
	 */
	socket_stream& get_stream(void) const
	{
		return client_;
	}

	/**
	 * �����Ƿ�����ı�־λ
	 * @param yes {bool}
	 * @return {websocket&}
	 */
	websocket& set_frame_fin(bool yes);

	/**
	 * ���ñ�����־λ
	 * @param yes {bool}
	 * @return {websocket&}
	 */
	websocket& set_frame_rsv1(bool yes);

	/**
	 * ���ñ�����־λ
	 * @param yes {bool}
	 * @return {websocket&}
	 */
	websocket& set_frame_rsv2(bool yes);

	/**
	 * ���ñ�����־λ
	 * @param yes {bool}
	 * @return {websocket&}
	 */
	websocket& set_frame_rsv3(bool yes);

	/**
	 * ��������֡���ͣ��μ����涨�壺FRAME_XXX
	 * @param type {unsigned char}
	 * @return {websocket&}
	 */
	websocket& set_frame_opcode(unsigned char type);

	/**
	 * ���ñ�����֡����������ܳ���
	 * @param len {unsigned long long}
	 * @return {websocket&}
	 */
	websocket& set_frame_payload_len(unsigned long long len);

	/**
	 * ��������֡���ݵ�����ֵ���ͻ���ģʽ�±������ô���
	 * @param mask {unsigned int}
	 * @return {websocket&}
	 */
	websocket& set_frame_masking_key(unsigned int mask);

	/**
	 * ��������֡�е������壬����ѭ�����ñ��������ͱ�֡�����ݣ���������
	 * �ܳ���(����ε��ñ����������ݳ���֮��)Ӧ�� set_frame_payload_len
	 * �������õ�ֵ��ͬ
	 * @param data {const void*}
	 * @param len {size_t}
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool send_frame_data(const void* data, size_t len);
	bool send_frame_data(void* data, size_t len);
	bool send_frame_data(const char* str);
	bool send_frame_data(char* str);

	/**
	 * ���� PONG ����֡
	 * @param data {const void*} PONG ����֡���������壬����Ϊ NULL
	 * @param len {size_t} data ���ݳ��ȣ��� data Ϊ NULL �� len Ϊ 0 ʱ��
	 *  ��ʾû�������غ�
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool send_frame_pong(const void* data, size_t len);
	bool send_frame_pong(void* data, size_t len);
	bool send_frame_pong(const char* str);
	bool send_frame_pong(char* str);

	/**
	 * ���� PING ����֡
	 * @param data {const void*} PING ����֡���������壬����Ϊ NULL
	 * @param len {size_t} data ���ݳ��ȣ��� data Ϊ NULL �� len Ϊ 0 ʱ��
	 *  ��ʾû�������غ�
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool send_frame_ping(const void* data, size_t len);
	bool send_frame_ping(void* data, size_t len);
	bool send_frame_ping(const char* str);
	bool send_frame_ping(char* str);

	/**
	 * ���÷��������ͽӿ��첽�������ݣ������������ݺ�Ӧ�ò�Ӧ�õ���
	 * reset() ��������״̬���ڷ���һ�����ݰ�ǰ��Ӧ�ò���Ҫ�������ϵ�
	 * set_frame_xxx ������������ÿһ�����ݰ���֡ͷ��Ϣ
	 * @param conn {aio_socket_stream&}
	 * @param data {void*} Ҫ���͵����ݣ��ڲ��ᱻ�޸�
	 * @param len {size_t} data ���ݳ���
	 * @return {bool} �Ƿ����
	 */
	bool send_frame_data(aio_socket_stream& conn, void* data, size_t len);

	/**
	 * �첽����һ�� FRAME_TEXT ���͵�����֡
	 * @param conn {aio_socket_stream&}
	 * @param data {char*}
	 * @param len {size_t}
	 * @return {bool}
	 */
	bool send_frame_text(aio_socket_stream& conn, char* data, size_t len);

	/**
	 * �첽����һ�� FRAME_BINARY ���͵�����֡
	 * @param conn {aio_socket_stream&}
	 * @param data {char*}
	 * @param len {size_t}
	 * @return {bool}
	 */
	bool send_frame_binary(aio_socket_stream& conn, void* data, size_t len);

	/**
	 * �첽����һ�� FRAME_PING ���͵�����֡
	 * @param conn {aio_socket_stream&}
	 * @param data {char*}
	 * @param len {size_t}
	 * @return {bool}
	 */
	bool send_frame_ping(aio_socket_stream& conn, void* data, size_t len);

	/**
	 * �첽����һ�� FRAME_PONG ���͵�����֡
	 * @param conn {aio_socket_stream&}
	 * @param data {char*}
	 * @param len {size_t}
	 * @return {bool}
	 */
	bool send_frame_pong(aio_socket_stream& conn, void* data, size_t len);

	/**
	 * ��ȡ����֡֡ͷ
	 * @return {bool}
	 */
	bool read_frame_head(void);

	/**
	 * ��ȡ����֡�����壬��Ҫѭ�����ñ�����ֱ���������������
	 * @param buf {void*} ������ݵĻ�����
	 * @param size {size_t} buf ���ݻ�������С
	 * @return {int} ����ֵ > 0 ��ʾ���������ݳ������ٴζ���== 0 ��ʾ��������
	 *  < 0 ��ʾ������
	 */
	int read_frame_data(void* buf, size_t size);

	/**
	 * ���ڷ���������ͨ���У����Զ�ȡ websocket ����ͷ������ѭ�����ñ�����
	 * �ߵ��÷������� true ��ʾ������������ websocket ͷ��������� false��
	 * ����ͨ�� eof() �������ж����������Ƿ��Ѿ��Ͽ����� eof() ���� true��
	 * ��Ӧ�ͷű�����
	 * @return {bool} ���� true ��ʾ������������ websocket ͷ������ͨ������
	 *  read_frame_data() ����ȡ������
	 */
	bool peek_frame_head(void);

	/**
	 * ���ڷ���������ͨ���У����Զ�ȡ websocket �����壬����ѭ�����ñ�����
	 * @param buf {char*} ��Ŷ���������
	 * @param size {size_t} buf �Ŀռ��С
	 * @return {int} ���������ݳ��ȣ�������ֵΪ��
	 *   0: ��ʾ��֡������������
	 *  -1: ��ʾ��������ͨ������ eof() �ж������Ƿ��Ѿ��ر�
	 *  >0: ��ʾ���ζ��������ݳ���
	 */
	int peek_frame_data(char* buf, size_t size);
	int peek_frame_data(string& buf, size_t size);

	/**
	 * �жϵ�ǰ�Ƿ��Ѷ��� websocket ֡ͷ����
	 * @return {bool}
	 */
	bool is_head_finish(void) const;

	/**
	 * �жϵ�ǰ���������Ƿ��Ѿ��Ͽ�
	 * @return {bool}
	 */
	bool eof(void);

	/**
	 * ��ö���������֡��֡ͷ
	 * @return {const frame_header&}
	 */
	const frame_header& get_frame_header(void) const
	{
		return header_;
	}

	/**
	 * �жϱ�֡�Ƿ�Ϊ����֡
	 * @return {bool}
	 */
	bool frame_is_fin(void) const
	{
		return header_.fin;
	}

	/**
	 * �жϱ�֡�Ƿ������˱�����־λ
	 * @return {bool}
	 */
	bool frame_is_rsv1(void) const
	{
		return header_.rsv1;
	}

	/**
	 * �жϱ�֡�Ƿ������˱�����־λ
	 * @return {bool}
	 */
	bool frame_is_rsv2(void) const
	{
		return header_.rsv2;
	}

	/**
	 * �жϱ�֡�Ƿ������˱�����־λ
	 * @return {bool}
	 */
	bool frame_is_rsv3(void) const
	{
		return header_.rsv3;
	}

	/**
	 * ��ñ�����֡��״̬�룬�μ����棺FRAME_XXX
	 * @return {unsigned char}
	 */
	unsigned char get_frame_opcode(void) const
	{
		return header_.opcode;
	}

	/**
	 * ��ñ�����֡�Ƿ�����������
	 * @return {bool}
	 */
	bool frame_has_mask(void) const
	{
		return header_.mask;
	}

	/**
	 * ��ñ�����֡�������峤��
	 * @return {unsigned long long}
	 */
	unsigned long long get_frame_payload_len(void) const
	{
		return header_.payload_len;
	}

	/**
	 * ��ñ�����֡������ֵ
	 * @return {unsigned int}
	 */
	unsigned int get_frame_masking_key(void) const
	{
		return header_.masking_key;
	}

	/**
	 * ��ñ�����֡�Ѷ��������ݳ���
	 * @return {unsigned long long}
	 */
	unsigned long long get_frame_payload_nread(void) const
	{
		return payload_nread_;
	}

private:
	socket_stream& client_;
	struct frame_header header_;
	char*  header_buf_;
	size_t header_size_;
	size_t header_len_;
	unsigned long long payload_nread_;
	unsigned long long payload_nsent_;
	bool header_sent_;

	unsigned status_;
	string*  peek_buf_;

	void make_frame_header(void);

	void update_head_2bytes(unsigned char ch1, unsigned ch2);
	bool peek_head_2bytes(void);
	bool peek_head_len_2bytes(void);
	bool peek_head_len_8bytes(void);
	bool peek_head_masking_key(void);

};

} // namespace acl
