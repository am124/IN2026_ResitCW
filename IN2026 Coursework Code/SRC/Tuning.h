#pragma once

#ifndef __TUNING_H__
#define __TUNING_H__

#include "GameObject.h"
#include "GameUtil.h"

class Tuning : public GameObject
{
public: 
	Tuning();
	virtual ~Tuning(void);


	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);



};
#endif