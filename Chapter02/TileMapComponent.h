#pragma once
#include "SpriteComponent.h"
#include <vector>
#include "Math.h"


class TileMapComponent : public SpriteComponent
{
public:
	TileMapComponent(class Actor* owner, SDL_Texture* texture, const std::string& filename, int drawOrder = 100);
	void Draw(SDL_Renderer* renderer) override;
	void SetScreenSize(const Vector2& size) { mScreenSize = size; }
private:
	void ReadTileMap(const std::string& filename);
	std::vector<int> mLayer;
	SDL_Texture* mTexture;
	Vector2 mScreenSize;
};
