#pragma once

namespace cgt
{

class Music : private NonCopyable
{
public:
    ~Music();

private:
    friend class Audio;
    Mix_Music *m_Music = nullptr;
};

class SoundEffect : private NonCopyable
{
public:
    ~SoundEffect();

private:
    friend class Audio;
    Mix_Chunk *m_SFX = nullptr;
};

class Audio
{
public:
    Audio();
    ~Audio();

    std::unique_ptr<Music> LoadMusic(const std::filesystem::path& absolutePath);
    std::unique_ptr<SoundEffect> LoadSoundEffect(const std::filesystem::path& absolutePath);

    void PlayMusic(Music& music, bool loop);
    void PlaySoundEffect(SoundEffect& effect);

private:
    bool m_InitSuccessful;
};

}
