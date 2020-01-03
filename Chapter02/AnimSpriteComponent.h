// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SpriteComponent.h"
#include <vector>



class AnimSpriteComponent : public SpriteComponent
{
public:
	AnimSpriteComponent(class Actor* owner, int drawOrder = 100);
	~AnimSpriteComponent();
	// Update animation every frame (overriden from component)
	void Update(float deltaTime) override;
	// Set the textures used for animation
	void SetAnimTextures(const std::vector<SDL_Texture*>& textures);

	void SetAnimRange(unsigned int min, unsigned int max, bool loop);

	// Set/get the animation FPS
	float GetAnimFPS() const { return mAnimFPS; }
	void SetAnimFPS(float fps) { mAnimFPS = fps; }

	void NextAnimation();

private:
	typedef struct
	{
		unsigned int min;
		unsigned int max;
		bool loop;
	} tsAnimationParameters;

	// All textures in the animation
	std::vector<SDL_Texture*> mAnimTextures;
	// Current frame displayed
	float mCurrFrame;
	// Animation frame rate
	float mAnimFPS;

	unsigned int mCurrentAnimation;
	bool mUpdateAnimation;
	bool mUpdateAnimationPending;

	std::vector<tsAnimationParameters*> mAnimRanges;
};
