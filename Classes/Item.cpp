#include "Item.h"

Item::Item()
{
	i_type = BulletLevelUp;
}

Item* Item::create(ItemTpye type)
{
	auto item = new Item();
	if (item && item->init(type))
	{
		item->autorelease();
		return item;
	}

	delete item;
	item = NULL;
	return NULL;
}

bool Item::init(ItemTpye type)
{
	i_type = type;

	switch(type)
	{
	case BulletLevelUp:
		Sprite::initWithSpriteFrameName("ufo1.png");
		return true;
		break;
	case Bomb:
		Sprite::initWithSpriteFrameName("ufo2.png");
		return true;
		break;
	default:
		break;
	}

	return false;
}

ItemTpye Item::getType()
{
	return i_type;
}