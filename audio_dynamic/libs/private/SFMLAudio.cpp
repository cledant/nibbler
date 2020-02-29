#include "SFMLAudio.hpp"

#include <iostream>

SFMLAudio::SFMLAudio()
  : _home()
  , _theme()
  , _theme_volume({ 50 })
  , _current_theme(IAudioTypes::GAME)
  , _sound_buffer()
  , _sound()
  , _sound_volume({ 50 })
{}

void
SFMLAudio::init(std::string const &home)
{
    static const char *sound_filename[] = { "eat.wav", "spawn.wav" };
    static const char *theme_filename[] = { "game.wav", "game_over.wav" };

    _home = home;
    std::string base_path = _home + "/.nibbler/nibbler_sounds/";

    for (uint32_t i = 0; i < IAudioConstants::NB_SOUND; ++i) {
        std::string file = base_path + sound_filename[i];

        if (!_sound_buffer[i].loadFromFile(file)) {
            throw std::runtime_error("SFML audio: failed to load:" + file);
        }
        _sound[i].setBuffer(_sound_buffer[i]);
    }
    for (uint32_t i = 0; i < IAudioConstants::NB_THEME; ++i) {
        std::string file = base_path + theme_filename[i];

        if (!_theme[i].openFromFile(file)) {
            throw std::runtime_error("SFML audio: failed to load:" + file);
        }
    }
}

void
SFMLAudio::terminate()
{
    for (auto &it : _theme) {
        it.stop();
    }
    for (auto &it : _sound) {
        it.stop();
    }
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