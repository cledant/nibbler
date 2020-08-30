#include "PortAudio.hpp"

#include "portaudio.h"

#include <stdexcept>
#include <cstdio>

PortAudio::PortAudio()
  : _home()
  , _is_init(0)
  , _current_theme(IAudioTypes::GAME)
  , _beep()
{}

void
PortAudio::init(std::string const &home)
{
    _home = home;
    if (_is_init) {
        return;
    }
    freopen("/dev/null", "w", stderr);
    if (Pa_Initialize()) {
        throw std::runtime_error("Portaudio: failed to init");
    }
    freopen("/dev/tty", "w", stderr);
    for (auto &it : _beep) {
        it.init(Pa_GetDefaultOutputDevice());
        it.setDuration(0.5);
    }
    _beep[IAudioTypes::EAT].generateBeep(2000, 20);
    _beep[IAudioTypes::PAUSE].generateBeep(20000, 1000);
    _beep[IAudioTypes::BONUS].generateBeep(4000, 20);
    _is_init = 1;
}

void
PortAudio::terminate()
{
    if (_is_init) {
        Pa_Terminate();
    }
}

void
PortAudio::playTheme(enum IAudioTypes::NibblerTheme theme)
{
    _current_theme = theme;
}

void
PortAudio::stopCurrentTheme()
{}

void
PortAudio::pauseCurrentTheme()
{}

void
PortAudio::setAllThemeVolume(float value)
{
    (void)value;
}

void
PortAudio::playSound(enum IAudioTypes::NibbleSoundEffect sound)
{
    _beep[sound].play();
}

void
PortAudio::stopAllSounds()
{
    for (auto &it : _beep) {
        it.stop();
    }
}

void
PortAudio::pauseAllSounds()
{
    for (auto &it : _beep) {
        it.pause();
    }
}

void
PortAudio::setAllSoundVolume(float value)
{
    for (auto &it : _beep) {
        it.setVolume(value);
    }
}

extern "C" IAudio *
creator()
{
    return (new PortAudio());
}

extern "C" void
deleter(IAudio *audio)
{
    delete audio;
}