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
#include <zlib.h>

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

static std::string getFileName(const std::string& path) {
	std::string::size_type s_index = path.find_last_of('\\');
	if (s_index == std::string::npos) s_index = path.find_last_of('/');
	if (s_index == std::string::npos) s_index = -1;
	std::string::size_type e_index = path.find_last_of('.');
	if (e_index == std::string::npos) e_index == path.length();
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

	bool skip(int length) { return !is.seekg(length, std::ios::cur).fail(); }

	bool skipToPadded() {
		uint pos = is.tellg();
		pos = ((pos + 3) >> 2) << 2;
		return !is.seekg(pos).fail();
	}

	bool read(byte* data, uint length) {
		return !is.read(data, length).fail();
	}

	bool readCompressedData(uint compr_len, byte* data, uint uncompr_len) {
		auto compr_data = new byte[uncompr_len];
		if(!read(compr_data, compr_len)) return false;

		z_stream d_stream;
		d_stream.zalloc = nullptr;
		d_stream.zfree = nullptr;
		d_stream.opaque = nullptr;

		d_stream.next_in = (ubyte*)compr_data;
		d_stream.avail_in = compr_len;

		if (inflateInit(&d_stream) != Z_OK) {
			Log::error("Inflate initial failed!");
			return false;
		}

		auto out_addr = (ubyte*)data;
		while (true) {
			//d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
			d_stream.next_out = out_addr;            /* discard the output */
			d_stream.avail_out = uncompr_len;
			int flag = inflate(&d_stream, Z_NO_FLUSH);
			if (flag == Z_STREAM_END) break;
			else if (flag != Z_OK) {
				Log::error("Inflate failed!");
				return false;
			}
		}

		if (inflateEnd(&d_stream) != Z_OK) {
			Log::error("Inflate exit failed!");
			return false;
		}

		return true;
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

	std::string readString(uint length) {
		std::vector<byte> data(length, '\0');
		is.read(data.data(), length);

		for (auto& b : data) b &= 0X7F;

		return std::string(data.begin(), data.end());
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
