#ifndef NIBBLER_IGRAPHIC_HPP
#define NIBBLER_IGRAPHIC_HPP

#include <cstdint>

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

    virtual void init() = 0;
    virtual void terminate() = 0;
    virtual void createWindow(std::string &&name) = 0;
    virtual void deleteWindow() = 0;
    virtual uint8_t shouldClose() = 0;
    virtual void triggerClose() = 0;
    virtual void getEvents(uint8_t (&buffer)[NB_EVENT]) = 0;
    virtual void draw() = 0;
    virtual void render() = 0;
    virtual void toggleFullscreen() = 0;

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
};

typedef IGraphic *gfx_creator_t();
typedef void gfx_deleter_t(IGraphic *);

#endif
