#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Core/CoreEvents.h>

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Input/Input.h>

#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/CollisionBox2D.h>
#include <Urho3D/Urho2D/CollisionCircle2D.h>

#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>

#include "Character.h"
#include "Enemy.h"
#include "Ball.h"

using namespace Urho3D;

class MyApp : public Application
{
public:

	SharedPtr<Node> cameraNode_;
	SharedPtr<Scene> scene_;
	Ball ball_;
	WeakPtr<Character> character_;
	WeakPtr<Enemy> enemy_;
	Text* textPlayerScore_;
	Text* textEnemiScore_;
	Text* centralText_;
	int playerScore_;
	int enemyScore_;

    MyApp(Context* context) :
        Application(context)
    {
		Character::RegisterObject(context);
		Enemy::RegisterObject(context);
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
		CreateScene();
		
		CreateText();

		SetupViewport();

		SubscribeToEvents();
    }
    
	virtual void CreateScene()
	{
		scene_ = new Scene(context_);
		scene_->CreateComponent<Octree>();
		scene_->CreateComponent<DebugRenderer>();

		CreateCamera();

		CreateGroundCeil(String("Ground"), Vector3(0.0f, -3.3f, 0.0f));
		CreateGroundCeil(String("Ceil"), Vector3(0.0f, 3.3f, 0.0f));

		CreateCharacter();

		CreateEnemy();

		ball_.LoadComponent(scene_, GetSubsystem<ResourceCache>());
	}

	virtual void CreateGroundCeil(String name, Vector3 position)
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		Sprite2D* boxSprite = cache->GetResource<Sprite2D>("Urho2D/Box.png");

		Node* groundCeilNode = scene_->CreateChild(name);
		groundCeilNode->SetPosition(position);
		groundCeilNode->SetScale(Vector3(200.0f, 1.0f, 0.0f));

		// Create 2D rigid body for gound
		groundCeilNode->CreateComponent<RigidBody2D>();

		StaticSprite2D* groundCeilSprite = groundCeilNode->CreateComponent<StaticSprite2D>();
		groundCeilSprite->SetSprite(boxSprite);

		// Create box collider for ground
		CollisionBox2D* groundCeilShape = groundCeilNode->CreateComponent<CollisionBox2D>();
		// Set box size
		groundCeilShape->SetSize(Vector2(0.32f, 0.32f));

		// Set friction
		groundCeilShape->SetFriction(0.0f);
	}

	virtual void CreateText() 
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		UI* ui = GetSubsystem<UI>();

		//Create Text Score
		// Construct new Text object, set string to display and font to use
		textEnemiScore_= ui->GetRoot()->CreateChild<Text>("EnemiScore");
		textEnemiScore_->SetText("0");
		textEnemiScore_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 45);

		// Position the text relative to the screen center
		textEnemiScore_->SetHorizontalAlignment(HA_LEFT);
		textEnemiScore_->SetVerticalAlignment(VA_CENTER);
		textEnemiScore_->SetPosition(ui->GetRoot()->GetWidth() / 32, 0);

		//Create Player Score
		// Construct new Text object, set string to display and font to use
		textPlayerScore_ = ui->GetRoot()->CreateChild<Text>("PlayerScore");
		textPlayerScore_->SetText("0");
		textPlayerScore_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 45);

		// Position the text relative to the screen center
		textPlayerScore_->SetHorizontalAlignment(HA_RIGHT);
		textPlayerScore_->SetVerticalAlignment(VA_CENTER);
		textPlayerScore_->SetPosition(-ui->GetRoot()->GetWidth() / 32, 0);

		//Create Central Text Score
		centralText_ = ui->GetRoot()->CreateChild<Text>("CentralText");
		centralText_->SetText("0");
		centralText_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 45);

		centralText_->SetHorizontalAlignment(HA_CENTER);
		centralText_->SetVerticalAlignment(VA_CENTER);
		centralText_->SetPosition(0, 0);
		centralText_->SetVisible(false);

		playerScore_ = 0;
		enemyScore_ = 0;
	}

	virtual void CreateCamera() 
	{
		// Create camera node
		cameraNode_ = scene_->CreateChild("Camera");
		// Set camera's position
		cameraNode_->SetPosition(Vector3(0.0f, 0.0f, -10.0f));

		Camera* camera = cameraNode_->CreateComponent<Camera>();
		camera->SetOrthographic(true);

		Graphics* graphics = GetSubsystem<Graphics>();
		camera->SetOrthoSize((float)graphics->GetHeight() * PIXEL_SIZE);
		camera->SetZoom(1.2f * Min((float)graphics->GetWidth() / 1280.0f, (float)graphics->GetHeight() / 800.0f)); // Set zoom according to user's resolution to ensure full visibility (initial zoom (1.2) is set for full visibility at 1280x800 resolution)
	}

	virtual void CreateCharacter()
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		Sprite2D* boxSprite = cache->GetResource<Sprite2D>("Urho2D/Box.png");

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

	virtual void CreateEnemy()
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		Sprite2D* boxSprite = cache->GetResource<Sprite2D>("Urho2D/Box.png");

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
		enemy_ = enemyNode->CreateComponent<Enemy>();
	}

	virtual void WriteCentralText(String textToWrite) 
	{
		// Construct new Text object, set string to display and font to use
		centralText_->SetText(textToWrite);
		centralText_->SetVisible(true);
	}

	virtual void SetPlayerScore(int score)
	{
		textPlayerScore_->SetText(String(score));
	}

	virtual void SetEnemiScore(int score) 
	{
		textEnemiScore_->SetText(String(score));
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

		if (playerScore_ == 5) 
		{
			
			WriteCentralText("You Win");
			ball_.RemoveAllComponents();
		}

		if (enemyScore_ == 5) 
		{
			WriteCentralText("You Lose");
			ball_.RemoveAllComponents();
		}

		if (character_) 
		{
			character_->controls_.Set(CTRL_UP, input->GetKeyDown(KEY_W));
			character_->controls_.Set(CTRL_DOWN, input->GetKeyDown(KEY_S));
		}

		Vector2 ballPosition = ball_.GetPosition();

		if (enemy_)
		{
			enemy_->SetBallPosition(ballPosition);
		}

		if (ballPosition.x_ < -6) 
		{
			playerScore_++;
			SetPlayerScore(playerScore_);
			ball_.ResetBall();
		}

		if (ballPosition.x_ > 6)
		{
			enemyScore_++;
			SetEnemiScore(enemyScore_);
			ball_.ResetBall();
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
		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MyApp, HandleUpdate));
	}
};
URHO3D_DEFINE_APPLICATION_MAIN(MyApp)

