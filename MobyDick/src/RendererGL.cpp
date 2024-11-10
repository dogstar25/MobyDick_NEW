#include "RendererGL.h"


#include "opengl/LineDrawBatch.h"
#include "opengl/SpriteDrawBatch.h"
#include "IMGui/IMGuiUtil.h"

#include "game.h"

extern std::unique_ptr<Game> game;

RendererGL::RendererGL()
{


}

RendererGL::~RendererGL()
{

	//glDeleteTextures(1, texture_id);

	//glDeleteVertexArrays(1, &m_vao);

	SDL_GL_DeleteContext(m_glcontext);

}


void RendererGL::init(SDL_Window* window)
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	SDL_GLContext m_glcontext = SDL_GL_CreateContext(window);

	if (SDL_GL_SetSwapInterval(1) != 0) {
		std::cerr << "Warning: Unable to disable VSync. " << SDL_GetError() << std::endl;
	}

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		std::cout << SDL_GetError();
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;

	m_projectionMatrix = glm::ortho(0.0f, (float)game->gameScreenResolution().x, (float)game->gameScreenResolution().y, 0.0f, 0.0f, 10.0f);

	//Create the different drawers
	m_spriteDrawer = GLDrawer(GLDrawerType::GLSPRITE);
	m_lineDrawer = GLDrawer(GLDrawerType::GLLINE);

	//Create all shaders
	m_shaders[(int)GLShaderType::BASIC] = Shader(GLShaderType::BASIC);
	m_shaders[(int)GLShaderType::UBER] = Shader(GLShaderType::UBER);
	m_shaders[(int)GLShaderType::LINE] = Shader(GLShaderType::LINE);
	m_shaders[(int)GLShaderType::GLOW] = Shader(GLShaderType::GLOW);

	int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(GLDebugCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	
}

void RendererGL::setClearColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{

	glClearColor(static_cast<GLfloat>(r), static_cast<GLfloat>(g), static_cast<GLfloat>(b), static_cast<GLfloat>(a));
}

bool RendererGL::clear()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return true;
}

void RendererGL::drawBatches()
{

	//Draw each batch if batching is turned on
	if (GameConfig::instance().openGLBatching() == true) {

		for (auto& drawBatch : m_drawBatches) {

			drawBatch.second->draw();

		}

		m_drawBatches.erase(m_drawBatches.begin(), m_drawBatches.end());

	}

}

bool RendererGL::present()
{

	SDL_GL_SwapWindow(game->window());
	//SDL_Delay(1);

	return true;
}

