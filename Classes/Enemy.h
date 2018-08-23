#pragma once

#include "cocos2d.h"
#include "Definitions.h"
USING_NS_CC;
using namespace std;

enum EnemyType
{
	Small,
	Middle,
	Big
};

class Enemy : public Sprite
{
public:
	Enemy();
	static Enemy* create(EnemyType);
	bool init(EnemyType);

	EnemyType e_type;
	float e_speed;
	int e_hp;

	void hit();
	void destroy();
	int getScore();
	bool isDestroy();

};
