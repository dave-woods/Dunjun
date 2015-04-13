#ifndef DUNJUN_RENDERTEXTURE_HPP
#define DUNJUN_RENDERTEXTURE_HPP

#include <Dunjun/OpenGL.hpp>
#include <Dunjun/Types.hpp>
#include <Dunjun/Texture.hpp>

namespace Dunjun
{
	class RenderTexture
	{
	public:
		enum TextureType
		{
			Color = 1,
			Depth = 2,
			ColorAndDepth = Color | Depth,
		};

		RenderTexture();
		virtual ~RenderTexture();

		bool create(u32 width, u32 height, TextureType tt, TextureFilter minMagFilter = TextureFilter::Linear, TextureWrapMode wrapMode = TextureWrapMode::ClampToEdge);

		void setActive(bool active = true);
		void flush();

		void bindTexture(TextureType tt, GLuint position) const;

		ReadOnly<u32, RenderTexture> width;
		ReadOnly<u32, RenderTexture> height;
		Texture colorTexture;
		Texture depthTexture;
		ReadOnly<GLuint, RenderTexture> fbo; //framebuffer object
		ReadOnly<TextureType, RenderTexture> type;
	};
}

#endif // !DUNJUN_RENDERTEXTURE_HPP
