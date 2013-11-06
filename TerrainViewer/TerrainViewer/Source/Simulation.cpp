#include <sdl.h>
#include <math.h>
#include "Main.h"
#include "Simulation.h"
#include "RenderObj.h"
#include "RenderObjTest.h"
#include "Obj3D.h"

static bool			enableGravity = true;
static glm::vec2	grav(0, 175);
static bool			enableRotation = true;
static float		imgRot = 0;

static bool			drawLines = false;

Simulation Simulation::singleton;

static RenderObjTest renderObjTest;
static Obj3D			obj3d;

static float			camDist = -4.f;

#pragma region Chapter 2 Shader tutorial
ShaderInfo m_crystalObj;
/*const GLchar* VertexShader =
{
	"#version 400\n"\

	"layout(location=0) in vec4 in_Position;\n"\
	"layout(location=1) in vec4 in_Color;\n"\
	"out vec4 ex_Color;\n"\

	"void main(void)\n"\
	"{\n"\
	"   gl_Position = in_Position;\n"\
	"   ex_Color = in_Color;\n"\
	"}\n"
};

const GLchar* FragmentShader =
{
	"#version 400\n"\

	"in vec4 ex_Color;\n"\
	"out vec4 out_Color;\n"\

	"void main(void)\n"\
	"{\n"\
	"   out_Color = ex_Color;\n"\
	"}\n"
};*/


void Cleanup(void);
void CreateVBO(void);
void DestroyVBO(void);
void CreateShaders(void);
void DestroyShaders(void);

