//
// Created by KingSun on 2018/07/11
//

#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <iomanip>

class Log {
public:
	static void debug() {
		std::clog << std::endl;
	}

	template <typename T>
	static void debug(const T& value) {
		std::clog << value << std::endl;
		//std::clog << value;
		//debug();
	}

	template <typename T, typename... Ts>
	static void debug(const T& value, const Ts&... args) {
		std::clog << value;
		debug(args...);
	}

	static void info() {
		std::cout << std::endl;
	}

	template <typename T>
	static void info(const T& value) {
		std::cout << value << std::endl;
	}

	template <typename T, typename... Ts>
	static void info(const T& value, const Ts&... args) {
		std::cout << value;
		info(args...);
	}

	static void error() {
		std::cerr << std::endl;
	}

	template <typename T>
	static void error(const T& value) {
		std::cerr << value << std::endl;
	}

	template <typename T, typename... Ts>
	static void error(const T& value, const Ts&... args) {
		std::cerr << value;
		error(args...);
	}
};

#endif // !LOG_H

