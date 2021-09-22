#include "audio.h"

namespace olc {

    AudioContext* AudioContext::currentCtx = nullptr;

    AudioContext::AudioContext(int rate): sampleRate(rate), stream(nullptr),
        autoDestroy(new std::thread(callbackAutoDestroy, this)), ctxRunning(true) {
        if(currentCtx == nullptr) {
            currentCtx = this;

            PaError err;
            err = Pa_Initialize();
            if(err != paNoError) {
                std::cout << "Error initializing PortAudio: " << Pa_GetErrorText(err) << std::endl;
                return;
            }

            err = Pa_OpenDefaultStream(&stream,0,2,
                                       paFloat32, sampleRate,
                                       16,
                                       //paFramesPerBufferUnspecified,
                                       AudioContext::callbackStatic, this);

            if(err != paNoError) {
                std::cout << "Error opening PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
                return;
            }

            err = Pa_StartStream(stream);
            if(err != paNoError) {
                std::cout << "Error starting PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
                return;
            }

        } else
            std::cout << "Warning: Creating multiple audio contexts" << std::endl;
    }

    AudioContext::~AudioContext() {
        PaError err = Pa_StopStream(stream);
        if(err != paNoError) std::cout << "Error closing PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();

        ctxRunning = false;
        autoDestroy->join();
        delete autoDestroy;

        for(int i=playlist.size()-1; i >= 0; --i)
            delete playlist[i];

        if(currentCtx == this) {
            currentCtx = nullptr;
        }
    }

    int AudioContext::callbackStatic(const void* in,void* out,unsigned long fpb,
        const PaStreamCallbackTimeInfo* tmi,PaStreamCallbackFlags flgs,void* dat) {
        return ((AudioContext*)dat)->callback(in,out,fpb,tmi,flgs);
    }

    int AudioContext::callback(const void* in,void* out,unsigned long fpb,
        const PaStreamCallbackTimeInfo* tmi,PaStreamCallbackFlags flgs) {

        std::lock_guard lock(locked);

        uint32_t playsz = playlist.size();

        float *samples_out = (float*)out;

        float L,R, lv,rv;
        for(unsigned int samp=0; samp<fpb; samp++) {
            L = R = 0;
            for(uint32_t i=0; i<playsz; i++) {
                if(playlist[i] == nullptr) continue;
                if(playlist[i]->sample(lv,rv)){
                    L += lv; R += rv;
                }
            }
            *samples_out++ = L;
            *samples_out++ = R;
        }

        return 0;
    }

    void AudioContext::callbackAutoDestroy(AudioContext* ctx) {
        std::vector<pSoundInstance> garbage;
        do {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            ctx->locked.lock();
            
            for(int i=ctx->playlist.size() - 1; i >= 0; --i){
                pSoundInstance& snd = ctx->playlist[i];
                if(snd != nullptr && snd->garbage){
                    garbage.push_back(snd);
                    snd = nullptr;
                }
                if(snd == nullptr){
                    ctx->playlist.erase(ctx->playlist.begin() + i);
                    continue;
                }
            }
            ctx->locked.unlock();

            for(pSoundInstance snd : garbage) delete snd;
            garbage.clear();

        } while(ctx->ctxRunning);
    }

    SoundBuffer::SoundBuffer():
        samples(nullptr), sampleCount(0), defVolume(1), defDestroy(false) {}

    SoundBuffer::~SoundBuffer() {
        AudioContext& ctx = AudioContext::current();

        ctx.locked.lock();
        for(auto snd : ctx.playlist){
            if(snd->sound == this) snd->stop(true);
        }
        ctx.locked.unlock();
        
        if(samples != nullptr)
            delete samples;
    }

    bool SoundBuffer::loadOGGFromFile(const std::string& fn) {
        if(samples != nullptr)
            delete samples;

        sampleCount = stb_vorbis_decode_filename(fn.c_str(), &channels, &sampleRate, &samples);

        if(sampleCount == -1) {
            samples = nullptr;
            std::cout << "Warning: unable to load vorbis from file '" << fn << "'" << std::endl;
            return false;
        }
        sampleFactor = float(sampleRate) / float(AudioContext::current().sampleRate);
        return true;
    }

    bool SoundBuffer::loadOGGFromMemory(const void* data,size_t size) {
        if(samples != nullptr)
            delete samples;

        sampleCount = stb_vorbis_decode_memory((const unsigned char*)data,size, &channels,&sampleRate,&samples);
        if(sampleCount == -1) {
            samples = nullptr;
            std::cout << "Warning: unable to load vorbis from memory" << std::endl;
            return false;
        }

        sampleFactor = float(sampleRate) / float(AudioContext::current().sampleRate);

        return true;
    }

