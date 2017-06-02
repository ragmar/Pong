#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Urho2D/RigidBody2D.h>

#include "Enemy.h"
#include "Character.h"

Enemy::Enemy(Context* context) :
	LogicComponent(context)
{
	SetUpdateEventMask(USE_FIXEDUPDATE);
}

void Enemy::RegisterObject(Context* context)
{
	context->RegisterFactory<Enemy>();
}

void Enemy::Start()
{
	node = GetNode();
	// Component has been inserted into its scene node. Subscribe to events now

}

//Calculate movement of the enemy
void Enemy::FixedUpdate(float timeStep)
{
	Node* node = GetNode();
	RigidBody2D* body = GetComponent<RigidBody2D>();

	if (ballPosition.x_ <= 0) 
	{
		if (node->GetPosition().y_ < CEIL_POSITION
			&& ballPosition.y_ > node->GetPosition().y_)
		{
			node->Translate2D(Vector2::DOWN*ENEMY_SPEED);
		}
		else if (node->GetPosition().y_ > FLOOR_POSITION
			&& ballPosition.y_ < node->GetPosition().y_)
		{
			node->Translate2D(Vector2::UP*ENEMY_SPEED);
		}
	}
}

//Set the current ball position to move the enemy
void Enemy::SetBallPosition(Vector2 ballPosition)
{
	this->ballPosition = ballPosition;
}
