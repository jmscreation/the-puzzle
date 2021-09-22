#ifndef __MEMLOADER__
#define __MEMLOADER__

#define OLC_IMAGE_LIBPNG
#include "olcPixelGameEngine.h"
#include "png.h"

namespace memloader {
    void pngReadStream(png_structp pngPtr, png_bytep data, png_size_t length);

	class MemoryImageLoader
	{
	public:
		MemoryImageLoader()=default;

		virtual bool LoadImageResource(olc::Sprite* spr, const char* sImageData, size_t sImageDataLength);
	};

}

#endif //__MEMLOADER__