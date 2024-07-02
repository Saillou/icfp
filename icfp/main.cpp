#include "Language.hpp"
#include "WebCom.hpp"

#include <iostream>

int main() {
	WebCom& webcom(WebCom::GetInstance());

	for(std::string request = "";;)
	{
		std::cout << "Human>";
		std::getline(std::cin, request);

		if (request == "quit") {
			break;
		}
		if (request.empty()) {
			std::cout << "use the command quit to quit." << std::endl;
			continue;
		}

		std::cout << Language::Decode(webcom.post(Language::Encode(request)).body) << std::endl;
	}

	return 0;
}