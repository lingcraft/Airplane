#include "Enemy.h"

Enemy::Enemy()
{
	this->e_type = Small;
	this->e_hp = 0;
}

Enemy* Enemy::create(EnemyType type)
{
	auto enemy = new Enemy();
	if (enemy && enemy->init(type))
	{
		enemy->autorelease();
		return enemy;
	}
	delete enemy;
	enemy = NULL;
	return NULL;
}

bool Enemy::init(EnemyType type)
{
	this->e_type = type;
	
	switch (type)
	{
	case Small:
		this->e_hp = SMALL_ENEMY_HP;
		this->e_speed = SMALL_ENEMY_SPEED;
		Sprite::initWithSpriteFrameName("enemy1.png");
		break;
	case Middle:
		this->e_hp = MIDDLE_ENEMY_HP;
		this->e_speed = MIDDLE_ENEMY_SPEED;
		Sprite::initWithSpriteFrameName("enemy2.png");
		break;
	case Big:
		this->e_hp = BIG_ENEMY_HP;
		this->e_speed = BIG_ENEMY_SPEED;
		Sprite::initWithSpriteFrameName("enemy3_n1.png");
		break;
	default:
		break;
	}
	return true;
}

void Enemy::hit()
{
	auto animation = Animation::create();

	switch (this->e_type)
	{
	case Middle:
		animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy2_hit.png"));
		animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy2.png"));
		break;
	case Big:
		animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_hit.png"));
		animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_n1.png"));
		break;
	default:
		break;
	}
	animation->setDelayPerUnit(ENEMY_HIT_DELAY);
	auto animate = Animate::create(animation);
	this->runAction(animate);
}

void Enemy::destroy()
{
	auto animation = Animation::create();
	switch (this->e_type)
	{
	case Small:
		for (int i = 0; i < 4; i++)
		{
			auto png = StringUtils::format("enemy1_down%d.png", i + 1);
			animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(png));
		}
		break;
	case Middle:
		for (int i = 0; i < 4; i++)
		{
			auto png = StringUtils::format("enemy2_down%d.png", i + 1);
			animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(png));
		}
		break;
	case Big:
		for (int i = 0; i < 4; i++)
		{
			auto png = StringUtils::format("enemy3_down%d.png", i + 1);
			animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(png));
		}
		break;
	default:
		break;
	}
	animation->setDelayPerUnit(ENEMY_DESTROY_DELAY);
	auto  animate = Animate::create(animation);

	auto callFuncN = CallFuncN::create([](Node* node)
	{
		node->removeFromParentAndCleanup(true);
	});

	this->runAction(Sequence::create(animate, callFuncN, NULL));

}

int Enemy::getScore()
{
	switch (this->e_type)
	{
	case Small:
		return DESTROY_SAMLL_SCORE;
		break;
	case Middle:
		return DESTROY_MIDDLE_SCORE;
		break;
	case Big:
		return DESTROY_BIG_SCORE;
		break;
	default:
		return 0;
		break;
	}
}

bool Enemy::isDestroy()
{
	return e_hp <= 0;
}

int Enemy::getHP()
{
	return this->e_hp;
}

void Enemy::loseHP(int loss)
{
	this->e_hp -= loss;
}

float Enemy::getSpeed()
{
	return this->e_speed;
}