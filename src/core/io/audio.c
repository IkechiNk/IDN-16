#include "idn16/io/audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Audio system state
static SDL_AudioDeviceID audio_device = 0;
static SDL_AudioStream *audio_stream = NULL;
static SDL_AudioSpec audio_spec;
static AudioChannel audio_channels[4] = {0};
static float master_volume = 1.0f;
static bool audio_enabled = true;

// Audio callback function
static void audio_callback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount) {
    uint8_t *memory = (uint8_t*)userdata;  // Get memory from userdata
    const int sample_rate = 48000;
    const int channels = 2; // Stereo
    const int bytes_per_sample = sizeof(float);
    const int frame_size = channels * bytes_per_sample;
    
    int samples_needed = total_amount / frame_size;
    float *buffer = (float*)malloc(samples_needed * frame_size);
    
    if (!buffer) return;
    
    memset(buffer, 0, samples_needed * frame_size);
    
    if (audio_enabled) {
        // Mix all active channels
        for (int ch = 0; ch < 4; ch++) {
            if (audio_channels[ch].enabled && audio_channels[ch].remaining_time > 0) {
                float ch_volume = (audio_channels[ch].volume / 255.0f) * master_volume;
                
                for (int i = 0; i < samples_needed; i++) {
                    // Generate sine wave
                    float sample = sinf(audio_channels[ch].phase) * ch_volume * 0.1f; // Reduce volume
                    
                    // Mix to stereo
                    buffer[i * 2] += sample;     // Left
                    buffer[i * 2 + 1] += sample; // Right
                    
                    // Update phase
                    audio_channels[ch].phase += 2.0f * M_PI * audio_channels[ch].frequency / sample_rate;
                    if (audio_channels[ch].phase >= 2.0f * M_PI) {
                        audio_channels[ch].phase -= 2.0f * M_PI;
                    }
                }
                
                // Update remaining time
                audio_channels[ch].remaining_time -= (float)samples_needed / sample_rate;
                if (audio_channels[ch].remaining_time <= 0) {
                    audio_channels[ch].enabled = false;
                    // Also disable in memory to prevent restart
                    if (memory) {
                        uint16_t ch_base = AUDIO_REG_START + (ch * 6);
                        memory_write_byte(memory, ch_base + 5, 0, true);
                    }
                }
            }
        }
    }
    
    SDL_PutAudioStreamData(stream, buffer, samples_needed * frame_size);
    free(buffer);
}

bool audio_init(uint8_t *memory) {
    // Initialize Audio
    SDL_AudioSpec desired_spec;
    SDL_zero(desired_spec);
    desired_spec.freq = 48000;
    desired_spec.format = SDL_AUDIO_F32;
    desired_spec.channels = 2;
    
    audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired_spec);
    if (audio_device == 0) {
        SDL_Log("Failed to open audio device: %s\n", SDL_GetError());
        return false;
    }
    
    // Create audio stream
    audio_stream = SDL_CreateAudioStream(&desired_spec, &desired_spec);
    if (!audio_stream) {
        SDL_Log("Failed to create audio stream: %s\n", SDL_GetError());
        SDL_CloseAudioDevice(audio_device);
        audio_device = 0;
        return false;
    }
    
    // Bind stream to device
    if (!SDL_BindAudioStream(audio_device, audio_stream)) {
        SDL_Log("Failed to bind audio stream to device: %s\n", SDL_GetError());
        SDL_DestroyAudioStream(audio_stream);
        SDL_CloseAudioDevice(audio_device);
        audio_stream = NULL;
        audio_device = 0;
        return false;
    }
    
    // Set callback
    if (!SDL_SetAudioStreamGetCallback(audio_stream, audio_callback, memory)) {
        SDL_Log("Failed to set audio stream callback: %s\n", SDL_GetError());
        SDL_DestroyAudioStream(audio_stream);
        SDL_CloseAudioDevice(audio_device);
        audio_stream = NULL;
        audio_device = 0;
        return false;
    }
    
    if (!SDL_ResumeAudioDevice(audio_device)) {
        SDL_Log("Failed to resume audio device: %s\n", SDL_GetError());
        SDL_DestroyAudioStream(audio_stream);
        SDL_CloseAudioDevice(audio_device);
        audio_stream = NULL;
        audio_device = 0;
        return false;
    }
    
    printf("Audio system initialized\n");
    return true;
}

void audio_destroy(void) {
    if (audio_stream) {
        SDL_DestroyAudioStream(audio_stream);
        audio_stream = NULL;
    }
    if (audio_device) {
        SDL_CloseAudioDevice(audio_device);
        audio_device = 0;
    }
}

void audio_update(uint8_t *memory) {
    if (!memory) return;
    
    // Update master volume
    uint8_t master_vol = memory_read_byte(memory, AUDIO_MASTER_VOLUME);
    master_volume = master_vol / 255.0f;
    
    // Update global audio enable
    audio_enabled = memory_read_byte(memory, AUDIO_GLOBAL_ENABLE) != 0;
    
    // Update each channel
    for (int ch = 0; ch < 4; ch++) {
        uint16_t ch_base = AUDIO_REG_START + (ch * 6);
        
        bool enabled = memory_read_byte(memory, ch_base + 5) != 0;
        
        if (enabled && !audio_channels[ch].enabled) {
            // Channel just got enabled - start new tone
            audio_channels[ch].frequency = memory_read_word(memory, ch_base);
            audio_channels[ch].duration = memory_read_word(memory, ch_base + 2);
            audio_channels[ch].volume = memory_read_byte(memory, ch_base + 4);
            audio_channels[ch].remaining_time = audio_channels[ch].duration / 1000.0f;
            audio_channels[ch].phase = 0.0f;
            audio_channels[ch].enabled = true;
        } else if (!enabled) {
            // Channel disabled
            audio_channels[ch].enabled = false;
        }
    }
}

void audio_set_enabled(bool enabled) {
    audio_enabled = enabled;
}

bool audio_get_enabled(void) {
    return audio_enabled;
}