#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Core/CoreEvents.h>

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Input/Input.h>

#include <Urho3D/Urho2D/Drawable2D.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/CollisionBox2D.h>
#include <Urho3D/Urho2D/CollisionCircle2D.h>

#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>

#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>

#include "Character.h"

using namespace Urho3D;

class MyApp : public Application
{
public:

	SharedPtr<Node> cameraNode_;
	SharedPtr<Scene> scene_;
	SharedPtr<Node> enemy_;
	SharedPtr<Node> ball_;
	WeakPtr<Character> character_;

    MyApp(Context* context) :
        Application(context)
    {
		Character::RegisterObject(context);
    }
    virtual void Setup()
    {
        // Called before engine initialization. engineParameters_ member variable can be modified here
		engineParameters_["FullScreen"] = false;
		engineParameters_["WindowWidth"] = 1280;
		engineParameters_["WindowHeight"] = 720;
		engineParameters_["WindowResizable"] = true;
    }

    virtual void Start()
    {
		//printf("%f %f %f %f\n", CEIL_POSITION, FLOOR_POSITION, node->GetPosition2D().y_, node->GetPosition2D().x_);
		//OpenConsoleWindow();

		CreateScene();
		/*
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		UI* ui = GetSubsystem<UI>();
		
		// Construct new Text object, set string to display and font to use
		Text* instructionText = ui->GetRoot()->CreateChild<Text>("AIScore");
		instructionText->SetText("0");
		instructionText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 45);

		// Position the text relative to the screen center
		instructionText->SetHorizontalAlignment(HA_LEFT);
		instructionText->SetVerticalAlignment(VA_CENTER);
		instructionText->SetPosition(ui->GetRoot()->GetWidth() / 32, 0);

		// Construct new Text object, set string to display and font to use
		instructionText = ui->GetRoot()->CreateChild<Text>("PlayerScore");
		instructionText->SetText("0");
		instructionText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 45);

		// Position the text relative to the screen center
		instructionText->SetHorizontalAlignment(HA_RIGHT);
		instructionText->SetVerticalAlignment(VA_CENTER);
		instructionText->SetPosition(- ui->GetRoot()->GetWidth() / 32, 0);*/
		

		SetupViewport();

		SubscribeToEvents();
    }
    
	virtual void CreateScene()
	{
		scene_ = new Scene(context_);
		scene_->CreateComponent<Octree>();
		scene_->CreateComponent<DebugRenderer>();

		CreateCamera();

		ResourceCache* cache = GetSubsystem<ResourceCache>();
		Sprite2D* boxSprite = cache->GetResource<Sprite2D>("Urho2D/Box.png");
		Sprite2D* ballSprite = cache->GetResource<Sprite2D>("Urho2D/Ball.png");

		/************************GROUND****************************/
		// Create ground.
		Node* groundNode = scene_->CreateChild("Ground");
		groundNode->SetPosition(Vector3(0.0f, -3.3f, 0.0f));
		groundNode->SetScale(Vector3(200.0f, 1.0f, 0.0f));

		// Create 2D rigid body for gound
		/*RigidBody2D* groundBody = */groundNode->CreateComponent<RigidBody2D>();

		StaticSprite2D* groundSprite = groundNode->CreateComponent<StaticSprite2D>();
		groundSprite->SetSprite(boxSprite);

		// Create box collider for ground
		CollisionBox2D* groundShape = groundNode->CreateComponent<CollisionBox2D>();
		// Set box size
		groundShape->SetSize(Vector2(0.32f, 0.32f));

		// Set friction
		groundShape->SetFriction(0.5f);
		/************************END GROUND****************************/

		/*********************CEIL***********************/
		Node* ceilNode = scene_->CreateChild("Ceil");
		ceilNode->SetPosition(Vector3(0.0f, 3.3f, 0.0f));
		ceilNode->SetScale(Vector3(200.0f, 1.0f, 0.0f));

		// Create 2D rigid body for gound
		/*RigidBody2D* groundBody = */ceilNode->CreateComponent<RigidBody2D>();

		StaticSprite2D* ceilSprite = ceilNode->CreateComponent<StaticSprite2D>();
		ceilSprite->SetSprite(boxSprite);

		CollisionBox2D* ceilShape = ceilNode->CreateComponent<CollisionBox2D>();

		ceilShape->SetSize(Vector2(0.32f, 0.32f));

		ceilShape->SetFriction(0.5f);

		/*********************END CEIL***********************/

		CreateCharacter(boxSprite);

		CreateEnemy(boxSprite);

		CreateBall(ballSprite);
	}

