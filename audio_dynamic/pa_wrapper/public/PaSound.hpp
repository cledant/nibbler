#ifndef PA_SOUND_HPP
#define PA_SOUND_HPP

#include <cstdint>
#include <memory>
#include <atomic>

#include "portaudio.h"

class PaSound
{
  public:
    PaSound();
    virtual ~PaSound() = default;
    PaSound(PaSound const &src) = delete;
    PaSound &operator=(PaSound const &rhs) = delete;
    PaSound(PaSound &&src) = delete;
    PaSound &operator=(PaSound &&rhs) = delete;

    virtual void init(PaDeviceIndex index);
    virtual void terminate();
    void play();
    void pause();
    void stop();
    void setVolume(float volume);
    void setDuration(double duration);
    void generateBeep(double fs, double f_min);

  private:
    static constexpr uint64_t SAMPLE_RATE = 44100;
    static constexpr uint64_t FRAMES_PER_BUFFER = 256;

    std::atomic<uint8_t> _is_init;
    std::atomic<uint8_t> _is_playing;
    std::unique_ptr<float[]> _sound_buffer;
    uint64_t _sound_buffer_length;
    std::atomic<uint64_t> _current_buffer_pos;
    PaStream *_stream;
    std::atomic<float> _volume;
    std::atomic<double> _duration;
    std::atomic<double> _current_duration;


    static int32_t _api_callback(void const *inputBuffer,
                                 void *outputBuffer,
                                 uint64_t framesPerBuffer,
                                 PaStreamCallbackTimeInfo const *timeInfo,
                                 PaStreamCallbackFlags statusFlags,
                                 void *userData);
    static void _api_stream_finished(void *userData);

    virtual int32_t _class_callback(void const *inputBuffer,
                                    void *outputBuffer,
                                    uint64_t framesPerBuffer,
                                    PaStreamCallbackTimeInfo const *timeInfo,
                                    PaStreamCallbackFlags statusFlags);
    virtual void _class_stream_finish();
};

#endif