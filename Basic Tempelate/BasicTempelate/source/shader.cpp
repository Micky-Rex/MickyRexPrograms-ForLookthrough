#include "ray_tracing/shader.h"
#define PRINT_SHADER_DEBUG false
namespace Graphics
{
	shader::shader(const char* vertexPath, const char* fragmentPath)
	{
		string vertexCode, fragmentCode;
		ifstream vShaderFile;
		ifstream fShaderFile;
		vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

		try
		{
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);

			stringstream vShaderStream, fShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (ifstream::failure e)
		{
			printf("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		unsigned int vertex, fragment;
		int success;
		char infolog[512];

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infolog);
			printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infolog);
		}

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infolog);
			printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infolog);
		}

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infolog);
			printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infolog);
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
		return;
	}

	void shader::use() const
	{
		glUseProgram(ID);
		return;
	}

	void shader::setBool(const std::string& name, bool value) const
	{
		GLint loc = glGetUniformLocation(ID, name.c_str());
		if (loc == -1) printf("ERROR::SHADER::UNIFORM::CANNOT_FIND_UNIFORM_NAMED: %s\n", name.c_str());
		glUniform1i(loc, (int)value);
		return;
	}

	void shader::setInt(const std::string& name, int value) const
	{
		GLint loc = glGetUniformLocation(ID, name.c_str());
		if (loc == -1) printf("ERROR::SHADER::UNIFORM::CANNOT_FIND_UNIFORM_NAMED: %s\n", name.c_str());
		glUniform1i(loc, value);
		if (PRINT_SHADER_DEBUG) printf("Set Uniform %s as %d\n", name.c_str(), value);
		return;
	}

	void shader::setFloat(const std::string& name, float value) const
	{
		GLint loc = glGetUniformLocation(ID, name.c_str());
		if (loc == -1) printf("ERROR::SHADER::UNIFORM::CANNOT_FIND_UNIFORM_NAMED: %s\n", name.c_str());
		glUniform1f(loc, value);
		if (PRINT_SHADER_DEBUG) printf("Set Uniform %s as %.2f\n", name.c_str(), value);
		return;
	}
	void shader::setVec(const string& name, vec2 value) const
	{
		GLint loc = glGetUniformLocation(ID, name.c_str());
		if (loc == -1) printf("ERROR::SHADER::UNIFORM::CANNOT_FIND_UNIFORM_NAMED: %s\n", name.c_str());
		glUniform2f(loc, value.x, value.y);
		if (PRINT_SHADER_DEBUG) printf("Set Uniform %s as", name.c_str());
		if (PRINT_SHADER_DEBUG) printf(string(" vec2(%.2f, %.2f)\n"), value);
		return;
	}
	void shader::setVec(const string& name, vec3 value) const
	{
		GLint loc = glGetUniformLocation(ID, name.c_str());
		if (loc == -1) printf("ERROR::SHADER::UNIFORM::CANNOT_FIND_UNIFORM_NAMED: %s\n", name.c_str());
		glUniform3f(loc, value.x, value.y, value.z);
		if (PRINT_SHADER_DEBUG) printf("Set Uniform %s as", name.c_str());
		if (PRINT_SHADER_DEBUG) printf(string(" vec3(%.2f, %.2f, %.2f)\n"), value);
		return;
	}
	void shader::setVec(const string& name, vec4 value) const
	{
		GLint loc = glGetUniformLocation(ID, name.c_str());
		if (loc == -1) printf("ERROR::SHADER::UNIFORM::CANNOT_FIND_UNIFORM_NAMED: %s\n", name.c_str());
		glUniform4f(loc, value.x, value.y, value.z, value.w);
		if (PRINT_SHADER_DEBUG) printf("Set Uniform %s as", name.c_str());
		if (PRINT_SHADER_DEBUG) printf(string(" vec4(%.2f, %.2f, %.2f, %.2f)\n"), value);
		return;
	}
	void shader::setMat(const string& name, mat3 mat) const
	{
		GLint loc = glGetUniformLocation(ID, name.c_str());
		if (loc == -1) printf("ERROR::SHADER::UNIFORM::CANNOT_FIND_UNIFORM_NAMED: %s\n", name.c_str());
		glUniformMatrix3fv(loc, 1, GL_FALSE, &mat[0][0]);
		if (PRINT_SHADER_DEBUG) printf("Set Uniform %s as", name.c_str());
		if (PRINT_SHADER_DEBUG) printf(string(" mat3(\n  %.2f, %.2f, %.2f\n  %.2f, %.2f, %.2f\n  %.2f, %.2f, %.2f )\n"), mat);
		return;
	}
	void shader::setMat(const string& name, mat4 mat) const
	{
		GLint loc = glGetUniformLocation(ID, name.c_str());
		if (loc == -1) printf("ERROR::SHADER::UNIFORM::CANNOT_FIND_UNIFORM_NAMED: %s\n", name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
		if (PRINT_SHADER_DEBUG) printf("Set Uniform %s as", name.c_str());
		if (PRINT_SHADER_DEBUG) printf(string(" mat4(\n  %.2f, %.2f, %.2f, %.2f\n  %.2f, %.2f, %.2f, %.2f\n  %.2f, %.2f, %.2f, %.2f\n  %.2f, %.2f, %.2f, %.2f )\n"), mat);
		return;
	}
}	// namespace Graphics