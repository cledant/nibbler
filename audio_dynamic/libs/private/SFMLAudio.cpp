#include "SFMLAudio.hpp"

#include <iostream>

SFMLAudio::SFMLAudio()
  : _home()
  , _theme_volume({ 50 })
  , _current_theme(IAudioTypes::GAME)
  , _sound_volume({ 50 })
{}

void
SFMLAudio::init(std::string const &home)
{
    _home = home;
    std::cout << "Loaded SFML Sound" << std::endl;
}

void
SFMLAudio::terminate()
{
    std::cout << "Closed SFML Sound" << std::endl;
}

void
SFMLAudio::playTheme(enum IAudioTypes::NibblerTheme theme)
{
    (void)theme;
}

void
SFMLAudio::stopCurrentTheme()
{}

void
SFMLAudio::pauseCurrentTheme()
{}

void
SFMLAudio::setAllThemeVolume(float value)
{
    for (auto &it : _theme_volume) {
        it = value;
    }
}

void
SFMLAudio::playSound(enum IAudioTypes::NibbleSoundEffect sound)
{
    (void)sound;
}

void
SFMLAudio::stopAllSounds()
{}

void
SFMLAudio::pauseAllSounds()
{}

void
SFMLAudio::setAllSoundVolume(float value)
{
    for (auto &it : _sound_volume) {
        it = value;
    }
}

extern "C" IAudio *
creator()
{
    return (new SFMLAudio());
}

extern "C" void
deleter(IAudio *audio)
{
    delete audio;
}