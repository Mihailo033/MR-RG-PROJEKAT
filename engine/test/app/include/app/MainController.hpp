
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
        void initialize() override;

        bool loop() override;

        void poll_events() override;

        void update() override;

        void begin_draw() override;

        void draw() override;

        void end_draw() override;

        void draw_light_source_mesh(const glm::vec3 &lightPos, float scale);

        void set_point_lights(auto shader);

        void draw_backpack(const glm::vec3 &modelPos);

        void draw_skybox();

        void update_camera();

        float m_backpack_scale{1.0f};
        bool m_draw_gui{false};
        bool m_cursor_enabled{true};
    };
}
#endif //MAINCONTROLLER_HPP
