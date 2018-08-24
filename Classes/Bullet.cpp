#include "Bullet.h"

Bullet::Bullet()
{
	this->b_type = Onefold;
}

Bullet* Bullet::create(BulletType type)
{
	auto bullet = new Bullet();
	if (bullet && bullet->init(type))
	{
		//放入自动释放池
		bullet->autorelease();
		return bullet;
	}

	//删除分配空间
	delete bullet;
	bullet = NULL;
	return NULL;
}

bool Bullet::init(BulletType type)
{
	this->b_type = type;

	switch(type)
	{
	case Onefold:
		Sprite::initWithSpriteFrameName("bullet1.png");
		this->b_attack = BULLET1_ATTACK;
		this->b_speed = BULLETS_SPEED;
		return true;
		break;
	case Twofold:
	case Multifold:
		Sprite::initWithSpriteFrameName("bullet2.png");
		this->b_attack = BULLET2_ATTACK;
		this->b_speed = BULLETS_SPEED;
		return true;
		break;
	default:
		break;
	}
	return false;
}

int Bullet::getATK()
{
	return this->b_attack;
}

float Bullet::getSpeed()
{
	return this->b_speed;
}