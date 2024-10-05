#include "include/sound_buffer.hxx"
#include "include/engine.hxx"

#include <mutex>

#include <SDL3/SDL.h>

namespace prip_engine
{

static SDL_AudioStream* stream{ nullptr };
SDL_AudioSpec           audio_in_spec;

static std::vector<sound_buffer*> sounds;

static void new_audio_callback(void*,
                               SDL_AudioStream* stream,
                               int              additional_amount,
                               int              total_amount)
{
    if (additional_amount > 0)
    {
        uint8_t* data = SDL_stack_alloc(uint8_t, additional_amount);
        if (data)
        {
            std::fill_n(data, additional_amount, '\0');
            for (sound_buffer* snd : sounds)
            {
                snd->lock_thread();
                if (snd->get_playing_status())
                {
                    uint32_t rest =
                        snd->get_length() - snd->get_current_index();
                    uint8_t* current_buff =
                        &snd->get_buffer()[snd->get_current_index()];

                    if (rest <= static_cast<uint32_t>(additional_amount))
                    {
                        SDL_MixAudio(data,
                                     current_buff,
                                     audio_in_spec.format,
                                     rest,
                                     0.5f);
                        snd->update_buffer(rest);
                    }
                    else
                    {
                        int res = SDL_MixAudio(
                            data,
                            current_buff,
                            audio_in_spec.format,
                            static_cast<uint32_t>(additional_amount),
                            0.5f);
                        snd->update_buffer(
                            static_cast<uint32_t>(additional_amount));
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

            int res = SDL_PutAudioStreamData(stream, data, additional_amount);
            SDL_stack_free(data);
        }
    }
}

bool init_sound()
{
    SDL_AudioSpec desired_audio_in_spec{ SDL_AUDIO_S16LE, 2, 44100 };
    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                       &desired_audio_in_spec,
                                       new_audio_callback,
                                       nullptr);

    if (!stream)
    {
        std::cerr << "Cant open audio device stream with desired spec. "
                  << SDL_GetError() << std::endl;

        stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                           nullptr,
                                           new_audio_callback,
                                           nullptr);
        if (!stream)
        {
            std::cerr << "Can't open audio device stream. " << SDL_GetError();
            return false;
        }
    }

    if (!SDL_GetAudioStreamFormat(stream, &audio_in_spec, nullptr))
    {
        std::cerr << "Can't get audio stream format. " << SDL_GetError();
        return false;
    }

    if (!SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream)))
    {
        std::cerr << "Can't resume audio device. " << SDL_GetError();
        return false;
    }

    return true;
}

class game_sound_buffer final : public sound_buffer
{
public:
    game_sound_buffer(const char* file_path)
    {
        SDL_AudioSpec audio_spec_from_file{};
        if (!SDL_LoadWAV(file_path, &audio_spec_from_file, &buffer, &length))
            throw std::runtime_error(
                std::string("Can't parse and load audio samples from file: ") +
                std::string(file_path));

        if (audio_spec_from_file.channels != audio_in_spec.channels ||
            audio_spec_from_file.format != audio_in_spec.format ||
            audio_spec_from_file.freq != audio_in_spec.freq)
        {
            uint8_t* output_bytes;
            int      output_length;

            bool convert_status =
                SDL_ConvertAudioSamples(&audio_spec_from_file,
                                        buffer,
                                        static_cast<int>(length),
                                        &audio_in_spec,
                                        &output_bytes,
                                        &output_length);
            if (!convert_status)
                throw std::runtime_error(
                    std::string("failed to convert WAV byte stream" +
                                std::string(SDL_GetError())));

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
    sounds.push_back(sound_buffer);
    return sound_buffer;
}
} // namespace prip_engine