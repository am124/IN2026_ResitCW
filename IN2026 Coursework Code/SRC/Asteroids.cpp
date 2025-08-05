#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"

#include "ExtraLives.h"
#include "Invincible.h"
#include "Tuning.h"



// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char *argv[])
	: GameSession(argc, argv)
{
	mLevel = 0;
	mAsteroidCount = 0;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	Animation *explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation *asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation *spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");

	Animation* heart_anim = AnimationManager::GetInstance().CreateAnimationFromFile("heart", 512, 512, 512, 512, "heart.png");
	Animation* shield_anim = AnimationManager::GetInstance().CreateAnimationFromFile("shield", 512, 512, 512, 512, "shield.png");
	Animation* gear_anim = AnimationManager::GetInstance().CreateAnimationFromFile("gear", 512, 512, 512, 512, "gear.png");



	// CUSTOM 
	
	// END
	// Create a spaceship and add it to the world
	//mGameWorld->AddObject(CreateSpaceship());
	// Create some asteroids and add them to the world
	CreateAsteroids(10);
	
	CreateStartGUI();
	// END
	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);

	// Start the game
	GameSession::Start();
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	if (mScoreSaveScreen) {
		switch (key)
		{
		case '1':
			mScoreSaveLabel->SetVisible(false);
			mCancelScoreSaveLabel -> SetVisible(false);
			CreateAskNameGUI();
			mScoreSaveScreen = false;
			return;
		case '2':
			mScoreSaveLabel->SetVisible(false);
			mCancelScoreSaveLabel->SetVisible(false);
			SetTimer(500, SHOW_GAME_OVER);
			mScoreSaveScreen = false;
			return;
		}
	}
	else if (mDifficultyScreen) {
		switch (key) {
		case '1':
			mIsHard = false;
			mCurrentDifficultyLabel->SetText("Current: Standard");
			break;
		case '2':
			mIsHard = true;
			mCurrentDifficultyLabel->SetText("Current: Hard");
			break;
		case 'b':
			HideDifficultyGUI();
			ShowStartGUI();
			break;
		}
		return;
	}

	else if (mNameScreen) {
		if (key == 13) { // ENTER key
			// Save name + score
			SaveHighScore(mNameInput, mCurrentScore);
			mNameInput = "";
			mNameScreen = false;
			mEnterHintLabel->SetVisible(false);
			SetTimer(500, SHOW_GAME_OVER);
		}
		else if (key == 8 && !mNameInput.empty()) { // BACKSPACE
			mNameInput.pop_back();
		}
		else if (isprint(key)) {
			mNameInput += key;
		}

		// Update label with current name input
		mNameLabel->SetText(mNameInput);
		return;
	}
	else if (mHighScoresScreen) {
		if (key == 'b') {
			HideHighScoresGUI();
			ShowStartGUI();
		}
	}

	else if (mGameStart == true) {
		switch (key)
		{
		case ' ':
			mSpaceship->Shoot();
			break;
		default:
			break;
		}
	}
	else if (!mGameStart && mInstructionsScreen) {
		switch (key)
		{
		case 'b':
			HideInstructionsGUI();
			ShowStartGUI();
			mInstructionsScreen = false;

			
		}
	}
	else {
		StartControls(key);
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	if (mGameStart == true) {
		// CUSTOM
		if (key == GLUT_KEY_DOWN && mTuningPickedUp) {
			mSpaceship->Thrust(-12);
		}
		// END 
		switch (key)
		{
			// If up arrow key is pressed start applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
			// If left arrow key is pressed start rotating anti-clockwise
		case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
			// If right arrow key is pressed start rotating clockwise
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
			// Default case - do nothing
		default: break;
		}
	}
	
	
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	if (mGameStart == true) {
		// CUSTOM
		if (key == GLUT_KEY_DOWN && mTuningPickedUp) {
			mSpaceship->Thrust(0);
		}
		// END 
		switch (key)
		{
			// If up arrow key is released stop applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
			// If left arrow key is released stop rotating
		case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
			// If right arrow key is released stop rotating
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
			// Default case - do nothing
		default: break;
		}
	}
	
	
	
}


// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("Asteroid"))
	{
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;
		if (mAsteroidCount <= 0) 
		{ 
			SetTimer(500, START_NEXT_LEVEL); 
		}
	}
	if (object->GetType() == GameObjectType("Invincible"))
	{
		mPlayer.EnableInvincibility();
		SetTimer(5000, END_INVINCIBILITY);
	}
	if (object->GetType() == GameObjectType("Tuning")){
		mTuningPickedUp = true; 
	}

}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}

	if (value == START_NEXT_LEVEL)
	{
		mLevel++;
		int num_asteroids = 10 + 2 * mLevel;
		CreateAsteroids(num_asteroids);
	}

	if (value == SHOW_GAME_OVER)
	{
		mGameOverLabel->SetVisible(true);
	}
	// CUSTOM
	if (value == SPAWN_EXTRA_LIFE) {
		// when game over stop spawning extras 
		if (mGameStart == true) {
			SpawnExtraLife();
			// schedules next spawn
			SetTimer(28000, SPAWN_EXTRA_LIFE);
		}
	
	}
	if (value == SPAWN_INVINCIBLE) {
		if (mGameStart == true) {
			SpawnInvincible();
			SetTimer(20000, SPAWN_INVINCIBLE);
		}
	

	}
	if (value == END_INVINCIBILITY) {
		mPlayer.DisableInvincibility();
	}
	if (value == SPAWN_TUNING) {
		if (mGameStart == true) {
			SpawnTuning();
		}
		
	}
	
	// END 
}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	// CUSTOM
	mSpaceship->SetPlayer(&mPlayer);
	// END
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mSpaceship;

}

void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		mGameWorld->AddObject(asteroid);
	}
}

void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component
		= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> game_over_component
		= static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));

}

void Asteroids::OnScoreChanged(int score)
{
	// CUSTOM
	// track the current score
	mCurrentScore = score;
	// END 
	 
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);

}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

	if (lives_left > 0) 
	{ 
		SetTimer(1000, CREATE_NEW_PLAYER); 
	}
	else
	{
		CreateScoreSaveGUI();
		mGameStart = false();
		
	}
}

shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}

// CUSTOM
void Asteroids::CreateStartGUI()
{
	mHeaderLabel = make_shared<GUILabel>("Asteroids Game");
	mHeaderLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mHeaderLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> header_component
		= static_pointer_cast<GUIComponent>(mHeaderLabel);
	mGameDisplay->GetContainer()->AddComponent(header_component, GLVector2f(0.5f, 0.93f));

	mStartLabel = make_shared<GUILabel>("Press 1 to Start");
	mStartLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mStartLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> start_component
		= static_pointer_cast<GUIComponent>(mStartLabel);
	mGameDisplay->GetContainer()->AddComponent(start_component, GLVector2f(0.5f, 0.8f));

	mDifficultyLabel = make_shared<GUILabel>("Press 2 to Change Difficulty");
	mDifficultyLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mDifficultyLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> difficulty_component
		= static_pointer_cast<GUIComponent>(mDifficultyLabel);
	mGameDisplay->GetContainer()->AddComponent(difficulty_component, GLVector2f(0.5f, 0.6f));

	mInstructionsLabel = make_shared<GUILabel>("Press 3 to View Instructions");
	mInstructionsLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mInstructionsLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> instructions_component
		= static_pointer_cast<GUIComponent>(mInstructionsLabel);
	mGameDisplay->GetContainer()->AddComponent(instructions_component, GLVector2f(0.5f, 0.4f));

	mTableLabel = make_shared<GUILabel>("Press 4 to View Scores");
	mTableLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mTableLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> table_component
		= static_pointer_cast<GUIComponent>(mTableLabel);
	mGameDisplay->GetContainer()->AddComponent(table_component, GLVector2f(0.5f, 0.2f));
}

