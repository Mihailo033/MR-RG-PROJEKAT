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
    ImGui::Text("Mass (m): %.2f kg", mainCtrl->m);
    ImGui::Text("Drive Force (F): %.2f N", mainCtrl->F);
    ImGui::Text("Air Density (rho): %.3f kg/m^3", mainCtrl->rho);
    ImGui::Text("Area (A): %.2f m^2", mainCtrl->A);
    ImGui::Text("Drag Coefficient (Cd): %.2f", mainCtrl->Cd);
    ImGui::Text("Viscous Force (fv): %.2f N·s/m", mainCtrl->fv);
    ImGui::Text("Crouch Force (fc): %.2f N", mainCtrl->fc);
    ImGui::Text("Crouch Time (tc): %.2f s", mainCtrl->tc);

    // ----- Race Settings -----
    ImGui::Text("Race Settings");
    ImGui::Separator();
    ImGui::Text("Finish Line: %.2f m", mainCtrl->finishLine);
    ImGui::Text("Max Time: %.2f s", mainCtrl->maxTime);
    ImGui::Text("Wind Speed (w): %.2f m/s", mainCtrl->w);

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
