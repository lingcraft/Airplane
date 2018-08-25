#pragma once

#include "Definitions.h"

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