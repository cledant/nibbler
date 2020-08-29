#ifndef IGRAPHICTYPES_HPP
#define IGRAPHICTYPES_HPP

namespace IGraphicTypes {

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
    SET_AUDIO_PORTAUDIO,
    SET_AUDIO_NONE,
    MUTE_UNMUTE
};

}
#endif
