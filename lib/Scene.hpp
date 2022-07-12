#pragma once
#include "Common/SlotMap.hpp"
#include "Context.hpp"
#include "GAPI/BufferAllocator.hpp"
#include "R1.h"
#include "Swapchain.hpp"
#include "shaders/Interface.glsl"

#include <boost/container/small_vector.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>

#include <queue>

namespace R1 {
using Scene = R1Scene;

struct ScenePresentInfo {
    GAL::Semaphore          semaphore;
    GAL::SemaphorePayload   wait_value;
    GAL::SemaphorePayload   signal_value;
};

enum class MeshID;
enum class MeshInstanceID;

struct MeshConfig {
    std::span<const glm::vec3>  positions;
    std::span<const glm::vec3>  normals;
    GAL::IndexFormat            index_format;
    std::span<const std::byte>  indices;
};

struct MeshInstanceConfig {
    glm::mat4   transform;
    MeshID      mesh;
};

struct Camera {
    glm::vec3 position  = {0.0f, 0.0f,  1.0f};
    glm::vec3 direction = {0.0f, 0.0f, -1.0f};
    glm::vec3 up        = {0.0f, 1.0f,  0.0f};
    float fov           = glm::radians(80.0f);
    float near          = 0.1f;
    float far           = 100.0f;
};

namespace GLSL {
DEFINE_GLSL_INTERFACE_TYPES
}
}

class R1Scene {
    struct MeshDesc {
        R1::GAL::Buffer             buffer;
        R1::GAL::SemaphorePayload   upload_time;
        unsigned                    vertex_count;
        R1::GAL::IndexFormat        index_format;
        unsigned                    index_count;
    };

protected:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

    R1::GAPI::SlotMap<MeshDesc>     m_meshes;
    using MeshKey = decltype(m_meshes)::key_type;
    std::vector<std::byte>          m_staging_storage;

    struct MeshStagingInfo {
        MeshKey     key;
        size_t      buffer_size;
    };

    std::vector<MeshStagingInfo>    m_mesh_staging_infos;
    R1::GAPI::HCommandPool          m_upload_command_pool;

    R1::GAPI::HSemaphore            m_upload_semaphore;
    R1::GAL::SemaphorePayload       m_last_upload_time = 0;

    struct UploadInfo {
        R1::GAL::Buffer staging_buffer;
        R1::GAL::CommandBuffer cmd_buffer;
        R1::GAL::SemaphorePayload upload_time;
    };

    std::queue<UploadInfo>          m_upload_queue;

    struct BufferDeleteInfo {
        R1::GAL::Buffer             buffer;
        R1::GAL::SemaphorePayload   upload_time;
        R1::GAL::SemaphorePayload   last_used;
    };

    std::vector<R1::MeshID>         m_mesh_delete_infos;
    struct BufferDeleteQueue: std::queue<BufferDeleteInfo> {
        R1::GAL::SemaphorePayload last_used;

        void push(R1::GAL::Buffer buffer) {
            emplace() = {
                .buffer = buffer,
                .last_used = last_used,
            };
        }
    };
    BufferDeleteQueue               m_buffer_delete_queue;

    struct MeshInstanceDesc {
        glm::mat4   transform;
        MeshKey     mesh;
    };

    R1::GAPI::SlotMap<MeshInstanceDesc> m_mesh_instances;
    using MeshInstanceKey = decltype(m_mesh_instances)::key_type;

    struct StreamingBufferUsageTraits {
        static constexpr R1::GAL::BufferUsageFlags UsageFlags =
            R1::GAL::BufferUsage::Storage;
        static constexpr R1::GAL::BufferMemoryUsage MemoryUsage =
            R1::GAL::BufferMemoryUsage::Streaming;
    };

    template<typename T>
    using StreamingBufferAllocator = R1::GAPI::ExclusiveBufferAllocator<
        T, StreamingBufferUsageTraits, BufferDeleteQueue>;
    template<typename T>
    class StreamingBufferVector: public std::vector<T, StreamingBufferAllocator<T>> {
    public:
        using std::vector<T, StreamingBufferAllocator<T>>::vector;
        R1::GAL::Buffer GetBackingBuffer() noexcept {
            return this->get_allocator().get_backing_buffer(this->data(), this->capacity());
        }
    };

    boost::container::small_vector<
        StreamingBufferVector<R1::GLSL::InstanceMatrices>, 3> m_instance_matrix_ring_buffer;

    R1::Camera m_camera;

public:
    R1Scene(R1::Context& ctx);
    R1Scene(const R1Scene&) = delete;
    R1Scene(R1Scene&& other) = default;
    R1Scene& operator=(const R1Scene&) = delete;
    R1Scene& operator=(R1Scene&&) = default;
    ~R1Scene();

    void ConfigOutputImages(
        unsigned width, unsigned height, unsigned count,
        R1::GAL::ImageUsageFlags image_usage_flags
    );
    std::tuple<unsigned, unsigned> GetOutputImageSize() const noexcept;
    R1::GAL::Format GetOutputImageFormat() const noexcept;
    R1::GAL::ImageLayout GetOutputImageStartLayout() const noexcept;
    R1::GAL::ImageLayout GetOutputImageEndLayout() const noexcept;
    size_t GetOutputImageCount() const noexcept;
    R1::GAL::Image GetOutputImage(size_t idx) const noexcept;
    size_t GetCurrentOutputImage() const noexcept;

    R1::ScenePresentInfo Draw();

    R1::MeshID CreateMesh(const R1::MeshConfig& config);
    void DestroyMesh(R1::MeshID mesh);

    R1::MeshInstanceID CreateMeshInstance(const R1::MeshInstanceConfig& config);
    void DestroyMeshInstance(R1::MeshInstanceID mesh_instance);

    const glm::mat4& GetMeshInstanceTransform(R1::MeshInstanceID mesh_instance) const noexcept;
    glm::mat4& GetMeshInstanceTransform(R1::MeshInstanceID mesh_instance) noexcept;

    const R1::Camera& GetCamera() const noexcept { return m_camera; }
    R1::Camera& GetCamera() noexcept { return m_camera; }

protected:
    void PushUploadQueue();
    void FlushUploadQueue();
    void PushDeleteQueue();
    void FlushDeleteQueue();
};