	virtual void CreateCamera() 
	{
		// Create camera node
		cameraNode_ = scene_->CreateChild("Camera");
		// Set camera's position
		//cameraNode_->SetPosition(Vector3(0.0f, 0.0f, -10.0f));
		cameraNode_->SetPosition(Vector3(0.0f, 0.0f, -10.0f));

		Camera* camera = cameraNode_->CreateComponent<Camera>();
		camera->SetOrthographic(true);

		Graphics* graphics = GetSubsystem<Graphics>();
		camera->SetOrthoSize((float)graphics->GetHeight() * PIXEL_SIZE);
		camera->SetZoom(1.2f * Min((float)graphics->GetWidth() / 1280.0f, (float)graphics->GetHeight() / 800.0f)); // Set zoom according to user's resolution to ensure full visibility (initial zoom (1.2) is set for full visibility at 1280x800 resolution)
	}
	virtual void CreateCharacter(Sprite2D* boxSprite)
	{
		Node* playerNode = scene_->CreateChild("Player");
		playerNode->SetPosition(Vector3(5.0f, 0.0f, 0.0f));
		playerNode->SetScale(Vector3(1.0f, 4.0f, 0.0f));

		// Create 2D rigid body for gound
		RigidBody2D* playerBody = playerNode->CreateComponent<RigidBody2D>();
		playerBody->SetBodyType(BT_KINEMATIC);

		StaticSprite2D* playerSprite = playerNode->CreateComponent<StaticSprite2D>();
		playerSprite->SetSprite(boxSprite);

		CollisionBox2D* playerShape = playerNode->CreateComponent<CollisionBox2D>();
		playerShape->SetSize(Vector2(0.32f, 0.32f));

		// Create the character logic component, which takes care of steering the rigidbody
		// Remember it so that we can set the controls. Use a WeakPtr because the scene hierarchy already owns it
		// and keeps it alive as long as it's not removed from the hierarchy
		character_ = playerNode->CreateComponent<Character>();
	}

	virtual void CreateEnemy(Sprite2D* boxSprite)
	{
		Node* enemyNode = scene_->CreateChild("Enemy");
		enemyNode->SetPosition(Vector3(- 5.0f, 0.0f, 0.0f));
		enemyNode->SetScale(Vector3(1.0f, 4.0f, 0.0f));
		enemyNode->SetRotation2D(180.0f);

		// Create 2D rigid body for gound
		RigidBody2D* enemyBody = enemyNode->CreateComponent<RigidBody2D>();
		enemyBody->SetBodyType(BT_KINEMATIC);

		StaticSprite2D* enemySprite = enemyNode->CreateComponent<StaticSprite2D>();
		enemySprite->SetSprite(boxSprite);

		CollisionBox2D* enemyShape = enemyNode->CreateComponent<CollisionBox2D>();
		enemyShape->SetSize(Vector2(0.32f, 0.32f));

		// Create the character logic component, which takes care of steering the rigidbody
		// Remember it so that we can set the controls. Use a WeakPtr because the scene hierarchy already owns it
		// and keeps it alive as long as it's not removed from the hierarchy
		enemy_ = enemyNode;
	}

	virtual void CreateBall(Sprite2D* ballSprite) 
	{
		ball_ = scene_->CreateChild("Ball");
		ball_->SetPosition(Vector3(0.0f, 0.0f, 0.0f));

		// Create rigid body
		RigidBody2D* ballBody = ball_->CreateComponent<RigidBody2D>();
		ballBody->SetBodyType(BT_DYNAMIC);
		//ballBody->SetLinearVelocity(Vector2(0.5f, 1.0f));
		ballBody->ApplyLinearImpulse(Vector2(-5.0f, 3.0f), Vector2(-2.0f, 3.0f), true);
		ballBody->SetGravityScale(0.0f);


		StaticSprite2D* staticSprite = ball_->CreateComponent<StaticSprite2D>();

		staticSprite->SetSprite(ballSprite);

		// Create circle
		CollisionCircle2D* circle = ball_->CreateComponent<CollisionCircle2D>();
		// Set radius
		circle->SetRadius(0.16f);
		// Set density
		circle->SetDensity(0.0f);
		// Set friction.
		circle->SetFriction(0.0f);
		// Set restitution
		circle->SetRestitution(1.0f);

	}

	virtual void SetupViewport() 
	{
		Renderer* renderer = GetSubsystem<Renderer>();

		// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
		SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
		renderer->SetViewport(0, viewport);
	}

	virtual void Stop()
    {
        // Perform optional cleanup after main loop has terminated
    }

	void HandleCollision(StringHash eventType, VariantMap& eventData)
	{

	}

	void HandleUpdate(StringHash eventType, VariantMap& eventData)
	{
		using namespace Update;
		
		Input* input = GetSubsystem<Input>();

		if (character_) 
		{
			character_->controls_.Set(CTRL_UP, input->GetKeyDown(KEY_W));
			character_->controls_.Set(CTRL_DOWN, input->GetKeyDown(KEY_S));
		}

		if (enemy_ && ball_) 
		{
			Vector2 enemyPosition	= enemy_->GetPosition2D();
			Vector2 ballPosition	= ball_->GetPosition2D();

			if (enemyPosition.y_ < ballPosition.y_) 
			{
				enemy_->Translate2D(Vector2::DOWN*0.0125f);
			}
			else 
			{
				enemy_->Translate2D(Vector2::UP*0.0125f);
			}
		}
	}

    void HandleKeyDown(StringHash eventType, VariantMap& eventData)
    {
        using namespace KeyDown;
        // Check for pressing ESC. Note the engine_ member variable for convenience access to the Engine object
        int key = eventData[P_KEY].GetInt();
        if (key == KEY_ESCAPE)
			engine_->Exit();
    }

	virtual void SubscribeToEvents() 
	{
		// Called after engine initialization. Setup application & subscribe to events here
		SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(MyApp, HandleKeyDown));
		SubscribeToEvent(E_NODECOLLISION, URHO3D_HANDLER(MyApp, HandleCollision));
		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MyApp, HandleUpdate));
	}
};
URHO3D_DEFINE_APPLICATION_MAIN(MyApp)

