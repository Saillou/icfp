#include "Language.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <queue>

using namespace std;

// Private
std::string translate_alien_to_human(const Language::Token&);
std::string translate_alien_to_human_str(const std::string&);
std::string translate_alien_to_human_int(const std::string&);

// Public
namespace Language 
{
	const std::vector<char> ALIEN_TABLE = { 
		'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
		'0','1','2','3','4','5','6','7','8','9',
		'!','"','#','$','%','&','\'','(',')',
		'*','+',',','-','.','/',':',';',
		'<','=','>','?','@','[','\\',']','^','_','`','|','~',' ','\n' 
	};

	const std::string Encode(const std::string& human_string) {
		// -- Init --
		static std::map<char, char> HUMAN_TABLE;
		if (HUMAN_TABLE.empty()) {
			for (size_t i_alien_char = 0; i_alien_char < ALIEN_TABLE.size(); i_alien_char++) {
				HUMAN_TABLE[ALIEN_TABLE[i_alien_char]] = char(i_alien_char);
			}
		}

		// -- Encode as string --
		std::stringstream alien;
		alien << "S";

		for (size_t i = 0; i < human_string.size(); i++) {
			char human_code = human_string[i];
			alien << char(HUMAN_TABLE[human_code] + 33);
		}

		return alien.str();
	}

	const std::string Decode(const std::string& alien_string) {
		std::stringstream alien(alien_string);
		std::vector<Token> tokens;

		for (std::string alien_command; std::getline(alien, alien_command, ' ');)
		{
			if (alien_command.empty())
				continue;

			tokens.push_back({ 
				alien_command[0], 
				alien_command.substr(1) 
			});
		}

		return Eval(tokens);
	}

	const std::string Eval(const std::vector<Token>& tokens) {
		if (tokens.empty())
			return "";

		std::string human_str;

		//std::stack<Token> st;
		//st.push(tokens[0]);

		//while (!st.empty()) {
		//	Token current_token = st.top();
		//	st.pop();
		//}

		for (size_t i = 0; i < tokens.size();) {
			//i += Eval(tokens, i);
		//	switch (tokens[i].indicator)
		//	{
		//		case '?': 
		//		{
		//			
		//		} 
		//		break;

		//		case 'B':
		//		{
		//			switch (tokens[i].body[0])
		//			{
		//				case '>': 
		//				{
		//					
		//				}
		//				break;
		//			}
		//		}
		//		break;
		//	}
			std::string human = translate_alien_to_human(tokens[i]);
			human_str += human;
			i++;
			//std::cout << tokens[i].indicator << " " << tokens[i].body << std::endl;
		}

		return human_str;
	}
}

// - Helpers -
std::string translate_alien_to_human(const Language::Token& token) {
	switch (token.indicator)
	{
	case 'S': return translate_alien_to_human_str(token.body);
	case 'I': return translate_alien_to_human_int(token.body);
	case 'T': return "1";
	case 'F': return "0";

	default:
		std::cout << "token indicator " << token.indicator << " can't be evalualted this way." << endl; 
	}
	return "";
}

std::string translate_alien_to_human_str(const std::string& alien_str) {
	std::string result;

	for (size_t i = 0; i < alien_str.size(); i++) {
		char alien_code = alien_str[i];
		result += char(Language::ALIEN_TABLE[alien_code - 33]);
	}

	return result;
}

std::string translate_alien_to_human_int(const std::string& alien_str) {
	int integer = 0;

	for (size_t i = 0; i < alien_str.size(); i++) {
		integer *= 94;
		integer += (int)alien_str[i] - 33;
	}

	return std::to_string(integer);
}