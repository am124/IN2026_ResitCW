#pragma once

#ifndef __INVINCIBLE_H__
#define __INVINCIBLE_H__

#include "GameObject.h"
#include "GameUtil.h"

class Invincible : public GameObject
{
public:
	Invincible();
	virtual ~Invincible(void);


	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);



};

#endif

