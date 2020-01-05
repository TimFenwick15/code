// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "MoveComponentNewtonian.h"
#include "Actor.h"

MoveComponentNewtonian::MoveComponentNewtonian(class Actor* owner, int updateOrder, float mass)
	:Component(owner, updateOrder)
	, mAngularSpeed(0.0f)
	, mMass(mass)
	, mMaxSpeed(1000.0f)
	, mMaxAcceleration(500.0f)
	, mImpulse(false)
{

}

void MoveComponentNewtonian::Update(float deltaTime)
{
	Vector2 position = mOwner->GetPosition();

	/* Decay the speed to simulate drag, and stop the ship if no force is applied */
	if (Math::NearZero(mVelocity.x))
	{
		mVelocity.x = 0;
	}
	if (Math::NearZero(mVelocity.y))
	{
		mVelocity.y = 0;
	}

	/* Decay speed with no force only so this deceleration doesn't fight our acceleration parameters */
	if (Math::NearZero(mForceVectorSum.x))
	{
		mVelocity.x *= 0.95;
	}
	if (Math::NearZero(mForceVectorSum.y))
	{
		mVelocity.y *= 0.95;
	}

	// F = ma; a = F / m. Be careful of divide by zero here...
	if (mMass != 0)
	{
		mAcceleration.x = mSignedMaxFloat(mForceVectorSum.x / mMass, mMaxAcceleration);
		mAcceleration.y = mSignedMaxFloat(mForceVectorSum.y / mMass, mMaxAcceleration);

		mVelocity.x = mSignedMaxFloat(mVelocity.x + mAcceleration.x * deltaTime, mMaxSpeed);
		mVelocity.y = mSignedMaxFloat(mVelocity.y + mAcceleration.y * deltaTime, mMaxSpeed);

		position.x = position.x + mVelocity.x * deltaTime;
		position.y = position.y + mVelocity.y * deltaTime;
	}

	/* Impulse forces are added straight to the mForceVectorSum. Constant forces to mBaseForce */
	mForceVectorSum.x = mBaseForce.x;
	mForceVectorSum.y = mBaseForce.y;

	/* Screen wrapping */
	if (position.x < 0.0f) { position.x = 1022.0f; }
	else if (position.x > 1024.0f) { position.x = 2.0f; }
	if (position.y < 0.0f) { position.y = 766.0f; }
	else if (position.y > 768.0f) { position.y = 2.0f; }

	mOwner->SetPosition(position);

	if (!Math::NearZero(mAngularSpeed))
	{
		float rot = mOwner->GetRotation();
		rot += mAngularSpeed * deltaTime;
		mOwner->SetRotation(rot);
	}
}

void MoveComponentNewtonian::AddForceVector(Vector2 forceVector, bool impulse)
{
	if (impulse)
	{
		mBaseForce = Vector2::Zero;
	}
	else
	{
		mBaseForce = forceVector;
	}
	mForceVectorSum = forceVector;
}

float MoveComponentNewtonian::mSignedMaxFloat(float newValue, float absoluteMax)
{
	if (abs(newValue) > absoluteMax)
	{
		if (newValue >= 0)
		{
			newValue = absoluteMax;
		}
		else
		{
			newValue = -absoluteMax;
		}
	}
	return newValue;
}
