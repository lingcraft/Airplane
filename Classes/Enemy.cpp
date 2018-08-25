#include "Enemy.h"

float Enemy::e_speedIncrement = 0;

Enemy::Enemy()
{
	e_type = Small;
	e_speed = 0;
	e_hp = 0;
	e_score = 0;
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
		e_score = DESTROY_SAMLL_SCORE;
		break;
	case Middle:
		Sprite::initWithSpriteFrameName("enemy2.png");
		e_hp = MIDDLE_ENEMY_HP;
		e_speed = MIDDLE_ENEMY_SPEED;
		e_score = DESTROY_MIDDLE_SCORE;
		break;
	case Big:
		{
			Sprite::initWithSpriteFrameName("enemy3_n1.png");
			e_hp = BIG_ENEMY_HP;
			e_speed = BIG_ENEMY_SPEED;
			e_score = DESTROY_BIG_SCORE;
			//大飞机飞行动画
			auto flyAnimation = Animation::create();
			flyAnimation = AnimationCache::getInstance()->getAnimation("big_enemy_fly");
			auto flyAnimate = Animate::create(flyAnimation);
			this->runAction(flyAnimate);
		}
		break;
	default:
		break;
	}

	return true;
}

void Enemy::hitAM()
{
	auto animation = Animation::create();
	switch (e_type)
	{
	case Middle:
		animation = AnimationCache::getInstance()->getAnimation("middle_enemy_hit");
		break;
	case Big:
		animation = AnimationCache::getInstance()->getAnimation("big_enemy_hit");
		break;
	default:
		break;
	}
	animation->setDelayPerUnit(ENEMY_HIT_DELAY);
	auto animate = Animate::create(animation);
	this->runAction(animate);
}

void Enemy::destroyAM()
{
	auto animation = Animation::create();
	switch (e_type)
	{
	case Small:
		SimpleAudioEngine::getInstance()->playEffect("enemy1_down.mp3");
		animation = AnimationCache::getInstance()->getAnimation("small_enemy_destroy");
		break;
	case Middle:
		SimpleAudioEngine::getInstance()->playEffect("enemy2_down.mp3");
		animation = AnimationCache::getInstance()->getAnimation("middle_enemy_destroy");
		break;
	case Big:
		SimpleAudioEngine::getInstance()->playEffect("enemy3_down.mp3");
		animation = AnimationCache::getInstance()->getAnimation("big_enemy_destroy");
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
		this->stopAllActions();
		this->runAction(Sequence::create(animate, callFunc, NULL));
	}
	else
	{
		this->runAction(animate);
	}
}

int Enemy::getScore()
{
	return e_score;
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

void Enemy::setHP(int hp)
{
	e_hp = hp;
}

void Enemy::speedUp()
{
	if (Enemy::e_speedIncrement >= 10)
	{
		return;
	}
	Enemy::e_speedIncrement += SPEED_INCREMENT;
}

float Enemy::getSpeed()
{
	return e_speed + e_speedIncrement;
}

void Enemy::clearSpeedIncrement()
{
	Enemy::e_speedIncrement = 0;
}