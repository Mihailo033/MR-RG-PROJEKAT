#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

namespace engine::test::app {
    class MainPlatformEventObserver final : public engine::platform::PlatformEventObserver {
    public:
        void on_key(engine::platform::Key key) override;

        void on_mouse_move(engine::platform::MousePosition position) override;
    };

    class MainController final : public engine::core::Controller {
    public:
        std::string_view name() const override {
            return "test::app::MainController";
        }

    private:
        // MSAA off-screen anti-aliasing
        static constexpr unsigned int MSAA_SAMPLES = 4; // број узорака
        unsigned int msFBO                         = 0; // MSAA FBO
        unsigned int msColorRBO                    = 0; // MSAA color renderbuffer
        unsigned int msDepthRBO                    = 0; // MSAA depth-stencil renderbuffer

        // Shadow map резолуција
        static constexpr unsigned int SHADOW_WIDTH  = 2048;
        static constexpr unsigned int SHADOW_HEIGHT = 2048;

        // Depth‐framebuffer и текстура
        unsigned int depthMapFBO = 0;
        unsigned int depthMap    = 0;

        // Матрица за трансформацију у светлосни простор
        glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

        void initialize() override;

        bool loop() override;

        void poll_events() override;

        void update() override;

        void begin_draw() override;

        void draw() override;

        void end_draw() override;

        void draw_mesh(auto model, auto shader, const glm::vec3 &position,
                       const glm::vec3 &rotation,
                       const glm::vec3 &scale);

        void draw_light_source_mesh(const glm::vec3 &lightPos, float scale);

        void set_lights(auto shader);

        void draw_skybox();

        void update_camera();

        float m_backpack_scale{1.0f};
        bool m_draw_gui{false};
        bool m_cursor_enabled{true};
    };
}
#endif //MAINCONTROLLER_HPP
