#ifndef DUNJUN_MESH_HPP
#define DUNJUN_MESH_HPP

#include <Dunjun/OpenGL.hpp>
#include <Dunjun/Common.hpp>
#include <Dunjun/Vertex.hpp>

#include <vector>

namespace Dunjun
{
class Mesh
{
public:
	struct Data
	{
		GLenum drawType = GL_TRIANGLES;

		std::vector<Vertex> vertices;
		std::vector<u32> indices;
	};

	Mesh();
	Mesh(const Data& data);

	virtual ~Mesh() { destroy(); }

	void generate();

	void draw();




	inline void destroy() const
	{
		glDeleteBuffers(1, &m_vbo);
		glDeleteBuffers(1, &m_ibo);
	}


private:
	Data m_data;
	b32 m_generated;
	
	GLuint m_vbo;
	GLuint m_ibo;
	GLenum m_drawType;
	GLint m_drawCount;
};
}


#endif // !DUNJUN_MESH_HPP
