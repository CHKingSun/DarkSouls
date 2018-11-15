//
// Created by KingSun on 2018/10/10
// Based on DSI Templates
//

#ifndef FLVER_FILE_H
#define FLVER_FILE_H

#include "BinaryFileReader.h"

#include <algorithm>
#include <set>
#include <cassert>

//#define assert(expression) ((void)0)

#pragma pack(4)

//TODO problem with value and unknown_id value
struct HitBox {
	Vec3 row1;
	ushort unknown_id1; //Most of it is 0, 33023(80FF), 51300(C864), 25700(6464), 255(00FF)
	ushort unknown_id2; //Most of it is 0XFF00 65280
	Vec3 row2;
	ushort unknown_id3;
	ushort unknown_id4;
	Vec3 row3;
	ushort unknown1; //Always 0XFFFF
	ushort unknown2; //Always 0X0100
	byte skip[16];

	bool operator()() {
		//if (unknown_id1 != 0) Log::info("hit_box.unknown_id1 is ", unknown_id1);
		//if (unknown_id2 != 0XFF00) Log::info("hit_box.unknown_id2 is ", unknown_id2);
		return unknown1 == 0XFFFF
			&& unknown2 == 0X0100
			&& std::all_of(skip, skip + 16, [](const byte& a) { return a == 0; });
	}
};

struct MaterialHeader {
	uint mtl_name_offset;
	uint mtd_filename_offset;
	uint param_count;
	uint param_start_index;
	uint flag; //Maybe use for texture transparency
	uint unknown1;
	uint unknown2; //Always 0
	uint unknown3; //Always 0

	bool operator()() {
		return unknown2 == 0 && unknown2 == 0;
	}
};

struct BoneHeader {
	Vec3 translation;
	uint filename_offset;
	Vec3 euler_radian;
	ushort parent_index;
	ushort first_child_index;
	Vec3 scale;
	ushort first_sibling_index;
	ushort copy_of_index;
	Vec3 bounder_lower;
	ushort is_nub; //Always 0
	ushort unknown1; //Always 0
	Vec3 bounder_upper;
	ushort unknown2; //Always 0
	ushort unknown3; //Always 0
	uint unknown4[12]; //Always 0

	bool operator()() {
		return is_nub == 0
			&& unknown1 == 0
			&& unknown2 == 0
			&& unknown3 == 0
			&& std::all_of(unknown4, unknown4 + 12, [](const uint& a) { return a == 0; });
	}
};

struct FaceSetHeader {
	uint flag; //0x00=Normal, 0x01/0x02/0x04/0x08=Not Visible, 0x10=Normal?, 0x80="Reptile" Transparency
	ubyte topology; //Always 0, 0 => triangles, 1 => strip
	ubyte cull_backface; //0 || 1
	ubyte unknown1;
	ubyte unknown2; //Always 0 
	uint index_count;
	uint index_buffer_offset;
	uint index_buffer_size;
	uint unknown3; //Always 0;
	uint unknown4; //Always 0;
	uint unknown5; //Always 0; 

	bool operator()() {
		//Log::info("unknown1: ", (uint)unknown1);
		return index_buffer_size == index_count * sizeof(ushort)
			&& topology == 0
			&& (cull_backface == 0 || cull_backface == 1)
			&& unknown2 == 0
			&& unknown3 == 0
			&& unknown4 == 0
			&& unknown5 == 0;
	}
};

struct MeshHeader {
	int is_dynamic;

	uint material_index;
	byte unknown1[8]; // Always 0

	uint default_bone_header_index;
	uint bone_header_indices_count;
	uint unknown2; // Always 0;
	uint bone_header_indices_offset;

	// Most of it is 3.
	// We will not use it. In ordered.
	uint faceset_header_indices_count;
	uint faceset_header_indices_offset;

	// Most of it is 1.
	// We will not use it. In ordered.
	uint vertex_header_indices_count;
	uint vertex_header_indices_offset;

