#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <tiny_obj_loader.h>
#include <glm/vec3.hpp>

class objLoader
{
public:
	objLoader() {}
	~objLoader()
	{
		//delete_obj_data(&data);
	}

    std::string base_dir = "../../objects";
    


			//this->vertexCount = data.vertex_count;
			//this->normalCount = data.vertex_normal_count;
			//this->textureCount = data.vertex_texture_count;
			//
			//this->faceCount = data.face_count;
			//this->sphereCount = data.sphere_count;
			//this->planeCount = data.plane_count;
			//
			//this->lightPointCount = data.light_point_count;
			//this->lightDiscCount = data.light_disc_count;
			//this->lightQuadCount = data.light_quad_count;
			//
			//this->materialCount = data.material_count;
			//
            std::vector<vec3> vertices;
            std::vector<uint32_t> indices;
			//this->normalList = data.vertex_normal_list;
			//this->textureList = data.vertex_texture_list;
			//
			//this->faceList = data.face_list;
			//this->sphereList = data.sphere_list;
			//this->planeList = data.plane_list;

	
	int load(const char *filename)
	{

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, base_dir.c_str())) {
            throw std::runtime_error(warn + err);
        }

        for (const auto& shape : shapes){
            for (const auto& index : shape.mesh.indices){
                indices.push_back(indices.size());
                glm::vec3 vector = glm::vec3(
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                        );
            }
        }

        //this->normalCount = data.vertex_normal_count;
        //this->textureCount = data.vertex_texture_count;
        //
        //this->faceCount = data.face_count;
        //this->sphereCount = data.sphere_count;
        //this->planeCount = data.plane_count;
        //
        //this->lightPointCount = data.light_point_count;
        //this->lightDiscCount = data.light_disc_count;
        //this->lightQuadCount = data.light_quad_count;
        //
        //this->materialCount = data.material_count;
        //
        //this->vertexList = data.vertex_list;
        //this->normalList = data.vertex_normal_list;
        //this->textureList = data.vertex_texture_list;
        //
        //this->faceList = data.face_list;
        //this->sphereList = data.sphere_list;
        //this->planeList = data.plane_list;
        //
        //this->lightPointList = data.light_point_list;
        //this->lightDiscList = data.light_disc_list;
        //this->lightQuadList = data.light_quad_list;
        //
        //this->materialList = data.material_list;
        //
        //this->camera = data.camera;

		return 1;
	}
	
private:
};

#endif
