#include <Dunjun/Mesh.hpp>

namespace Dunjun
{
	enum class AttribLocation : u32
	{
		Position = 0,
		Color = 1,
		TexCoord = 2,
	};

	Mesh::Mesh()
		: m_data()
		, m_generated(false)
		, m_vbo(0)
		, m_ibo(0)
		, m_drawType(GL_TRIANGLES)
		, m_drawCount(0)
	{
		glGenBuffers(1, &m_vbo);
		glGenBuffers(1, &m_ibo);
	}

	Mesh::Mesh(const Data& data)
		: m_data(data)
		, m_generated(false)
		, m_vbo(0)
		, m_ibo(0)
		, m_drawType(GL_TRIANGLES)
		, m_drawCount(0)
	{
		glGenBuffers(1, &m_vbo);
		glGenBuffers(1, &m_ibo);

		generate();
	}

	void Mesh::generate()
	{
		if (m_generated)
			return;

		m_drawType = m_data.drawType;
		m_drawCount = m_data.indices.size();

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_data.vertices.size(), &m_data.vertices[0], GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(u32) * m_data.indices.size(), &m_data.indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_generated = true;
	}

	void Mesh::draw()
	{
		if (!m_generated)
			generate();

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

		glEnableVertexAttribArray((u32)AttribLocation::Position);
		glEnableVertexAttribArray((u32)AttribLocation::Color);
		glEnableVertexAttribArray((u32)AttribLocation::TexCoord);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)0);
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const GLvoid*)(sizeof(Dunjun::Vector3)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(sizeof(Dunjun::Vector3) + sizeof(Dunjun::Color)));

		glDrawElements(m_drawType, m_drawCount, GL_UNSIGNED_INT, nullptr);

		glDisableVertexAttribArray((u32)AttribLocation::Position);
		glDisableVertexAttribArray((u32)AttribLocation::Color);
		glDisableVertexAttribArray((u32)AttribLocation::TexCoord);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}