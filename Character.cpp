//
// Copyright (c) 2008-2017 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/Constraint.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/IO/MemoryBuffer.h>

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Engine/Console.h>

#include <Urho3D/Urho2D/Drawable2D.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/CollisionBox2D.h>
#include <Urho3D/Urho2D/CollisionCircle2D.h>

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
	// Component has been inserted into its scene node. Subscribe to events now
	SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(Character, HandleNodeCollision));
}

void Character::FixedUpdate(float timeStep)
{
	/// \todo Could cache the components for faster access instead of finding them each frame
	Node* node = GetNode();
	RigidBody2D* body = GetComponent<RigidBody2D>();

	const Vector3& velocity = body->GetLinearVelocity();
	// Velocity on the XZ plane
	Vector3 planeVelocity(velocity.x_, 0.0f, velocity.z_);

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

void Character::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
	// Check collision contacts and see if character is standing on ground (look for a contact that has near vertical normal)
	using namespace NodeCollision;

	MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());

	while (!contacts.IsEof())
	{
		Vector3 contactPosition = contacts.ReadVector3();
		Vector3 contactNormal = contacts.ReadVector3();
		contacts.ReadFloat();
		contacts.ReadFloat();

		// If contact is below node center and pointing up, assume it's a ground contact
		if (contactPosition.y_ < (node_->GetPosition().y_ + 1.0f))
		{
			float level = contactNormal.y_;
			if (level > 0.75)
				onGround_ = true;
		}
	}
}