#include <memory>
#include <spdlog/spdlog.h>
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <app/MainController.hpp>
#include <app/GUIController.hpp>

namespace engine::myapp {

void MainController::initialize() {
    // User initialization
    engine::graphics::OpenGL::enable_depth_testing();

    // ─── Lighting ─────────────────────────────────────────────────
    m_lighting.initialize();

    // 1) Directional light
    graphics::lighting::DirectionalLight dl;
    dl.base.color = glm::vec3(1.0f);
    dl.base.ambientIntensity = 0.1f;
    dl.base.diffuseIntensity = 0.25f;
    dl.direction = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
    m_lighting.setDirectionalLight(dl);

    // 2) Point light
    graphics::lighting::PointLight pl;
    pl.position = g_light_pos;
    pl.base.color = glm::vec3(1.0f, 0.8f, 0.6f);
    pl.base.ambientIntensity = 0.1f;
    pl.base.diffuseIntensity = 1.0f;
    pl.atten = {1.0f, 0.09f, 0.032f};
    m_lighting.setPointLight(pl);

    // 3) Material
    graphics::lighting::Material mat;
    mat.ambientColor = glm::vec3(1.0f);
    mat.diffuseColor = glm::vec3(1.0f);
    mat.specularColor = glm::vec3(1.0f);
    m_lighting.setMaterial(mat);

    // 4) Pocetni intenzitet svetla
    m_lighting.setLightIntensity(g_point_light_intensity);
    // ───────────────────────────────────────────────────────────────

    // ─── MSAA off-screen ─────────────────────────────────────────
    auto *platform = core::Controller::get<platform::PlatformController>();
    int width = platform->window()->width();
    int height = platform->window()->height();
    m_msaa = std::make_unique<engine::graphics::MSAA>(width, height, /*samples=*/4);
    // ───────────────────────────────────────────────────────────────
}

bool MainController::loop() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE)
                .state() == engine::platform::Key::State::JustPressed) { return false; }
    return true;
}

void MainController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    // 1)
    if (platform->key(engine::platform::KeyId::KEY_F1)
                .state() == engine::platform::Key::State::JustPressed) {
        m_cursor_enabled = !m_cursor_enabled;
        platform->set_enable_cursor(m_cursor_enabled);
    }
}

void MainController::update() { update_camera(); }

void MainController::begin_draw() {
    auto *graphics = core::Controller::get<engine::graphics::GraphicsController>();

    if (g_msaa_enabled) { m_msaa->bindForWriting(); } else { graphics->bind_framebuffer(GL_FRAMEBUFFER, 0); }
    graphics->clear();
}