    bool SoundBuffer::loadRawFromMemory(const void* data,size_t len,int rate,int channs,int bytes) {
        if(samples != nullptr)
            delete samples;

        sampleCount = len;
        sampleRate = rate;
        sampleFactor = float(sampleRate) / float(AudioContext::current().sampleRate);
        channels = channs;
        int l = sampleCount*channels;
        samples = new short[l];

        switch(bytes) {
            case 2:
                for(int i=0;i<l;i++)
                    samples[i] = ((short*)data)[i];
                break;
            case 4:
                for(int i=0;i<l;i++)
                    samples[i] = (short)(((int*)data)[i] >> 16);
                break;
            case 8:
                for(int i=0;i<l;i++)
                    samples[i] = (short)(((long long int*)data)[i] >> 48);
                break;
            default:
                samples = nullptr;
                return false;
        }

        return true;
    }

    int SoundBuffer::sampleLength() {
        return sampleCount;
    }

    float SoundBuffer::length() {
        return float(sampleCount) / float(sampleRate);
    }

    pSoundInstance SoundBuffer::play() {
        return new SoundInstance(this, 1, defDestroy, defVolume);
    }

    pSoundInstance SoundBuffer::play(bool dest) {
        return new SoundInstance(this, 1, dest, defVolume);
    }

    pSoundInstance SoundBuffer::loop() {
        return new SoundInstance(this, 2, defDestroy, defVolume);
    }

    pSoundInstance SoundBuffer::loop(bool dest) {
        return new SoundInstance(this, 2, dest, defVolume);
    }

    pSoundInstance SoundBuffer::create() {
        return new SoundInstance(this, 0, defDestroy, defVolume);
    }

    pSoundInstance SoundBuffer::create(bool dest) {
        return new SoundInstance(this,0,dest,defVolume);
    }

    void SoundBuffer::sample(float& L, float& R, const double& p) {
        float t = p - floor(p);
        int i = (int)floor(p);
        i = i<0 ? 0 : i>sampleCount-1 ? sampleCount-1 : i;
        if(channels==1)
            L = R = (float(samples[i])*(1-t) + float(samples[i+1])*t) * (1.0/65536.0);
        else if(channels > 1) { // use first two channels
            L = (float(samples[i*channels])*(1-t) + float(samples[(i+1)*channels])*t) * (1.0/65536.0);
            R = (float(samples[i*channels+1])*(1-t) + float(samples[(i+1)*channels+1])*t) * (1.0/65536.0);
        } else
            L = R = 0;
    }

    SoundInstance::SoundInstance(SoundBuffer* buf, int playmode, bool dest, float volume):
        sound(buf), pos(0), speed(playmode==0 ? 0 : buf->sampleFactor), volPan(0), vol(volume), pspeed(1), destroy(dest), looping(playmode==2), garbage(false) {
        AudioContext& ctx = AudioContext::current();

        ctx.locked.lock();
        ctx.playlist.push_back(this);
        ctx.locked.unlock();
    }

    SoundInstance::~SoundInstance() {
        AudioContext& ctx = AudioContext::current();
        ctx.locked.lock();
        auto i = std::find(ctx.playlist.begin(), ctx.playlist.end(), this);
        if(i != ctx.playlist.end()) *i = nullptr;
        ctx.locked.unlock();
    }

    void SoundInstance::play() {
        speed = pspeed * sound->sampleFactor;
        looping = false;
    }

    void SoundInstance::loop() {
        speed = pspeed * sound->sampleFactor;
        looping = true;
    }

    void SoundInstance::setspeed(float spd) {
        pspeed = spd;
        if(isPlaying() || pspeed == 0) speed = pspeed * sound->sampleFactor;
    }

    void SoundInstance::pause() {
        speed = 0;
    }

    void SoundInstance::stop(bool dest) {
        speed = 0;
        pos = 0;
        if(dest) garbage = true;
    }

    int SoundInstance::samplePosition() {
        return (int)floor(pos);
    }

    void SoundInstance::samplePosition(int p) {
        pos = p<0 ? 0 : p>sound->sampleCount-1 ? double(sound->sampleCount-1) : (double)p;
    }

    double SoundInstance::position() {
        return pos / (double)sound->sampleRate;
    }

    void SoundInstance::position(double p) {
        pos = p * (double)sound->sampleRate;

        if(pos<0)
            pos = 0;
        else if(pos>(double)(sound->sampleCount-1))
            pos = double(sound->sampleCount-1);
    }

    bool SoundInstance::sample(float& L,float& R) {
        if(garbage) return false;

        sound->sample(L,R,pos);
        L *= (1-(volPan<0 ? 0 : volPan)) * vol;
        R *= ((volPan>1 ? 1 : volPan)+1) * vol;
        pos += speed;
        if(speed) {
            if(pos > double(sound->sampleCount-1)) {
                if(looping)
                    pos -= double(sound->sampleCount-1);
                else
                    stop(destroy);
            }
            if(pos < 0) {
                if(looping)
                    pos += double(sound->sampleCount-1);
                else
                    pos = 0;
            }
        }

        return true;
    }

}
