#include "include/sound_buffer.hxx"
#include "include/engine.hxx"

#include <iostream>
#include <mutex>

#include <SDL3/SDL.h>

namespace prip_engine
{

static SDL_AudioDeviceID          audio_device;
static SDL_AudioSpec              device_audio_spec;
static std::vector<sound_buffer*> sounds;

static void audio_callback(void*, uint8_t* stream, int stream_size)
{
    std::fill_n(stream, stream_size, '\0');

    for (sound_buffer* snd : sounds)
    {
        snd->lock_thread();
        if (snd->get_playing_status())
        {
            uint32_t rest = snd->get_length() - snd->get_current_index();
            uint8_t* current_buff =
                &snd->get_buffer()[snd->get_current_index()];

            if (rest <= static_cast<uint32_t>(stream_size))
            {
                SDL_MixAudioFormat(stream,
                                   current_buff,
                                   device_audio_spec.format,
                                   rest,
                                   SDL_MIX_MAXVOLUME);
                snd->update_buffer(rest);
            }
            else
            {
                SDL_MixAudioFormat(stream,
                                   current_buff,
                                   device_audio_spec.format,
                                   static_cast<uint32_t>(stream_size),
                                   SDL_MIX_MAXVOLUME);
                snd->update_buffer(static_cast<uint32_t>(stream_size));
            }

            if (snd->get_current_index() == snd->get_length())
            {
                if (snd->get_loop_property())
                    snd->replay();
                else
                    snd->stop();
            }
        }
        snd->unlock_thread();
    }
}

bool sound_buffer::init(std::string_view platform)
{
    using namespace std::string_view_literals;

    SDL_AudioSpec desired_audio_spec{};
    desired_audio_spec.freq     = 44100;
    desired_audio_spec.format   = AUDIO_S16LSB;
    desired_audio_spec.channels = 2;
    desired_audio_spec.samples  = 1024;
    desired_audio_spec.callback = audio_callback;
    desired_audio_spec.userdata = nullptr;

    const char* audio_device_name = nullptr;
    const int   num_audio_devices = SDL_GetNumAudioDevices(SDL_FALSE);
    for (int i = 0; i < num_audio_devices; i++)
    {
        std::cout << "audio device #" << i << ": "
                  << SDL_GetAudioDeviceName(i, SDL_FALSE) << '\n';
    }
    audio_device_name = SDL_GetAudioDeviceName(0, SDL_FALSE);

    if (platform == "Windows"sv)
        audio_device_name = SDL_GetAudioDeviceName(1, SDL_FALSE);

    audio_device = SDL_OpenAudioDevice(audio_device_name,
                                       SDL_FALSE,
                                       &desired_audio_spec,
                                       &device_audio_spec,
                                       SDL_AUDIO_ALLOW_ANY_CHANGE);

    if (audio_device == 0)
        return false;

    SDL_PlayAudioDevice(audio_device);
    return true;
}

class game_sound_buffer final : public sound_buffer
{
public:
    game_sound_buffer(const char* file_path)
    {
        SDL_RWops* data = SDL_RWFromFile(file_path, "rb");
        if (data == nullptr)
        {
            throw std::runtime_error(std::string("Can't open audio file: ") +
                                     std::string(file_path));
        }

        SDL_AudioSpec audio_spec_from_file{};
        if (SDL_LoadWAV_RW(data, 1, &audio_spec_from_file, &buffer, &length) ==
            nullptr)
        {
            throw std::runtime_error(
                std::string("Can't parse and load audio samples from file: ") +
                std::string(file_path));
        }

        if (audio_spec_from_file.channels != device_audio_spec.channels ||
            audio_spec_from_file.format != device_audio_spec.format ||
            audio_spec_from_file.freq != device_audio_spec.freq)
        {
            uint8_t* output_bytes;
            int      output_length;

            int convert_status =
                SDL_ConvertAudioSamples(audio_spec_from_file.format,
                                        audio_spec_from_file.channels,
                                        audio_spec_from_file.freq,
                                        buffer,
                                        static_cast<int>(length),
                                        device_audio_spec.format,
                                        device_audio_spec.channels,
                                        device_audio_spec.freq,
                                        &output_bytes,
                                        &output_length);
            if (convert_status != 0)
            {
                throw std::runtime_error(
                    std::string("failed to convert WAV byte stream" +
                                std::string(SDL_GetError())));
            }

            SDL_free(buffer);
            buffer = output_bytes;
            length = static_cast<uint32_t>(output_length);
        }
    }
    void update_buffer(uint32_t rest) final { current_index += rest; }
    void play(audio_properties properties)
    {
        lock_thread();
        current_index = 0;
        is_playing    = true;
        is_looped     = (properties == audio_properties::looped);
        unlock_thread();
    }
    void     replay() final { current_index = 0; }
    void     stop() final { is_playing = 0; }
    void     lock_thread() final { audio_mutex.lock(); }
    void     unlock_thread() final { audio_mutex.unlock(); }
    uint8_t* get_buffer() final { return buffer; }
    uint32_t get_length() final { return length; }
    size_t   get_current_index() final { return current_index; }
    bool     get_playing_status() final { return is_playing; }
    bool     get_loop_property() final { return is_looped; }

private:
    uint8_t*   buffer        = nullptr;
    uint32_t   length        = 0;
    size_t     current_index = 0;
    bool       is_playing    = false;
    bool       is_looped     = false;
    std::mutex audio_mutex;
};

sound_buffer::~sound_buffer() = default;

sound_buffer* create_sound_buffer(const char* file_path)
{
    sound_buffer* sound_buffer = new game_sound_buffer(file_path);
    sound_buffer->lock_thread();
    sounds.push_back(sound_buffer);
    sound_buffer->unlock_thread();
    return sound_buffer;
}
} // namespace prip_engine