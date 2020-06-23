#include "CompositeObject.h"

#include "TextureManager.h"

#include "Globals.h"


#include "GameObjectManager.h"

CompositeObject::CompositeObject(std::string gameObjectId, float xMapPos, float yMapPos, float angleAdjust) :
	WorldObject(gameObjectId, xMapPos, yMapPos, angleAdjust)
{

	_buildComposite();

}

CompositeObject::~CompositeObject()
{


}
void CompositeObject::update()
{
	//Call base game object update
	WorldObject::update();

	_updatePieces();
}

void CompositeObject::render()
{
	WorldObject::render();

	for (GameObjectPiece pieceObject : m_pieces)
	{
		if(pieceObject.isDestroyed == false)
		{
			pieceObject.pieceObject->render();
		}
		
	}
	

}

void CompositeObject::_buildComposite()
{

	SDL_Texture* blueprintTexure;
	SDL_Surface* blueprintSurface;
	SDL_PixelFormat* fmt;
	SDL_Color* color;
	std::string blueprintTexureId;
	Uint8 red, green, blue, alpha;

	//Get the texture and the surface
	blueprintTexureId = this->definition()->compositeDetails.blueprint.textureId;
	blueprintSurface = TextureManager::instance().getTexture(blueprintTexureId)->surface;
	int bpp = blueprintSurface->format->BytesPerPixel;

	SDL_LockSurface(blueprintSurface);

	//Loop through entire image, top to bottom, left to right and build the pieces
	for (int y = 0; y < blueprintSurface->h; y++)
	{
		for (int x = 0; x < blueprintSurface->w; x++)
		{

			//get the pixel at this location
			Uint8* currentPixel = (Uint8*)blueprintSurface->pixels + y * blueprintSurface->pitch + x * bpp;

			//Parse the pixel info into a color
			SDL_GetRGBA(*(Uint32*)currentPixel, blueprintSurface->format, &red, &green, &blue, &alpha);
			SDL_Color currentPixelcolor = { red, green, blue };

			//Loop through the legend to find which gameObject should be built
			for (CompositeLegendItem legendItem : this->definition()->compositeDetails.blueprint.legend)
			{
				if (currentPixelcolor == legendItem.color == true)
				{
					_buildPiece(legendItem, x, y);
				}
			}

		}
	}

	SDL_UnlockSurface(blueprintSurface);
}

void CompositeObject::_buildPiece(CompositeLegendItem legendItem, int xPos, int yPos)
{
	float xOffset, yOffset;
	GameObjectPiece piece = {};
	piece.currentlevel = 1;
	piece.isDestroyed = false;
	piece.time_snapshot = std::chrono::steady_clock::now();

	/*
	Build the game objects off screen. They will be placed in exect location duriing update loop
	*/
	WorldObject* worldObject =
		GameObjectManager::instance().buildGameObject<WorldObject>(legendItem.gameObjectId, -5, -5, 0);
	worldObject->setStrength(this->definition()->compositeDetails.levels[0].strength);
	piece.pieceObject = worldObject;
		

	//calculate the X,Y offset position in relating to the base object
	xOffset = xPos * piece.pieceObject->size().x;
	yOffset = yPos * piece.pieceObject->size().y;

	piece.parentPositionOffset.x = xOffset;
	piece.parentPositionOffset.y = yOffset;

	//Temp color setting
	piece.pieceObject->setColor(255,0,0,255);

	//Initialize color and strength to level 1
	piece.pieceObject->setColor(this->definition()->compositeDetails.levels[0].color);

	m_pieces.push_back(piece);

}

void CompositeObject::_updatePieces()
{
	for (auto& pieceObject : m_pieces)
	{

		//Update the state of the piece
		_updatePieceState(pieceObject);

		//Update the position of the piece
		_updatePiecePosition(pieceObject);

		//The piece object itself is a gameObject that should have its update called
		pieceObject.pieceObject->update();

	}



}

void CompositeObject::_updatePieceState(GameObjectPiece& piece)
{
	//Get now time
	std::chrono::steady_clock::time_point now_time = std::chrono::steady_clock::now();

	//Should this object be removed?
	if (piece.pieceObject->removeFromWorld() == true)
	{

		piece.pieceObject->setActive(false);
		piece.isDestroyed = true;
		piece.time_snapshot = now_time;
		piece.pieceObject->setRemoveFromWorld(false);
	}

	//Has enough time gone by to regenerate the next armor level
	if (piece.isDestroyed == true)
	{
		std::chrono::duration<double, std::milli> timeDiffMilliSeconds = now_time - piece.time_snapshot;
		if (timeDiffMilliSeconds.count() >= this->definition()->compositeDetails.levelUpSpeed)
		{

			//Level up the piece object
			_levelUp(piece);
		}

	}


}

void CompositeObject::_updatePiecePosition(GameObjectPiece& piece)
{

	b2Vec2 piecePosition{ 0,0 };
	b2Vec2 adjustment{ 0,0 };

	//calculate the X,Y offset position in relating to the base object
	SDL_Rect parentPositionRect = this->getPositionRect();

	//piecePositions
	SDL_Rect piecePositionRect{};
	piecePositionRect.x = parentPositionRect.x + piece.parentPositionOffset.x;
	piecePositionRect.y = parentPositionRect.y + piece.parentPositionOffset.y;
	piecePositionRect.w = piece.pieceObject->size().x;
	piecePositionRect.h = piece.pieceObject->size().y;

	//Adjust the piece position based on the base objects rotation/angle
	adjustment = this->matchParentRotation(piecePositionRect, parentPositionRect, this->angle());
	piecePositionRect.x += adjustment.x;
	piecePositionRect.y += adjustment.y;

	//Create a vec position object to pass to setPosition
	piecePosition.x = piecePositionRect.x;
	piecePosition.y = piecePositionRect.y;
	piece.pieceObject->setPosition(piecePosition, this->angle());
}

void CompositeObject::_levelUp(GameObjectPiece& piece)
{
	int nextLevel = piece.currentlevel + 1;

	for (CompositeLevel level : this->definition()->compositeDetails.levels)
	{

		piece.isDestroyed = false;

		if( level.levelNum == nextLevel)
		{
			piece.currentlevel = level.levelNum;
//			piece.isDestroyed = false;

			piece.pieceObject->setColor(level.color);
			piece.pieceObject->setStrength( level.strength);
			piece.pieceObject->setActive(true);
		
		}

	}


}