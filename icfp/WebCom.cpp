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
}

WebCom::~WebCom() {
	WSACleanup();
}

bool WebCom::_connect() 
{
	_socketConnection = (int)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(_Port);
	inet_pton(AF_INET, _ServerIP.c_str(), &sockAddr.sin_addr);

	if (connect(_socketConnection, (SOCKADDR*)(&sockAddr), sizeof(sockAddr)) != 0) {
		_disconnect();
	}

	return _socketConnection > 0;
}

void WebCom::_disconnect() 
{
	if (_socketConnection) {
		closesocket(_socketConnection);
		_socketConnection = 0;
	}
}


WebCom::HtmlPage WebCom::post(const std::string& data)
{
	HtmlPage page;

	if (!_connect()) {
		cerr << "[WebCom] Not connected" << endl;

		page.header.return_code = 500;
		return page;
	}
	
	// - Send the request - (no chunks, just a big content)
	std::string request =
		"POST "  + _EndPoint + " HTTP/1.1\r\n"
		"Host: " + _ServerIP + ":" + to_string(_Port) + "\r\n"
		"Authorization: " + _Authorization + "\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Connection: close\r\n"
		"Content-Length: " + to_string(data.size()) + "\r\n"
		"\r\n" + data;

	send(_socketConnection, request.c_str(), (int)request.size(), 0);

	// - Receive answer - (expecting chunks)
	constexpr int _buffer_max_size		  = 0x8000; // size max of a chunk (from what i saw)
	static char _buffer[_buffer_max_size] = { 0 };

	stringstream answer;
	for(;;)
	{
		int _current_buffer_usage = recv(_socketConnection, _buffer, _buffer_max_size, 0);
		if (_current_buffer_usage == 0)
			break;

		answer << std::string(_buffer, _buffer + _current_buffer_usage);
	}
	page.header = _parse_header(answer);
	page.body	= _parse_body(answer);

	// End the job
	_disconnect();
	return page;
}

WebCom::HtmlPage::Header WebCom::_parse_header(std::stringstream& in_header) {
	WebCom::HtmlPage::Header header;

	for (std::string line; std::getline(in_header, line) && line.size() > 1;)
	{
		size_t delim_pos = line.find(' ');
		if (delim_pos == std::string::npos)
			continue;

		const std::string key = line.substr(0, delim_pos);
		const std::string value = line.substr(delim_pos + 1);

		if (key == "HTTP/1.1") {
			const std::string code_str = value.substr(0, value.find(' '));
			header.return_code = std::stoi(code_str);
			continue;
		}
		if (key == "Transfer-Encoding:") {
			header.encoding_flags = HtmlPage::encoding_flag::None;

			if (std::string::npos != value.find("chunked"))	 header.encoding_flags |= HtmlPage::encoding_flag::Chunked;
			if (std::string::npos != value.find("compress")) header.encoding_flags |= HtmlPage::encoding_flag::Compress;
			if (std::string::npos != value.find("deflate"))	 header.encoding_flags |= HtmlPage::encoding_flag::Deflate;
			if (std::string::npos != value.find("gzip"))	 header.encoding_flags |= HtmlPage::encoding_flag::Gzip;
			continue;
		}
		if (key == "Content-Type:") {
			header.content_type = value;
			continue;
		}
	}

	return header;
}

std::string WebCom::_parse_body(std::stringstream& in_body) {
	std::string content;

	while (!in_body.eof())
	{
		std::string chunk_size(size_t(10), '\0');
		in_body.get(&chunk_size[0], chunk_size.size());

		// Last chunk: size 0 (or empty size)
		int body_size = std::stoi(chunk_size.c_str(), nullptr, 0x10);
		if (body_size == 0)
			break;

		std::string body_chunk((size_t)body_size+3, '\0');
		in_body.read(&body_chunk[0], body_chunk.size());
		content += body_chunk.substr(1, body_size);
	}

	return content;
}
