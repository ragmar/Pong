#pragma once

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

const int CTRL_UP = 1;
const int CTRL_DOWN = 2;

const float MOVE_SPEED = 0.04f;
const float CEIL_POSITION = 2.485f;
const float FLOOR_POSITION = -2.485f;

/// Character component, responsible for physical movement according to controls, as well as animation.
class Character : public LogicComponent
{
	URHO3D_OBJECT(Character, LogicComponent);

public:
	/// Construct.
	Character(Context* context);

	/// Register object factory and attributes.
	static void RegisterObject(Context* context);

	/// Handle startup. Called by LogicComponent base class.
	virtual void Start();
	/// Handle physics world update. Called by LogicComponent base class.
	virtual void FixedUpdate(float timeStep);

	/// Movement controls. Assigned by the main program each frame.
	Controls controls_;

private:
	/// Handle physics collision event.
	void HandleNodeCollision(StringHash eventType, VariantMap& eventData);

	/// Grounded flag for movement.
	bool onGround_;
};