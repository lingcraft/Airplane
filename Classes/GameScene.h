#pragma once

#include "cocos2d.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Item.h"

USING_NS_CC;
using namespace std;

class GameScene : public Layer
{
public:
	GameScene();
	~GameScene();
	static Scene* createScene();
	CREATE_FUNC(GameScene);
	virtual bool init();
	void update(float);
	void pauseAndResume(Ref*);
	void bomb(Ref*);

	void createBullet(float);
	void createSmallEnemmy(float);
	void createBigEnemy(float);
	void createBossEnemy(float);
	void createEnemy(EnemyType);
	void createItems(float);
	void updateBombCount();

private:

	Point m_correctVector;
	Vector<Bullet*> m_vBullets;
	Vector<Enemy*> m_vEnemies;
	Vector<Item*> m_vItems;
	int m_totalScore;
	bool m_isDoubleBullet;
	int m_doubleBulletNumber;
	int m_bombCount;

};