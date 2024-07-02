#include "Language.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

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
			alien << char(HUMAN_TABLE[human_string[i]] + 33);
		}

		return alien.str();
	}

	const std::string Decode(const std::string& alien_string) {
		std::stringstream alien(alien_string);
		std::stringstream human;

		for (std::string alien_command; std::getline(alien, alien_command, ' ');)
		{
			if (alien_command.empty())
				continue;

			const char token = alien_command[0];

			switch (token)
			{

			// -- Decode as string --
			case 'S': 
				for (size_t i = 1; i < alien_command.size(); i++) {
					char alien_code = alien_command[i];
					if (alien_code < 33 || alien_code > ALIEN_TABLE.size() + 33) {
						human << "_";
						continue;
					}
					human << char(ALIEN_TABLE[alien_code - 33]);
				}
				break;
			
			
			default: 
				std::cout << "Unknown token: " << token << std::endl;
				continue;
			}
		}

		return human.str();
	}

}