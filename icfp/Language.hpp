#pragma once

#include <vector>
#include <string>

namespace Language 
{
	struct Token {
		char indicator = 0;
		std::string body;
	};

	const std::string Encode(const std::string&);
	const std::string Decode(const std::string&);
	const std::string Eval(const std::vector<Token>&);

};