void RendererGL::drawSprite(int layer, SDL_FRect destQuad, SDL_Color color, Texture* texture, SDL_Rect* textureSrcQuad, float angle,
	bool outline, SDL_Color outlineColor, RenderBlendMode textureBlendMode)
{

	auto normalizedcolor = util::glNormalizeColor(color);

	//if (static_cast<OpenGLTexture*>(texture)->textureId == 16) {

	//	int todd = 1;
	//	destQuad.x = 0;
	//	destQuad.y = 0;


	//}

	glm::vec2 glPosition{ destQuad.x, destQuad.y};
	glm::vec2 glSize{ destQuad.w, destQuad.h };
	glm::vec4 glColor{ normalizedcolor.r, normalizedcolor.g, normalizedcolor.b, normalizedcolor.a};
	//std::shared_ptr<SpriteVertex> vertex;

	//Array of 4 vertices
	std::vector<SpriteVertex> spriteVertexBuffer;

	//Initilaize a new translation matrix with one to start it out as an identity matrix
	glm::mat4 translationMatrix(1.0f);

	//Apply the position to the translation matrix
	translationMatrix = glm::translate(translationMatrix, glm::vec3(glPosition.x, glPosition.y, 1.0));

	angle = util::degreesToRadians(angle);

	////Apply the rotation - move to center, rotate, move back
	translationMatrix = glm::translate(translationMatrix, glm::vec3(glSize.x / 2, glSize.y / 2, 0.0));
	translationMatrix = glm::rotate(translationMatrix, angle, glm::vec3(0.0, 0.0, 1.0));
	translationMatrix = glm::translate(translationMatrix, glm::vec3(-(glSize.x / 2), -(glSize.y / 2), 0.0));

	//
	// Vertex Buffer Data
	// 	   Build the 4 vertices that make a quad/rectangle/square
	// 
	int zIndex = -1;

	//xPosition values
	int x1 = 0;
	int x2 = glSize.x;
	int x3 = glSize.x;
	int x4 = 0;

	//Horizontal Flip
	if (texture->applyFlip == true) {
		x1 = glSize.x;
		x2 = 0;
		x3 = 0;
		x4 = glSize.x;
	}

	SpriteVertex vertex;
	//v0
	vertex.positionAttribute = glm::vec3(x1, 0, zIndex);
	vertex.colorAttribute = glColor;
	glm::vec2 calculatedTextureCoordinates = { textureSrcQuad->x, textureSrcQuad->y };
	auto normalizedTextureCoords = util::glNormalizeTextureCoords(
		{ calculatedTextureCoordinates.x, calculatedTextureCoordinates.y }, 
		{ texture->surface->w, texture->surface->h });
	vertex.textureCoordsAttribute = normalizedTextureCoords;
	spriteVertexBuffer.push_back(vertex);

	//v1
	vertex.positionAttribute = glm::vec3{ x2, 0, zIndex };
	vertex.colorAttribute = glColor;

	calculatedTextureCoordinates = { textureSrcQuad->x + textureSrcQuad->w, textureSrcQuad->y };
	normalizedTextureCoords = util::glNormalizeTextureCoords(
		{ calculatedTextureCoordinates.x, calculatedTextureCoordinates.y },
		{ texture->surface->w, texture->surface->h });
	vertex.textureCoordsAttribute = normalizedTextureCoords;

	spriteVertexBuffer.push_back(vertex);

	//v2
	vertex.positionAttribute = glm::vec3{ x3, glSize.y, zIndex };
	vertex.colorAttribute = glColor;

	calculatedTextureCoordinates = { textureSrcQuad->x + textureSrcQuad->w, textureSrcQuad->y + textureSrcQuad->h };
	normalizedTextureCoords = util::glNormalizeTextureCoords(
		{ calculatedTextureCoordinates.x, calculatedTextureCoordinates.y },
		{ texture->surface->w, texture->surface->h });
	vertex.textureCoordsAttribute = normalizedTextureCoords;

	spriteVertexBuffer.push_back(vertex);

	//v3
	vertex.positionAttribute = glm::vec3{ x4, glSize.y, zIndex };
	vertex.colorAttribute = glColor;

	calculatedTextureCoordinates = { textureSrcQuad->x , textureSrcQuad->y + textureSrcQuad->h };
	normalizedTextureCoords = util::glNormalizeTextureCoords(
		{ calculatedTextureCoordinates.x, calculatedTextureCoordinates.y },
		{ texture->surface->w, texture->surface->h });
	vertex.textureCoordsAttribute = normalizedTextureCoords;

	spriteVertexBuffer.push_back(vertex);

	if (static_cast<OpenGLTexture*>(texture)->textureId == 16) {

		int todd = 1;

	}
	//Apply the tranlation matrix to each vertex
	for (int i = 0; i < 4; i++) {

		spriteVertexBuffer[i].positionAttribute = translationMatrix * glm::vec4(spriteVertexBuffer[i].positionAttribute, 1.0);

	}

	//shader needs to be passed in
	auto shadertype = GLShaderType::BASIC;
	//auto shadertype = GLShaderType::GLOW;

	if (GameConfig::instance().openGLBatching() == true && isRenderingToScreen()){
		_addVertexBufferToBatch(spriteVertexBuffer, GLDrawerType::GLSPRITE, texture, shadertype, textureBlendMode, layer);
	}
	else {
		Shader& shader = static_cast<RendererGL*>(game->renderer())->shader(shadertype);
		m_spriteDrawer.draw(spriteVertexBuffer, spriteindexBuffer, shader, texture, textureBlendMode);

	}

	//Outline the object if defined so
	if (outline) {

		outlineObject(destQuad, outlineColor, (GameLayer)layer);
	}


}

