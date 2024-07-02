#pragma once

#include <string>

struct WebCom {
	static WebCom& GetInstance();
	virtual ~WebCom();

	struct HtmlPage {
		enum encoding_flag {
			None		= 0b0000,
			Chunked		= 0b0001,
			Compress	= 0b0010,
			Deflate		= 0b0100,
			Gzip		= 0b1000,
		};

		struct Header {
			int			return_code    = 0;
			int			encoding_flags = encoding_flag::None;
			std::string content_type   = "";
		} header;

		std::string body;
	};

	HtmlPage post(const std::string& data);

protected:
	WebCom();
	bool _connect();
	void _disconnect();

private:

	static HtmlPage::Header _parse_header(std::stringstream& in_header);
	static std::string		_parse_body(std::stringstream& in_body);

	int _socketConnection			 = 0;
	const std::string _ServerIP		 = "152.228.163.160";
	const unsigned int _Port	     = 8000;
	const std::string _EndPoint		 = "/communicate";
	const std::string _Authorization = "Bearer 00000000-0000-0000-0000-000000000000";
};