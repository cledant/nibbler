#ifndef SFML_AUDIO_HPP
#define SFML_AUDIO_HPP

#include <array>

#include "IAudio.hpp"
#include "IAudioConstants.hpp"

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
    void playTheme(enum IAudioTypes::NibblerTheme theme) override;
    void stopCurrentTheme() override;
    void pauseCurrentTheme() override;
    void setAllThemeVolume(float value) override;
    void playSound(enum IAudioTypes::NibbleSoundEffect sound) override;
    void stopAllSounds() override;
    void pauseAllSounds() override;
    void setAllSoundVolume(float value) override;

  private:
    std::string _home;

    // Theme related
    std::array<float, IAudioConstants::NB_THEME> _theme_volume;
    IAudioTypes::NibblerTheme _current_theme;

    // Sound Related
    std::array<float, IAudioConstants::NB_SOUND> _sound_volume;
};

#endif
