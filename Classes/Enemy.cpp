#include "Enemy.h"

Enemy::Enemy()
{
	e_type = Small;
	e_speed = 0;
	e_hp = 0;
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
	e_type = type;
	
	switch (type)
	{
	case Small:
		Sprite::initWithSpriteFrameName("enemy1.png");
		e_hp = SMALL_ENEMY_HP;
		e_speed = SMALL_ENEMY_SPEED;

		return true;
		break;
	case Middle:
		Sprite::initWithSpriteFrameName("enemy2.png");
		e_hp = MIDDLE_ENEMY_HP;
		e_speed = MIDDLE_ENEMY_SPEED;

		return true;
		break;
	case Big:
		{
			Sprite::initWithSpriteFrameName("enemy3_n1.png");
			e_hp = BIG_ENEMY_HP;
			e_speed = BIG_ENEMY_SPEED;

			//大飞机飞行动画
			auto flyAnimation = Animation::create();
			flyAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_n1.png"));			
			flyAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_n2.png"));
			flyAnimation->setDelayPerUnit(BIG_ENEMY_FLY_DELAY);
			flyAnimation->setLoops(REPEAT_FOREVER);
			auto flyAnimate = Animate::create(flyAnimation);
			this->runAction(flyAnimate);
		}
		return true;
		break;
	default:
		break;
	}

	return false;
}

void Enemy::hit()
{
	auto animation = Animation::create();

	switch (e_type)
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
	switch (e_type)
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
	
	auto callFunc = CallFuncN::create([=](Node* node)
	{
		node->removeFromParentAndCleanup(true);
	});
	
	if (isDestroy())
	{
		this->runAction(Sequence::create(animate, callFunc, NULL));
	}
	else
	{
		this->runAction(animate);
	}
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
	return e_hp;
}

void Enemy::loseHP(int loss)
{
	if (loss > e_hp)
	{
		e_hp = 0;
	}
	else
	{
		e_hp -= loss;
	}
}

float Enemy::getSpeed()
{
	return e_speed;
}

void Enemy::setHP(int hp)
{
	e_hp = hp;
}