	bool operator()() {
		return std::all_of(unknown1, unknown1 + 8, [](const byte& a) {return a == 0; })
			&& unknown2 == 0;
	}
};

struct VertexHeader {

	//The vertex header index in mesh.
	//Most of it is 0 for most meshes just have one vertex header. 
	uint index;
	uint vertex_descriptor_index; // vertex_info_index
	uint vertex_size;
	uint vertex_count;
	uint unknown2; //Always 0
	uint unknown3; //Always 0
	uint vertex_buffer_size;
	uint vertex_buffer_offset;

	bool operator()() {
		return unknown2 == 0 && unknown3 == 0
			&& vertex_size * vertex_count == vertex_buffer_size;
	}
};

struct StreamDescriptor {
	uint unknown1; //Always 0
	uint offset;

	uint unknown2; //Maybe related to type

	uint data_type;

	//The index in vertex data.
	//Most of it is 0 for every type in vertex data just has one.
	uint index;

	bool operator()() {
		return unknown1 == 0;
	}
};

struct VertexDescriptor {
	uint stream_descriptor_count;
	uint unknown1; //Always 0
	uint unknown2; //Always 0
	uint offset;

	//int old_pos = file.tell();
	//file.seek(offset);
	//uint stream_descriptors[stream_descriptor_count];
	//file.seek(old_pos);

	bool operator()() {
		return unknown1 == 0 && unknown2 == 0;
	}
};

//struct BoneIndices { byte bone_indices[4]; };
//struct BoneWeights { ushort bone_weight[4]; };

struct MaterialParameter {
	uint filename_offset;
	uint type_name_offset;
	union { uint uint_type; float float_type; } unknown1; //Most of it is 1.f
	union { uint uint_type; float float_type; } unknown2; //Most of it is 1.f
	uint unknown3; //Some sort of flags
	uint unknown4[3]; //Always 0

	bool operator()() {
		//if (unknown1.float_type != 1.f) Log::info("mat_params.unknown1 is ", unknown1.float_type);
		//if (unknown2.float_type != 1.f) Log::info("mat_params.unknown2 is ", unknown2.float_type);
		return (unknown1.uint_type & 0XFFFF) == 0
			&& (unknown2.uint_type & 0XFFFF) == 0
			&& unknown4[0] == 0
			&& unknown4[1] == 0
			&& unknown4[2] == 0;
	}
};

struct FlverHeader {
	byte signature[6]; //FLVER
	byte endian[2]; //L => little endian
	ushort version_minor;
	ushort version_major;
	
	uint data_offset;
	uint data_size;

	uint hitbox_header_count;
	uint material_header_count;
	uint bone_header_count;
	uint mesh_header_count;
	uint vertex_header_count;

	Vec3 bounder_lower; //Maybe
	Vec3 bounder_upper; //Maybe

	uint unknown1;  //Optional? Setting to 0 doesn't seem to do anything!
	uint unknown2;  //Optional? Setting to 0 doesn't seem to do anything!
	uint unknown3;  //Setting to 0 makes weapon invisible
	uint unknown4;

	uint faceset_header_count;
	uint vertex_descriptor_count;
	uint material_param_count;

	uint unknown5[9]; //Always 1 or 0

	bool operator()() {
		//if (!std::all_of(unknown5 + 1, unknown5 + 9, [](const uint& a) { return a == 0; }))
		//	Log::info("flver_header.unknown5 may be not correct.");
		return unknown4 == 0 && std::all_of(unknown5, unknown5 + 9, [](const uint& a) { return a == 0 || a == 1; });
	}
};

#pragma pack()

struct MeshInfo {
	MeshHeader mesh_header;
	std::vector<FaceSetHeader> face_set_headers;
	std::vector<VertexHeader> vertex_headers;
};

struct VertexInfo {
	VertexDescriptor vertex_descriptor;
	std::vector<StreamDescriptor> stream_descriptors;
};

