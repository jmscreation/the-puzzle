#include "memloader.h"

namespace memloader {
	struct char_buffer {
		const char* data;
		size_t length;
		size_t pos;
	};

    void pngReadStream(png_structp pngPtr, png_bytep data, png_size_t length)
	{
		png_voidp a = png_get_io_ptr(pngPtr);
		char_buffer* buf = (char_buffer*)a;

		size_t dataLeft = buf->length - buf->pos;
		if(length > dataLeft) length = dataLeft;

		memcpy((char*)data, buf->data + buf->pos, length);

		buf->pos += length; // increment reader
	}

	bool MemoryImageLoader::LoadImageResource(olc::Sprite* spr, const char* sImageData, size_t sImageDataLength)
	{
		spr->pColData.clear();

		png_structp png;
		png_infop info;

		auto loadPNG = [&]()
		{
			png_read_info(png, info);
			png_byte color_type;
			png_byte bit_depth;
			png_bytep* row_pointers;
			spr->width = png_get_image_width(png, info);
			spr->height = png_get_image_height(png, info);
			color_type = png_get_color_type(png, info);
			bit_depth = png_get_bit_depth(png, info);
			if (bit_depth == 16) png_set_strip_16(png);
			if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
			if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)	png_set_expand_gray_1_2_4_to_8(png);
			if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
			if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
				png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
			if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
				png_set_gray_to_rgb(png);
			png_read_update_info(png, info);
			row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * spr->height);
			for (int y = 0; y < spr->height; y++) {
				row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
			}
			png_read_image(png, row_pointers);
			////////////////////////////////////////////////////////////////////////////
			// Create sprite array
			spr->pColData.resize(spr->width * spr->height);
			// Iterate through image rows, converting into sprite format
			for (int y = 0; y < spr->height; y++)
			{
				png_bytep row = row_pointers[y];
				for (int x = 0; x < spr->width; x++)
				{
					png_bytep px = &(row[x * 4]);
					spr->SetPixel(x, y, olc::Pixel(px[0], px[1], px[2], px[3]));
				}
			}

			for (int y = 0; y < spr->height; y++) // Thanks maksym33
				free(row_pointers[y]);
			free(row_pointers);
			png_destroy_read_struct(&png, &info, nullptr);
		};

		png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png) goto fail_load;

		info = png_create_info_struct(png);
		if (!info) goto fail_load;

		if (setjmp(png_jmpbuf(png))) goto fail_load;

		{
			char_buffer buf({
				sImageData,
				sImageDataLength,
				0
			});

			png_set_read_fn(png, (png_voidp)&buf, pngReadStream);
			loadPNG();
		}

		return true;

	fail_load:
		spr->width = 0;
		spr->height = 0;
		spr->pColData.clear();
		return false;
	}
}