void MainController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

    engine::graphics::lighting::PointLight pl;
    pl.base.color = glm::vec3(1.0f, 0.8f, 0.6f);
    pl.base.ambientIntensity = 0.1f;
    pl.base.diffuseIntensity = 1.0f;
    pl.position = g_light_pos;// GUI-vrednost
    pl.atten.constant = 1.0f;
    pl.atten.linear = 0.09f;
    pl.atten.exp = 0.032f;
    m_lighting.setPointLight(pl);

    // ───── DEPTH PASS ────────────────────────────────────────────────────────────────
    m_lighting.beginDepthPass();
    m_lighting.renderDepthPass([&](const engine::resources::Shader *depthShader) {
        depthShader->use();

        // 2) Poles
        {
            std::vector<glm::vec3> polePositions = {
                    {-15.0f, -7.0f, 180.0f},
                    {-15.0f, -7.0f, 150.0f},
                    {-15.0f, -7.0f, 120.0f},
                    {-15.0f, -7.0f, 90.0f},
                    {-15.0f, -7.0f, 60.0f}
            };
            for (auto &pos: polePositions) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
                model = glm::scale(model, glm::vec3(2.0f));
                depthShader->set_mat4("model", model);
                resources->model("pole")->draw(depthShader);
            }
        }

        // 3) Terrain
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -10.0f, 0.0f));
            model = glm::scale(model, glm::vec3(3.0f));
            depthShader->set_mat4("model", model);
            resources->model("terrain")->draw(depthShader);
        }

        // 4) Trees
        {
            std::vector<glm::vec3> treePositions = {
                    // Prvi set
                    {25.0f, 2.9f, 120.0f},
                    {30.0f, 3.0f, 100.0f},
                    {25.0f, 3.0f, 90.0f},
                    {30.0f, 3.0f, 70.0f},
                    {25.0f, 3.0f, 30.0f},
                    {25.0f, 3.0f, 15.0f},
                    {30.0f, 3.0f, 5.0f},
                    {25.0f, 3.0f, -20.0f}
            };

            for (auto &pos: treePositions) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
                model = glm::scale(model, glm::vec3(10.0f));
                model = glm::rotate(model, -80.0f, glm::vec3(0, 0, 1));
                depthShader->set_mat4("model", model);
                resources->model("tree")->draw(depthShader);
            }
        }

        // 6) Medieval House
        {
            glm::vec3 pos(-5.0f, -8.0f, 10.0f);
            glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
            model = glm::rotate(model, 0.0f, glm::vec3(0, 0, 0));
            model = glm::scale(model, glm::vec3(2.0f));
            depthShader->set_mat4("model", model);
            resources->model("medieval_house")->draw(depthShader);
        }

        // 7) Cottage
        {
            glm::vec3 pos(0.0f, -8.0f, 200.0f);
            glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
            model = glm::rotate(model, 0.0f, glm::vec3(0, 0, 0));
            model = glm::scale(model, glm::vec3(0.1f));
            depthShader->set_mat4("model", model);
            resources->model("cottage")->draw(depthShader);
        }

        // 8) Runner
        draw_mesh(resources->model("police_car"), depthShader,
                  m_runnerPosition,
                  glm::vec3(0.0f, glm::radians(180.0f), 0.0f), glm::vec3(0.5f));
    });

    if (finishLine >= neg_z && t < maxTime) {
        update_racer();
        m_runnerPosition = glm::vec3(0.0f, -7.5f, 185.0f - neg_z);
    }

    m_lighting.endDepthPass();

    // umesto glBindFramebuffer(...)
    if (g_msaa_enabled) { m_msaa->bindForWriting(); } else { graphics->bind_framebuffer(GL_FRAMEBUFFER, 0); }

    auto *platform = core::Controller::get<platform::PlatformController>();
    int width = platform->window()->width();
    int height = platform->window()->height();
    glViewport(0, 0, width, height);

    m_lighting.setCameraPosition(graphics->camera()->Position);
    m_lighting.setLightIntensity(g_point_light_intensity);

    // ───── LIGHTING PASS ─────────────────────────────────────────────────────────────
    m_lighting.renderLightingPass([&](const engine::resources::Shader *lightShader) {
        lightShader->use();

        // Projection & view
        lightShader->set_mat4("projection", graphics->projection_matrix());
        lightShader->set_mat4("view", graphics->camera()->view_matrix());

        // Intenzitet point svetla
        lightShader->set_float("uLightIntensity", g_point_light_intensity);

        // Pozicija point svetla
        lightShader->set_vec3("lightPos", g_light_pos);

        // 2) Poles
        std::vector<glm::vec3> polePositions = {
                {-15.0f, -7.0f, 180.0f},
                {-15.0f, -7.0f, 150.0f},
                {-15.0f, -7.0f, 120.0f},
                {-15.0f, -7.0f, 90.0f},
                {-15.0f, -7.0f, 60.0f}
        };

        for (auto &pos: polePositions) {
            draw_mesh(resources->model("pole"), lightShader,
                      pos,
                      glm::vec3(0.0f),
                      glm::vec3(2.0f));
        }

        // 3) Terrain
        draw_mesh(resources->model("terrain"), lightShader,
                  glm::vec3(0.0f, -10.0f, 0.0f),
                  glm::vec3(0.0f), glm::vec3(3.0f));

        // 4) Trees
        std::vector<glm::vec3> treePositions = {
                // Prvi set
                {25.0f, 2.9f, 120.0f},
                {30.0f, 3.0f, 100.0f},
                {25.0f, 3.0f, 90.0f},
                {30.0f, 3.0f, 70.0f},
                {25.0f, 3.0f, 30.0f},
                {25.0f, 3.0f, 15.0f},
                {30.0f, 3.0f, 5.0f},
                {25.0f, 3.0f, -20.0f}
        };

        for (auto &pos: treePositions) {
            draw_mesh(resources->model("tree"), lightShader,
                      pos,
                      glm::vec3(0.0f, 0.0f, -80.0f),
                      glm::vec3(10.0f));
        }

        // 6) Medieval House
        draw_mesh(resources->model("medieval_house"), lightShader,
                  glm::vec3(-5.0f, -8.0f, 10.0f),
                  glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(2.0f));

        // 6) Cottage
        draw_mesh(resources->model("cottage"), lightShader,
                  glm::vec3(0.0f, -8.0f, 200.0f),
                  glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.1f));

        // 7) Runner
        draw_mesh(resources->model("police_car"), lightShader,
                  m_runnerPosition,
                  glm::vec3(0.0f, glm::radians(180.0f), 0.0f),
                  glm::vec3(0.5f));
    });

    if (finishLine >= neg_z && t < maxTime) {
        update_racer();
        m_runnerPosition = glm::vec3(0.0f, -7.5f, 185.0f - neg_z);
        g_light_pos = m_runnerPosition + glm::vec3(0.0f, 15.0f, 0.0f);
    }

    m_lighting.renderLightBulb(g_light_pos, 3.0f);

    // ───── SKYBOX ────────────────────────────────────────────────────────────────────
    draw_skybox();
}

