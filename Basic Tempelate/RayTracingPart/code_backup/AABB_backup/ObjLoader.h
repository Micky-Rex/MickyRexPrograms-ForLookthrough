#pragma once
#include "Basic.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <array>
#include <cctype>

namespace OBJLoader {

	struct Triangle {
		std::array<int, 3> vertexIndices;
	};

	struct Model {
	public:
		std::vector<vec3> vertices;
		std::vector<Triangle> triangles;
		
		inline void loadOBJ(const std::string& filepath) {
			Model& model = *this;
			std::ifstream file(filepath);
			if (!file.is_open()) {
				std::cerr << "Error: Failed to open OBJ file: " << filepath << std::endl;
				return;
			}

			std::string line;
			while (std::getline(file, line)) {
				std::istringstream iss(line);
				std::string prefix;
				iss >> prefix;

				// 处理顶点数据 [1]
				if (prefix == "v") {
					double x, y, z;
					iss >> x >> y >> z;
					model.vertices.push_back(vec3(x, y, z));

					AABB_left_buttom_frontend.x = min(AABB_left_buttom_frontend.x, x);
					AABB_left_buttom_frontend.y = min(AABB_left_buttom_frontend.y, y);
					AABB_left_buttom_frontend.z = min(AABB_left_buttom_frontend.z, z);

					AABB_right_top_backend.x = max(AABB_right_top_backend.x, x);
					AABB_right_top_backend.y = max(AABB_right_top_backend.x, y);
					AABB_right_top_backend.z = max(AABB_right_top_backend.x, z);
				}
				// 处理面数据 [2]
				else if (prefix == "f") {
					std::vector<int> faceIndices;
					std::string vertexToken;

					while (iss >> vertexToken) {
						// 提取顶点索引部分 (忽略纹理/法线索引)
						size_t pos = vertexToken.find('/');
						if (pos != std::string::npos) {
							vertexToken = vertexToken.substr(0, pos);
						}

						try {
							int idx = std::stoi(vertexToken);
							// OBJ索引从1开始，转换为0-based
							faceIndices.push_back((idx > 0) ? idx - 1 : model.vertices.size() + idx);
						}
						catch (...) {
							continue; // 跳过无效数据
						}
					}

					// 将多边形面转换为三角面 [3]
					if (faceIndices.size() >= 3) {
						for (size_t i = 2; i < faceIndices.size(); ++i) {
							model.triangles.push_back({
								{faceIndices[0], faceIndices[i - 1], faceIndices[i]}
								});
						}
					}
				}
			}
			return;
		}

		vec3 AABB_left_buttom_frontend = vec3(infinity, infinity, infinity);	// AABB包围盒的前左下点
		vec3 AABB_right_top_backend = vec3(-infinity, -infinity, -infinity);	// AABB包围盒的后右上点
	};


} // namespace OBJLoader