void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}
void CreateVBO(void)
{
	Vertex Vertices[] =
	{
		{ { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
		// Top
		{ { -0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { 0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { 0.0f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		// Bottom
		{ { -0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { 0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { 0.0f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { 0.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		// Left
		{ { -0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { -0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { -1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		// Right
		{ { 0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { 0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { 0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }
	};

	// defines the order to draw vertices in
	GLubyte Indices[] = {
		// Top
		0, 1, 3,
		0, 3, 2,
		3, 1, 4,
		3, 4, 2,
		// Bottom
		0, 5, 7,
		0, 7, 6,
		7, 5, 8,
		7, 8, 6,
		// Left
		0, 9, 11,
		0, 11, 10,
		11, 9, 12,
		11, 12, 10,
		// Right
		0, 13, 15,
		0, 15, 14,
		15, 13, 16,
		15, 16, 14
	};


	GLenum ErrorCheckValue = glGetError();
	const size_t BufferSize = sizeof(Vertices);
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].Position);

	// Generates array that describes how the vertex attributes are stored in VBO
	glGenVertexArrays(1, &m_crystalObj.VaoId);
	glBindVertexArray(m_crystalObj.VaoId);

	// Generates the actual buffer, and handle, to store the vertices
	glGenBuffers(1, &m_crystalObj.VboId);
	// Makes VboId the current buffer by binding it as a GL_ARRAY_BUFFER. 
	// Until another is bound, or this one unbound,
	// all buffer related operations execute on this buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_crystalObj.VboId);
	// Uploads data to currently bound buffer
	glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_STATIC_DRAW);
	// glVertexAttribPointer defines the data that was just buffered by labeling 
	// it as the type associated with index=0 in the vertex shader.
	// We point to the attrib index, define the count, describe the type of data it is,
	// disable normalization, 
	// describe the stride between variables as VertexSize because that's
	// the size of the struct we're using as a variable,
	// and finally the pointer offset is 0 because we passed a single type data array
	// and there is no required offset to access the data we uploaded.
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	// index has changed to 1 because it is the second vertex attribute.
	// changed pointer offset to RgbOffset to signify distance from start of 
	// individual vertex data to the color data
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
	// Enables the vertex attribute (variable) for use during drawing.
	// Should be disabled after this buffer is done being drawn.
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Generates index buffer id
	glGenBuffers(1, &m_crystalObj.IndexBufferId);
	// specifies that the buffer we're uploading is an array that details
	// which vertices we are using and in what order
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_crystalObj.IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);


	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
}

void DestroyVBO(void)
{
	GLenum ErrorCheckValue = glGetError();

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	// Binds nothing to GL_ARRAY_BUFFER: RESETS
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &m_crystalObj.VboId);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &m_crystalObj.IndexBufferId);

	// unbind vertex array
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &m_crystalObj.VaoId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not destroy the VBO: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
}

void CreateShaders(void)
{
	GLenum ErrorCheckValue = glGetError();

	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create the shaders: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}

	m_crystalObj.VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_crystalObj.VertexShaderId, 1, &VertexShader, NULL);
	glCompileShader(m_crystalObj.VertexShaderId);

	m_crystalObj.FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_crystalObj.FragmentShaderId, 1, &FragmentShader, NULL);
	glCompileShader(m_crystalObj.FragmentShaderId);

	m_crystalObj.ProgramId = glCreateProgram();
	glAttachShader(m_crystalObj.ProgramId, m_crystalObj.VertexShaderId);
	glAttachShader(m_crystalObj.ProgramId, m_crystalObj.FragmentShaderId);
	glLinkProgram(m_crystalObj.ProgramId);
	glUseProgram(m_crystalObj.ProgramId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create the shaders: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
}

void DestroyShaders(void)
{
	GLenum ErrorCheckValue = glGetError();

	glUseProgram(0);

	glDetachShader(m_crystalObj.ProgramId, m_crystalObj.VertexShaderId);
	glDetachShader(m_crystalObj.ProgramId, m_crystalObj.FragmentShaderId);

	glDeleteShader(m_crystalObj.FragmentShaderId);
	glDeleteShader(m_crystalObj.VertexShaderId);

	glDeleteProgram(m_crystalObj.ProgramId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not destroy the shaders: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
}

#pragma endregion Chapter 2 Shader tutorial

Simulation::Simulation()
{
	m_terrainRes = glm::vec2(0.f,0.f);

}


void Simulation::Shutdown()
{
	for(int i = 0; i < MAX_SPRITES; ++i)
	{
		glDeleteTextures( 1, &sprites[i] );
	}
	renderObjTest.Cleanup();
	obj3d.Cleanup();
}

// Draw all the sprites
void Simulation::DrawFrame()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
	obj3d.Render();
	renderObjTest.Render();
}

void Simulation::SimulateOneFrame(float deltaTime)
{
	// Drag balls with mouse
	static int prevX = 0;
	static int prevY = 0;
	int currX, currY;
	unsigned char buttons = SDL_GetMouseState( &currX, &currY );
	glm::vec2 dragDelta( (float)(currX - prevX), (float)(currY - prevY) );
	glm::vec2 mousePos( (float)currX, (float)currY );
	prevX = currX;
	prevY = currY;
	if( drawLines )
	{
		if( buttons & SDL_BUTTON(1) )
			DrawLine( glm::vec2(SCREEN_W/2.0f, SCREEN_H/2.0f), mousePos, Color(1, 0, 0) );
		else
			DrawLine( glm::vec2(SCREEN_W/2.0f, SCREEN_H/2.0f), mousePos, Color(0, 1, 0) );
	}

	RotationWrap();
}

glm::mat4 Simulation::GetViewMat()
{
	// Returns an identity matrix that has been pushed backwards
	// This mat keeps track of the camera position, rotation, and scale
	glm::mat4 retval = glm::mat4(1.0f);
	retval = glm::translate( retval, glm::vec3(0,0,camDist) );
	return retval;
}
glm::mat4 Simulation::GetProjMat()
{
	// This matrix keeps track of the camera's lens
	glm::mat4 retval = glm::perspective(60.f, (float)SCREEN_W / SCREEN_H, 1.0f, 1000.f );
	return 	retval;
}

void Simulation::RotationWrap()
{
	// Wrap any value greater than or equal to 180
	imgRot = fmod(imgRot,180.f);
	if(imgRot < 0)
	{
		imgRot += 360;
	}
}

void Log(const char *fmt, ...)
{
#if _DEBUG
	static char buf[1024];
	va_list args;
	va_start(args,fmt);
	vsprintf_s(buf,fmt,args);
	va_end(args);
	OutputDebugStringA(buf);
#endif
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void Simulation::Init()
{
	obj3d.Setup();
	renderObjTest.Setup();

#if TWEAK_MENU
#define TWEAK_SEPERATOR()	TwAddSeparator(TwkBar(), "", "")

	
	TwAddVarRW(TwkBar(), "Cam dist",		TW_TYPE_FLOAT, &camDist,				"min=-500 max=500");
	//TwAddVarRW(TwkBar(), "Gravity On",		TW_TYPE_BOOL8, &enableGravity,			"");
	//TwAddVarRW(TwkBar(), "Gravity",			TW_TYPE_FLOAT, &grav.y,					"min=0 max=200");
	//TwAddVarRW(TwkBar(), "Rotation On",		TW_TYPE_BOOL8, &enableRotation,			"");
	//TwAddVarRW(TwkBar(), "Rotation",		TW_TYPE_FLOAT, &imgRot,					"min=-360 max=360");

	TWEAK_SEPERATOR();
	//TwAddVarRW(TwkBar(), "Draw Lines",		TW_TYPE_BOOL8, &drawLines,				"");
#endif // TWEAK_MENU
}


