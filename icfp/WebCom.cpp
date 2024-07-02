#include "WebCom.hpp"

#include <ws2tcpip.h>
#include <winsock2.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <chrono>
#include <thread>

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
	constexpr int _buffer_max_size		  = 1024;
	static char _buffer[_buffer_max_size] = { 0 };

	_html_page page;
	stringstream answer;
	{
		int _current_buffer_usage = recv(_socketConnection, _buffer, _buffer_max_size, 0);
		answer << std::string(_buffer, _buffer + _current_buffer_usage);
	}
	_parse_header(answer, page);

	for(;;)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		unsigned long remaining = 0;
		ioctlsocket(_socketConnection, FIONREAD, &remaining);
		if (remaining == 0)
			break;

		int _current_buffer_usage = recv(_socketConnection, _buffer, _buffer_max_size, 0);

		answer << std::string(_buffer, _buffer + _current_buffer_usage);
	}
	_parse_body(answer, page);

	return page.body;
}

void WebCom::_parse_header(std::stringstream& in_header, _html_page& page) {
	for (std::string line; std::getline(in_header, line, '\n') && line.size() >= 2;)
	{
		size_t delim_pos = line.find(' ');
		if (delim_pos == std::string::npos)
			continue;

		const std::string key = line.substr(0, delim_pos);
		const std::string value = line.substr(delim_pos + 1);

		if (key == "HTTP/1.1") {
			const std::string code_str = value.substr(0, value.find(' '));
			page.header.return_code = std::stoi(code_str);
			continue;
		}
		if (key == "Transfer-Encoding:") {
			page.header.encoding_flags = _html_page::_encoding_flag::None;

			if (std::string::npos != value.find("chunked"))	 page.header.encoding_flags |= _html_page::_encoding_flag::Chunked;
			if (std::string::npos != value.find("compress")) page.header.encoding_flags |= _html_page::_encoding_flag::Compress;
			if (std::string::npos != value.find("deflate"))	 page.header.encoding_flags |= _html_page::_encoding_flag::Deflate;
			if (std::string::npos != value.find("gzip"))	 page.header.encoding_flags |= _html_page::_encoding_flag::Gzip;
			continue;
		}
		if (key == "Content-Type:") {
			page.header.content_type = value;
			continue;
		}
	}

}
void WebCom::_parse_body(std::stringstream& in_body, _html_page& page) {
	// Body | Assuming "Transfer-Encoding: chunked"
	for (size_t iPos = 0; !in_body.eof();)
	{
		// Chunk: 
		//	 - [size]\r\n
		//	 - [CHUNKY content] (size)
		//	 - 0\r\n
		//	 - \r\n

		std::string body_line_size;
		std::getline(in_body, body_line_size, '\n');

		if (body_line_size.size() <= 2)
			continue;

		size_t body_size = (size_t)std::stoi(body_line_size.c_str(), nullptr, 16);
		if (body_size < 1)
			continue;

		page.body.resize(page.body.size() + body_size);
		in_body.read(&page.body[iPos], body_size);
		iPos += body_size;
	}
}
