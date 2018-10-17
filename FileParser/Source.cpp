//
// Created by KingSun on 2018/10/10
//

#include "BinaryFileReader.h"
#include "TpfFile.h"
#include "FlverFile.h"

#include <filesystem>

int main() {
#if 1
	std::string filename = "your flver file name";
	std::string output_path = "you output path";
	std::string name = "output name (no suffix)";
	BinaryFileReader reader(filename);
	if (reader.is_exist()) {
		FlverFile flver(reader);
		flver.toObj(output_path, name);
	}
#else
	std::string filename("F:/BinderTool/Game/GameDataEbl/model/chr/output/c6800/c6800.flver");
 	std::string output_path = "F:/BinderTool/Game/GameDataEbl/model/chr/obj/";
 	std::experimental::filesystem::create_directories(output_path);
 	std::ifstream is("F:/BinderTool/Game/GameDataEbl/model/chr/output/flver_file.txt");
	while (std::getline(is, filename)) {
		Log::info("filename: ", filename);
		BinaryFileReader reader(filename);
		if (reader.is_exist()) {
			FlverFile flver(reader);
			//flver.toObj("C:/Users/KingSun/Desktop/", getFileName(filename));
			flver.toObj(output_path, getFileName(filename));
		}
	}

	for (const auto& p : p_data) p();
	for (const auto& name : types) Log::info(name);
#endif

	std::cin.get();
}