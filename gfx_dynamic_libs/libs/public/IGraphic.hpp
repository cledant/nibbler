#ifndef NIBBLER_IGRAPHIC_HPP
#define NIBBLER_IGRAPHIC_HPP

#include <array>

#include <cstdint>

#include "glm/glm.hpp"

class IGraphic
{
  public:
    IGraphic() = default;
    virtual ~IGraphic() = default;
    IGraphic(IGraphic const &src) = delete;
    IGraphic &operator=(IGraphic const &rhs) = delete;
    IGraphic(IGraphic &&src) = delete;
    IGraphic &operator=(IGraphic &&rhs) = delete;

    static uint8_t constexpr NB_EVENT = 14;
    static uint32_t constexpr WIN_H = 720;
    static uint32_t constexpr WIN_W = 1280;

    static constexpr uint32_t MAX_SNAKES = 3;
    static constexpr uint32_t MAX_W = 100;
    static constexpr uint32_t MAX_H = 100;
    static constexpr uint32_t MAX_SNAKE_SIZE = MAX_W * MAX_H;

    enum NibblerEvent
    {
        CLOSE_WIN = 0,
        PAUSE,
        TOGGLE_WIN,
        P1_UP,
        P1_RIGHT,
        P1_DOWN,
        P1_LEFT,
        P2_UP,
        P2_RIGHT,
        P2_DOWN,
        P2_LEFT,
        SET_GLFW,
        SET_SFML,
        SET_SDL,
    };

    struct Snake
    {
        uint32_t curr_size;
        std::array<glm::vec2, MAX_SNAKE_SIZE> pos;
        std::array<glm::vec3, MAX_SNAKE_SIZE> color;
    };

    enum SnakeType
    {
        APPLES = 0,
        P1,
        P2,
    };

    enum DisplayType
    {
        DIRECT = 0,
        SQUARE,
    };

    virtual void init() = 0;
    virtual void terminate() = 0;
    virtual void createWindow(std::string &&name) = 0;
    virtual void deleteWindow() = 0;
    virtual uint8_t shouldClose() = 0;
    virtual void triggerClose() = 0;
    virtual void getEvents(uint8_t (&buffer)[NB_EVENT]) = 0;
    virtual void draw(Snake const &snake,
                      enum SnakeType snakeType,
                      enum DisplayType displayType) = 0;
    virtual void render() = 0;
    virtual void toggleFullscreen() = 0;
};

typedef IGraphic *gfx_creator_t();
typedef void gfx_deleter_t(IGraphic *);

#endif
