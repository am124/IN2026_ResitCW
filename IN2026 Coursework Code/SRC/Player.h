#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "GameUtil.h"

#include "GameObject.h"
#include "GameObjectType.h"
#include "IPlayerListener.h"
#include "IGameWorldListener.h"

// CUSTOM
#include "ExtraLives.h"


// END

class Player : public IGameWorldListener
{
public:
	Player() { mLives = 3; }
	virtual ~Player() {}

	void OnWorldUpdated(GameWorld* world) {}

	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}

	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
	{
		if (object->GetType() == GameObjectType("Spaceship")) {
			if (!mIsInvincible) {
				mLives -= 1;
				FirePlayerKilled();
			}
			
		}
		// CUSTOM
		if (object->GetType() == GameObjectType("ExtraLives")) {
			mLives += 1;
			FireLivesPickedUp();
		}
		
		// END
	}

	void AddListener(shared_ptr<IPlayerListener> listener)
	{
		mListeners.push_back(listener);
	}

	void FirePlayerKilled()
	{
		// Send message to all listeners
		for (PlayerListenerList::iterator lit = mListeners.begin();
			lit != mListeners.end(); ++lit) {
			(*lit)->OnPlayerKilled(mLives);
		}
	}
	// CUSTOM 
	void FireLivesPickedUp() {
		// Send message to all listeners
		for (PlayerListenerList::iterator lit = mListeners.begin();
			lit != mListeners.end(); ++lit) {
			(*lit)->OnPlayerPickedUpLife(mLives);
		}
	}
	void EnableInvincibility() { 
		mIsInvincible = true;
	}
	void DisableInvincibility() {
		mIsInvincible = false;
	}
	bool IsInvincible() const { return mIsInvincible; }
	// END

private:
	int mLives;
	// CUSTOM
	bool mIsInvincible = false; 
	//int InvincibleDuration = 10000; 

	// END 

	typedef std::list< shared_ptr<IPlayerListener> > PlayerListenerList;

	PlayerListenerList mListeners;
};

#endif
