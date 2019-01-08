struct ObjRef {
    const char * path;
    std::vector<glm::vec3> &out_vertices;
    std::vector<glm::vec2> &out_uvs;
    std::vector<glm::vec3> &out_normals;
}

class ObjLoader {
    public:
        ObjLoader();
        bool loadOBJ(obj_ref);
}
