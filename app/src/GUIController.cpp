#include <imgui.h>
#include <engine/core/Engine.hpp>
#include <app/GUIController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <app/MainController.hpp>

namespace engine::myapp {
void GUIController::initialize() { set_enable(false); }

void GUIController::poll_events() {
    const auto platform = engine::core::Controller::get<platform::PlatformController>();
    if (platform->key(platform::KeyId::KEY_F2)
                .state() == platform::Key::State::JustPressed) { set_enable(!is_enabled()); }
}

void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    auto mainCtrl = engine::core::Controller::get<MainController>();

    graphics->begin_gui();
    auto &cfg = engine::util::Configuration::config()["window"];
    static bool fullscreen = cfg.value("fullscreen", false);

    ImGui::Begin("Settings");

    ImGui::SliderFloat("Point Light Intensity", &mainCtrl->g_point_light_intensity, 0.0f, 20.0f);
    ImGui::SliderFloat3("Light Position", &mainCtrl->g_light_pos.x,
                        -200.0f, 200.0f,
                        "%.1f");

    if (ImGui::Checkbox("Fullscreen", &fullscreen)) { core::Controller::get<platform::PlatformController>()->set_fullscreen(fullscreen); }

    // MSAA
    ImGui::Checkbox("Enable MSAA", &mainCtrl->g_msaa_enabled);

    ImGui::End();

    // Počni novi ImGui prozor
    ImGui::Begin("Racer Simulation");

    // ----- Simulation Parameters -----
    ImGui::Text("Simulation Parameters");
    ImGui::Separator();
    ImGui::InputDouble("Mass (m)", &mainCtrl->m, 1.0, 10.0, "%.2f kg");
    ImGui::InputDouble("Drive Force (F)", &mainCtrl->F, 10.0, 100.0, "%.2f N");
    ImGui::InputDouble("Air Density (rho)", &mainCtrl->rho, 0.01, 0.10, "%.3f kg/m^3");
    ImGui::InputDouble("Area (A)", &mainCtrl->A, 0.10, 1.00, "%.2f m^2");
    ImGui::InputDouble("Drag Coef (Cd)", &mainCtrl->Cd, 0.01, 0.10, "%.2f");
    ImGui::InputDouble("Viscous Fric (fv)", &mainCtrl->fv, 1.0, 10.0, "%.2f N·s/m");
    ImGui::InputDouble("Crouch Force (fc)", &mainCtrl->fc, 10.0, 100.0, "%.2f N");
    ImGui::InputDouble("Crouch Time (tc)", &mainCtrl->tc, 0.01, 0.10, "%.2f s");

    // ----- Race Settings -----
    ImGui::Text("Race Settings");
    ImGui::Separator();
    ImGui::InputDouble("Finish Line (m)", &mainCtrl->finishLine, 1.0, 1000.0, "%.2f");
    ImGui::InputDouble("Max Time (s)", &mainCtrl->maxTime, 1.0, 3600.0, "%.2f");
    ImGui::InputDouble("Wind Speed (w)", &mainCtrl->w, 0.1, 100.0, "%.2f");

    // ----- Simulation State -----
    ImGui::Text("Simulation State");
    ImGui::Separator();
    ImGui::Text("Time (t): %.2f s", mainCtrl->t);
    ImGui::Text("Velocity (v): %.2f m/s", mainCtrl->v);
    ImGui::Text("Acceleration (a): %.2f m/s^2", mainCtrl->a);
    ImGui::Text("Distance (neg_z): %.2f m", mainCtrl->neg_z);

    ImGui::End();

    graphics->end_gui();
}
}
