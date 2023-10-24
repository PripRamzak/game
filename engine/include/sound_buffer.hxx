#pragma once

#include <cstdint>
#include <iostream>

enum class audio_properties
{
    once,
    looped
};

class sound_buffer
{
public:
    virtual void     update_buffer(uint32_t rest)      = 0;
    virtual void     play(audio_properties properties) = 0;
    virtual void     replay()                          = 0;
    virtual void     stop()                            = 0;
    virtual void     lock_thread()                     = 0;
    virtual void     unlock_thread()                   = 0;
    virtual uint8_t* get_buffer()                      = 0;
    virtual uint32_t get_length()                      = 0;
    virtual size_t   get_current_index()               = 0;
    virtual bool     get_playing_status()              = 0;
    virtual bool     get_loop_property()               = 0;
    virtual ~sound_buffer();
};

sound_buffer* create_sound_buffer(const char* file_path, void* audio_spec);