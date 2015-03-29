#ifndef DUNJUN_SHADER_PROGRAM
#define DUNJUN_SHADER_PROGRAM

#include <Dunjun/OpenGL.hpp>
#include <Dunjun/NonCopyable.hpp>

#include <Dunjun/Common.hpp>

#include <Dunjun/Math.hpp>
#include <Dunjun/Transform.hpp>

#include <map>
#include <string>

namespace Dunjun
{
enum class ShaderType
{
	Vertex,
	Fragment,
};

class ShaderProgram : public NonCopyable
{
public:
	ShaderProgram();
	virtual ~ShaderProgram(); //destructor
		
	bool attachShaderFromFile(ShaderType type, const std::string& filename);
	bool attachShaderFromMemory(ShaderType type, const std::string& filename);

	void use() const;
	bool isInUse() const;
	void stopUsing() const;
	void checkInUse() const; // if not in use, throw runtime error

	bool link();

	void bindAttribLocation(GLuint location, const std::string& name);

	GLint getAttribLocation(const std::string& name);
	GLint getUniformLocation(const std::string& name);

	void setUniform(const std::string& name, f32 x);
	void setUniform(const std::string& name, f32 x, f32 y);
	void setUniform(const std::string& name, f32 x, f32 y, f32 z);
	void setUniform(const std::string& name, f32 x, f32 y, f32 z, f32 w);
	void setUniform(const std::string& name, u32 x);
	void setUniform(const std::string& name, s32 x);
	void setUniform(const std::string& name, bool x);
	void setUniform(const std::string& name, const Vector2& v);
	void setUniform(const std::string& name, const Vector3& v);
	void setUniform(const std::string& name, const Vector4& v);
	void setUniform(const std::string& name, const Matrix4& m);
	void setUniform(const std::string& name, const Quaternion& q);
	void setUniform(const std::string& name, const Transform& t);

	ReadOnly<GLuint, ShaderProgram> object;
	ReadOnly<bool, ShaderProgram> isLinked;
	ReadOnly<std::string, ShaderProgram> errorLog;

private:
	std::map<std::string, GLint> m_attribLocations;
	std::map<std::string, GLint> m_uniformLocations;
};
} // namespace Dunjun

#endif
