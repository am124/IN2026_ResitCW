#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h"
#include "IGameWorldListener.h"
#include "IScoreListener.h" 
#include "ScoreKeeper.h"
#include "Player.h"
#include "IPlayerListener.h"
#include <vector>



class GameObject;
class Spaceship;
class GUILabel;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener
{
public:
	Asteroids(int argc, char *argv[]);
	virtual ~Asteroids(void);

	virtual void Start(void);
	virtual void Stop(void);

	// Declaration of IKeyboardListener interface ////////////////////////////////

	void OnKeyPressed(uchar key, int x, int y);
	void OnKeyReleased(uchar key, int x, int y);
	void OnSpecialKeyPressed(int key, int x, int y);
	void OnSpecialKeyReleased(int key, int x, int y);

	// Declaration of IScoreListener interface //////////////////////////////////

	void OnScoreChanged(int score);

	// Declaration of the IPlayerLister interface //////////////////////////////

	void OnPlayerKilled(int lives_left);

	// Declaration of IGameWorldListener interface //////////////////////////////

	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);

	// Override the default implementation of ITimerListener ////////////////////
	void OnTimer(int value);

	// Custom
	void OnPlayerPickedUpLife(int lives_left);

private:
	shared_ptr<Spaceship> mSpaceship;
	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mGameOverLabel;

	uint mLevel;
	uint mAsteroidCount;

	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	void CreateGUI();
	void CreateAsteroids(const uint num_asteroids);
	shared_ptr<GameObject> CreateExplosion();
	
	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;
	// CUSTOM
	const static uint SPAWN_EXTRA_LIFE = 12;
	const static uint END_INVINCIBILITY = 6;
	const static uint SPAWN_INVINCIBLE = 20;
	const static uint SPAWN_TUNING = 25;


	// END 

	ScoreKeeper mScoreKeeper;
	Player mPlayer;

	// CUSTOM
	shared_ptr<GUILabel> mHeaderLabel;
	shared_ptr<GUILabel> mStartLabel;
	shared_ptr<GUILabel> mDifficultyLabel;
	shared_ptr<GUILabel> mInstructionsLabel;
	shared_ptr<GUILabel> mTableLabel;
	shared_ptr<GUILabel> mInstruction1;
	shared_ptr<GUILabel> mInstruction2;
	shared_ptr<GUILabel> mInstruction3;
	shared_ptr<GUILabel> mInstruction4;
	shared_ptr<GUILabel> mInstruction5;
	shared_ptr<GUILabel> mInstruction6;
	shared_ptr<GUILabel> mInstruction7;

	void CreateStartGUI();
	void HideDefaultGUI();
	void HideStartGUI();
	void ShowDefaultGUI();
	void ShowStartGUI();
	void CreateInstructionsGUI();
	void HideInstructionsGUI();
	void ShowInstructionsGUI();
	void StartControls(int key);
	void GameControls(int key);
	bool mGameStart = false;
	bool mInstructionsScreen = false;
	bool mInstructionsCreated = false;

	void CreateScoreSaveGUI();
	bool mScoreSaveScreen = false;
	shared_ptr<GUILabel> mScoreSaveLabel;
	shared_ptr<GUILabel> mCancelScoreSaveLabel;

	void CreateAskNameGUI();
	std::string mNameInput;
	shared_ptr<GUILabel> mNameLabel;
	shared_ptr<GUILabel> mEnterHintLabel;
	bool mNameScreen = false;
	int mCurrentScore = 0;

	void SaveHighScore(const std::string& name, int score);

	void ShowHighScoresGUI();
	void HideHighScoresGUI();
	bool mHighScoresScreen = false;
	std::vector<shared_ptr<GUILabel>> mScoreLabels;
	shared_ptr<GUILabel> mHighScoresHeader;
	shared_ptr<GUILabel> mBackHintLabel;
	
	bool mIsHard = false; 
	bool mDifficultyScreen = false; 
	shared_ptr<GUILabel> mDifficultyHeaderLabel;
	shared_ptr<GUILabel> mStandardOptionLabel;
	shared_ptr<GUILabel> mHardOptionLabel;
	shared_ptr<GUILabel> mCurrentDifficultyLabel;
	shared_ptr<GUILabel> mBackLabelDifficulty;
	void CreateDifficultyGUI();
	void HideDifficultyGUI();

	void SpawnExtraLife();
	void SpawnInvincible();

	void SpawnTuning();
	bool mTuningPickedUp = false;


	// END

};

#endif