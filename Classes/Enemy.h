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
	void hit();				//受伤动画
	void destroy();			//坠毁动画
	int getScore();			//歼灭得分
	bool isDestroy();
	int getHP();
	void loseHP(int loss);
	float getSpeed();

private:
	EnemyType e_type;
	float e_speed;
	int e_hp;
};
