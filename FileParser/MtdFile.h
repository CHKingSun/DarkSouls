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
};

#pragma pack()

class MtdFile {
	MtdHeader mtd_header;

	std::string original_filename;
	std::string description;

	std::string readNextString(BinaryFileReader& reader) {
		auto length = reader.readType<uint>();
		std::string text = reader.readString(length);
		reader.skipToPadded();

		return text;
	}

public:
	MtdFile(BinaryFileReader& reader) {
		mtd_header = reader.readType<MtdHeader>();

		original_filename = readNextString(reader);
		description = readNextString(reader);


		//TODO
	}
};

#endif // !MTD_FILE_H