// Based on material header
struct MaterialInfo {
	std::string mtl_name;
	std::string mtd_name;
	uint param_count;
	uint param_start_index;
};

//struct FaceData {
//	bool draw_type; //false => triangles, true => strip
//	std::vector<ushort> face;
//
//	FaceData(ubyte draw_type, std::vector<ushort>&& face) : draw_type(draw_type), face(face) {}
//};

// Based on material parameter
struct MaterialData {
	std::string filename;

	//g_BlendMaskTexture
	//g_BreakBumpmapTexture
	//g_BumpmapTexture
	//g_BumpmapTexture2
	//g_DetailBumpmapTexture
	//g_DetailBumpmapTexture2
	//g_DiffuseTexture
	//g_DiffuseTexture2
	//g_EmissiveTexture
	//g_FlowMapTexture
	//g_HighlightTexture
	//g_SpecularTexture
	//g_SpecularTexture2
	//g_VectorTexture
	std::string type_name;

	//Maybe have other data.
};

struct VertexData {
	//Maybe all data have just one or zero
	//unknown2: 0X02, data_type: 0X00, index: 0, data size: 12
	Vec3 position; //position

	//unknown2: 0X13, data_type: 0X01, index: 0, data size: 04
	Byte4 data_1;
	//unknown2: 0X11, data_type: 0X02, index: 0, data size: 04
	Byte4 data_2;
	//unknown2: 0X11, data_type: 0X03, index: 0, data size: 04
	Byte4 data_3;

	//unknown2: 0X15, data_type: 0X05, index: 0, data size: 04
	//unknown2: 0X15, data_type: 0X05, index: 1, data size: 04
	//unknown2: 0X13, data_type: 0X05, index: 2, data size: 04
	//unknown2: 0X13, data_type: 0X05, index: 3, data size: 04
	Vec2 diffuse_uv[4]; //Most of it just has one

	//unknown2: 0X11, data_type: 0X06, index: 0, data size: 04
	//unknown2: 0X11, data_type: 0X06, index: 1, data size: 04
	Byte4 data_6[2];
	//unknown2: 0X13, data_type: 0X0A, index: 0, data size: 04
	//unknown2: 0X13, data_type: 0X0A, index: 1, data size: 04
	//unknown2: 0X13, data_type: 0X0A, index: 2, data size: 04
	Byte4 data_A[3];

	//std::vector<BoneIndices> bone_indices; // 0X11
	//std::vector<PackedNormal> normals; // 0X13 => type & 0X3 => data_type
	//std::vector<PackedTangent> bitangents; // 0X13 => type & 0X6 => data_type
	//std::vector<Color> colors; // 0X13 => type & 0XA => data_type
	//std::vector<Vec2> lightmap_uvs; // 0X16
	//std::vector<BoneWeights> bone_weights; // 0X1A
};

struct MeshData {
	ubyte data_used[12]; //Save the information which vertex data we can get, one byte for padding.
	uint material_info_index;

	std::vector<ushort> faces;
	std::vector<VertexData> vertices;
};

// Get the data type and data count information in stream descriptor
// For test
struct Pair {
	uint unknown2;
	uint data_type;
	uint index;
	uint data_size;

	void operator()()const {
		printf("unknown2: 0X%02X, data_type: 0X%02X, index: %d, data size: %02d\n", unknown2, data_type, index, data_size);
	}
};
bool operator<(const Pair& p1, const Pair& p2) {
	return p1.data_type < p2.data_type || (p1.data_type == p2.data_type && p1.index < p2.index)
		|| (p1.data_type == p2.data_type && p1.index == p2.index && p1.unknown2 < p2.unknown2);
}
std::set<Pair> p_data;
std::set<std::string> types;
ubyte all_data_count[12]{ 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF };

class FlverFile {
	FlverHeader flver_header;
	std::vector<HitBox> hit_boxes;
	std::vector<MaterialHeader> mat_headers;
	std::vector<BoneHeader> bone_headers;