void Asteroids::HideDefaultGUI() 
{
	mScoreLabel->SetVisible(false);
	mLivesLabel->SetVisible(false);	
}
void Asteroids::HideStartGUI()
{
	mHeaderLabel->SetVisible(false);
	mStartLabel->SetVisible(false);
	mDifficultyLabel->SetVisible(false);
	mInstructionsLabel->SetVisible(false);
	mTableLabel->SetVisible(false);
}
void Asteroids::ShowDefaultGUI()
{
	mScoreLabel->SetVisible(true);
	mLivesLabel->SetVisible(true);
}
void Asteroids::ShowStartGUI()
{
	mHeaderLabel->SetVisible(true);
	mStartLabel->SetVisible(true);
	mDifficultyLabel->SetVisible(true);
	mInstructionsLabel->SetVisible(true);
	mTableLabel->SetVisible(true);
}
void Asteroids::CreateInstructionsGUI() {

	mInstruction1 = make_shared<GUILabel>("Arrows Keys To Navigate Spaceship");
	mInstruction1->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mInstruction1->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> instruction1_component
		= static_pointer_cast<GUIComponent>(mInstruction1);
	mGameDisplay->GetContainer()->AddComponent(instruction1_component, GLVector2f(0.5f, 0.8f));

	mInstruction2 = make_shared<GUILabel>("Spacebar to Shoot Spaceship");
	mInstruction2->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mInstruction2->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> instruction2_component
		= static_pointer_cast<GUIComponent>(mInstruction2);
	mGameDisplay->GetContainer()->AddComponent(instruction2_component, GLVector2f(0.5f, 0.7f));

	mInstruction3 = make_shared<GUILabel>("Destory The Asteroids");
	mInstruction3->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mInstruction3->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> instruction3_component
		= static_pointer_cast<GUIComponent>(mInstruction3);
	mGameDisplay->GetContainer()->AddComponent(instruction3_component, GLVector2f(0.5f, 0.6f));

	mInstruction4 = make_shared<GUILabel>("Save Your Score At The End (Name)");
	mInstruction4->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mInstruction4->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> instruction4_component
		= static_pointer_cast<GUIComponent>(mInstruction4);
	mGameDisplay->GetContainer()->AddComponent(instruction4_component, GLVector2f(0.5f, 0.5f));

	mInstruction5 = make_shared<GUILabel>("View Saved Scores From The Start Menu");
	mInstruction5->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mInstruction5->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> instruction5_component
		= static_pointer_cast<GUIComponent>(mInstruction5);
	mGameDisplay->GetContainer()->AddComponent(instruction5_component, GLVector2f(0.5f, 0.4f));

	mInstruction6 = make_shared<GUILabel>("Change Difficulty From The Start Menu");
	mInstruction6->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mInstruction6->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> instruction6_component
		= static_pointer_cast<GUIComponent>(mInstruction6);
	mGameDisplay->GetContainer()->AddComponent(instruction6_component, GLVector2f(0.5f, 0.3f));

	mInstruction7 = make_shared<GUILabel>("Press b to Return");
	mInstruction7->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mInstruction7->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> instruction7_component
		= static_pointer_cast<GUIComponent>(mInstruction7);
	mGameDisplay->GetContainer()->AddComponent(instruction7_component, GLVector2f(0.5f, 0.2f));
}

void Asteroids::HideInstructionsGUI() {
	mInstruction1->SetVisible(false);
	mInstruction2->SetVisible(false);
	mInstruction3->SetVisible(false);
	mInstruction4->SetVisible(false);
	mInstruction5->SetVisible(false);
	mInstruction6->SetVisible(false);
	mInstruction7->SetVisible(false);
}
void Asteroids::ShowInstructionsGUI() {
	mInstruction1->SetVisible(true);
	mInstruction2->SetVisible(true);
	mInstruction3->SetVisible(true);
	mInstruction4->SetVisible(true);
	mInstruction5->SetVisible(true);
	mInstruction6->SetVisible(true);
	mInstruction7->SetVisible(true);
}

