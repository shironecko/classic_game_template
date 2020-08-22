#include <engine/pch.h>

#include <engine/audio.h>
#include <engine/assets.h>

namespace cgt
{

Music::~Music()
{
    if (m_Music)
    {
        Mix_FreeMusic(m_Music);
    }
}

SoundEffect::~SoundEffect()
{
    if (m_SFX)
    {
        Mix_FreeChunk(m_SFX);
    }
}

Audio::Audio()
{
    Mix_Init(0);

    m_InitSuccessful = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) == 0;
    CGT_ASSERT_RECOVERABLE_MSG(m_InitSuccessful, "Failed to open an audio device!\nYou can continue playing, though you will hear no sounds.\nMix error:\n{}", Mix_GetError());
}

Audio::~Audio()
{
    Mix_CloseAudio();
    Mix_Quit();
}

std::unique_ptr<Music> Audio::LoadMusic(const std::filesystem::path& absolutePath)
{
    CGT_ASSERT(absolutePath.is_absolute());

    auto music = std::make_unique<Music>();
    auto pathUtf8 = absolutePath.u8string();
    music->m_Music = Mix_LoadMUS(pathUtf8.c_str());
    CGT_ASSERT_RECOVERABLE_MSG(music->m_Music, "Failed to load a music file at:\n{}\nMix error:\n{}", absolutePath, Mix_GetError());

    return music;
}

std::unique_ptr<SoundEffect> Audio::LoadSoundEffect(const std::filesystem::path& absolutePath)
{
    CGT_ASSERT(absolutePath.is_absolute());

    auto sfx = std::make_unique<SoundEffect>();
    auto pathUtf8 = absolutePath.u8string();
    sfx->m_SFX = Mix_LoadWAV(pathUtf8.c_str());
    CGT_ASSERT_RECOVERABLE_MSG(sfx->m_SFX, "Failed to load a sound effect file at:\n{}\nMix error:\n{}", absolutePath, Mix_GetError());

    return sfx;
}

void Audio::PlayMusic(Music& music, bool loop)
{
    if (!m_InitSuccessful || !music.m_Music)
    {
        return;
    }

    Mix_PlayMusic(music.m_Music, loop ? -1 : 0);
}

void Audio::PlaySoundEffect(SoundEffect& effect)
{
    if (!m_InitSuccessful || !effect.m_SFX)
    {
        return;
    }

    Mix_PlayChannel(-1, effect.m_SFX, 0);
}

}