	std::vector<MeshInfo> mesh_infos;
	std::vector<VertexInfo> vertex_infos;
	std::vector<MaterialInfo> mat_infos;

	std::vector<MaterialData> mat_data;
	std::vector<MeshData> meshes;

	void readMeshInfo(BinaryFileReader& reader) {

		// Read MeshHeader
		mesh_infos.resize(flver_header.mesh_header_count);
		for (auto& mesh_info : mesh_infos) {
			mesh_info.mesh_header = reader.readType<MeshHeader>();
			assert(mesh_info.mesh_header());
		}

		// Read all FaceSetHeader
		// Note: all the faceset_header_indices in meshes_headers are in ordered(not just for one mesh_header),
		// so we don't use the index to get faceset_header. We just save it in our mesh_info data.
		uint faceset_header_count = 0;
		for (auto& mesh_info : mesh_infos) {
			mesh_info.face_set_headers.reserve(mesh_info.mesh_header.faceset_header_indices_count);
			faceset_header_count += mesh_info.mesh_header.faceset_header_indices_count;
			for (uint i = 0; i < mesh_info.mesh_header.faceset_header_indices_count; ++i) {
				mesh_info.face_set_headers.emplace_back(reader.readType<FaceSetHeader>());
				assert(mesh_info.face_set_headers.back()());
			}
		}
		assert(faceset_header_count == flver_header.faceset_header_count);

		// Read all FaceSetHeader
		uint vertex_header_count = 0;
		for (auto& mesh_info : mesh_infos) {
			mesh_info.vertex_headers.reserve(mesh_info.mesh_header.vertex_header_indices_count);
			vertex_header_count += mesh_info.mesh_header.vertex_header_indices_count;
			for (uint i = 0; i < mesh_info.mesh_header.vertex_header_indices_count; ++i) {
				mesh_info.vertex_headers.emplace_back(reader.readType<VertexHeader>());
				assert(mesh_info.vertex_headers.back()());
			}
		}
		assert(vertex_header_count == flver_header.vertex_header_count);

		//Just for test, maybe later will be used.
		uint old_pos = reader.tell();
		std::vector<uint> bone_header_indices;
		std::vector<uint> faceset_header_indices;
		std::vector<uint> vertex_header_indices;
		for (const auto& mesh_info : mesh_infos) {
			reader.seek(mesh_info.mesh_header.bone_header_indices_offset);
			for (uint i = 0; i < mesh_info.mesh_header.bone_header_indices_count; ++i)
				bone_header_indices.emplace_back(reader.readType<uint>());

			reader.seek(mesh_info.mesh_header.faceset_header_indices_offset);
			for (uint i = 0; i < mesh_info.mesh_header.faceset_header_indices_count; ++i)
				faceset_header_indices.emplace_back(reader.readType<uint>());

			reader.seek(mesh_info.mesh_header.vertex_header_indices_offset);
			for (uint i = 0; i < mesh_info.mesh_header.vertex_header_indices_count; ++i)
				vertex_header_indices.emplace_back(reader.readType<uint>());
		}
		// The data in faceset_header_indices and vertex_header_indices are in ordered, start form 0.
		reader.seek(old_pos);
	}

