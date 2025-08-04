
#include "GameWorld.h"
#include "Tuning.h"
#include "BoundingSphere.h"

Tuning::Tuning() : GameObject("Tuning") {}

Tuning::~Tuning(void) {}

bool Tuning::CollisionTest(shared_ptr<GameObject> o) {
	if (o->GetType() != GameObjectType("Spaceship")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}
void Tuning::OnCollision(const GameObjectList& objects) {

	mWorld->FlagForRemoval(GetThisPtr());
}