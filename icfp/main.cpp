#include "Language.hpp"
#include "WebCom.hpp"

#include <iostream>
#include <unordered_map>

int main() 
{
	enum class AppMode {
		Post, 
		Read, 
		Write
	} current_mode = AppMode::Post;

	static const std::unordered_map<std::string, AppMode> mode_commands = {
		{"post",  AppMode::Post},
		{"read",  AppMode::Read},
		{"write", AppMode::Write},
	};

	// Usage
	{
		std::cout << "+----------------- Command list: ------------------------------+" << std::endl;
		std::cout << "| `post`  : change app mode into communication [default]       |" << std::endl;
		std::cout << "| `read`  : change app mode into read test    (alien -> human) |" << std::endl;
		std::cout << "| `write` : change app mode into writing test (human -> alien) |" << std::endl;
		std::cout << "| `test`  : launch some tests, then quit                       |" << std::endl;
		std::cout << "| `quit`  : close this application                             |" << std::endl;
		std::cout << "+--------------------------------------------------------------+" << std::endl;
		std::cout << std::endl;
	}

	// Main
	WebCom& webcom(WebCom::GetInstance());

	for(std::string request = "";;)
	{
		std::cout << "Human>";
		std::getline(std::cin, request);

		// Commands
		if (request.empty()) {
			continue;
		}
		if (request == "quit") {
			break;
		}
		if (request == "test") {
			//std::cout << Language::Decode(R"(B$ B$ L" B$ L# B$ v" B$ v# v# L# B$ v" B$ v# v# L" L# ? B= v# I! I" B$ L$ B+ B$ v" v$ B$ v" v$ B- v# I" I%)") << std::endl;
			//std::cout << Language::Decode(R"(B$ L# B$ L" B+ v" v" B* I$ I# v8)") << std::endl;
			std::cout << Language::Decode(R"(? B> I# I$ S9%3 S./)") << std::endl;
			break;
		}

		if (mode_commands.find(request) != mode_commands.cend()) {
			current_mode = mode_commands.at(request);
			std::cout << "Changed mode to: " << request << std::endl;
			continue;
		}

		// Check mode
		switch (current_mode)
		{
		case AppMode::Post:
			std::cout << Language::Decode(webcom.post(Language::Encode(request)).body) << std::endl;
			break;

		case AppMode::Read:
			std::cout << Language::Decode(request) << std::endl;
			break;

		case AppMode::Write:
			std::cout << Language::Encode(request) << std::endl;
			break;
		}
	}

	return 0;
}