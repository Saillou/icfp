#pragma once

#include <vector>
#include <string>

struct WebCom {
	static WebCom& GetInstance();
	virtual ~WebCom();

	std::string post(const std::string& data) const;

protected:
	WebCom();

private:
	struct _html_page {
		std::vector<std::string> header;
		std::string body;
	};

	static _html_page _parseHtml(std::stringstream& html_page);

	int _socketConnection			 = 0;
	const std::string _ServerIP		 = "152.228.163.160";
	const unsigned int _Port	     = 8000;
	const std::string _EndPoint		 = "/communicate";
	const std::string _Authorization = "Bearer 00000000-0000-0000-0000-000000000000";
};