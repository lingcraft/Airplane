#pragma once

#include "Definitions.h"

enum BulletType
{
	Onefold,
	Twofold,
	Multifold
};

class Bullet : public Sprite
{
public:
	Bullet();
	static Bullet* create(BulletType);
	bool init(BulletType);
	int getATK();
	float getSpeed();

private:
	BulletType b_type;
	float b_speed;
	int b_attack;
};