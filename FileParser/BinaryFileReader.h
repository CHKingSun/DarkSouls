//
// Created by KingSun on 2018/10/10
//

#ifndef BINARY_FILE_READER_H
#define BINARY_FILE_READER_H

#include "Log.h"

#include <string>
#include <cstdlib>
#include <vector>
#include <fstream>

using byte = char;
using ubyte = unsigned char;
using uint = unsigned int;
using ushort = unsigned short int;

#pragma pack(4)

struct Byte4 {
	ubyte data[4];
};

struct Color {
	byte r;
	byte g;
	byte b;
	byte a;
};

//unsigned short vector2
struct Vec2 {
	ushort x;
	ushort y;

	float getX()const { return (float)(x & 0X7FFF) / 2048.f; }
	float getY(bool convert = true)const {
		if(convert) return 1.f - (float)(y & 0X7FFF) / 2048.f;
		else return (float)(y & 0X7FFF) / 2048.f;
	}
};

struct Vec3 {
	float x;
	float y;
	float z;
};

struct PackedNormal {
	ubyte x;
	ubyte y;
	ubyte z;
	ubyte padding; //no use

	float getX()const { return (x - 127.f) / 127.f; }
	float getY()const { return (y - 127.f) / 127.f; }
	float getZ()const { return (z - 127.f) / 127.f; }
};

struct PackedTangent {
	ubyte x;
	ubyte y;
	ubyte z;
	ubyte w;

	float getX()const { return (x - 127.f) / 127.f; }
	float getY()const { return (y - 127.f) / 127.f; }
	float getZ()const { return (z - 127.f) / 127.f; }
	float getW()const { return (w - 127.f) / 127.f; }
};

static std::string wstrToStr(const std::wstring& wstr) {
	auto p_str = new char[wstr.size() * 2 + 1];
	std::wcstombs(p_str, wstr.data(), wstr.size() * 2 + 1);
	std::string str(p_str);
	delete[] p_str;
	return str;
}

static std::wstring strToWStr(const std::string& str) {
	auto p_wstr = new wchar_t[str.size() + 1];
	std::mbstowcs(p_wstr, str.data(), str.size() + 1);
	std::wstring wstr(p_wstr);
	delete[] p_wstr;
	return wstr;
}

//note: cannot deal with no '/' or '\' and no '.' path
static std::string getFileName(const std::string& path) {
	uint index = path.find_last_of('\\');
	if (index == std::string::npos) index = path.find_last_of('/');
	return path.substr(index + 1, path.find_last_of('.') - index - 1);
}

//note: cannot deal with no '/' or '\' and no '.' path
static std::string getFileParent(const std::string& path) {
	uint index = path.find_last_of('\\');
	if (index == std::string::npos) index = path.find_last_of('/');
	return path.substr(0, index + 1);
}

class BinaryFileReader {
	std::ifstream is;

public:
	BinaryFileReader(const std::string& filename) {
		is.open(filename, std::ios::binary);
		if (!is.is_open()) Log::info("File open failed!");
	}

	bool is_exist() { return is.is_open(); }

	bool seek(uint offset) { return !is.seekg(offset).fail(); }

	uint tell() { return is.tellg(); }

	void skip(int length) { is.seekg(length, std::ios::cur); }

	bool read(byte* data, int length) {
		return !is.read(data, length).fail();
	}

	template <typename Type>
	Type readType() {
		union {
			byte byte_data[sizeof(Type)];
			Type type_data;
		} data;

		is.read(data.byte_data, sizeof(Type));
		return data.type_data;
	}

	//read a string with '\0' end
	std::string readString() {
		std::string data;
		char ch;
		is.get(ch);
		while (ch != '\0') {
			data.push_back(ch);
			is.get(ch);
		}

		return data;
	}

	std::string readWString() {
		std::wstring data;
		wchar_t ch;
		ch = readType<wchar_t>();
		while (ch != '\0') {
			data.push_back(ch);
			ch = readType<wchar_t>();
		}

		return wstrToStr(data);
	}
};

#pragma pack()

#endif // !BINARY_FILE_READER_H
