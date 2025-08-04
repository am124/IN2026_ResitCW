
#include "GameWorld.h"
#include "Invincible.h"
#include "BoundingSphere.h"

Invincible::Invincible() : GameObject("Invincible") {}

Invincible::~Invincible(void) {}

bool Invincible::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Spaceship")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}
// this will also flag for removal
void Invincible::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());

}