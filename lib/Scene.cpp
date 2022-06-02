#include "Scene.hpp"

namespace R1 {
Scene::Scene(GAPI::Context* ctx):
    m_context{ctx} {}

void Scene::draw() {
    m_context->draw();
}
}
