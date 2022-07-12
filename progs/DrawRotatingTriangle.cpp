#include "ProgsCommon.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include <array>
#include <chrono>

class DrawRotatingTriangleApp: public AppBase<DrawRotatingTriangleApp> {
    friend AppBase<DrawRotatingTriangleApp>;

    static constexpr auto app_name = "Draw rotating triangle";
    static constexpr float angular_velocity = glm::radians(180.0f);

    R1Mesh                                  m_tri_mesh = 0;
    R1MeshInstance                          m_tri_model = 0;
    glm::mat4                               m_tri_scale{1.0f};
    std::chrono::steady_clock::time_point   m_start_time = std::chrono::steady_clock::now();


public:
    using AppBase<DrawRotatingTriangleApp>::AppBase;

private:
    int Init();
    void Iterate();
    void TearDown();
};

int DrawRotatingTriangleApp::Init() {
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

    m_tri_scale = glm::scale(glm::mat4{1.0f}, glm::vec3{0.5f});
    R1MeshInstanceConfig mesh_instance_config = {
        .mesh = m_tri_mesh,
    };
    m_tri_model = R1_CreateMeshInstance(m_scene, &mesh_instance_config);

    return 0;
}

void DrawRotatingTriangleApp::TearDown() {
    R1_DestroyMeshInstance(m_scene, m_tri_model);
    R1_DestroyMesh(m_scene, m_tri_mesh);
}

void DrawRotatingTriangleApp::Iterate() {
    auto time = std::chrono::steady_clock::now();
    auto delta_time = std::chrono::duration_cast<std::chrono::duration<float>>(time - m_start_time).count();
    auto transform =
        glm::rotate(glm::mat4{1.0f}, delta_time * angular_velocity, {0.0f, 0.0f, 1.0f}) *
        m_tri_scale;
    R1_SetMeshInstanceTransform(m_scene, m_tri_model, glm::value_ptr(transform));
}

int main() {
    return DrawRotatingTriangleApp{}.Run();
}
