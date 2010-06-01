//**********************************
// OpenGL tessellation
// 14/05/2010
//**********************************
// Christophe Riccio
// g.truc.creation@gmail.com
//**********************************
// G-Truc Creation
// www.g-truc.net
//**********************************

#include <glf/glf.hpp>

namespace
{
	std::string const SAMPLE_NAME = "OpenGL Tessellation";	
	std::string const SAMPLE_VERTEX_SHADER(glf::DATA_DIRECTORY + "400/tess.vert");
	std::string const SAMPLE_CONTROL_SHADER(glf::DATA_DIRECTORY + "400/tess.cont");
	std::string const SAMPLE_EVALUATION_SHADER(glf::DATA_DIRECTORY + "400/tess.eval");
	std::string const SAMPLE_FRAGMENT_SHADER(glf::DATA_DIRECTORY + "400/tess.frag");
	int const SAMPLE_SIZE_WIDTH = 640;
	int const SAMPLE_SIZE_HEIGHT = 480;
	int const SAMPLE_MAJOR_VERSION = 4;
	int const SAMPLE_MINOR_VERSION = 0;

	glf::window Window(glm::ivec2(SAMPLE_SIZE_WIDTH, SAMPLE_SIZE_HEIGHT));

	GLsizei const VertexCount = 4;
	GLsizeiptr const PositionSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const PositionData[VertexCount] =
	{
		glm::vec2(-1.0f,-1.0f),
		glm::vec2( 1.0f,-1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f)
	};

	GLuint ProgramName = 0;
	GLuint ElementBufferName = 0;
	GLuint ArrayBufferName = 0;
	GLuint VertexArrayName = 0;
	GLint UniformMVP = 0;
	GLint UniformDiffuse = 0;

}//namespace

bool initProgram()
{
	bool Validated = true;
	
	// Create program
	if(Validated)
	{
		GLuint VertexShader = glf::createShader(GL_VERTEX_SHADER, SAMPLE_VERTEX_SHADER);
		glf::checkError("VertexShader");
		GLuint ControlShader = glf::createShader(GL_TESS_CONTROL_SHADER, SAMPLE_CONTROL_SHADER);
		glf::checkError("ControlShader");
		GLuint EvaluationShader = glf::createShader(GL_TESS_EVALUATION_SHADER, SAMPLE_EVALUATION_SHADER);
		glf::checkError("EvaluationShader");
		GLuint FragmentShader = glf::createShader(GL_FRAGMENT_SHADER, SAMPLE_FRAGMENT_SHADER);
		glf::checkError("FragmentShader");

		ProgramName = glCreateProgram();
		glAttachShader(ProgramName, VertexShader);
		glAttachShader(ProgramName, ControlShader);
		glAttachShader(ProgramName, EvaluationShader);
		glAttachShader(ProgramName, FragmentShader);
		glDeleteShader(VertexShader);
		glDeleteShader(ControlShader);
		glDeleteShader(EvaluationShader);
		glDeleteShader(FragmentShader);

		glLinkProgram(ProgramName);
		Validated = glf::checkProgram(ProgramName);
	}

	// Get variables locations
	if(Validated)
	{
		UniformMVP = glGetUniformLocation(ProgramName, "MVP");
		UniformDiffuse = glGetUniformLocation(ProgramName, "Diffuse");
	}

	// Set some variables 
	if(Validated)
	{
		// Bind the program for use
		glUseProgram(ProgramName);

		// Set uniform value
		glUniform4fv(UniformDiffuse, 1, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		// Unbind the program
		glUseProgram(0);
	}

	return Validated && glf::checkError("initProgram");
}

bool initVertexArray()
{
	// Build a vertex array object
	glGenVertexArrays(1, &VertexArrayName);
    glBindVertexArray(VertexArrayName);
		glBindBuffer(GL_ARRAY_BUFFER, ArrayBufferName);
		glVertexAttribPointer(glf::semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(glf::semantic::attr::POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return glf::checkError("initVertexArray");
}

bool initArrayBuffer()
{
	// Generate a buffer object
	glGenBuffers(1, &ArrayBufferName);
    glBindBuffer(GL_ARRAY_BUFFER, ArrayBufferName);
    glBufferData(GL_ARRAY_BUFFER, PositionSize, PositionData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return glf::checkError("initArrayBuffer");
}

bool begin()
{
	GLint MajorVersion = 0;
	GLint MinorVersion = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &MajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &MinorVersion);
	bool Validated = (MajorVersion * 10 + MinorVersion) >= (SAMPLE_MAJOR_VERSION * 10 + SAMPLE_MINOR_VERSION);

	if(Validated)
		Validated = initProgram();
	if(Validated)
		Validated = initArrayBuffer();
	if(Validated)
		Validated = initVertexArray();

	return Validated && glf::checkError("begin");
}

bool end()
{
	glDeleteVertexArrays(1, &VertexArrayName);
	glDeleteBuffers(1, &ArrayBufferName);
	glDeleteBuffers(1, &ElementBufferName);
	glDeleteProgram(ProgramName);

	return glf::checkError("end");
}

void display()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Compute the MVP (Model View Projection matrix)
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Window.TranlationCurrent.y));
	glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, Window.RotationCurrent.y, glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 View = glm::rotate(ViewRotateX, Window.RotationCurrent.x, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	// Set the display viewport
	glViewport(0, 0, Window.Size.x, Window.Size.y);

	// Clear color buffer with black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Bind program
	glUseProgram(ProgramName);

	// Set the value of MVP uniform.
	glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);

	// Bind vertex array & draw 
	glBindVertexArray(VertexArrayName);
		glPatchParameteri(GL_PATCH_VERTICES, VertexCount);
		glDrawArrays(GL_PATCHES, 0, VertexCount);
	glBindVertexArray(0);

	// Unbind program
	glUseProgram(0);

	glf::swapBuffers();
	glf::checkError("display");
}

int main(int argc, char* argv[])
{
	if(glf::run(
		argc, argv,
		glm::ivec2(::SAMPLE_SIZE_WIDTH, ::SAMPLE_SIZE_HEIGHT), 
		::SAMPLE_MAJOR_VERSION, 
		::SAMPLE_MINOR_VERSION))
		return 0;
	return 1;
}