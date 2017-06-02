#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Model.h>

#include "Character.h"

Character::Character(Context* context) :
	LogicComponent(context)
{
	// Only the physics update event is needed: unsubscribe from the rest for optimization
	SetUpdateEventMask(USE_FIXEDUPDATE);
}

void Character::RegisterObject(Context* context)
{
	context->RegisterFactory<Character>();
}

void Character::Start()
{
	Node* node = GetNode();
}

void Character::FixedUpdate(float timeStep)
{
	node = GetNode();

	Vector2 moveDir = Vector2::ZERO;

	if (controls_.IsDown(CTRL_UP))
		moveDir += Vector2::UP;
	if (controls_.IsDown(CTRL_DOWN))
		moveDir += Vector2::DOWN;
	
	// Normalize move vector so that diagonal strafing is not faster
	if (moveDir.LengthSquared() > 0.0f)
		moveDir.Normalize();

	if ((node->GetPosition().y_ < CEIL_POSITION && moveDir.y_ > 0)
		|| (node->GetPosition().y_ > FLOOR_POSITION && moveDir.y_ < 0))
	{
		node->Translate2D(moveDir * MOVE_SPEED);
	}
}
