#pragma once

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

class Ball 
{

public:
	/// Construct.
	Ball();

	///Load all the components for the ball in the scene
	virtual void LoadComponent(Scene* scene, ResourceCache* cache);

	///Get position from the ball
	Vector2 GetPosition();

	///Reset the ball to the middle
	virtual void ResetBall();

	///Remove the ball from the scene.
	virtual void RemoveAllComponents();

	~Ball();

private:

	SharedPtr<Node> ball_;
	RigidBody2D* ballBody_;
	CollisionCircle2D* ballCircle_;
};