void Asteroids::StartControls(int key)
{
	switch (key)
	{
	case '1':
		HideStartGUI();
		CreateGUI();
		mGameStart = true;
		// CUSTOM
		// Create a spaceship and add it to the world
		mGameWorld->AddObject(CreateSpaceship());
		// spawns first life, here use a boolean for difficulty to change timer
		if (!mIsHard) {
			SetTimer(28000, SPAWN_EXTRA_LIFE);
			SetTimer(20000, SPAWN_INVINCIBLE);
			SetTimer(16000, SPAWN_TUNING);
		}
		break;
	case '2':
		HideStartGUI();
		CreateDifficultyGUI();
		mDifficultyScreen = true;
		break;
	case '3':
		// optimisation
		if (!mInstructionsCreated) {
			CreateInstructionsGUI();
			mInstructionsCreated = true;
		}
		else {
			ShowInstructionsGUI();
		}
		HideStartGUI();
		mInstructionsScreen = true;
		break;
	case '4':
		HideStartGUI();
		ShowHighScoresGUI();
		break;
	default:
		break;
	}
}
void Asteroids::GameControls(int key) {

}
void Asteroids::CreateScoreSaveGUI() {
	mScoreSaveScreen = true;
	mScoreSaveLabel = make_shared<GUILabel>("Press 1 to Save Score");
	mScoreSaveLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mScoreSaveLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> save_component
		= static_pointer_cast<GUIComponent>(mScoreSaveLabel);
	mGameDisplay->GetContainer()->AddComponent(save_component, GLVector2f(0.5f, 0.6f));

	mCancelScoreSaveLabel = make_shared<GUILabel>("(Press 2 to Discard Score)");
	mCancelScoreSaveLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mCancelScoreSaveLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> cancel_component
		= static_pointer_cast<GUIComponent>(mCancelScoreSaveLabel);
	mGameDisplay->GetContainer()->AddComponent(cancel_component, GLVector2f(0.5f, 0.4f));
}

void Asteroids::CreateAskNameGUI() {
	mGameStart = false;
	mNameScreen = true;
	mScoreSaveScreen = false;
	// name input displays (string declared in header) 
	mNameInput = "";


	mNameLabel = make_shared<GUILabel>("Provide Your Name");
	mNameLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mNameLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> name_component
		= static_pointer_cast<GUIComponent>(mNameLabel);
	mGameDisplay->GetContainer()->AddComponent(name_component, GLVector2f(0.5f, 0.6f));

	mEnterHintLabel = make_shared<GUILabel>("(Press Enter After Providing Name)");
	mEnterHintLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mEnterHintLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	shared_ptr<GUIComponent> enter_component
		= static_pointer_cast<GUIComponent>(mEnterHintLabel);
	mGameDisplay->GetContainer()->AddComponent(enter_component, GLVector2f(0.5f, 0.4f));

}
void Asteroids::SaveHighScore(const std::string& name, int score)
{
	std::ofstream file("highscores.txt", std::ios::app); 

	if (file.is_open()) {
		file << name << " " << score << std::endl;
		file.close();
	}
	else {
		// doesnt print due to setup but still implement good practise 
		std::cerr << "Error: Could not open highscores.txt for writing." << std::endl;
	}
}
void Asteroids::ShowHighScoresGUI() {
	mHighScoresScreen = true;

	std::ifstream file("highscores.txt");
	if (!file.is_open()) {
		std::cerr << "Could not open highscores.txt" << std::endl;
		return;
	}

	// header 
	mHighScoresHeader = make_shared<GUILabel>("High Scores");
	mHighScoresHeader->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mHighScoresHeader->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mGameDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(mHighScoresHeader), GLVector2f(0.5f, 0.9f)
	);

	// read lines
	std::string line;
	int index = 0;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string name;
		int score;
		// restart the loop if potentially malformed (correct row order 
		// not maintained)
		if (!(iss >> name >> score)) continue;

		std::ostringstream labelText;
		labelText << index + 1 << ". " << name << " - " << score;

		auto label = make_shared<GUILabel>(labelText.str());
		label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		label->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);

		mGameDisplay->GetContainer()->AddComponent(
			static_pointer_cast<GUIComponent>(label), GLVector2f(0.5f, 0.8f - index * 0.05f)
		);

		mScoreLabels.push_back(label);
		index++;
		if (index >= 10) break; // Show top 10
	}

	
	mBackHintLabel = make_shared<GUILabel>("Press b to Return");
	mBackHintLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mBackHintLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	mGameDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(mBackHintLabel), GLVector2f(0.5f, 0.05f)
	);

}
void Asteroids::HideHighScoresGUI() {
	mHighScoresScreen = false;

	for (auto& label : mScoreLabels) {
		label->SetVisible(false);
	}
	mScoreLabels.clear();

	if (mHighScoresHeader) mHighScoresHeader->SetVisible(false);
	if (mBackHintLabel) mBackHintLabel->SetVisible(false);
}