void RendererGL::drawLine(glm::vec2 pointA, glm::vec2 pointB, glm::uvec4 color, GameLayer layer)
{

	glm::vec4 normalizedcolor = util::glNormalizeColor(color);
	glm::vec4 redcolor = util::glNormalizeColor(glm::uvec4(255,0,0,255));

	//Array of 4 vertices
	std::vector<LineVertex> lineVertexBuffer;

	//
	// Vertex Buffer Data
	// 	   Build the 2 vertices that make a line
	// 
	int zIndex = -1;

	LineVertex vertex;
	//v0
	vertex.positionAttribute = glm::vec3{ pointA.x, pointA.y, zIndex };
	vertex.colorAttribute = normalizedcolor;
	lineVertexBuffer.push_back(vertex);

	//v1
	vertex.positionAttribute = glm::vec3{ pointB.x, pointB.y, zIndex };
	vertex.colorAttribute = normalizedcolor;
	lineVertexBuffer.push_back(vertex);

	//shader needs to be passed in
	auto shadertype = GLShaderType::LINE;

	if (GameConfig::instance().openGLBatching() == true && isRenderingToScreen()) {
		_addVertexBufferToBatch(lineVertexBuffer, GLDrawerType::GLLINE, shadertype, (int)layer);
	}
	else {
		Shader shader = static_cast<RendererGL*>(game->renderer())->shader(shadertype);
		m_lineDrawer.draw(lineVertexBuffer, 2, shader);
	}

}

void RendererGL::drawPoints(std::vector<SDL_FPoint> points, SDL_Color color, GameLayer layer)
{

	for (size_t i = 0; i < points.size() - 1; ++i) {

		glm::vec2 pointA(points[i].x, points[i].y);
		glm::vec2 pointB(points[i+1].x, points[i+1].y);
		
		glm::uvec4 colorGL = { color.r,color.b, color.g, color.a };

		drawLine(pointA, pointB, colorGL, layer);

	}

}

void RendererGL::resetRenderTarget() 
{
	// Unbind the framebuffer (go back to the default framebuffer, i.e., the screen)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Reset projection Matrix
	m_projectionMatrix = glm::ortho(0.0f, (float)game->gameScreenResolution().x, (float)game->gameScreenResolution().y, 0.0f, 0.0f, 10.0f);

	// Reset viewport to the window size
	glViewport(0, 0, game->gameScreenResolution().x, game->gameScreenResolution().y);
}

std::shared_ptr<Texture> RendererGL::createEmptyTexture(int width, int height, std::string name)
{

	std::shared_ptr<OpenGLTexture> texture = std::make_shared<OpenGLTexture>();

	texture->name = name;

	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId); // Bind it so that subsequent texture functions apply to it

	texture->textureId = textureId;

	texture->textureAtlasQuad.x = 0;
	texture->textureAtlasQuad.y = 0;
	texture->textureAtlasQuad.w = width;
	texture->textureAtlasQuad.h = height;

	texture->surface= SDL_CreateRGBSurface(0, width, height, 32,
		0x00FF0000, // Red mask
		0x0000FF00, // Green mask
		0x000000FF, // Blue mask
		0xFF000000  // Alpha mask
	);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->surface->pixels);

	static_cast<RendererGL*>(game->renderer())->prepTexture(texture.get());

	glBindTexture(GL_TEXTURE_2D, 0);

	// Generate and bind the framebuffer only once
	glGenFramebuffers(1, &texture->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, texture->fbo);

	// Cast texture and attach it to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->textureId, 0);

	// Check for framebuffer completeness
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error: Framebuffer is not complete!" << std::endl;
	}

	// Unbind framebuffer to reset to default
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return texture;
}


