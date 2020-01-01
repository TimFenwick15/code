// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "AnimSpriteComponent.h"
#include "Math.h"

AnimSpriteComponent::AnimSpriteComponent(Actor* owner, int drawOrder)
	:SpriteComponent(owner, drawOrder)
	, mCurrFrame(0.0f)
	, mAnimFPS(24.0f)
	, mCurrentAnimation(0)
	, mUpdateAnimation(false)
	, mUpdateAnimationPending(false)
{
}

/**
 * Move an animated sprite on to its next frame
 */
void AnimSpriteComponent::Update(float deltaTime)
{
	SpriteComponent::Update(deltaTime);

	if (mAnimTextures.size() > 0 && mAnimRanges.size() > 0)
	{
		// Update the current frame based on frame rate
		// and delta time
		mCurrFrame += mAnimFPS * deltaTime;

		/* If we change annimation, we need to move the frame into the current animation range */
		if (mCurrFrame < mAnimRanges[mCurrentAnimation]->min)
		{
			mCurrFrame = mAnimRanges[mCurrentAnimation]->min;
		}
		if (mCurrFrame >= mAnimRanges[mCurrentAnimation]->max)
		{
			if (mAnimRanges[mCurrentAnimation]->loop == false)
			{
				mUpdateAnimation = true;
			}
			mCurrFrame = static_cast<float>(mAnimRanges[mCurrentAnimation]->min);
		}

		// Set the current texture
		SetTexture(mAnimTextures[static_cast<int>(mCurrFrame)]);
	}

	/* It's weird to have this keyboard response here, but there's no link from Ship we can use to access this scope */
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_SPACE])
	{
		mUpdateAnimationPending = true;
	}

	/* Debounce the space bar */
	if (!state[SDL_SCANCODE_SPACE] && mUpdateAnimationPending)
	{
		mUpdateAnimation = true;
	}
	if (mUpdateAnimation)
	{
		NextAnimation();
		mUpdateAnimation = false;
		mUpdateAnimationPending = false;
	}
}

void AnimSpriteComponent::SetAnimTextures(const std::vector<SDL_Texture*>& textures)
{
	mAnimTextures = textures;
	if (mAnimTextures.size() > 0)
	{
		// Set the active texture to first frame
		mCurrFrame = 0.0f;
		SetTexture(mAnimTextures[0]);
	}
}

/**
 * A big problem with this is if the user doesn't call this function, everything will break
 */
void AnimSpriteComponent::SetAnimRange(unsigned int min, unsigned int max, bool loop)
{
	tsAnimationParameters* tempParams = new tsAnimationParameters; /* memory leak I think */
	tempParams->min = min;
	tempParams->max = max;
	tempParams->loop = loop;
	mAnimRanges.emplace_back(tempParams);
}

void AnimSpriteComponent::NextAnimation()
{
	mCurrentAnimation++;
	if (mCurrentAnimation >= mAnimRanges.size())
	{
		mCurrentAnimation = 0;
	}
}