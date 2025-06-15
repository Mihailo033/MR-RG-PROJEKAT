// GUIController.cpp
#include <imgui.h>
#include <engine/core/Engine.hpp>
#include <app/GUIController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <app/MainController.hpp>   // Potrebno da učitaš deklaraciju MainController

namespace engine::test::app {

void GUIController::initialize() { set_enable(false); }

void GUIController::poll_events() {
    const auto platform = engine::core::Controller::get<platform::PlatformController>();
    if (platform->key(platform::KeyId::KEY_F2)
                .state() == platform::Key::State::JustPressed) { set_enable(!is_enabled()); }
}

void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    // Uzmi MainController kako bismo modifikovali m_pointLightIntensity:
    auto mainCtrl = engine::core::Controller::get<MainController>();

    // Otvorimo ImGui pass
    graphics->begin_gui();

    // 1) Kamera info prozor (postojeći)
    ImGui::Begin("Camera info");
    const auto &c = *camera;
    ImGui::Text("Camera position: (%.1f, %.1f, %.1f)", c.Position.x, c.Position.y, c.Position.z);
    ImGui::Text("(Yaw, Pitch): (%.1f, %.1f)", c.Yaw, c.Pitch);
    ImGui::Text("Camera front: (%.1f, %.1f, %.1f)", c.Front.x, c.Front.y, c.Front.z);
    ImGui::End();

    // 2) Dodaj novi prozor za kontrolu intenziteta point light-a
    ImGui::Begin("Postavke svetla");
    // Ubaci slider koji menja mainCtrl->m_pointLightIntensity
    // Tasteri: od 0.0 do 5.0 (možeš prilagoditi range)
    ImGui::SliderFloat("Point Light Intensity", &mainCtrl->pointLightIntensity, 0.0f, 7.0f);
    ImGui::SliderFloat3("Light Position", &mainCtrl->lightPos.x,
                        -200.0f, 200.0f,
                        "%.1f");

    // MSAA
    ImGui::Checkbox("Enable MSAA", &mainCtrl->msaaEnabled);

    ImGui::End();

    // Zatvorimo ImGui pass
    graphics->end_gui();
}

}// namespace engine::test::app
