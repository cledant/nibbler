#include "SFMLAudio.hpp"

#include <iostream>

SFMLAudio::SFMLAudio()
  : _home()
  , _theme_volume(50)
  , _eat_sound_volume(50)
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
SFMLAudio::playGameTheme()
{}

void
SFMLAudio::playGameOverTheme()
{}

void
SFMLAudio::stopCurrentTheme()
{}

void
SFMLAudio::pauseCurrentTheme()
{}

void
SFMLAudio::setThemeVolume(float value)
{
    _theme_volume = value;
}

void
SFMLAudio::playEatSound()
{}

void
SFMLAudio::stopEatSound()
{}

void
SFMLAudio::pauseEatSound()
{}

void
SFMLAudio::setEatSoundVolume(float value)
{
    _eat_sound_volume = value;
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