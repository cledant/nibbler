#ifndef NIBBLER_IAUDIO_HPP
#define NIBBLER_IAUDIO_HPP

#include <string>

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
    virtual void playGameTheme() = 0;
    virtual void playGameOverTheme() = 0;
    virtual void stopCurrentTheme() = 0;
    virtual void pauseCurrentTheme() = 0;
    virtual void setThemeVolume(float value) = 0;
    virtual void playEatSound() = 0;
    virtual void stopEatSound() = 0;
    virtual void pauseEatSound() = 0;
    virtual void setEatSoundVolume(float value) = 0;

    typedef IAudio *creator_t();
    typedef void deleter_t(IAudio *);
};

#endif
