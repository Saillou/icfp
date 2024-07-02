#pragma once

#include <string>

struct WebCom {
	static WebCom& GetInstance();
	virtual ~WebCom();

	std::string post(const std::string& data) const;

protected:
	WebCom();

private:
	struct _html_page {
		enum _encoding_flag {
			None		= 0b0000,
			Chunked		= 0b0001,
			Compress	= 0b0010,
			Deflate		= 0b0100,
			Gzip		= 0b1000,
		};

		struct {
			int			return_code;
			int			encoding_flags;
			std::string content_type;
		} header;

		std::string body;
	};

	static void _parse_header(std::stringstream& in_header, _html_page& parsed_page);
	static void _parse_body(std::stringstream& in_body, _html_page& parsed_page);

	int _socketConnection			 = 0;
	const std::string _ServerIP		 = "152.228.163.160";
	const unsigned int _Port	     = 8000;
	const std::string _EndPoint		 = "/communicate";
	const std::string _Authorization = "Bearer 00000000-0000-0000-0000-000000000000";
};