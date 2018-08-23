#include "GameScene.h"

GameScene::GameScene()
{
	m_totalScore = 0;
	m_isDoubleBullet = false;
	m_doubleBulletNumber = 0;
}

GameScene::~GameScene()
{

}

Scene* GameScene::createScene()
{
	auto scene = Scene::create();
	auto layer = GameScene::create();

	scene->addChild(layer);  //将层添加到场景里
	return scene;
}

bool GameScene::init()
{
	if (!Layer::init()) { return false; }

	//导入plist
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("shoot_background.plist");  
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("shoot.plist");

	//创建精灵
	//背景
	auto bg = Sprite::createWithSpriteFrameName("background.png"); 
	auto bg_next = Sprite::createWithSpriteFrameName("background.png");
	
	//飞机
	auto hero = Sprite::createWithSpriteFrameName("hero1.png");  

	//计分标签
	auto lbl_score = Label::createWithBMFont("num.fnt","0");
	auto lbl_stable = LabelTTF::create("Score:", "Arial", 36);

	//设置位置
	//背景
	bg->setAnchorPoint(Point::ZERO);  //锚点
	bg->setPosition(0, 0);
	bg->getTexture()->setAliasTexParameters();  //抗锯齿
	this->addChild(bg, BG_LAYER, BG_TAG);

	bg_next->setAnchorPoint(Point::ZERO);
	bg_next->setPosition(0, visSize.height);
	bg_next->getTexture()->setAliasTexParameters();
	this->addChild(bg_next, BG_LAYER, BGNEXT_TAG);
	
	//飞机
	hero->setPosition(visSize.width / 2, hero->getContentSize().height / 2);
	this->addChild(hero, PLANE_LAYER, HERO_TAG);

	//分数标签
	lbl_score->setAnchorPoint(Point::ZERO);
	lbl_score->setPosition(120, visSize.height - 63);
	this->addChild(lbl_score, SCORE_LAYER, SCORE_TAG);
	lbl_score->setColor(Color3B::BLACK);

	//固定的“得分”标签
	lbl_stable->setAnchorPoint(Point::ZERO);
	lbl_stable->setPosition(10, visSize.height - 45);
	this->addChild(lbl_stable,SCORE_LAYER, STABLE_LABEL_TAG);
	lbl_stable->setColor(Color3B::BLACK);

	
	auto animation = Animation::create();  //飞机动画
	
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("hero1.png"));  //动画帧
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("hero2.png"));
	
	animation->setDelayPerUnit(PLANE_INTERVAL_DELAY);  //切换时间
	
	animation->setLoops(REPEAT_FOREVER);  //循环次数:无限
	
	auto animate = Animate::create(animation);  //动画对象->动作对象
	hero->runAction(animate);
	
	auto listener = EventListenerTouchOneByOne::create();   //鼠标控制事件
	
	listener->onTouchBegan = [=](Touch* touch, Event*)
	{
		log("=====Began=====");
		auto touchPos = touch->getLocation();
		bool isControl = hero->getBoundingBox().containsPoint(touchPos);
		if (isControl)
		{
			m_correctVector = hero->getPosition() - touchPos;
		}
		return isControl;
	};

	//限定移动范围
	const float minX = hero->getContentSize().width / 2;
	const float maxX = visSize.width - minX;
	const float minY = hero->getContentSize().height / 2;
	const float maxY = visSize.height - minY;

	listener->onTouchMoved = [=](Touch* touch, Event*)
	{
		log("=====Move====");
		auto touchPos = touch->getLocation() + m_correctVector;
		hero->setPosition(MAX(minX, MIN(touchPos.x, maxX)), MAX(minY, MIN(touchPos.y, maxY)));
	};

	listener->onTouchEnded = [](Touch*, Event*)
	{
		log("=====End=====");
	};
	
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, hero);

	//子弹
	schedule(schedule_selector(GameScene::createBullet), BULLET_INTERVAL_DELAY); 
	//敌机
	schedule(schedule_selector(GameScene::createSmallEnemmy), SMALL_ENEMY_INTERVAL_DELAY, REPEAT_FOREVER, SMALL_ENEMY_APPEAR_DELAY);  
	schedule(schedule_selector(GameScene::createBigEnemy), MIDDLE_ENEMY_INTERVAL_DELAY, REPEAT_FOREVER, MIDDLE_ENEMY_APPEAR_DELAY);
	schedule(schedule_selector(GameScene::createBossEnemy), BIG_ENEMY_INTERVAL_DELAY, REPEAT_FOREVER, BIG_ENEMY_APPEAR_DELAY);
	schedule(schedule_selector(GameScene::createItems),ITEM_INTERVAL_DELAY, REPEAT_FOREVER, ITEM_APPEAR_DELAY);

	scheduleUpdate();  //updata图像
    
    return true;
}

