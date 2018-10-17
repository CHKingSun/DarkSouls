//
// Created by KingSun on 2018/10/10
// Based on DS1Templates
//

#ifndef TPF_FILE_H
#define TPF_FILE_H

#include "BinaryFileReader.h"
#include "Log.h"

#pragma pack(4)

struct TpfHeader {
	byte signature[4];
	uint all_data_size;
	uint texture_count;
	byte version;
	byte unknown[3]; //Maybe for encode

	//TexHeader[texture_count]
};

struct DdsHeader {
	byte signature[4];
	int header_size;
	int flags;
	int height;
	int width;
	int pitch_or_linear_size;
	int depth;
	int mipmap_count;
	int reserved1[11];

	struct {
		int size;
		int flags;
		int four_cc;
		int rgb_bit_count;
		int r_bit_mask;
		int g_bit_mask;
		int b_bit_mask;
		int a_bit_mask;
	} pixel_format;

	int caps1;
	int caps2;
	int caps3;
	int caps4;
	int reserved2;
};

struct TexHeader {
	uint data_offset;
	uint data_size;
	byte format;
	byte type; //0 texture, 1 cubemap
	byte mipmap_count;
	byte flag;
	uint fliename_offset;
	uint unknown;

	//int old_pos = file.tell();
	//file.seek(fliename_offset);
	//std::string filename; //till nullptr
	//file.seek(old_pos);

	//int old_pos = file.tell();
	//file.seek(data_offset);
	//DdsHeader header; //Maybe
	//byte[data_size];
	//file.seek(old_pos);
};

#pragma  pack()

struct DdsData {
	std::string filename;
	std::vector<byte> data;

	DdsData(const std::string& filename, uint length) : filename(filename) { data.resize(length); }
};

class TpfFile {
	std::vector<DdsData> dds_datas;

	void readDdsData(BinaryFileReader& reader) {
		auto header = reader.readType<TexHeader>();
		Log::info("sizeof(TexHeader): ", sizeof(TexHeader));
		//header.data_offset = reader.readType<uint>();
		//header.data_size = reader.readType<uint>();
		//header.format = reader.readType<byte>();
		//header.type = reader.readType<byte>();
		//header.mipmap_count = reader.readType<byte>();
		//header.flag = reader.readType<byte>();
		//header.fliename_offset = reader.readType<uint>();
		//header.unknown = reader.readType<uint>();

		uint now_pos = reader.tell();
		reader.seek(header.fliename_offset);
		Log::info("File name offset: ", header.fliename_offset);
		DdsData dds(reader.readString(), header.data_size);
		dds.filename += ".dds";
		
		reader.seek(header.data_offset);
		Log::info("Data offset: ", header.data_offset);
		reader.read(dds.data.data(), dds.data.size());
		reader.seek(now_pos);

		dds_datas.emplace_back(dds);
	}

public:
	TpfFile(BinaryFileReader& reader) {
		auto header = reader.readType<TpfHeader>();
		Log::debug("sizeof(TpfHeader): ", sizeof(TpfHeader));
		//reader.read(header.signature, 4);
		//header.all_data_size = reader.readType<uint>();
		//header.texture_count = reader.readType<uint>();
		//header.version = reader.readType<byte>();
		//reader.read(header.unknown, 3);

		dds_datas.reserve(header.texture_count);

		for (uint i = 0; i < header.texture_count; ++i) readDdsData(reader);
	}

	void toDdsFile(const std::string& output_path) {
		for (const auto& dds : dds_datas) {
			std::ofstream os(output_path + '/' + dds.filename, std::ios::binary);
			os.write(dds.data.data(), dds.data.size());
		}
	}
};

#endif // !TPF_FILE_H
