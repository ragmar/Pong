#pragma once

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

const float ENEMY_SPEED = 0.0125f;

/// Character component, responsible for physical movement according to controls, as well as animation.
class Enemy : public LogicComponent
{
	URHO3D_OBJECT(Enemy, LogicComponent);

public:
	/// Construct.
	Enemy(Context* context);

	/// Register object factory and attributes.
	static void RegisterObject(Context* context);

	/// Handle startup. Called by LogicComponent base class.
	virtual void Start();

	/// Handle physics world update. Called by LogicComponent base class.
	virtual void FixedUpdate(float timeStep);

	//Set ball Position to move the enemy
	virtual void SetBallPosition(Vector2 ballPosition);

private:

	Vector2 ballPosition;
	Node* node;
};