void GameScene::update(float data)
{
	auto bg = this->getChildByTag(BG_TAG);
	auto bg_next = this->getChildByTag(BGNEXT_TAG);
	auto lbl_score = (Label*)this->getChildByTag(SCORE_TAG);
	auto hero = this->getChildByTag(HERO_TAG);

	bg->setPositionY(bg->getPositionY() - BG_SPEED);
	bg_next->setPositionY(bg->getPositionY() + bg->getContentSize().height);

	if (!bg_next->getPositionY())
	{
		bg->setPositionY(0);
	}

	//迭代器
	auto it_enemy = m_vEnemies.begin();
	auto it_bullet = this->m_vBullets.begin();
	auto it_item = this->m_vItems.begin();

	//子弹移动
	for (it_bullet = this->m_vBullets.begin(); it_bullet != this->m_vBullets.end();)
	{
		(*it_bullet)->setPositionY((*it_bullet)->getPositionY() + BULLETS_SPEED);
		if ((*it_bullet)->getPositionY() - (*it_bullet)->getContentSize().height / 2 >= visSize.height)
		{
			this->removeChild((*it_bullet));
			it_bullet = m_vBullets.erase(it_bullet);
		}
		else
		{
			it_bullet++;
		}
	}

	//敌机移动
	for (it_enemy = m_vEnemies.begin(); it_enemy != m_vEnemies.end();)
	{
		(*it_enemy)->setPositionY((*it_enemy)->getPositionY() - (*it_enemy)->e_speed);
		if ((*it_enemy)->getPositionY() + (*it_enemy)->getContentSize().height / 2 <= 0)
		{
			this->removeChild((*it_enemy));
			it_enemy = m_vEnemies.erase(it_enemy);
		}
		else
		{
			it_enemy++;
		}
	}

	//道具出边界
	for (it_item = m_vItems.begin(); it_item != m_vItems.end();)
	{
		if ((*it_item)->getPositionY() + (*it_item)->getContentSize().height / 2 <= 0)
		{
			this->removeChild((*it_item));
			it_item = m_vItems.erase(it_item);
		}
		else
		{
			it_item++;
		}
	}

	//子弹与敌机碰撞
	for (it_enemy = m_vEnemies.begin(); it_enemy != m_vEnemies.end();)
	{
		bool isHit = false;
		for (it_bullet = m_vBullets.begin(); it_bullet != m_vBullets.end();)
		{
			if (m_vEnemies.empty())
			{
				break;
			}
			if ((*it_bullet)->getBoundingBox().intersectsRect((*it_enemy)->getBoundingBox()))  //两者矩形框有重叠即为相碰
			{
				(*it_enemy)->e_hp -= (*it_bullet)->b_attack;
				this->removeChild((*it_bullet));
				it_bullet = m_vBullets.erase(it_bullet);

				isHit = true;
			}
			else
			{
				it_bullet++;
			}
		}
		//相撞
		if (isHit)
		{
			(*it_enemy)->hit();
			if ((*it_enemy)->isDestroy())
			{
				m_totalScore += (*it_enemy)->getScore();
				auto str_score = StringUtils::format("%d", m_totalScore);
				lbl_score->setString(str_score);
				(*it_enemy)->destroy();
				it_enemy = m_vEnemies.erase(it_enemy);
			}
		}
		else
		{
			it_enemy++;
		}
	}

	//道具与英雄碰撞
	for (it_item = this->m_vItems.begin(); it_item != m_vItems.end();)
	{
		bool isGet = false;
		if ((*it_item)->getBoundingBox().intersectsRect(hero->getBoundingBox()))
		{
			this->removeChild((*it_item));
			it_item = m_vItems.erase(it_item);
			isGet = true;
		}
		else
		{
			it_item++;
		}

		if (isGet)
		{
			this->m_isDoubleBullet = true;
			this->m_doubleBulletNumber = DOUBLE_BULLETS_NUMBER;
		}
	}

}

