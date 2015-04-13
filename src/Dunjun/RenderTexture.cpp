#include <Dunjun/RenderTexture.hpp>

#include <vector>

namespace Dunjun
{
	RenderTexture::RenderTexture()
		: fbo(0)
		, colorTexture()
		, depthTexture()
		, width(0)
		, height(0)
		, type(Color)
	{
		glGenFramebuffersEXT(1, &fbo);
	}

	RenderTexture::~RenderTexture()
	{
		glDeleteFramebuffersEXT(1, &fbo);
	}

	bool RenderTexture::create(u32 w, u32 h, TextureType tt, TextureFilter minMagFilter, TextureWrapMode wrapMode)
	{
		if (w == width.data && h == height.data && tt == type.data)
			return true;

		type = tt;
		width = w;
		height = h;

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo.data);

		GLuint depthRenderBuffer;
		glGenRenderbuffersEXT(1, &depthRenderBuffer);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthRenderBuffer);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, (GLsizei)width, (GLsizei) height);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthRenderBuffer);

		if (type.data & Color)
		{
			glBindTexture(GL_TEXTURE_2D, (GLuint)colorTexture.m_object);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)width, (GLsizei)height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
			
			colorTexture.width = width;
			colorTexture.height = height;

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)minMagFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)minMagFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)wrapMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)wrapMode);

			glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, colorTexture.m_object, 0);
		}
		if (type.data & Depth)
		{
			glBindTexture(GL_TEXTURE_2D, (GLuint)depthTexture.m_object);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, (GLsizei)width, (GLsizei)height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

			depthTexture.width = width;
			depthTexture.height = height;

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)minMagFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)minMagFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)wrapMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)wrapMode);

			glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, depthTexture.m_object, 0);
		}

		std::vector<GLenum> drawBuffers;
		if (type.data & Color)
			drawBuffers.push_back(GL_COLOR_ATTACHMENT0_EXT);
		if (type.data & Depth)
			drawBuffers.push_back(GL_DEPTH_COMPONENT);

		glDrawBuffers(drawBuffers.size(), &drawBuffers[0]);

		if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
			return false;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
		return true;
	}

	void RenderTexture::setActive(bool active)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, active ? fbo : 0);
	}

	void RenderTexture::flush()
	{
		glFlush();
	}

	void RenderTexture::bindTexture(TextureType tt, GLuint position) const
	{
		if (position > 31)
		{
			std::cerr << "Textures can only be bound to position [0 ... 31]\n";
			std::cerr << "Will bind to position [31]" << std::endl;
			position = 31;
		}

		if (colorTexture.m_object && tt == Color)
			Texture::bind(&colorTexture, position);
		else if (depthTexture.m_object && tt == Depth)
			Texture::bind(&depthTexture, position);
		else if (colorTexture.m_object && depthTexture.m_object && tt == ColorAndDepth)
			Texture::bind(&depthTexture, position);
		else
			Texture::bind(nullptr, position);
	}

}