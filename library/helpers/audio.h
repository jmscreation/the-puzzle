#ifndef __ENGINE_AUDIO_H__
#define __ENGINE_AUDIO_H__

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.h"
#include "portaudio.h"

#include <windows.h>
#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <cmath>

namespace olc {

	class AudioContext;
	class SoundBuffer;
	class SoundInstance;


	typedef SoundInstance* pSoundInstance;

	class AudioContext {
		static AudioContext* currentCtx;

		std::vector<pSoundInstance> playlist;
		std::mutex locked;
		int sampleRate;
		PaStream* stream;
		std::thread* autoDestroy;
		std::atomic<bool> ctxRunning;

	public:
		AudioContext(int sampleRate=48000);
		virtual ~AudioContext();

		inline static AudioContext& current() { if(currentCtx == nullptr) currentCtx = new AudioContext(); return *currentCtx; }
		inline static void free() { if(currentCtx != nullptr) { delete currentCtx; currentCtx = nullptr; } }

	private:
		static int callbackStatic(const void* in,void* out,unsigned long fpb,
				const PaStreamCallbackTimeInfo* tmi,PaStreamCallbackFlags flgs,void* dat);
		int callback(const void* in,void* out,unsigned long fpb,
				const PaStreamCallbackTimeInfo* tmi,PaStreamCallbackFlags flgs);
		static void callbackAutoDestroy(AudioContext* ctx);

		friend class SoundBuffer;
		friend class SoundInstance;
	};

	class SoundBuffer {
		short* samples;
		int sampleCount;
		int sampleRate, channels;
		float sampleFactor;
		float defVolume;
		bool defDestroy;

	public:
		SoundBuffer();
		virtual ~SoundBuffer();

		bool loadOGGFromFile(const std::string& fn);
		bool loadOGGFromMemory(const void* data,size_t size);
		bool loadRawFromMemory(const void* data,size_t size,int sampleRate=0,int channels=2,int bytes=2);

		int sampleLength();
		float length();
		inline void defaultDestroy(bool defdest) { defDestroy=defdest; }
		inline bool defaultDestroy() { return defDestroy; }
		inline void defaultVolume(float defvol) { defVolume=defvol; }
		inline float defaultVolume() { return defVolume; }

		pSoundInstance play();
		pSoundInstance play(bool dest);
		pSoundInstance loop();
		pSoundInstance loop(bool dest);
		pSoundInstance create();
		pSoundInstance create(bool dest);

	private:
		void sample(float& L,float& R,const double& p);

		friend class SoundInstance;
	};

	class SoundInstance {
		SoundBuffer* sound;
		double pos;
		float speed, volPan, vol, pspeed;
		bool destroy, looping;
		std::atomic<bool> garbage;
	public:
		SoundInstance(SoundBuffer* buf, int playmode=0, bool destroy=false, float volume=1);
		virtual ~SoundInstance();

		inline bool fromSoundBuffer(SoundBuffer* ptr) { return ptr == sound; }
		void play();
		void loop();
		void pause();
		void stop(bool dest=false);
		void setspeed(float spd);
		inline float getspeed() { return pspeed; }

		bool isPlaying() { return (speed != 0); }

		float pan() { return volPan; }
		float volume() { return vol; }
		inline void volume(float v) { vol=v<0 ? 0 : v; }
		inline void pan(float pan) { volPan=pan<-1 ? -1 : pan>1 ? 1 : pan; }
		inline SoundBuffer* getSoundBuffer() { return sound; }

		int samplePosition();
		void samplePosition(int pos);
		double position();
		void position(double pos);
		inline int sampleLength() { return sound->sampleLength(); }
		inline float length() { return sound->length(); }

	private:
		bool sample(float& L,float& R);

		friend class SoundBuffer;
		friend class AudioContext;
	};

}

#endif // __ENGINE_AUDIO_H__