	void readVertexInfo(BinaryFileReader& reader) {

		// Read VertexDescriptor
		vertex_infos.resize(flver_header.vertex_descriptor_count);
		for (auto& vertex_info : vertex_infos) {
			vertex_info.vertex_descriptor = reader.readType<VertexDescriptor>();
			assert(vertex_info.vertex_descriptor());
		}

		// Read StreamDescriptor
		uint old_pos = reader.tell();
		for (auto& vertex_info : vertex_infos) {
			vertex_info.stream_descriptors.reserve(vertex_info.vertex_descriptor.stream_descriptor_count);
			reader.seek(vertex_info.vertex_descriptor.offset);
			for (uint i = 0; i < vertex_info.vertex_descriptor.stream_descriptor_count; ++i)
				vertex_info.stream_descriptors.emplace_back(reader.readType<StreamDescriptor>());
		}
		reader.seek(old_pos);

		//Just for test.
		ubyte data_count[11];
		for (const auto& vertex_info : vertex_infos) {
			//prove that the offset of the start stream descriptor is 0
			assert(vertex_info.vertex_descriptor.stream_descriptor_count == 0
				|| vertex_info.stream_descriptors[0].offset == 0);
			//prove that each type in any vertex info just has one
			memset(data_count, 0, 11);
			for (const auto& stream : vertex_info.stream_descriptors) {
				assert(!(data_count[stream.data_type] & (1 << stream.index)));
				data_count[stream.data_type] |= (1 << stream.index);
			}

			uint i = 0;
			//prove that the data index is in order.
			//Also all the stream are continuous storage.
			for (; i < vertex_info.vertex_descriptor.stream_descriptor_count - 1; ++i) {
				assert(i == 0 || vertex_info.stream_descriptors[i].index == 0
					|| (vertex_info.stream_descriptors[i - 1].index == 0 && vertex_info.stream_descriptors[i].index == 1)
					|| (vertex_info.stream_descriptors[i - 1].index == 1 && vertex_info.stream_descriptors[i].index == 2));

				p_data.insert({ vertex_info.stream_descriptors[i].unknown2, vertex_info.stream_descriptors[i].data_type,
					vertex_info.stream_descriptors[i].index,
					vertex_info.stream_descriptors[i + 1].offset - vertex_info.stream_descriptors[i].offset });
			}
			assert(vertex_info.stream_descriptors[i].index == 0
				|| (vertex_info.stream_descriptors[i - 1].index == 0 && vertex_info.stream_descriptors[i].index == 1)
				|| (vertex_info.stream_descriptors[i - 1].index == 1 && vertex_info.stream_descriptors[i].index == 2)
				|| (vertex_info.stream_descriptors[i - 1].index == 2 && vertex_info.stream_descriptors[i].index == 3));
			p_data.insert({ vertex_info.stream_descriptors.back().unknown2, vertex_info.stream_descriptors.back().data_type,
				vertex_info.stream_descriptors.back().index, 4 });
			//if (vertex_info.stream_descriptors.back().index == 3) {
			//	Log::info("Index is 3");
			//}
		}
	}

	void readMaterial(BinaryFileReader& reader) {

		// Read MaterialParameter
		std::vector<MaterialParameter> mat_params;
		mat_params.reserve(flver_header.material_param_count);
		for (uint i = 0; i < flver_header.material_param_count; ++i) {
			mat_params.emplace_back(reader.readType<MaterialParameter>());
			assert(mat_params.back()());
		}

		uint old_pos = reader.tell();
		
		mat_infos.reserve(mat_headers.size());
		for (const auto& mat_header : mat_headers) {
			MaterialInfo info;
			info.param_count = mat_header.param_count;
			info.param_start_index = mat_header.param_start_index;
			reader.seek(mat_header.mtl_name_offset);
			info.mtl_name = reader.readWString();
			reader.seek(mat_header.mtd_filename_offset);
			info.mtd_name = reader.readWString();
			mat_infos.emplace_back(std::move(info));
		}

		mat_data.reserve(mat_params.size());
		for (const auto& param : mat_params) {
			MaterialData data;
			reader.seek(param.filename_offset);
			data.filename = reader.readWString();
			reader.seek(param.type_name_offset);
			data.type_name = reader.readWString();
			types.insert(data.type_name);
			mat_data.emplace_back(std::move(data));
		}

		reader.seek(old_pos);
	}

