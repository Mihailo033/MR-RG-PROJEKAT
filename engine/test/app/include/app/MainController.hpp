#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <GL/gl.h>
#include <vector>
#include <string>
#include <tuple>
#include <glm/glm.hpp>
#include <engine/graphics/MSAA.hpp>

namespace engine::test::app {
    struct ScheduledEvent {
        float triggerTime;     // vreme kada se event aktivira (u sekundama)
        std::string eventName; // npr. "START_FLICKER" ili "SPAWN_MODEL"
    };

    class MainPlatformEventObserver final : public engine::platform::PlatformEventObserver {
    public:
        void on_key(engine::platform::Key key) override;

        void on_mouse_move(engine::platform::MousePosition position) override;
    };

    class MainController final : public engine::core::Controller {
    public:
        // MSAA
        bool msaaEnabled = true;

        float pointLightIntensity = 7.0f;
        glm::vec3 lightPos{-10.0f, 10.0f, 2.0f};

        // -------Scheduled event--------------------------------------------------------
        // Queue svih zakazanih događaja
        std::vector<ScheduledEvent> eventQueue;

        // Flag i vreme za početnu akciju
        float currentTime       = 0.0f; // u sekundama, broji vreme od starta
        bool actionTriggered    = false;
        float actionTriggerTime = 0.0f;

        // Za treptanje svetla
        float flickerDuration  = 2.0f;
        bool flickerActive     = false;
        float flickerStartTime = 0.0f;

        // Za spawn event
        float spawnDelay = 3.0f;

        // Spawn-ovani objekti: ime modela + pozicija, rotacija, skala
        std::vector<std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3> > spawnedObjects;

        void executeEvent(const std::string &eventName);

        // ---------------------------------------------------------------------------------------

        std::string_view name() const override {
            return "test::app::MainController";
        }

    private:
        int width, height;

        // MSAA
        std::unique_ptr<engine::graphics::MSAA> _msaa;

        // Point shadows
        static constexpr unsigned SHADOW_WIDTH  = 2048;
        static constexpr unsigned SHADOW_HEIGHT = 2048;
        GLuint depthMapFBO                      = 0;
        GLuint depthCubemap                     = 0;
        float near_plane                        = 1.0f, far_plane = 25.0f;
        glm::mat4 shadowMatrices[6];

        void renderSceneDepth(const resources::Shader *depthShader);

        void renderSceneLit(const resources::Shader *litShader);

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

        bool m_cursor_enabled{true};
    };
}

#endif // MAINCONTROLLER_HPP
