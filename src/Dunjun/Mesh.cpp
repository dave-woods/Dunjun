#include <Dunjun/Mesh.hpp>

namespace Dunjun
{
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
		, m_drawType(data.drawType)
		, m_drawCount(data.indices.size())
	{
		glGenBuffers(1, &m_vbo);
		glGenBuffers(1, &m_ibo);

		generate();
	}

	void Mesh::addData(const Data& data)
	{
		m_data = data;
		m_generated = false;
		m_drawType = data.drawType;
		m_drawCount = data.indices.size();
	}

	void Mesh::generate() const
	{
		if (m_generated)
			return;

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_data.vertices.size(), &m_data.vertices[0], GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * m_data.indices.size(), &m_data.indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_generated = true;
	}

	void Mesh::draw() const
	{
		if (!m_generated)
			generate();

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

		glEnableVertexAttribArray((u32)AttribLocation::Position);
		glEnableVertexAttribArray((u32)AttribLocation::TexCoord);
		glEnableVertexAttribArray((u32)AttribLocation::Color);

		glVertexAttribPointer((u32)AttribLocation::Position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)0);
		glVertexAttribPointer((u32)AttribLocation::TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(sizeof(Vector3)));
		glVertexAttribPointer((u32)AttribLocation::Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const GLvoid*)(sizeof(Vector3) + sizeof(Vector2)));

		glDrawElements(m_drawType, m_drawCount, GL_UNSIGNED_INT, nullptr);

		glDisableVertexAttribArray((u32)AttribLocation::Position);
		glDisableVertexAttribArray((u32)AttribLocation::TexCoord);
		glDisableVertexAttribArray((u32)AttribLocation::Color);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}