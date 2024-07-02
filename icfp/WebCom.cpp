#include "WebCom.hpp"

#include <ws2tcpip.h>
#include <winsock2.h>
#include <iostream>
#include <sstream>
#include <memory>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

WebCom& WebCom::GetInstance() {
	struct _WebCom : public WebCom {};

	static std::shared_ptr<WebCom> self = std::make_shared<_WebCom>();
	return *self;
}

WebCom::WebCom() {
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
#ifdef _DEBUG
		cerr << "WSAStartup failed.\n";
#endif
		return;
	}

	_socketConnection = (int)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(_Port);
	inet_pton(AF_INET, _ServerIP.c_str(), &sockAddr.sin_addr);

	if (connect(_socketConnection, (SOCKADDR*)(&sockAddr), sizeof(sockAddr)) != 0) {
		closesocket(_socketConnection);
		_socketConnection = 0;
		return;
	}

#ifdef _DEBUG
	cerr << "[WebCom] Connection established " << endl;
#endif
}

WebCom::~WebCom() {
	if (_socketConnection) {
#ifdef _DEBUG
		cerr << "[WebCom] Disconnected " << endl;
#endif
		closesocket(_socketConnection);
	}

	WSACleanup();
}


std::string WebCom::post(const std::string& data) const
{
	if (!_socketConnection) {
#ifdef _DEBUG
		cerr << "[WebCom] Not connected" << endl;
#endif
		return "";
	}
	
	// Send the request
	std::string request =
		"POST "  + _EndPoint + " HTTP/1.1\r\n"
		"Host: " + _ServerIP + ":" + to_string(_Port) + "\r\n"
		"Authorization: " + _Authorization + "\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: " + to_string(data.size()) + "\r\n"
		"\r\n" + data;

	send(_socketConnection, request.c_str(), (int)request.size(), 0);

	// Receive answer
	stringstream http_answer;
	constexpr int _buffer_max_size = 1024;
	static char _buffer[_buffer_max_size] = { 0 };

	for(int _current_buffer_usage = 0;;) {
		_current_buffer_usage = recv(_socketConnection, _buffer, _buffer_max_size, 0);

		http_answer << std::string(_buffer, _buffer + _current_buffer_usage);

		// Still have some data ?
		unsigned long count = 0;
		ioctlsocket(_socketConnection, FIONREAD, &count);
		if(count == 0)
			break;
	}

	return _parseHtml(http_answer).body;
}

WebCom::_html_page WebCom::_parseHtml(std::stringstream& html_page_raw) {
	_html_page page;

	// Header
	for (std::string line; std::getline(html_page_raw, line, '\n') && line.size() > 1;)
	{
		page.header.push_back(line);
	}

	// Body
	std::string body_line_size;
	std::getline(html_page_raw, body_line_size, '\n');

	size_t body_size = (size_t)std::stoi(body_line_size.c_str(), nullptr, 16);
	page.body.resize(body_size);
	html_page_raw.read(& page.body[0], body_size);

	return page;
}