#include "Scene.hpp"

#include <vulkan/vulkan.h>

namespace R1 {
Scene::Scene(Context* ctx):
    m_context{ctx} {}

void Scene::draw() {
    m_context->draw();
}
}
