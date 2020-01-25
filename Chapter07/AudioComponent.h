// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Component.h"
#include "SoundEvent.h"
#include "MoveComponent.h"
#include <vector>
#include <string>

class AudioComponent : public Component
{
public:
	AudioComponent(class Actor* owner, int updateOrder = 200, class MoveComponent* mc = nullptr);
	~AudioComponent();

	void Update(float deltaTime) override;
	void OnUpdateWorldTransform() override;

	SoundEvent PlayEvent(const std::string& name);
	void StopAllEvents();
private:
	std::vector<SoundEvent> mEvents2D;
	std::vector<SoundEvent> mEvents3D;
	class MoveComponent* mMoveComponent;
};