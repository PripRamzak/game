#include "sound_buffer.hxx"

#include <iostream>
#include <mutex>

#include <SDL3/SDL.h>

class game_sound_buffer final : public sound_buffer
{
public:
    uint8_t*   start            = nullptr;
    uint32_t   length           = 0;
    size_t     current_position = 0;
    bool       is_playing       = false;
    bool       is_looped        = false;
    std::mutex audio_mutex;
    // static SDL_AudioSpec spec;

public:
    game_sound_buffer(const char* file_path, SDL_AudioSpec* device_audio_spec)
    {
        SDL_RWops* data = SDL_RWFromFile(file_path, "rb");
        if (data == nullptr)
        {
            throw std::runtime_error(std::string("Can't open audio file: ") +
                                     std::string(file_path));
        }

        SDL_AudioSpec audio_spec_from_file{};
        if (SDL_LoadWAV_RW(data, 1, &audio_spec_from_file, &start, &length) ==
            nullptr)
        {
            throw std::runtime_error(
                std::string("Can't parse and load audio samples from file: ") +
                std::string(file_path));
        }

        if (audio_spec_from_file.channels != device_audio_spec->channels ||
            audio_spec_from_file.format != device_audio_spec->format ||
            audio_spec_from_file.freq != device_audio_spec->freq)
        {
            Uint8* output_bytes;
            int    output_length;

            int convert_status =
                SDL_ConvertAudioSamples(audio_spec_from_file.format,
                                        audio_spec_from_file.channels,
                                        audio_spec_from_file.freq,
                                        start,
                                        static_cast<int>(length),
                                        device_audio_spec->format,
                                        device_audio_spec->channels,
                                        device_audio_spec->freq,
                                        &output_bytes,
                                        &output_length);
            if (convert_status != 0)
            {
                throw std::runtime_error(
                    std::string("failed to convert WAV byte stream" +
                                std::string(SDL_GetError())));
            }

            SDL_free(start);
            start  = output_bytes;
            length = static_cast<uint32_t>(output_length);
        }
    }
    void play(audio_properties properties)
    {
        lock_thread();
        current_position = 0;
        is_playing       = true;
        is_looped        = (properties == audio_properties::looped);
        unlock_thread();
    }
    void     lock_thread() final { audio_mutex.lock(); }
    void     unlock_thread() final { audio_mutex.unlock(); }
    uint8_t* get_start() final { return start; }
    uint32_t get_length() final { return length; }
    size_t   get_current_position() final { return current_position; }
    bool     get_playing_status() final { return is_playing; }
    bool     get_loop_property() final { return is_looped; }
    void     add_rest(uint32_t rest) final { current_position += rest; }
    void     reset() final { current_position = 0; }
};

sound_buffer* create_sound_buffer(const char* file_path, void* audio_spec)
{
    sound_buffer* sound_buffer = new game_sound_buffer(
        file_path, reinterpret_cast<SDL_AudioSpec*>(audio_spec));
    return sound_buffer;
}