#pragma once

#define GL_GLEXT_PROTOTYPES

#include <memory>
#include <vector>
#include <unordered_map>
#include <array>

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "opengl/GLDebugCallback.h"
#include "opengl/GLDrawer.h"
#include "Renderer.h"
#include "opengl/Vertex.h"
#include "opengl/Shader.h"
#include "BaseConstants.h"



class DrawBatch;
class OpenGLTexture;

class RendererGL : public Renderer
{

	const std::vector<glm::uint> spriteindexBuffer{ 0,1,2,2,3,0 };
	const std::vector<glm::uint> fspriteindexBuffer{ 1,0,3,3,2,1 };
	const std::vector<glm::uint> lineindexBuffer{ 0,1};


public:
	RendererGL();
	virtual ~RendererGL();

	SDL_Renderer* sdlRenderer() { return nullptr; }
	void init(SDL_Window* window);
	bool present();
	void drawBatches() override;
	virtual void setClearColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	bool clear();
	SDL_Texture* createTextureFromSurface(SDL_Surface* surface) { return nullptr; };
	void drawSprite(int layer, SDL_FRect quad, SDL_Color color, Texture* texture, SDL_Rect* textureSrcQuad, float angle, 
		bool outline, SDL_Color outlineColor, RenderBlendMode textureBlendMode) override;
	void drawLine(glm::vec2 pointA, glm::vec2 pointB, glm::uvec4 color, int layer);
	const GLDrawer& spriteDrawer(){ return m_spriteDrawer; }
	const GLDrawer& lineDrawer() { return m_lineDrawer; }
	void prepTexture(OpenGLTexture* texture);
	void renderPrimitives(int layerIndex);
	virtual bool isRenderingToScreen() override;

	Shader& shader(GLShaderType shaderType) {
		return m_shaders[(int)shaderType];
	}

	glm::mat4  projectionMatrix() { return m_projectionMatrix; }
	void setProjectionMatrix(glm::mat4 projectionMatrix) { m_projectionMatrix = projectionMatrix; }

	//new
	int setRenderTarget(Texture* targetTexture) override;
	void resetRenderTarget() override;
	std::shared_ptr<Texture> createEmptyTexture(int width, int height, std::string name) override;


private:

	void _addVertexBufferToBatch(const std::vector<SpriteVertex>& spriteVertices, GLDrawerType objectType, Texture* texture, GLShaderType shaderType,
		RenderBlendMode textureBlendMode, int layer);
	void _addVertexBufferToBatch(const std::vector<LineVertex>& lineVertices, GLDrawerType objectType, GLShaderType shaderType, int layer);
	
	GLuint _addTexture(Texture* texture);
	SDL_GLContext m_glcontext{};
	GLDrawer m_spriteDrawer;
	GLDrawer m_lineDrawer;

	//Projection matrix
	glm::mat4  m_projectionMatrix{1.0};

	using BatchKey = std::tuple<int, GLDrawerType, GLuint, GLShaderType, RenderBlendMode>;
	std::map<BatchKey, std::shared_ptr<DrawBatch>> m_drawBatches;

	std::array<Shader, int(GLShaderType::count) +1> m_shaders;

	
};