	void readMesh(BinaryFileReader& reader) {
		meshes.reserve(flver_header.mesh_header_count);
		for (auto& mesh_info : mesh_infos) {
			MeshData mesh;
			//prove that the mesh has data.
			assert(mesh_info.mesh_header.faceset_header_indices_count != 0 && mesh_info.mesh_header.vertex_header_indices_count != 0);

			mesh.material_info_index = mesh_info.mesh_header.material_index;

			// Note: faceset_header.topology is always 0
			// so we push all the face indices into one container.
			uint face_indices_count = 0;
			for (auto& face_set : mesh_info.face_set_headers) face_indices_count += face_set.index_count;
			mesh.faces.reserve(face_indices_count);
			for (auto& face_set : mesh_info.face_set_headers) {
				reader.seek(flver_header.data_offset + face_set.index_buffer_offset);
				for (uint i = 0; i < face_set.index_count; ++i)
					mesh.faces.emplace_back(reader.readType<ushort>());
			}

			//prove that each type in the vertex data just has one
			//prove that vertex count is the same.
			memset(mesh.data_used, 0, 12);
			uint vertex_count = mesh_info.vertex_headers[0].vertex_count;
			for (const auto& vert_header : mesh_info.vertex_headers) {
				assert(vert_header.vertex_count == vertex_count);
				uint data_size = 0;
				for (const auto& stream : vertex_infos[vert_header.vertex_descriptor_index].stream_descriptors) {
					assert(!(mesh.data_used[stream.data_type] & (1 << stream.index)));
					mesh.data_used[stream.data_type] |= (1 << stream.index);
					if (stream.data_type == 0X00) data_size += 12;
					else data_size += 4;
				}
				assert(vert_header.vertex_buffer_size == data_size * vertex_count);
			}
			for (uint i = 0; i < 12; ++i) all_data_count[i] &= mesh.data_used[i];

			// Note: most mesh_info just has one vertex_header.
			// Sometimes you will get two vertex_header in one mesh_info,
			// but you can merge it into one VertexData.
			// Proof as above.
			mesh.vertices.resize(mesh_info.vertex_headers[0].vertex_count);
			for (const auto& vert_header : mesh_info.vertex_headers) {
				reader.seek(flver_header.data_offset + vert_header.vertex_buffer_offset);
				readVertex(reader, vertex_infos[vert_header.vertex_descriptor_index], mesh.vertices);
			}

			meshes.emplace_back(std::move(mesh));
		}
	}

	void readVertex(BinaryFileReader& reader, const VertexInfo& vertex_info, std::vector<VertexData>& vertex_data) {
		for (auto& data : vertex_data) {
			for (const auto& stream : vertex_info.stream_descriptors) {
				switch (stream.data_type)
				{
				case 0X00:
					data.position = reader.readType<Vec3>();
					break;
				case 0X01:
					data.data_1 = reader.readType<Byte4>();
					break;
				case 0X02:
					data.data_2 = reader.readType<Byte4>();
					break;
				case 0X03:
					data.data_3 = reader.readType<Byte4>();
					break;
				case 0X05:
					data.diffuse_uv[stream.index] = reader.readType<Vec2>();
					break;
				case 0x06:
					data.data_6[stream.index] = reader.readType<Byte4>();
					break;
				case 0X0A:
					data.data_A[stream.index] = reader.readType<Byte4>();
					break;
				default:
					assert(false);
					break;
				}
			}
		}
	}

public:
	FlverFile(BinaryFileReader& reader) {
		flver_header = reader.readType<FlverHeader>();
		assert(flver_header());

		hit_boxes.reserve(flver_header.hitbox_header_count);
		for (uint i = 0; i < flver_header.hitbox_header_count; ++i) {
			hit_boxes.emplace_back(reader.readType<HitBox>());
			assert(hit_boxes.back()());
		}

		mat_headers.reserve(flver_header.material_header_count);
		for (uint i = 0; i < flver_header.material_header_count; ++i) {
			mat_headers.emplace_back(reader.readType<MaterialHeader>());
			assert(mat_headers.back()());
		}

		bone_headers.reserve(flver_header.bone_header_count);
		for (uint i = 0; i < flver_header.bone_header_count; ++i) {
			bone_headers.emplace_back(reader.readType<BoneHeader>());
			assert(bone_headers.back()());
		}

		readMeshInfo(reader);

		readVertexInfo(reader);

		readMaterial(reader);

		readMesh(reader);
	}

