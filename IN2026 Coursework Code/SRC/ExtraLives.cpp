#include "GameWorld.h"
#include "ExtraLives.h"
#include "BoundingSphere.h"

// constructor - standard mode TTL 15000
ExtraLives::ExtraLives() : GameObject("ExtraLives"), mTimeToLive(15000) {}

ExtraLives::~ExtraLives(void) {}


bool ExtraLives::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Spaceship")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}
// this will also flag for removal
void ExtraLives::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}

