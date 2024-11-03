#pragma once
#include <glad/glad.h>
#include <vector>
#include <memory>

#include "GLDrawer.h"
#include "Vertex.h"
//#include "IndexBuffer.h"
#include "Shader.h"
#include "../texture.h"

class DrawBatch
{
public:

	DrawBatch() = default;
	
	virtual void addVertexBuffer(const std::vector<SpriteVertex>& spriteVertices) {};
	virtual void addVertexBuffer(const std::vector<LineVertex>& lineVertices) {};
	virtual void draw() = 0;
	virtual void clear() = 0;
	
	std::vector<glm::uint> const& indexes() { return  m_indexes; }

protected:

	std::vector<glm::uint> m_indexes;
	GLDrawer m_glDrawer;
	Shader m_shader;
	int m_objectCount{};
	std::string m_textureName{};

	//virtual void _addSpriteIndexBuffer() = 0;

};

