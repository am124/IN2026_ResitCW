#ifndef __EXTRALIVES_H__
#define __EXTRALIVES_H__

#include "GameObject.h"
#include "GameUtil.h"

class ExtraLives : public GameObject
{
public:
    ExtraLives();
	virtual ~ExtraLives(void);

	void SetTimeToLive(int ttl) { mTimeToLive = ttl; }
	int GetTimeToLive(void) { return mTimeToLive; }

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);


protected:
	int mTimeToLive;
};

#endif
