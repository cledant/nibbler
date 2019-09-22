#ifndef NIBBLER_IGRAPHIC_HPP
#define NIBBLER_IGRAPHIC_HPP

#include <cstdint>

class IGraphic
{
  public:
    IGraphic() = default;
    virtual ~IGraphic() = default;

    static uint8_t constexpr NB_EVENT = 14;

    virtual void init() = 0;
    virtual void terminate() = 0;
    virtual void createWindow(int32_t w, int32_t h, std::string &&name) = 0;
    virtual void deleteWindow() = 0;
    virtual uint8_t shouldClose() = 0;
    virtual void getEvents(uint8_t (&buffer)[NB_EVENT]) = 0;

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