int RendererGL::setRenderTarget(Texture* targetTexture)
{
	// Bind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<OpenGLTexture*>(targetTexture)->fbo);

	//set projection matrix size to match the render target
	//The vertical size is the oppositte of the normal projection matrix when rendering to a texture
	m_projectionMatrix = glm::ortho(0.0f, (float)static_cast<OpenGLTexture*>(targetTexture)->textureAtlasQuad.w, 
		0.0f, (float)static_cast<OpenGLTexture*>(targetTexture)->textureAtlasQuad.h, 0.0f, 1.0f);

	// Set the viewport to the size of the target texture
	glViewport(0, 0, targetTexture->textureAtlasQuad.w, targetTexture->textureAtlasQuad.h);

	return 0;
}

void RendererGL::_addVertexBufferToBatch(const std::vector<SpriteVertex>& spriteVertices, GLDrawerType objectType, Texture* texture,
	GLShaderType shaderType, RenderBlendMode textureBlendMode, int layer)
{

	//Build the map key
	//keyString << (int)layer << "_" << (int)objectType << "_" << texturePtrString.str() << "_" << (int)shaderType << "_" << (int)textureBlendMode;
	auto textureId = static_cast<OpenGLTexture*>(texture)->textureId;
	BatchKey key = std::make_tuple(layer, objectType, textureId, shaderType, textureBlendMode);

	//See if the drawBatch for this combo exists yet
	if (m_drawBatches.find(key) == m_drawBatches.end()) {

		auto spriteBatch = std::make_shared<SpriteDrawBatch>(objectType, texture, shaderType, textureBlendMode);
		spriteBatch->addVertexBuffer(spriteVertices);
		m_drawBatches[key] = spriteBatch;

	}
	else {

		m_drawBatches[key]->addVertexBuffer(spriteVertices);

	}


}


void RendererGL::_addVertexBufferToBatch(const std::vector<LineVertex>& lineVertices, GLDrawerType objectType, GLShaderType shaderType, int layer)
{

	BatchKey key = std::make_tuple(layer, objectType, (GLuint)0, shaderType, RenderBlendMode::BLEND);

	//See if the drawBatch for this combo exists yet
	if (m_drawBatches.find(key) == m_drawBatches.end()) {

		auto lineBatch = std::make_shared<LineDrawBatch>(shaderType);
		lineBatch->addVertexBuffer(lineVertices);
		m_drawBatches[key] = lineBatch;

	}
	else {

		m_drawBatches[key]->addVertexBuffer(lineVertices);

	}

}

void RendererGL::prepTexture(OpenGLTexture* texture)
{


	SDL_Surface* surf = texture->surface;
	GLenum texture_format{ GL_RGB };

	auto nOfColors = surf->format->BytesPerPixel;
	if (nOfColors == 4)     // contains an alpha channel
	{
		if (surf->format->Rmask == 0x000000ff)
			texture_format = GL_RGBA;
		else
			texture_format = GL_RGBA; //WEIRD PROBLEM assume always that the image is GL_RGBA
	}
	else if (nOfColors == 3)     // no alpha channel
	{
		if (surf->format->Rmask == 0x000000ff)
			texture_format = GL_RGB;
		else
			texture_format = GL_BGR;
	}

	//Set the minification and magnification filters.  In this case, when the texture is minified (i.e., the texture's pixels (texels) are
	//*smaller* than the screen pixels you're seeing them on, linearly filter them (i.e. blend them together).  This blends four texels for
	//each sample--which is not very much.  Mipmapping can give better results.  Find a texturing tutorial that discusses these issues
	//further.  Conversely, when the texture is magnified (i.e., the texture's texels are *larger* than the screen pixels you're seeing
	//them on), linearly filter them.  Qualitatively, this causes "blown up" (overmagnified) textures to look blurry instead of blocky.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//load in the image data
	glTexImage2D(GL_TEXTURE_2D, 0, texture_format, surf->w, surf->h, 0, texture_format, GL_UNSIGNED_BYTE, surf->pixels);

	return;
}

bool RendererGL::isRenderingToScreen()
{
	GLint currentFramebuffer = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFramebuffer);

	if (currentFramebuffer == 0) {

		return true;
	}
	else {
		return false;
	}

	return false;
}

