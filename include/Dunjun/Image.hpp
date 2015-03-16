#ifndef DUNJUN_IMAGE_HPP
#define DUNJUN_IMAGE_HPP

#include <Dunjun/Types.hpp>

namespace Dunjun
{
enum class ImageFormat
{
	None = 0,
	Greyscale = 1,
	GreyscaleAlpha = 2,
	RGB = 3,
	RGBA = 4,
};

class Image
{
public:
	Image();
	Image(u32 w, u32 h, ImageFormat f, const u8* p = nullptr);
	Image(const Image& other);
	Image& operator=(const Image& other);
	~Image();

	bool loadFromFile(const char* filename);
	bool loadFromMemory(u32 w, u32 h, ImageFormat f, const u8* p);

	u8* getPixel(u32 column, u32 row) const;
	void setPixel(u32 column, u32 row, const u32* pix);

	void flipVertically();
	//TODO
	//void rotate90CCW();
	//void copyRectFromImage(const Image& src, u32 srcCol, u32 srcRow, u32 destCol, u32 destRow, u32 width, u32 height);

	ReadOnly<ImageFormat, Image> format;
	ReadOnly<u32, Image> width;
	ReadOnly<u32, Image> height;
	ReadOnly<u8*, Image> pixels;
};
} //namespace Dunjun

#endif
