#include "ProgsCommon.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/gtc/type_ptr.hpp>

class DrawLoadedMeshApp: public AppBase<DrawLoadedMeshApp> {
    friend AppBase<DrawLoadedMeshApp>;

    const char*                 app_name;
    std::vector<R1Mesh>         m_meshes;
    std::vector<R1MeshInstance> m_models;

public:
    DrawLoadedMeshApp(const char* path):
        app_name{path} {}

private:
    int Init();
    void TearDown();
};

int DrawLoadedMeshApp::Init() {
    Assimp::Importer imp;
    imp.SetPropertyBool(AI_CONFIG_PP_PTV_NORMALIZE, true);
    imp.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS,
        aiComponent_ANIMATIONS |
        aiComponent_BONEWEIGHTS |
        aiComponent_CAMERAS |
        aiComponent_COLORS |
        aiComponent_LIGHTS |
        aiComponent_MATERIALS |
        aiComponent_TANGENTS_AND_BITANGENTS |
        aiComponent_TEXCOORDS |
        aiComponent_TEXTURES |
    0);
    imp.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE,
        aiPrimitiveType_LINE |
        aiPrimitiveType_POINT |
    0);

    std::cout << "Load scene from " << app_name << "\n";
    auto* scene = imp.ReadFile(app_name,
        aiProcess_FindDegenerates |
        aiProcess_FindInstances |
        aiProcess_FindInvalidData |
        aiProcess_GenSmoothNormals |
        aiProcess_ImproveCacheLocality |
        aiProcess_JoinIdenticalVertices |
        aiProcess_OptimizeMeshes |
        aiProcess_PreTransformVertices |
        aiProcess_RemoveComponent |
        aiProcess_RemoveRedundantMaterials |
        aiProcess_SortByPType |
        aiProcess_Triangulate |
        aiProcess_ValidateDataStructure |
    0);
    if (!scene) {
        std::cerr << "Failed to load scene: " << imp.GetErrorString() << "\n";
        return -1;
    }

    m_meshes.reserve(scene->mNumMeshes);
    { std::vector<unsigned> indices;
    for (auto* mesh: std::span{scene->mMeshes, scene->mNumMeshes}) {
        indices.resize(mesh->mNumFaces * 3);
        for (size_t f = 0; f < mesh->mNumFaces; f++) {
            assert(mesh->mFaces[f].mNumIndices == 3);
            std::ranges::copy_n(mesh->mFaces[f].mIndices, 3, indices.begin() + 3 * f);
        }
        R1MeshConfig config = {
            .positions = reinterpret_cast<const float*>(mesh->mVertices),
            .normals = reinterpret_cast<const float*>(mesh->mNormals),
            .vertex_count = mesh->mNumVertices,
            .index_format = R1_INDEX_FORMAT_32,
            .indices = indices.data(),
            .index_count = static_cast<unsigned>(indices.size()),
        };
        m_meshes.push_back( R1_CreateMesh(m_scene, &config) );
    } }
    std::cout << "Created " << scene->mNumMeshes << " mesh(es)\n";

    auto create_object = [this] (const aiNode* node) {
        auto transform = node->mTransformation;
        transform.Transpose();
        for (auto mesh_idx: std::span{node->mMeshes, node->mNumMeshes}) {
            auto mesh = m_meshes[mesh_idx];
            R1MeshInstanceConfig config = {
                .mesh = mesh
            };
            std::memcpy(config.transform, &transform, sizeof(transform));
            m_models.push_back( R1_CreateMeshInstance(m_scene, &config) );
        }
    };

    auto* root = scene->mRootNode;
    assert(root->mTransformation.IsIdentity());
    create_object(root);
    for (auto* node: std::span{root->mChildren, root->mNumChildren}) {
        assert(node->mNumChildren == 0);
        create_object(node);
    }
    std::cout << "Created " << m_models.size() << " object(s)\n";

    glm::vec3 pos = {-3.0f, 0.0f, 0.0f};
    glm::vec3 fwd = {1.0f, 0.0f, 0.0f};
    glm::vec3 up = {0.0f, 0.0f, 1.0f};
    R1_SetCameraPosition(m_scene, glm::value_ptr(pos));
    R1_SetCameraDirection(m_scene, glm::value_ptr(fwd));
    R1_SetCameraUp(m_scene, glm::value_ptr(up));

    return 0;
}

void DrawLoadedMeshApp::TearDown() {
    for (auto model: m_models) {
        R1_DestroyMeshInstance(m_scene, model);
    }
    for (auto mesh: m_meshes) {
        R1_DestroyMesh(m_scene, mesh);
    }
}

int main(int argc, const char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [file]\n";
        return 0;
    }
    return DrawLoadedMeshApp(argv[1]).Run();
}