void Asteroids::CreateDifficultyGUI() {
	mDifficultyScreen = true;

	mDifficultyHeaderLabel = make_shared<GUILabel>("Select Difficulty:");
	mDifficultyHeaderLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mDifficultyHeaderLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mGameDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(mDifficultyHeaderLabel), GLVector2f(0.5f, 0.8f));

	mStandardOptionLabel = make_shared<GUILabel>("Press 1 for Standard");
	mStandardOptionLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mStandardOptionLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mGameDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(mStandardOptionLabel), GLVector2f(0.5f, 0.6f));

	mHardOptionLabel = make_shared<GUILabel>("Press 2 for Hard");
	mHardOptionLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mHardOptionLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mGameDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(mHardOptionLabel), GLVector2f(0.5f, 0.5f));

	std::string currentLabelText;
	if (mIsHard) {
		currentLabelText = "Current: Hard";
	}
	else {
		currentLabelText = "Current: Standard";
	}
	mCurrentDifficultyLabel = make_shared<GUILabel>(currentLabelText);
	mCurrentDifficultyLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mCurrentDifficultyLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mGameDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(mCurrentDifficultyLabel), GLVector2f(0.5f, 0.3f));

	mTipLabel = make_shared<GUILabel>("(Hard mode disables powerups)");
	mTipLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mTipLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mGameDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(mTipLabel), GLVector2f(0.5f, 0.2f));

	mBackLabelDifficulty = make_shared<GUILabel>("Press b to Return");
	mBackLabelDifficulty->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mBackLabelDifficulty->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mGameDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(mBackLabelDifficulty), GLVector2f(0.5f, 0.1f));

}
void Asteroids::HideDifficultyGUI() {
	mDifficultyHeaderLabel->SetVisible(false);
	mStandardOptionLabel->SetVisible(false);
	mHardOptionLabel->SetVisible(false);
	mCurrentDifficultyLabel->SetVisible(false);
	mBackLabelDifficulty->SetVisible(false);
	mTipLabel->SetVisible(false);
	mDifficultyScreen = false;
}

void Asteroids::OnPlayerPickedUpLife(int lives_left)
{
	

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

}
void Asteroids::SpawnExtraLife()
{
	// Create the pickup
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("heart");
	shared_ptr<Sprite> extralife_sprite
		= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	extralife_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> extralife = make_shared<ExtraLives>();
	extralife->SetBoundingShape(make_shared<BoundingSphere>(extralife->GetThisPtr(), 10.0f));
	extralife->SetSprite(extralife_sprite);
	extralife->SetScale(0.02);
	GLfloat x = rand() % 200 - 100; // example range: [-100, 100]
	GLfloat y = rand() % 200 - 100;
	extralife->SetPosition(GLVector3f(x, y, 0));;
	extralife->SetRotation(30);
	mGameWorld->AddObject(extralife);
}
void Asteroids::SpawnInvincible() {
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("shield");
	shared_ptr<Sprite> invincible_sprite
		= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	invincible_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> invincible = make_shared<Invincible>();
	invincible->SetBoundingShape(make_shared<BoundingSphere>(invincible->GetThisPtr(), 10.0f));
	invincible->SetSprite(invincible_sprite);
	invincible->SetScale(0.02);
	GLfloat x = rand() % 200 - 100; // example range: [-100, 100]
	GLfloat y = rand() % 200 - 100;
	invincible->SetPosition(GLVector3f(x, y, 0));;
	invincible->SetRotation(30);
	mGameWorld->AddObject(invincible);

}
void Asteroids::SpawnTuning() {
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("gear");
	shared_ptr<Sprite> tuning_sprite
		= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	tuning_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> tuning = make_shared<Tuning>();
	tuning->SetBoundingShape(make_shared<BoundingSphere>(tuning->GetThisPtr(), 10.0f));
	tuning->SetSprite(tuning_sprite);
	tuning->SetScale(0.02);
	GLfloat x = rand() % 200 - 100; // example range: [-100, 100]
	GLfloat y = rand() % 200 - 100;
	tuning->SetPosition(GLVector3f(x, y, 0));;
	tuning->SetRotation(30);
	mGameWorld->AddObject(tuning);
}


// END 




