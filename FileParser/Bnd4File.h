//
// Created by KingSun on 2018/10/31
// Based on BinderTool's Bnd4File
//

#ifndef BND4_FILE_H
#define BND4_FILE_H

#include "BinaryFileReader.h"

#include <algorithm>
#include <cassert>

// #define assert(expression) ((void)0)

#pragma pack(4)

struct Bnd4Header {
	byte signature[4]; // BND4
	uint unknown1[2]; // 0, 0X00010000(65536)
	uint file_count;
	uint unknown2[2]; // 0X00000040(64), 0
	byte version[8];
	uint offset_struct_size; // 36
	uint unknown3; // 0
	uint data_offset;
	uint unknown4; // 0
	byte encoding; // 0 => string, 1 => wstring
	byte skip[15]; // skip[0] = 't' or 'T', other = '\0'

	bool operator()() {
		return unknown1[0] == 0
			&& unknown1[1] == 0X00010000
			&& unknown2[0] == 0X00000040
			&& unknown2[1] == 0
			&& offset_struct_size == 36
			&& unknown3 == 0
			&& unknown4 == 0
			&& (encoding == '\0' || encoding == '\1')
			&& (skip[0] == 't' || skip[0] == 'T')
			&& std::all_of(skip + 1, skip + 15, [](const byte& b) { return b == '\0'; })
			;
	}
};

struct OffsetInfo {
	uint unknown1[2];
	uint file_size;
	uint unknown2;

	uint file_offset;
	uint filename_offset;
};

struct Offset36Info {
	uint unknown1[2]; // 0X00000040, 0XFFFFFFFF
	uint file_size;
	uint unknown2; // 0

	// the next 36 byte
	uint unknown3[2]; //[0] = file_size(maybe), [1] = unknown2
	uint file_offset;
	uint file_index;
	uint filename_offset;
};

#pragma pack()

struct FileData {
	std::string filename;
	std::vector<byte> data;

	FileData(uint f_size, std::string&& f_name) :
		data(f_size), filename(std::move(f_name)) {}
};

class Bnd4File {
	Bnd4Header bnd4_header;

	std::vector<FileData> files_data;

	template <typename TInfo>
	void getFileInfo(BinaryFileReader& reader) {
		files_data.reserve(bnd4_header.file_count);

		std::vector<TInfo> offset_infos;
		offset_infos.reserve(bnd4_header.file_count);

		for (uint i = 0; i < bnd4_header.file_count; ++i)
			offset_infos.emplace_back(reader.readType<TInfo>());

		uint old_pos = reader.tell();
		for (const auto& offset_info : offset_infos) {
			reader.seek(offset_info.filename_offset);
			if(bnd4_header.encoding == 0) 
				files_data.emplace_back(offset_info.file_size, reader.readString());
			else
				files_data.emplace_back(offset_info.file_size, reader.readWString());

			reader.seek(offset_info.file_offset);
			reader.read(files_data.back().data.data(), files_data.back().data.size());
		}
		reader.seek(old_pos);
	}

public:
	Bnd4File(BinaryFileReader& reader) {
		bnd4_header = reader.readType<Bnd4Header>();
		assert(bnd4_header());

 		if (bnd4_header.offset_struct_size == 36) getFileInfo<Offset36Info>(reader);
 		else getFileInfo<OffsetInfo>(reader);
	}

	void unpack(const std::string& output_dir) {
		for (const auto& file : files_data) {
			std::ofstream os(output_dir + '/' + file.filename, std::ios::binary);
			os.write(file.data.data(), file.data.size());
		}
	}
};

#endif // !BND4_FILE_H
