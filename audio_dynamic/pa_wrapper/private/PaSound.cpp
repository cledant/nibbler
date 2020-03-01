#include "PaSound.hpp"

#include <chrono>
#include <cmath>

PaSound::PaSound()
  : _is_init(0)
  , _is_playing(0)
  , _sound_buffer_length(0)
  , _current_buffer_pos(0)
  , _stream(nullptr)
  , _volume(1.0f)
  , _duration(1.0)
  , _current_duration(0.0)
{}

void
PaSound::init(PaDeviceIndex index)
{
    if (index == paNoDevice) {
        return;
    }

    PaStreamParameters outputParameters;
    outputParameters.device = index;
    outputParameters.channelCount = 2;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency =
      Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    PaError err = Pa_OpenStream(&_stream,
                                nullptr,
                                &outputParameters,
                                SAMPLE_RATE,
                                FRAMES_PER_BUFFER,
                                paClipOff,
                                &PaSound::_api_callback,
                                this);
    if (err != paNoError) {
        throw std::runtime_error("Failed to open stream for sound");
    }

    err = Pa_SetStreamFinishedCallback(_stream, &PaSound::_api_stream_finished);
    if (err != paNoError) {
        throw std::runtime_error("Failed to set stream close callback");
    }
    generateBeep(10000, 500);
    _is_init = 1;
}

void
PaSound::terminate()
{
    stop();
    if (_is_init) {
        Pa_CloseStream(_stream);
    }
}

void
PaSound::play()
{
    if (!_is_init || _is_playing) {
        return;
    }
    Pa_StopStream(_stream);
    Pa_StartStream(_stream);
    _is_playing = 1;
}

void
PaSound::pause()
{
    if (!_is_init || !_is_playing) {
        return;
    }
    Pa_StopStream(_stream);
    _is_playing = 0;
}

void
PaSound::stop()
{
    if (!_is_init || !_is_playing) {
        return;
    }
    Pa_StopStream(_stream);
    _current_buffer_pos = 0;
    _is_playing = 0;
    _current_duration = 0.0;
}

void
PaSound::setVolume(float volume)
{
    if (volume < 0.0f) {
        _volume = 0.0f;
        return;
    } else if (volume >= 100.0f) {
        _volume = 1.0f;
        return;
    } else {
        _volume = volume / 100.0f;
    }
}

void
PaSound::setDuration(double duration)
{
    _duration = duration;
}

void
PaSound::generateBeep(double fs, double f_min)
{
    _sound_buffer_length = floor(fs / f_min) + 1;
    _sound_buffer = std::make_unique<float[]>(_sound_buffer_length);

    for (uint64_t i = 0; i < _sound_buffer_length; ++i) {
        _sound_buffer[i] = sin(
          (static_cast<double>(i) / static_cast<double>(_sound_buffer_length)) *
          2.0 * M_PI);
    }
}

int32_t
PaSound::_api_callback(void const *inputBuffer,
                       void *outputBuffer,
                       uint64_t framesPerBuffer,
                       PaStreamCallbackTimeInfo const *timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void *userData)
{
    return (reinterpret_cast<PaSound *>(userData)->_class_callback(
      inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags));
}

void
PaSound::_api_stream_finished(void *userData)
{
    return (reinterpret_cast<PaSound *>(userData)->_class_stream_finish());
}

int32_t
PaSound::_class_callback(void const *inputBuffer,
                         void *outputBuffer,
                         uint64_t framesPerBuffer,
                         PaStreamCallbackTimeInfo const *timeInfo,
                         PaStreamCallbackFlags statusFlags)
{
    static_cast<void>(inputBuffer);
    static_cast<void>(timeInfo);
    static_cast<void>(statusFlags);
    auto *output = static_cast<float *>(outputBuffer);
    auto ref = std::chrono::high_resolution_clock::now();

    for (uint64_t i = 0; i < framesPerBuffer; ++i) {
        if (_current_buffer_pos >= _sound_buffer_length) {
            _current_buffer_pos = 0;
        }
        *output++ = _volume * _sound_buffer[_current_buffer_pos];
        *output++ = _volume * _sound_buffer[_current_buffer_pos];
        ++_current_buffer_pos;
    }

    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> loop_diff = now - ref;
    _current_duration = _current_duration + loop_diff.count() * 1000;

    if (_current_duration.load() >= _duration) {
        return (paComplete);
    } else {
        return (paContinue);
    }
}

void
PaSound::_class_stream_finish()
{
    _current_buffer_pos = 0;
    _is_playing = 0;
    _current_duration = 0.0;
}