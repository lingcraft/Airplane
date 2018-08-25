#include "Bullet.h"

Bullet::Bullet()
{
	b_type = Onefold;
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
	b_type = type;

	switch(type)
	{
	case Onefold:
		Sprite::initWithSpriteFrameName("bullet1.png");
		b_attack = BULLET1_ATTACK;
		b_speed = BULLETS_SPEED;
		break;
	case Twofold:
	case Multifold:
		Sprite::initWithSpriteFrameName("bullet2.png");
		b_attack = BULLET2_ATTACK;
		b_speed = BULLETS_SPEED;
		break;
	default:
		break;
	}

	return true;
}

int Bullet::getATK()
{
	return b_attack;
}

float Bullet::getSpeed()
{
	return b_speed;
}