void GameScene::createBullet(float)
{
	auto hero = this->getChildByTag(HERO_TAG);

	if (this->m_isDoubleBullet)  //双重子弹
	{
		//左侧子弹
		auto leftBullet = Bullet::create(Twofold);
		leftBullet->setPosition(hero->getPosition() + Point(-hero->getContentSize().width / 3 + 2, hero->getContentSize().height / 5 + 5));
		this->addChild(leftBullet, BULLET_LAYER);
		m_vBullets.push_back(leftBullet);

		//右侧子弹
		auto rightBullet = Bullet::create(Twofold);
		rightBullet->setPosition(hero->getPosition() + Point(hero->getContentSize().width / 3 - 2, hero->getContentSize().height / 5 + 5));
		this->addChild(rightBullet, BULLET_LAYER);
		m_vBullets.push_back(rightBullet);

		this->m_doubleBulletNumber--;

		if (this->m_doubleBulletNumber <= 0)
		{
			this->m_isDoubleBullet = false;
		}
	}
	else  //单重子弹
	{
		auto Bullet = Bullet::create(Onefold);
		Bullet->setPosition(hero->getPosition() + Point(0, hero->getContentSize().height / 2 + 10));
		this->addChild(Bullet, BULLET_LAYER);
		m_vBullets.push_back(Bullet);
	}
}

void GameScene::createEnemy(EnemyType type)
{
	auto enemy = Enemy::create(type);
	float minX = enemy->getContentSize().width / 2;
	float maxX = visSize.width - enemy->getContentSize().width / 2;
	float x = rand() % (int)(maxX - minX + 1) + minX; 
	float y = visSize.height;
	enemy->setPosition(x, y);
	m_vEnemies.push_back(enemy);
	this->addChild(enemy, ENEMY_LAYER);
}

void GameScene::createSmallEnemmy(float)
{
	this->createEnemy(Small);
}

void GameScene::createBigEnemy(float)
{
	this->createEnemy(Middle);
}

void GameScene::createBossEnemy(float)
{
	this->createEnemy(Big);
}

void GameScene::createItems(float)
{
	//创建道具
	auto itemType = rand() % 2;
	//auto item = Sprite::createWithSpriteFrameName(StringUtils::format("ufo%d.png",itemType + 1));
	auto item = Sprite::createWithSpriteFrameName("ufo1.png");
	float minX = item->getContentSize().width / 2;
	float maxX = visSize.width - minX;
	float x = rand() % (int)(maxX - minX + 1) + minX;
	float y = visSize.height + item->getContentSize().height / 2;
	item->setPosition(x, y);
	this->addChild(item, ITEM_LAYER, ITEM_TAG);
	this->m_vItems.push_back(item);

	//移动道具
	auto move1 = MoveBy::create(1, Point(0, -300));  //进入屏幕
	auto move2 = MoveTo::create(2, Point(item->getPositionX(), -item->getContentSize().height / 2));  //移出下边界
	auto callFunc = CallFuncN::create([](Node* node)
	{
		node->removeFromParentAndCleanup(true);
	});

	item->runAction(Sequence::create(move1, move1->reverse(), move2, NULL));
}