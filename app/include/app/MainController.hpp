#ifndef MYAPP_MAINCONTROLLER_HPP
#define MYAPP_MAINCONTROLLER_HPP

#include <string_view>
#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>
#include <engine/graphics/MSAA.hpp>
#include <engine/graphics/Lighting.hpp>

namespace engine::myapp {

class MainController final : public engine::core::Controller {
public:
    // MSAA
    bool g_msaa_enabled = true;

    // Lighting
    float g_point_light_intensity = 20.0f;                                 // Intenzitet point light svetla
    glm::vec3 g_light_pos{m_runnerPosition + glm::vec3(0.0f, 15.0f, 0.0f)};// Pozicija point light svetla

    std::string_view name() const override { return "test::app::MainController"; }

    // --- fizički parametri i stanje trkača ---
    double m = 80.0;   // kg (mass)
    double F = 400.0;  // N (constant driving force)
    double rho = 1.293;// kg/m^3 (air density)
    double A = 0.45;   // m^2 (cross-sectional area)
    double Cd = 1.2;   // drag coefficient
    double fv = 25.8;  // N·s/m (velocity dependent force)
    double fc = 488.0; // N (initial crouch force)
    double tc = 0.67;  // s (characteristic crouch time)

    double finishLine = 160.0f;
    double maxTime = 15.0f;
    double neg_z = 0.0;
    double w = 10.0f;
    double t = 0.0;
    double v = 0.0;
    double a = 0.0;
    // ----------------------------------------------------------

private:
    // MSAA
    std::unique_ptr<engine::graphics::MSAA> m_msaa;

    // Lighting
    engine::graphics::lighting::LightingSystem m_lighting{2048, 2048};

    // SprintSim
    bool m_raceStarted = false;
    bool m_loggedFinish = false;
    bool m_autoMoveLeft = false;
    float m_leftTargetDistance = 160.0f;
    float m_leftMovedDistance = 0.0f;

    glm::vec3 m_runnerPosition = glm::vec3(0.0f, -7.7f, 185.0f);

    double calculateAirResistance(double t, double v, double w);

    double calculateDrivingForce(double t, double v);

    void update_racer();

    void updateModelMatrix();

    // --------------------------------------------------------------------

    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

    void draw_mesh(auto model, auto shader,
                   const glm::vec3 &position,
                   const glm::vec3 &rotation,
                   const glm::vec3 &scale);

    void draw_light_source_mesh(const glm::vec3 &lightPos, float scale);

    void set_lights(auto shader);

    void draw_skybox();

    void update_camera();

    void update_racer(double dt, double maxTime, double w, double finishLine);

    float m_backpack_scale{1.0f};
    bool m_draw_gui{false};
    bool m_cursor_enabled{true};
};

}// namespace engine::myapp

#endif
