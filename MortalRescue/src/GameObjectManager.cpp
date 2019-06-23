#include "game.h"
#include "GameObjectManager.h"
#include "TextureManager.h"
#include "GameObject.h"
#include "PlayerObject.h"
#include "WorldObject.h"

#include <SDL.h>
#include <SDL_image.h>
#include <json/json.h>

#include <iostream>
#include <fstream>

bool GameObjectManager::init()
{
	load("gameObjects_Common");
	load("gameObjects_Level1");
	//load("gameObjects_Test");
	

	return true;
}



/*


Build Animation object


*/
GameObjectAnimation* GameObjectManager::buildAnimation(GameObjectDefinition* gameObjectDefinition,
														string id, string textureId, int frames,
														float speed)
{

	GameObjectAnimation* animation=nullptr;
	animation = new GameObjectAnimation();

	animation->id = id;
	animation->frameCount = frames;
	animation->speed = speed;

	//Get pointer to textture
	animation->texture = game->textureManager.getTexture(textureId)->sdlTexture;

	//Calculate how many columns and rows this animation texture has
	int width, height;
	//First get width of textture
	SDL_QueryTexture(animation->texture, NULL, NULL, &width, &height);

	//Calculate nnumber of rows and columns - remember to convert the gameObject size to pixels first
	int rows, columns;
	columns = width / (gameObjectDefinition->xSize * game->config.scaleFactor);
	rows = height / (gameObjectDefinition->ySize * game->config.scaleFactor);

	//Calculate top left corner of each animation frame
	SDL_Point point;
	int frameCount = 0;
	for (int rowIdx = 0; rowIdx < rows; rowIdx++) {
		for (int colIdx = 0; colIdx < columns; colIdx++) {

			point.x = colIdx * (gameObjectDefinition->xSize * game->config.scaleFactor);
			point.y = rowIdx * (gameObjectDefinition->ySize * game->config.scaleFactor);
			//animation->animationFramePositions[frameCount] = point;
			animation->animationFramePositions.push_back(point);

			//do not exceed the maximum number of frames that this texture holds
			frameCount++;
			if (frameCount >= animation->frameCount) {
				break;
			}
		}
	}
	
	//DRUNK: Initialze the current source rect to the first animation frame
	//SDL_Rect* sourceRect = nullptr;
	//animation->currentTextureAnimationSrcRect

	return animation;



}



void GameObjectManager::load(string gameObjectAssetsFilename)
{
	//Read file and stream it to a JSON object
	Json::Value root;
	string filename = "assets/" + gameObjectAssetsFilename + ".json";
	ifstream ifs(filename);
	ifs >> root;

	string id, textureId;
	GameObjectDefinition* gameObjectDefinition;

	for (auto itr : root["gameObjects"])
	{
		gameObjectDefinition = new GameObjectDefinition();
		gameObjectDefinition->id = itr["id"].asString();
		gameObjectDefinition->description = itr["description"].asString();
		gameObjectDefinition->speed = itr["speed"].asDouble();
		gameObjectDefinition->xSize = itr["xSize"].asFloat();
		gameObjectDefinition->ySize = itr["ySize"].asFloat();
		gameObjectDefinition->texture = itr["texture"].asString();

		//If this has a textture then get and store it
		if (itr["primativeShape"].isNull() == false)
		{
			gameObjectDefinition->isPrimitiveShape = true;

			//color
			if (itr["primativeColor"]["random"].asBool() == true)
			{
				gameObjectDefinition->primativeColor = game->util.generateRandomColor();
			}
			else
			{
				gameObjectDefinition->primativeColor.r = itr["primativeColor"]["red"].asInt();
				gameObjectDefinition->primativeColor.g = itr["primativeColor"]["green"].asInt();
				gameObjectDefinition->primativeColor.b = itr["primativeColor"]["blue"].asInt();
				gameObjectDefinition->primativeColor.a = itr["primativeColor"]["alpha"].asInt();
			}
		}

		//If this is a physics object then build the box2d body
		if (itr["physicsObject"].isNull() == false)
		{
			gameObjectDefinition->isPhysicsObject = true;
			gameObjectDefinition->friction = itr["physicsObject"]["friction"].asFloat();
			gameObjectDefinition->density = itr["physicsObject"]["density"].asFloat();
			gameObjectDefinition->linearDamping = itr["physicsObject"]["linearDamping"].asFloat();
			gameObjectDefinition->angularDamping = itr["physicsObject"]["angularDamping"].asFloat();
			gameObjectDefinition->physicsType = itr["physicsObject"]["type"].asString();
			gameObjectDefinition->collisionShape = itr["physicsObject"]["collisionShape"].asString();
			gameObjectDefinition->collisionRadius = itr["physicsObject"]["collisionRadius"].asFloat();

		}

		//Store Animations
		if (itr["animations"].isNull() == false)
		{
			gameObjectDefinition->isAnimated = true;
			GameObjectAnimation* animation;
			for (auto animItr : itr["animations"])
			{
				string texture = animItr["texture"].asString();
				string id = animItr["id"].asString();
				int frames = animItr["frames"].asInt();
				float speed = animItr["speed"].asFloat();
				animation = buildAnimation(gameObjectDefinition, id, texture, frames, speed);
				gameObjectDefinition->animations[id] = animation;
			}
		}

		this->gameObjectDefinitions[gameObjectDefinition->id] = gameObjectDefinition;

	}
}

GameObjectManager::GameObjectManager() 
{

}

GameObjectManager::~GameObjectManager() 
{

	//clean and delete all of the game objects
	/*
	for (auto gameObjectDefinition : this->gameObjectDefinitions) {

		delete gameObjectDefinition.second;
	}
	*/
	this->gameObjectDefinitions.clear();
}
