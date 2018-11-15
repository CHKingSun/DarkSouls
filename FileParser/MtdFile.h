//
// Created by KingSun on 2018/10/10
// Based on DSI Templates
//

#ifndef MTD_FILE_H
#define MTD_FILE_H

#include "BinaryFileReader.h"

#pragma pack(4)

struct MtdHeader {
	uint unknown1; // 0
	uint file_size;
	
	uint unknown2[9]; // 0, 3, unk, 0, 0X1C, 1, 2, unk, 4
	byte signature[4]; // 'MTD '
	uint unknown3[4]; // unk, 0X3E8, unk, 0

	uint data_size;
	uint unknown4[3]; // 2, 4, unk

	bool operator()() {
		return unknown1 == 0
			&& unknown2[0] == 0
			&& unknown2[1] == 3
			&& unknown2[3] == 0
			&& unknown2[4] == 0X1C
			&& unknown2[5] == 1
			&& unknown2[6] == 2
			&& unknown2[8] == 4
			&& unknown3[1] == 0X03E8
			&& unknown3[3] == 0
			&& unknown4[0] == 2
			&& unknown4[1] == 4;
	}
};

#pragma pack()

enum ParamType{
	BOOL, FLOAT, FLOAT2, FLOAT3, INT, INT2
};

struct ParamMsg {
	std::string param_name;
	ParamType type;

	union {
		struct {
			bool bool_value;
		};
		struct {
			int int_value[2];
		};
		struct {
			float  float_value[3];
		};
	} data;
};

struct TexMsg {
	std::string tex_name;
	uint shader_data_index;
};

class MtdFile {
	MtdHeader mtd_header;

	std::string original_filename;
	std::string description;

	std::vector<ParamMsg> params;
	std::vector<TexMsg> textures;

	std::string readNextString(BinaryFileReader& reader, byte end) {
		auto length = reader.readType<uint>();
		std::string text = reader.readString(length);
		text = Util::unicodeToUtf8(Util::shiftjisToUnicode(text));
		assert(reader.readType<byte>() == end);
		reader.skipToPadded();

		return text;
	}

	ParamMsg readParamMsg(BinaryFileReader& reader) {
		ParamMsg param;
		
		//TODO

		return param;
	}

	TexMsg readTexMsg(BinaryFileReader& reader) {
		TexMsg texture;

		//TODO

		return texture;
	}

public:
	MtdFile(BinaryFileReader& reader) {
		mtd_header = reader.readType<MtdHeader>();
		assert(mtd_header());

		original_filename = readNextString(reader, 0xA3);
		description = readNextString(reader, 0x03);

		assert(reader.readType<uint>() == 1);
		assert(reader.readType<uint>() == 0);

		auto param_size = reader.readType<uint>();

		assert(reader.readType<uint>() == 3);
		assert(reader.readType<uint>() == 4);
		reader.readType<uint>(); // unk
		assert(reader.readType<uint>() == 0);
		reader.readType<uint>(); // unk

		auto param_count = reader.readType<uint>();
		params.reserve(param_count);
		for (uint i = 0; i < param_count; ++i)
			params.emplace_back(readParamMsg(reader));

		reader.readType<uint>(); // unk

		auto tex_count = reader.readType<uint>();
		textures.reserve(tex_count);
		for (uint i = 0; i < tex_count; ++i)
			textures.emplace_back(readTexMsg(reader));

		reader.readType<uint>(); // unk
		assert(reader.readType<uint>() == 0);
		reader.readType<uint>(); // unk
		assert(reader.readType<uint>() == 0);
		reader.readType<uint>(); // unk
		assert(reader.readType<uint>() == 0);
	}
};

#endif // !MTD_FILE_H
