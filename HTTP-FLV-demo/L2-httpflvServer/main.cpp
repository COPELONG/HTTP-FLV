//
// Created by bxc on 2023/1/7.
//

#include <stdint.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include "Log.h"
#pragma comment(lib, "ws2_32.lib")

int main() {

	int port = 8080;
	const char* filename = "../data/test.flv";
	LOGI("httpflvServer http://127.0.0.1:%d/test.flv", port);

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		LOGE("WSAStartup error");
		return -1;
	}

	SOCKET serverFd;
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//server_addr.sin_addr.s_addr = inet_addr("192.168.2.61");
	server_addr.sin_port = htons(port);
	serverFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (bind(serverFd, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		LOGE("socket bind error");
		return -1;
	}
	if (listen(serverFd, SOMAXCONN) < 0) {
		LOGE("socket listen error");
		return -1;
	}

	constexpr char http_headers[] = \
		"HTTP/1.1 200 OK\r\n" \
		"Access-Control-Allow-Origin: * \r\n" \
		"Content-Type: video/x-flv\r\n" \
		"Content-Length: -1\r\n" \
		"Connection: Keep-Alive\r\n" \
		"Expires: -1\r\n" \
		"Pragma: no-cache\r\n" \
		"\r\n"
		;
	int http_headers_len = strlen(http_headers);

	/*
	
constexpr char http_headers[] = \
"HTTP/1.1 200 OK\r\n" \
"Access-Control-Allow-Origin: * \r\n" \
"Cache-Control: no-cache\r\n" \
"Content-Type: video/x-flv\r\n" \
"Connection: close\r\n" \
"Expires: -1\r\n" \       //设置资源的有效期来控制http的缓存
"Pragma: no-cache\r\n" \  //用于客户端发送的请求中。客户端会要求所有的中间服务器不返回缓存的资源
"\r\n"
;
	*/

	while (true)
	{
		LOGI("等待新连接...");
		int len = sizeof(SOCKADDR);
		SOCKADDR_IN accept_addr;
		int clientFd = accept(serverFd, (SOCKADDR*)&accept_addr, &len);
		//const char* clientIp = inet_ntoa(accept_addr.sin_addr);

		if (clientFd == SOCKET_ERROR) {
			LOGE("accept connection error");
			break;
		}
		LOGI("发现新连接 clientFd=%d", clientFd);
		unsigned char buf[5000];

		char bufRecv[2000] = { 0 };


		//https://blog.csdn.net/qq_32331073/article/details/82148409
		FILE* fp;
		fp = fopen(filename, "rb");
		if (!fp) {
			LOGE("fopen %s fail!", filename);
		}
		else {

			int times = 0;
			while (true) {
				times++;

				if (times == 1) {
					int bufRecvSize = recv(clientFd, bufRecv, 2000, 0);
					LOGI("bufRecvSize=%d,bufRecv=%s", bufRecvSize, bufRecv);

					send(clientFd, http_headers, http_headers_len, 0);

				}
				else {
					//usleep(1000 * 100);
					Sleep(10);
					int bufLen = fread(buf, 1, sizeof(buf), fp);
					//                        fseek(srcFile, 0, SEEK_CUR);
					int ret = send(clientFd, (char*)buf, bufLen, 0);

					if (ret <= 0) {
						break;
					}
					else {
						//printf("send bufLen=%d,ret=%d \n", bufLen, ret);
					}

				}

			}
		}



		if (fp) {
			fclose(fp);
		}
		closesocket(clientFd);
		LOGI("关闭连接 clientFd=%d", clientFd);
	}

	closesocket(serverFd);
	return 0;
}