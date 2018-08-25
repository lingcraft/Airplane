#pragma  once

#include "Definitions.h"
#include "GameScene.h"

class GameOverScene : public Layer
{
public:
	static Scene* createScene(int score);
	static GameOverScene* create(int score);
	bool init(int score);
};