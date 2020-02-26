#ifndef SFML_AUDIO_HPP
#define SFML_AUDIO_HPP

#include "IAudio.hpp"

class SFMLAudio : public IAudio
{
  public:
    SFMLAudio();
    ~SFMLAudio() override = default;
    SFMLAudio(SFMLAudio const &src) = delete;
    SFMLAudio &operator=(SFMLAudio const &rhs) = delete;
    SFMLAudio(SFMLAudio &&src) = delete;
    SFMLAudio &operator=(SFMLAudio &&rhs) = delete;

    void init(std::string const &home) override;
    void terminate() override;
    void playGameTheme() override;
    void playGameOverTheme() override;
    void stopCurrentTheme() override;
    void pauseCurrentTheme() override;
    void setThemeVolume(float value) override;
    void playEatSound() override;
    void stopEatSound() override;
    void pauseEatSound() override;
    void setEatSoundVolume(float value) override;

  private:
    std::string _home;
    float _theme_volume;
    float _eat_sound_volume;
};

#endif
