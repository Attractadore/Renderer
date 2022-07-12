#include "ProgsCommon.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include <array>

class DrawTriangleApp: public AppBase<DrawTriangleApp> {
    friend AppBase<DrawTriangleApp>;

    static constexpr auto app_name = "Draw triangle";

    R1Mesh          m_tri_mesh = 0;
    R1MeshInstance  m_tri_model = 0;

public:
    using AppBase<DrawTriangleApp>::AppBase;

private:
    int Init();
    void TearDown();
};

int DrawTriangleApp::Init() {
    std::array<glm::vec3, 3> positions = {{
        { 0.0f,  glm::sqrt(3.0f) / 3.0f, 0.0f},
        { 0.5f, -glm::sqrt(3.0f) / 6.0f, 0.0f},
        {-0.5f, -glm::sqrt(3.0f) / 6.0f, 0.0f},
    }};
    std::array<unsigned short, 3> indices = {2, 1, 0};
    auto normals = GenerateNormals(positions, indices);
    R1MeshConfig mesh_config = {
        .positions = glm::value_ptr(positions[0]),
        .normals = glm::value_ptr(normals[0]),
        .vertex_count = 3,
        .index_format = R1_INDEX_FORMAT_16,
        .indices = indices.data(),
        .index_count = indices.size(),
    };
    m_tri_mesh = R1_CreateMesh(m_scene, &mesh_config);

    glm::mat4 transform = glm::scale(glm::mat4{1.0f}, glm::vec3{0.5f});
    R1MeshInstanceConfig mesh_instance_config = {
        .mesh = m_tri_mesh,
    };
    std::memcpy(mesh_instance_config.transform, &transform, sizeof(transform));
    m_tri_model = R1_CreateMeshInstance(m_scene, &mesh_instance_config);

    return 0;
}

void DrawTriangleApp::TearDown() {
    R1_DestroyMeshInstance(m_scene, m_tri_model);
    R1_DestroyMesh(m_scene, m_tri_mesh);
}

int main() {
    return DrawTriangleApp{}.Run();
}
