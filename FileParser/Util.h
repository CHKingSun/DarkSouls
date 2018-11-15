//
// Created by KingSun on 2018/11/14
//

#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>
#include <locale>

class Util {
	static const std::vector<std::string> path_roots;

public:

	static std::string unrootPath(const std::string& path) {
		for (const auto& root : path_roots) {
			if (path.compare(0, root.length(), root) == 0) {
				return path.substr(root.length());
			}
		}

		if (path.find_first_of(':') != std::string::npos) {
			return path.substr(path.find_first_of(':') + 2);
		}

		return path;
	}

	static std::wstring shiftjisToUnicode(const std::string& str) {
		return std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>>
			(new std::codecvt_byname<wchar_t, char, std::mbstate_t>("jpn")).from_bytes(str);

		//auto p_wstr = new wchar_t[str.length() + 1];
		//std::size_t converted_num = 0;
		//_mbstowcs_s_l(&converted_num, p_wstr, str.length() + 1, str.data(), _TRUNCATE, _create_locale(LC_ALL, "jpn"));
		//std::wstring wstr(p_wstr);
		//delete[] p_wstr;
		//return wstr;
	}

	static std::string unicodeToUtf8(const std::wstring& wstr) {
		return std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>>
			(new std::codecvt_byname<wchar_t, char, std::mbstate_t>("")).to_bytes(wstr);

		//auto p_str = new char[wstr.length() * 2 + 1];
		//std::size_t converted_num = 0;
		//_wcstombs_s_l(&converted_num, p_str, wstr.length() * 2 + 1, wstr.data(), _TRUNCATE, _create_locale(LC_ALL, ""));
		//std::string str(p_str);
		//delete[] p_str;
		//return str;
	}

	static std::string getFileName(const std::string& path) {
		std::string::size_type s_index = path.find_last_of('\\');
		if (s_index == std::string::npos) s_index = path.find_last_of('/');
		if (s_index == std::string::npos) s_index = -1;
		std::string::size_type e_index = path.find_last_of('.');
		if (e_index == std::string::npos) e_index = path.length();
		return path.substr(s_index + 1, e_index - s_index - 1);
	}

	static std::string getFileSuffix(const std::string& path) {
		std::string::size_type index = path.find_last_of('.');
		if (index == std::string::npos) return path;
		return path.substr(index + 1);
	}

	static std::string getFileParent(const std::string& path) {
		std::string::size_type index = path.find_last_of('\\');
		if (index == std::string::npos) index = path.find_last_of('/');
		if (index == std::string::npos) return "/";
		return path.substr(0, index + 1);
	}
};

// Note: Remember to move the definition to cpp file
// if you have more than one cpp file.
const std::vector<std::string> Util::path_roots = {
	// Demon's Souls (duh)
	R"(N:\DemonsSoul\data\)",
	R"(N:\DemonsSoul\Source\)",
	R"(N:\DemonsSoul\)",
	// Dark Souls 1
	R"(N:\FRPG\data\)",
	R"(N:\FRPG\Source\)",
	R"(N:\FRPG\)",
	// Dark Souls 2
	R"(N:\FRPG2\data\)",
	R"(N:\FRPG2\Source\)",
	R"(N:\FRPG2\)",
	// Dark Souls 3
	R"(N:\FDP\data\)",
	R"(N:\FDP\Source\)",
	R"(N:\FDP\)",
	// Bloodborne
	R"(N:\SPRJ\data\)",
	R"(N:\SPRJ\Source\)",
	R"(N:\SPRJ\)",
	// Just because I don't like leading backslash
	R"(\)",
};

#endif // !UTIL_H

