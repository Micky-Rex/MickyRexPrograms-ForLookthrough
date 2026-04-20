/*
* 最低 ISO-C++14 标准
* 基于sfml2.6.1的基本网络通信库
* 保证 %INCLUDE% 路径包含sfml头文件路径
* 保证 %LIB%\SFML 路径包含sfml静态链接库文件路径
* 
* 依赖sfml-system.lib, ws2_32.lib, sfml-network.lib
*/
#pragma once
#include "Basic.h"
#include <SFML/Network.hpp>

#define Status Socket::Status

Clock _basic_timer;
/*
* udp接收结果
* 失败 code = -1
* 成功 code = 0，可从packet读取内容
*/
struct udp_result
{
	int code;
	udp_result()
	{
		code = -1;
	}
	udp_result(Packet _packet)
	{
		packet = _packet;
		code = 0;
		return;
	}
	udp_result& operator =(const udp_result& right)
	{
		this->code = right.code;
		this->packet = right.packet;
		return *this;
	}
	Packet packet;
};

/*
* udp主机
*/
struct udp_host
{
	IpAddress ip;
	unsigned short port;
	udp_host()
	{
		this->ip = IpAddress();
	}
	udp_host(IpAddress _ip, unsigned short _port)
	{
		this->ip = _ip;
		this->port = _port;
		return;
	}
};

inline Status udp_send(UdpSocket& socket, Packet& packet, udp_host client)
{
	socket.setBlocking(true);
	if (PRINT_NETWORK_DEBUG_INFO) printf("Send to port %hu of address %s.\n", client.port, client.ip.toString().c_str());
	return socket.send(packet, client.ip, client.port);
}

inline Packet udp_receive(UdpSocket& socket, udp_host client)
{
	Packet packet;
	socket.setBlocking(true);
	socket.receive(packet, client.ip, client.port);
	if (PRINT_NETWORK_DEBUG_INFO) printf("Receive from port %hu of address %s.\n", client.port, client.ip.toString().c_str());
	return packet;
}

inline udp_result udp_receive_without_blocking(UdpSocket& socket, udp_host client, float timeout = 8.f)
{
	udp_result result;
	float time_stamp = _basic_timer.getElapsedTime().asSeconds();

	Packet packet;
	socket.setBlocking(false);
	Status status = Socket::Partial;
	while (status == Socket::Partial or status == Socket::NotReady)
	{
		status = socket.receive(packet, client.ip, client.port);
		if (status == Socket::Error)
		{
			if (PRINT_NETWORK_DEBUG_INFO) printf("Failed to receive!\n");
			return udp_result();
		}
		if (_basic_timer.getElapsedTime().asSeconds() - time_stamp > timeout)
		{
			if (PRINT_NETWORK_DEBUG_INFO) printf("No packet to receive!\n");
			return udp_result();
		}
	}
	if (PRINT_NETWORK_DEBUG_INFO) printf("Receive from port %hu of address %s.\n", client.port, client.ip.toString().c_str());
	return udp_result(packet);
}
