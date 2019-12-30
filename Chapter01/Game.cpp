// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)
,mPaddleDir1(0)
,mPaddleDir2(0)
{
	
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1 /* Can be otherwise if we have multiple windows */
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC /* vsync meaning don't swap buffers while the display is drawing */
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	//
	mPaddlePos1.x = 10.0f;
	mPaddlePos1.y = 768.0f / 2.0f;
	mPaddlePos2.x = 999.0f;
	mPaddlePos2.y = 768.0f / 2.0f;

	mBall.push_back({ { 1024.0f / 2.0f, 768.0f / 2.0f}, {-200.0f, 235.0f} });
	mBall.push_back({ { 1024.0f / 2.0f, 300.0f / 2.0f}, { 200.0f, 235.0f} });
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning) /* A basic game loop */
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	// Update paddle direction based on W/S keys
	mPaddleDir1 = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir1 -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir1 += 1;
	}

	// Update paddle direction 2 based on I/K keys
	mPaddleDir2 = 0;
	if (state[SDL_SCANCODE_I])
	{
		mPaddleDir2 -= 1;
	}
	if (state[SDL_SCANCODE_K])
	{
		mPaddleDir2 += 1;
	}
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	/* This is "frame limiting", so the game can't run at any arbitary frame rate and */
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f; /* SDL_GetTicks returns ms since SDL_Init call, minus the last value of SDL_GetTicks */
	
	// Clamp maximum delta time value
	/* Otherwise if there is a large time between draws (breakpoint for eg) game would skip forwards */
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();

	// Update paddle position based on direction
	if (mPaddleDir1 != 0)
	{
		/* 300 pixels per second * timeElapsed sinse last draw.
		* Use of deltaTime removes dependence of game movement on code execution speed.
		*/
		mPaddlePos1.y += mPaddleDir1 * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mPaddlePos1.y < (paddleH / 2.0f + thickness))
		{
			mPaddlePos1.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos1.y >(768.0f - paddleH / 2.0f - thickness))
		{
			mPaddlePos1.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	// Update paddle position based on direction
	if (mPaddleDir2 != 0)
	{
		/* 300 pixels per second * timeElapsed sinse last draw.
		* Use of deltaTime removes dependence of game movement on code execution speed.
		*/
		mPaddlePos2.y += mPaddleDir2 * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mPaddlePos2.y < (paddleH / 2.0f + thickness))
		{
			mPaddlePos2.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos2.y >(768.0f - paddleH / 2.0f - thickness))
		{
			mPaddlePos2.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}
	
	// Update ball position based on ball velocity
	//for (Ball b : mBall)
	for (unsigned short i = 0; i < mBall.size(); i++)
	{
		mBall[i].ballPos.x += mBall[i].ballVel.x * deltaTime;
		mBall[i].ballPos.x += mBall[i].ballVel.x * deltaTime;
		mBall[i].ballPos.y += mBall[i].ballVel.y * deltaTime;

		float diff1 = mPaddlePos1.y - mBall[i].ballPos.y;
		float diff2 = mPaddlePos2.y - mBall[i].ballPos.y;

		// Take absolute value of difference
		diff1 = (diff1 > 0.0f) ? diff1 : -diff1;
		diff2 = (diff2 > 0.0f) ? diff2 : -diff2;
		if (
			// Our y-difference is small enough
			diff1 <= paddleH / 2.0f &&
			// We are in the correct x-position
			mBall[i].ballPos.x <= 25.0f && mBall[i].ballPos.x >= 20.0f &&
			// The ball is moving to the left
			mBall[i].ballVel.x < 0.0f)
		{
			mBall[i].ballVel.x *= -1.0f;
		}
		else if (
			diff2 <= paddleH / 2.0f &&
			mBall[i].ballPos.x <= 1004.0f && mBall[i].ballPos.x >= 999.0f &&
			mBall[i].ballVel.x > 0.0f)
		{
			mBall[i].ballVel.x *= -1.0f;
		}
		// Did the ball go off the screen? (if so, end game)
		else if (mBall[i].ballPos.x <= 0.0f || mBall[i].ballPos.x >= 1024.0f)
		{
			mIsRunning = false;
		}
		
		// Did the ball collide with the top wall?
		if (mBall[i].ballPos.y <= thickness && mBall[i].ballVel.y < 0.0f)
		{
			mBall[i].ballVel.y *= -1;
		}
		// Did the ball collide with the bottom wall?
		else if (mBall[i].ballPos.y >= (768 - thickness) &&
			mBall[i].ballVel.y > 0.0f)
		{
			mBall[i].ballVel.y *= -1;
		}
	}
}

void Game::GenerateOutput()
{
	/* 1. Clear the back buffer
	 * 2. Draw to the back buffer
	 * 3. Swap the buffers
	 */

	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B /* This assumes we will always use 8-bit colours, which is the most common. Chosing floats 0.0 -> 1.0 doesn't assume this, but SDL uses the former */
		255		// A
	);
	
	// Clear back buffer
	SDL_RenderClear(mRenderer);

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	
	// Draw top wall
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw right wall
	/*wall.x = 1024 - thickness;
	wall.y = 0;
	wall.w = thickness;
	wall.h = 1024;
	SDL_RenderFillRect(mRenderer, &wall);*/

	// Draw paddle
	SDL_Rect paddle1{
		static_cast<int>(mPaddlePos1.x),
		static_cast<int>(mPaddlePos1.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle1);
	
	// Draw paddle 2
	SDL_Rect paddle2{
		static_cast<int>(mPaddlePos2.x),
		static_cast<int>(mPaddlePos2.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle2);
	
	// Draw ball
	for (Ball b : mBall)
	{
		SDL_Rect ball{	
			static_cast<int>(b.ballPos.x - thickness/2),
			static_cast<int>(b.ballPos.y - thickness/2),
			thickness,
			thickness
		};
		SDL_RenderFillRect(mRenderer, &ball);
	}
	
	// Swap front buffer and back buffer
	/* The reason we do this is if the display read the buffer while we were writting to it, you'd get a "screen tear".
	 * Writting to the back buffer and then swapping them in one action prevents this.
	 * This is called "double buffering".
	 * We must also wait for the display to finish drawing the front buffer because swapping buffers during a draw would cause the same issue.
	 * This is called "vertical synchronisation", or vsync after the signal the display sends indicating a refresh.
	 * vsync is often offered as an option, because it is a trade off between an occassional screen tear, and an occassional stutter.
	 * "Addaptive refresh rate" solves the problem by allowing the game to request a refresh, but it is not widely available.
	 */
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
