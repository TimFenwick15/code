#include "TileMapComponent.h"
#include "Actor.h"
#include "Game.h"
#include <fstream>
#include <sstream>

TileMapComponent::TileMapComponent(class Actor* owner, SDL_Texture* texture, const std::string& filename, int drawOrder)
	:SpriteComponent(owner, drawOrder)
{
	mTexture = texture;
	ReadTileMap(filename);
}

void TileMapComponent::Draw(SDL_Renderer* renderer)
{
	constexpr unsigned int tileSide = 32;
	constexpr unsigned int blankTileNumber = 191;
	SDL_Rect srcRect;
	srcRect.w = tileSide;
	srcRect.h = tileSide;
	SDL_Rect dstRect;
	dstRect.w = tileSide;
	dstRect.h = tileSide;

	for (unsigned int i = 0; i < mLayer.size(); i++)
	{
		int tileNumber = mLayer[i];
		if (tileNumber == -1)
		{
			tileNumber = blankTileNumber;
		}
		srcRect.x = (tileNumber % 8) * tileSide;
		srcRect.y = (tileNumber / 8) * tileSide;
		dstRect.x = (i % 32) * tileSide;
		dstRect.y = (i / 32) * tileSide;
		SDL_RenderCopy(renderer, /* the book says to use SD:_RenderCopyEx, but this works */
			mTexture,
			&srcRect,
			&dstRect
		);
	}
}

void TileMapComponent::ReadTileMap(const std::string& filename)
{
	std::ifstream input_from_file(filename);
	std::string line;
	while (getline(input_from_file, line)) {
		line += ",";
		std::stringstream ss(line);
		std::string word;
		std::vector<std::string> words;
		while (getline(ss, word, ','))
		{
			mLayer.emplace_back(std::stoi(word));
		}
	}
}