	void toObj(const std::string& output_path, std::string name) {
		if (meshes.empty()) {
			Log::info("No mesh in this file.");
			return;
		}

		//uint count = 0;
		//for (const auto& vertex : meshes[6].vertices) {
		//	printf("%02d: %04X, %04X\n", count++, vertex.data_3.x, vertex.data_3.y);
		//}

		Log::info("Material file: ", output_path + name + ".mtl");
		std::ofstream os(output_path + name + ".mtl");

		os << "# Material count: " << mat_infos.size() << "\n\n";
		for (const auto& mat_info : mat_infos) {
			os << "newmtl " << mat_info.mtl_name << "\n";
			os << "Kd 0.8 0.8 0.8\n";
			os << "Ks 0.48 0.48 0.48\n";
			for (uint i = mat_info.param_start_index; i < mat_info.param_count + mat_info.param_start_index; ++i) {
				if (mat_data[i].type_name == "g_DiffuseTexture" || mat_data[i].type_name == "g_DiffuseTexture2") {
					os << "map_Kd " << Util::getFileName(mat_data[i].filename) << ".dds\n";
				}
				else if (mat_data[i].type_name == "g_SpecularTexture" || mat_data[i].type_name == "g_SpecularTexture2") {
					os << "map_Ks " << Util::getFileName(mat_data[i].filename) << ".dds\n";
				}
				else if (mat_data[i].type_name == "g_BumpmapTexture" || mat_data[i].type_name == "g_BumpmapTexture2") {
					os << "map_Bump " << Util::getFileName(mat_data[i].filename) << ".dds\n";
				}
				else if (mat_data[i].type_name == "g_DetailBumpmapTexture" || mat_data[i].type_name == "g_DetailBumpmapTexture2") {
					os << "map_Bump " << Util::getFileName(mat_data[i].filename) << ".dds\n";
				}
				else if (mat_data[i].type_name == "g_HighlightTexture") {
					os << "map_Ns " << Util::getFileName(mat_data[i].filename) << ".dds\n";
				}
			}
			os << "\n";
		}

		os.flush();
		os.close();
		Log::info("Mesh file: ", output_path + name + ".obj");
		os.open(output_path + name + ".obj");

		os << "# Mesh count: " << meshes.size() << "\n\n";
		os << "mtllib ./" << name << ".mtl\n\n";
		uint vertex_count = 1;
		for (const auto& mesh : meshes) {
			for (const auto& vertex : mesh.vertices) {
				os << "v " << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << "\n";
			}
			os << "# " << mesh.vertices.size() << " vertices.\n\n";

			for (const auto& vertex : mesh.vertices) {
				os << "vt " << vertex.diffuse_uv[0].getX() << " " << vertex.diffuse_uv[0].getY() << "\n";
			}
			os << "\n";

			os << "o " << mat_infos[mesh.material_info_index].mtl_name << "\n";
			os << "usemtl " << mat_infos[mesh.material_info_index].mtl_name << "\n";
			for (uint i = 0; i < mesh.faces.size(); i += 3) {
				os << "f " << mesh.faces[i + 2] + vertex_count << "/" << mesh.faces[i + 2] + vertex_count
					<< " " << mesh.faces[i + 1] + vertex_count << "/" << mesh.faces[i + 1] + vertex_count
					<< " " << mesh.faces[i] + vertex_count << "/" << mesh.faces[i] + vertex_count << "\n";
			}
			os << "\n";
			vertex_count += mesh.vertices.size();
		}

		os.flush();
	}
};

#endif // !FLVER_FILE_H
