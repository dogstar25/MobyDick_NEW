#include "GLDrawer.h"

#include "../texture.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

//initialize static variables
RenderBlendMode GLDrawer::m_lastRenderBlendMode = RenderBlendMode::NONE;
GLuint GLDrawer::m_lastTextureId = 0;
GLuint GLDrawer::m_lastShaderProgramId = 0;

GLDrawer::GLDrawer(GLDrawerType drawerType)
{

	//Generate and bind to a new VOA (Vertex Object Array)
	glGenVertexArrays(1, &m_vaoId);
	glBindVertexArray(m_vaoId);

	//Generate and bind to a VBO
	glGenBuffers(1, &m_vboId);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboId);

	_setVertexBufferAttriubuteLayout(drawerType);

	//Generate a IBO
	glGenBuffers(1, &m_iboId);

	//unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Get a reference to the openGL renderer
	m_rendererGL = static_cast<RendererGL*>(game->renderer());

}

GLDrawer::~GLDrawer()
{
}

void GLDrawer::prepare()
{

	glBindVertexArray(m_vaoId);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboId);


}


void GLDrawer::draw(const std::vector<SpriteVertex>& spriteVertices, const std::vector<glm::uint>& spriteVertexIndexes, 
	Shader& shader, Texture* texture, RenderBlendMode textureBlendMode)
{

	OpenGLTexture* openGLTexture = static_cast<OpenGLTexture*>(texture);

	if (textureBlendMode != m_lastRenderBlendMode) {
		m_lastRenderBlendMode = textureBlendMode;

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glEnable(GL_BLEND);

		//reset blend mode
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);

		switch (textureBlendMode) {

		case RenderBlendMode::BLEND:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			break;
		case RenderBlendMode::ADD:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glBlendEquation(GL_FUNC_ADD);
			break;
		case RenderBlendMode::MULTIPLY:
			glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			break;
		case RenderBlendMode::MODULATE:
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
			glBlendEquation(GL_FUNC_ADD);
			break;
		case RenderBlendMode::CUSTOM_1_MASKED_OVERLAY:
			glBlendFuncSeparate(GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO);
			glBlendEquationSeparate(GL_MAX, GL_MIN);
			break;
		case RenderBlendMode::NONE:
			glBlendFunc(GL_ONE, GL_ZERO);
			glBlendEquation(GL_FUNC_ADD);
			break;
		}
	}
	
	prepare();

	glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteVertex) * spriteVertices.size(), nullptr, GL_DYNAMIC_DRAW);

	//Use the program first
	GLuint shaderProgramId = shader.shaderProgramId();
	if (m_lastShaderProgramId != shaderProgramId) {
		m_lastShaderProgramId = shaderProgramId;
		glUseProgram(shaderProgramId);
	}

	glm::mat4 projection_matrix = m_rendererGL->projectionMatrix();

	glUniformMatrix4fv(shader.projectionMatrixUniformId(), 1, false, (float*)&projection_matrix);

	//Initialize the texture and set the texture uniform
	glUniform1i(shader.textureUniformId(), 0);

	GLuint textureId = static_cast<OpenGLTexture*>(texture)->textureId;
	if (m_lastTextureId != textureId) {
		m_lastTextureId = textureId;
		glBindTexture(GL_TEXTURE_2D, textureId);
	}

	//Submit the vertices
	auto bufferSize = spriteVertices.size() * sizeof(SpriteVertex);
	glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, &spriteVertices[0]);

	//Submit the vertex indices
	auto indexBufferSize = sizeof(GL_UNSIGNED_INT) * spriteVertexIndexes.size();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, &spriteVertexIndexes[0], GL_DYNAMIC_DRAW);

	glDrawElements(GL_TRIANGLES, (GLsizei)spriteVertexIndexes.size(), GL_UNSIGNED_INT, 0);

}

void GLDrawer::draw(const std::vector<LineVertex>& lineVertices, int vertexCount, Shader& shader)
{

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_BLEND);

	//Primitives are always blend mode for now
	if (RenderBlendMode::BLEND != m_lastRenderBlendMode) {

		m_lastRenderBlendMode = RenderBlendMode::BLEND;

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
	}

	prepare();

	glBufferData(GL_ARRAY_BUFFER, sizeof(LineVertex) * lineVertices.size(), nullptr, GL_DYNAMIC_DRAW);

	GLuint shaderProgramId = shader.shaderProgramId();
	if (m_lastShaderProgramId != shaderProgramId) {
		m_lastShaderProgramId = shaderProgramId;
		glUseProgram(shader.shaderProgramId());
	}

	//Set the Projection matrix uniform
	auto projection_matrix = m_rendererGL->projectionMatrix();
	glUniformMatrix4fv(shader.projectionMatrixUniformId(), 1, false, (float*)&projection_matrix);

	//Submit the vertices
	auto swize = sizeof(LineVertex);
	auto bufferSize = lineVertices.size() * sizeof(LineVertex);
	glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, &lineVertices[0]);

	glDrawArrays(GL_LINES, 0, vertexCount);

}

void GLDrawer::bind()
{

}

void GLDrawer::_setVertexBufferAttriubuteLayout(GLDrawerType drawerType)
{

	if (drawerType == GLDrawerType::GLSPRITE) {

		// vertex attrubute indexes
		const int attrib_position = 0;
		const int attrib_color = 1;
		const int attrib_texture = 2;

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(attrib_position, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, 0);
		glVertexAttribPointer(attrib_color, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void*)(3 * sizeof(float)));
		glVertexAttribPointer(attrib_texture, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void*)(7 * sizeof(float)));


	}
	else if (drawerType == GLDrawerType::GLLINE) {

		// vertex attrubute indexes
		const int attrib_position = 0;
		const int attrib_color = 1;

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(attrib_position, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);
		glVertexAttribPointer(attrib_color, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)(3 * sizeof(float)));

	}


}
