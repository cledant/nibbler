#ifndef SFML_AUDIO_HPP
#define SFML_AUDIO_HPP

#include <array>

#include "IAudio.hpp"
#include "IAudioConstants.hpp"
#include "PaSound.hpp"

class PortAudio : public IAudio
{
  public:
    PortAudio();
    ~PortAudio() override = default;
    PortAudio(PortAudio const &src) = delete;
    PortAudio &operator=(PortAudio const &rhs) = delete;
    PortAudio(PortAudio &&src) = delete;
    PortAudio &operator=(PortAudio &&rhs) = delete;

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
    uint8_t _is_init;
    IAudioTypes::NibblerTheme _current_theme;
    std::array<PaSound, IAudioConstants::NB_SOUND> _beep;
};

#endif
