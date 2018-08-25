#pragma once

#include "cocos2d.h"
#include "Definitions.h"

USING_NS_CC;
using namespace std;

enum ItemTpye
{
	BulletLevelUp,
	Bomb,
};

class Item : public Sprite
{
public:
	Item();
	static Item* create(ItemTpye);
	bool init(ItemTpye);
	ItemTpye getType();

private:
	ItemTpye i_type;
};