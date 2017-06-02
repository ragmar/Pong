#pragma once

#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/CollisionCircle2D.h>

#include "Ball.h"

Ball::Ball()
{

}

void Ball::LoadComponent(Scene* scene_, ResourceCache* cache)
{
	Sprite2D* ballSprite = cache->GetResource<Sprite2D>("Urho2D/Ball.png");

	ball_ = scene_->CreateChild("Ball");
	ball_->SetPosition2D(Vector2(0.0f, 0.0f));
	// Create rigid body
	ballBody_ = ball_->CreateComponent<RigidBody2D>();
	ballBody_->SetBodyType(BT_DYNAMIC);

	// Put movement to the ball (always start on enemy side)
	ballBody_->ApplyLinearImpulse(Vector2(-5.0f, Random(-3.0f, 3.0f)), Vector2(0.0f, 0.0f), true);
	ballBody_->SetGravityScale(0.0f);


	StaticSprite2D* staticSprite = ball_->CreateComponent<StaticSprite2D>();
	staticSprite->SetSprite(ballSprite);

	// Create circle
	ballCircle_ = ball_->CreateComponent<CollisionCircle2D>();
	// Set radius
	ballCircle_->SetRadius(0.16f);
	// Set density
	ballCircle_->SetDensity(0.0f);
	// Set friction.
	ballCircle_->SetFriction(0.0f);
	// Set restitution
	ballCircle_->SetRestitution(1.0f);
}

void Ball::ResetBall()
{
	ball_->SetPosition2D(0, 0);

	/*THIS IS INNEFICENT COULDN'T MAKE IT WORK ANY OTHER WAY*/
	ball_->RemoveComponent<RigidBody2D>();
	ball_->RemoveComponent<CollisionCircle2D>();

	ballBody_ = ball_->CreateComponent<RigidBody2D>();
	ballBody_->SetBodyType(BT_DYNAMIC);
	ballBody_->ApplyLinearImpulse(Vector2(-5.0f, Random(-3.0f, 3.0f)), Vector2(0.0f, 0.0f), true);
	ballBody_->SetGravityScale(0.0f);


	ballCircle_ = ball_->CreateComponent<CollisionCircle2D>();
	ballCircle_->SetRadius(0.16f);
	ballCircle_->SetDensity(0.0f);
	ballCircle_->SetFriction(0.0f);
	ballCircle_->SetRestitution(1.0f);
}

void Ball::RemoveAllComponents()
{
	ball_->RemoveAllComponents();
}

Vector2 Ball::GetPosition() 
{
	return ball_->GetPosition2D();
}

Ball::~Ball() 
{
	ballBody_->Remove();
	ballCircle_->Remove();
	ball_->Remove();
}
