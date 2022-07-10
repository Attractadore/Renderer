#include "ProgsCommon.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include <array>

class DrawTrianglesApp: public AppBase<DrawTrianglesApp> {
    friend AppBase<DrawTrianglesApp>;

    static constexpr auto app_name = "Draw triangles";

    R1Mesh                          m_tri_mesh = 0;
    std::array<R1MeshInstance, 4>   m_tri_models = {{}};

public:
    using AppBase<DrawTrianglesApp>::AppBase;

private:
    int Init();
    void TearDown();
};

int DrawTrianglesApp::Init() {
    std::array<glm::vec3, 3> data = {{
        { 0.0f,  glm::sqrt(3.0f) / 3.0f, 0.0f},
        { 0.5f, -glm::sqrt(3.0f) / 6.0f, 0.0f},
        {-0.5f, -glm::sqrt(3.0f) / 6.0f, 0.0f},
    }};
    R1MeshConfig mesh_config = {
        .vertices = glm::value_ptr(data[0]),
        .vertex_count = 3,
    };
    m_tri_mesh = R1_CreateMesh(m_scene, &mesh_config);

    std::array<glm::mat4, 4> tri_translations = {{
        glm::translate(glm::mat4{1.0f}, { 0.3f,  0.3f, 0.0f}),
        glm::translate(glm::mat4{1.0f}, {-0.3f,  0.3f, 0.0f}),
        glm::translate(glm::mat4{1.0f}, { 0.3f, -0.3f, 0.0f}),
        glm::translate(glm::mat4{1.0f}, {-0.3f, -0.3f, 0.0f}),
    }};
    for (size_t i = 0; i < m_tri_models.size(); i++) {
        auto transform =
            tri_translations[i] *
            glm::scale(glm::mat4{1.0f}, {0.5f, 0.5f, 0.5f});
        R1MeshInstanceConfig model_config = {
            .mesh = m_tri_mesh,
        };
        std::memcpy(model_config.transform, &transform, sizeof(transform));
        m_tri_models[i] = R1_CreateMeshInstance(m_scene, &model_config);
    }

    return 0;
}

void DrawTrianglesApp::TearDown() {
    for (auto tri_model: m_tri_models) {
        R1_DestroyMeshInstance(m_scene, tri_model);
    }
    R1_DestroyMesh(m_scene, m_tri_mesh);
}

int main() {
    return DrawTrianglesApp{}.Run();
}
