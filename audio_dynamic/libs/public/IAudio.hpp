#ifndef NIBBLER_IAUDIO_HPP
#define NIBBLER_IAUDIO_HPP

#include <string>

#include "IAudioTypes.hpp"

class IAudio
{
  public:
    IAudio() = default;
    virtual ~IAudio() = default;
    IAudio(IAudio const &src) = delete;
    IAudio &operator=(IAudio const &rhs) = delete;
    IAudio(IAudio &&src) = delete;
    IAudio &operator=(IAudio &&rhs) = delete;

    virtual void init(std::string const &home) = 0;
    virtual void terminate() = 0;
    virtual void playTheme(enum IAudioTypes::NibblerTheme theme) = 0;
    virtual void stopCurrentTheme() = 0;
    virtual void pauseCurrentTheme() = 0;
    virtual void setAllThemeVolume(float value) = 0;
    virtual void playSound(enum IAudioTypes::NibbleSoundEffect sound) = 0;
    virtual void stopAllSounds() = 0;
    virtual void pauseAllSounds() = 0;
    virtual void setAllSoundVolume(float value) = 0;

    typedef IAudio *creator_t();
    typedef void deleter_t(IAudio *);
};

#endif