void MainController::end_draw() {
    // 1) Resolve MSAA FBO -> default framebuffer
    if (g_msaa_enabled) { m_msaa->resolveToDefault(); }

    // 2) Swap
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

// USER DEFINED
// ---------------------------------------------------------------------------------------------------------------------------

void MainController::draw_mesh(auto model, auto shader,
                               const glm::vec3 &position,
                               const glm::vec3 &rotation,// Eulerovi uglovi u radijanima: (rotX, rotY, rotZ)
                               const glm::vec3 &scale) {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    // Kreiramo model matricu
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, position);
    // Rotacija – prvo rotiramo oko X, pa Y, pa Z ose
    modelMat = glm::rotate(modelMat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    modelMat = glm::rotate(modelMat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMat = glm::rotate(modelMat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMat = glm::scale(modelMat, scale);
    shader->set_mat4("model", modelMat);

    // Crtamo model
    model->draw(shader);
}

void MainController::draw_skybox() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
    auto skybox_cube = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox("skybox");
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
}

void MainController::update_camera() {
    auto gui = core::Controller::get<GUIController>();
    if (gui->is_enabled()) { return; }

    auto platform = core::Controller::get<platform::PlatformController>();
    auto camera = core::Controller::get<graphics::GraphicsController>()->camera();
    float dt = platform->dt();

    // 1)
    if (platform->key(platform::KeyId::KEY_L)
                .state() == platform::Key::State::JustPressed) {
        m_autoMoveLeft = !m_autoMoveLeft;
        m_leftMovedDistance = 0.0f;
    }

    // 2)
    if (m_autoMoveLeft) {
        float speedMultiplier = (platform->key(platform::KeyId::KEY_LEFT_SHIFT)
                                         .state() == platform::Key::State::Pressed)
                                    ? 4.0f
                                    : 2.0f;

        float stepDist = camera->MovementSpeed * dt * speedMultiplier;

        if (m_leftMovedDistance + stepDist >= m_leftTargetDistance) {
            float remaining = m_leftTargetDistance - m_leftMovedDistance;
            float remDt = remaining / (camera->MovementSpeed * speedMultiplier);
            camera->move_camera(graphics::Camera::Movement::LEFT, remDt * speedMultiplier);
            m_autoMoveLeft = false;
        } else {
            camera->move_camera(graphics::Camera::Movement::LEFT, dt * speedMultiplier);
            m_leftMovedDistance += stepDist;
        }
    }
    // 3)
    else {
        float speedMultiplier = 2.0f;
        if (platform->key(platform::KeyId::KEY_LEFT_SHIFT)
                    .state() == platform::Key::State::Pressed) { speedMultiplier = 4.0f; }

        if (platform->key(platform::KeyId::KEY_W)
                    .state() == platform::Key::State::Pressed)
            camera->move_camera(graphics::Camera::Movement::FORWARD, dt * speedMultiplier);
        if (platform->key(platform::KeyId::KEY_S)
                    .state() == platform::Key::State::Pressed)
            camera->move_camera(graphics::Camera::Movement::BACKWARD, dt * speedMultiplier);
        if (platform->key(platform::KeyId::KEY_A)
                    .state() == platform::Key::State::Pressed)
            camera->move_camera(graphics::Camera::Movement::LEFT, dt * speedMultiplier);
        if (platform->key(platform::KeyId::KEY_D)
                    .state() == platform::Key::State::Pressed)
            camera->move_camera(graphics::Camera::Movement::RIGHT, dt * speedMultiplier);
        if (platform->key(platform::KeyId::KEY_E)
                    .state() == platform::Key::State::Pressed)
            camera->move_camera(graphics::Camera::Movement::UP, dt * speedMultiplier);
        if (platform->key(platform::KeyId::KEY_Q)
                    .state() == platform::Key::State::Pressed)
            camera->move_camera(graphics::Camera::Movement::DOWN, dt * speedMultiplier);

        auto mouse = platform->mouse();
        camera->rotate_camera(mouse.dx, mouse.dy);
        camera->zoom(mouse.scroll);
    }
}

void MainController::update_racer() {

    auto platform = core::Controller::get<platform::PlatformController>();
    float dt = 0.01;

    double Fd = F + fc * exp(-pow(t / tc, 2)) - fv * v;
    double At = A * (1.0 - 0.25 * exp(-pow(t / tc, 2)));
    double D = 0.5 * At * rho * Cd * pow(v - w, 2);

    a = (Fd - D) / m;
    // Euler step - modified to ensure stability
    v = v + a * dt;
    neg_z = neg_z + v * dt;
    // Next timestep
    t += dt;

    if (t < maxTime && finishLine < neg_z) { spdlog::info("Car finished race in {:.2f}s", t); } else { spdlog::info("Race over, didn't finish in time ({:.2f}s)", t); }

}

}
