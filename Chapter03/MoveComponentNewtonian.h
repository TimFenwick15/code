// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Component.h"
#include "Math.h"

class MoveComponentNewtonian : public Component
{
public:
	// Lower update order to update first
	MoveComponentNewtonian(class Actor* owner, int updateOrder = 10, float mass = 1); /* Avoid divide by zero errors */

	void Update(float deltaTime) override;
	void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
	void AddForceVector(Vector2 forceVector, bool impulse);
private:
	float mAngularSpeed;
	Vector2 mForceVectorSum;
	Vector2 mBaseForce;
	float mForceSum;
	Vector2 mVelocity;
	float mMass;
	Vector2 mPosition;
	Vector2 mAcceleration;
	float mMaxSpeed;
	float mMaxAcceleration;
	float mSignedMaxFloat(float newValue, float max);
	bool mImpulse;
};
