//
// Created by KingSun on 2018/10/10
//

#include "BinaryFileReader.h"
#include "TpfFile.h"
#include "FlverFile.h"
#include "Bnd4File.h"

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
	//std::string filename("F:/BinderTool/Game/GameDataEbl/model/chr/output/c6800/c6800.flver");
 //	//std::string output_path = "F:/BinderTool/Game/GameDataEbl/model/chr/obj/";
 //	//std::experimental::filesystem::create_directories(output_path);
 //	//std::ifstream is("F:/BinderTool/Game/GameDataEbl/model/chr/output/flver_file.txt");
	////while (std::getline(is, filename)) {
	//	Log::info("filename: ", filename);
	//	BinaryFileReader reader(filename);
	//	if (reader.is_exist()) {
	//		FlverFile flver(reader);
	//		flver.toObj("C:/Users/KingSun/Desktop/", getFileName(filename));
	//		//flver.toObj(getFileParent(filename), getFileName(filename));
	//	}
	////}

	//for (const auto& p : p_data) p();
	//for (const auto& name : types) Log::info(name);

	std::string filename("F:/BinderTool/Game/FlverShader.bnd");
	//filename = "F:/BinderTool/Game/GameDataEbl/model/chr/c6800.bnd";
	//std::ifstream is("F:/BinderTool/Game/bnd4_file.txt");
	//while (std::getline(is, filename)) {
		Log::info("filename: ", filename);
		BinaryFileReader reader(filename);
		if (reader.is_exist()) {
			Bnd4File bnd4(reader);
			bnd4.unpack("C:/Users/KingSun/Desktop/");
		}
	//}

	Log::info("Parse completed!");
#endif

	std::cin.get();
}