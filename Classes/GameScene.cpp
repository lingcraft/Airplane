#include "GameScene.h"

GameScene::GameScene()
{
	m_totalScore = 0;
	m_isDoubleBullet = false;
	m_doubleBulletNumber = 0;
	m_bombCount = 0;
	m_canMove = true;
	m_planeActive = true;
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

	//背景音乐
	SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.3f);
	SimpleAudioEngine::getInstance()->playBackgroundMusic("game_music.mp3", true);

	/*
	//====导入plist====
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("shoot_background.plist");  
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("shoot.plist");
	*/

	//====对象创建====

	//背景相关
	auto bg = Sprite::createWithSpriteFrameName("background.png"); 
	auto bg_next = Sprite::createWithSpriteFrameName("background.png");

	//暂停菜单
	auto pause_normal = Sprite::createWithSpriteFrameName("game_pause_nor.png");
	auto pause_pressed = Sprite::createWithSpriteFrameName("game_pause_pressed.png");
	auto menu_item_pause = MenuItemSprite::create(pause_normal, pause_pressed);

	//继续菜单
	auto resume_normal = Sprite::createWithSpriteFrameName("game_resume_nor.png");
	auto resume_pressed = Sprite::createWithSpriteFrameName("game_resume_pressed.png");
	auto menu_item_resume = MenuItemSprite::create(resume_normal, resume_pressed);

	//切换
	auto menu_item_toggle = MenuItemToggle::createWithCallback(CC_CALLBACK_1(GameScene::pauseAndResume, this), menu_item_pause, menu_item_resume, nullptr);

	//飞机
	auto hero = Sprite::createWithSpriteFrameName("hero1.png");

	//计分标签
	auto lbl_score = Label::createWithBMFont("num.fnt","0");
	auto lbl_stable = LabelTTF::create("Score:", "Arial", 36);

	//炸弹
	auto bomb = Sprite::createWithSpriteFrameName("bomb.png");
	auto menu_item_bomb = MenuItemSprite::create(bomb, bomb, CC_CALLBACK_1(GameScene::bomb, this));
	
	auto lbl_bomb = Label::createWithBMFont("num.fnt", "X0");

	//====对象设置====

	//背景1
	bg->setAnchorPoint(Point::ZERO);  //锚点
	bg->setPosition(0, 0);
	bg->getTexture()->setAliasTexParameters();  //抗锯齿
	this->addChild(bg, BG_LAYER, BG_TAG);

	//背景2
	bg_next->setAnchorPoint(Point::ZERO);
	bg_next->setPosition(0, visSize.height);
	bg_next->getTexture()->setAliasTexParameters();
	this->addChild(bg_next, BG_LAYER, BGNEXT_TAG);

	//飞机
	hero->setPosition(visSize.width / 2, hero->getContentSize().height / 2);
	this->addChild(hero, PLANE_LAYER, HERO_TAG);

	//分数标签
	lbl_score->setAnchorPoint(Point::ZERO);
	lbl_score->setPosition(130, visSize.height - 73);
	this->addChild(lbl_score, SCORE_LAYER, LABEL_SCORE_TAG);
	lbl_score->setColor(Color3B::BLACK);

	//固定的“得分”标签
	lbl_stable->setAnchorPoint(Point::ZERO);
	lbl_stable->setPosition(20, visSize.height - 55);
	this->addChild(lbl_stable, SCORE_LAYER, STABLE_LABEL_TAG);
	lbl_stable->setColor(Color3B::BLACK);

	//切换标签
	menu_item_toggle->setPosition(Point(visSize - menu_item_pause->getContentSize()));

	//菜单
	auto menu = Menu::create();
	menu->addChild(menu_item_toggle, UI_LAYER, MENU_TOOGLE_TAG);
	menu->setPosition(Point::ZERO);
	this->addChild(menu, UI_LAYER, MENU_TAG);

	//炸弹
	menu_item_bomb->setPosition(Point(bomb->getContentSize()));
	menu->addChild(menu_item_bomb, UI_LAYER, MENU_ITEM_BOMB_TAG);
	menu_item_bomb->setVisible(false);

	lbl_bomb->setPosition(bomb->getContentSize().width + lbl_bomb->getContentSize().width + 15, bomb->getContentSize().height);
	this->addChild(lbl_bomb, UI_LAYER, LABEL_BOMB_TAG);
	lbl_bomb->setColor(Color3B::BLACK);
	lbl_bomb->setVisible(false);

	//敌机等级叠加速度初始化
	Enemy::clearSpeedIncrement();

	//====hero动画====
	auto animation = Animation::create();  //飞机动画
	animation = AnimationCache::getInstance()->getAnimation("hero_fly");
	auto animate = Animate::create(animation);  //动画对象->动作对象
	hero->runAction(animate);
	
	//====控制====

	auto listener = EventListenerTouchOneByOne::create();   //鼠标控制事件
	
	listener->onTouchBegan = [=](Touch* touch, Event*)
	{
		//log("=====Began=====");
		if (Director::getInstance()->isPaused()) { return false; }  //游戏暂停状态就不处理

		auto touchPos = touch->getLocation();
		bool isControl = hero->getBoundingBox().containsPoint(touchPos);
		if (isControl)
		{
			m_correctVector = hero->getPosition() - touchPos;
		}
		return isControl && m_canMove;
	};

	//限定移动范围
	const float minX = hero->getContentSize().width / 2;
	const float maxX = visSize.width - minX;
	const float minY = hero->getContentSize().height / 2;
	const float maxY = visSize.height - minY;

	listener->onTouchMoved = [=](Touch* touch, Event*)
	{
		//log("=====Move====");
		auto touchPos = touch->getLocation() + m_correctVector;
		hero->setPosition(MAX(minX, MIN(touchPos.x, maxX)), MAX(minY, MIN(touchPos.y, maxY)));
	};

	listener->onTouchEnded = [](Touch*, Event*)
	{
		//log("=====End=====");
	};
	
	hero->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, hero);

	//====对象更新====

	//子弹
	schedule(schedule_selector(GameScene::createBullet), BULLET_INTERVAL_DELAY); 
	
	//敌机
	schedule(schedule_selector(GameScene::createSmallEnemmy), SMALL_ENEMY_INTERVAL_DELAY, REPEAT_FOREVER, SMALL_ENEMY_APPEAR_DELAY);  
	schedule(schedule_selector(GameScene::createMiddleEnemy), MIDDLE_ENEMY_INTERVAL_DELAY, REPEAT_FOREVER, MIDDLE_ENEMY_APPEAR_DELAY);
	schedule(schedule_selector(GameScene::createBigEnemy), BIG_ENEMY_INTERVAL_DELAY, REPEAT_FOREVER, BIG_ENEMY_APPEAR_DELAY);
	schedule(schedule_selector(GameScene::createItems),ITEM_INTERVAL_DELAY, REPEAT_FOREVER, ITEM_APPEAR_DELAY);

	scheduleUpdate();  //updata图像
    
    return true;
}

void GameScene::update(float data)
{
	auto bg = this->getChildByTag(BG_TAG);
	auto bg_next = this->getChildByTag(BGNEXT_TAG);
	auto lbl_score = (Label*)this->getChildByTag(LABEL_SCORE_TAG);
	auto hero = this->getChildByTag(HERO_TAG);
	
	//背景重置位置
	bg->setPositionY(bg->getPositionY() - BG_SPEED);
	bg_next->setPositionY(bg->getPositionY() + bg->getContentSize().height);

	if (!bg_next->getPositionY())
	{
		bg->setPositionY(0);
	}

	/*
	//==迭代器==
	vector<Bullet*>::iterator it_bullet;
	vector<Enemy*>::iterator it_enemy;
	vector<Item*>::iterator it_item;
	*/

	//========各种状态判断========

	//====子弹移动====
	/*
	for (it_bullet = this->m_vBullets.begin(); it_bullet != this->m_vBullets.end();)
	{
		(*it_bullet)->setPositionY((*it_bullet)->getPositionY() + (*it_bullet)->getSpeed());
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
	*/
	vector<Bullet*> removeableBullets;
	for (auto bullet : m_vBullets)
	{
		bullet->setPositionY(bullet->getPositionY() + bullet->getSpeed());
		if (bullet->getPositionY() - bullet->getContentSize().height / 2 >= visSize.height)
		{
			removeableBullets.push_back(bullet);
			this->removeChild(bullet);
		}
	}

	//====敌机移动====
	/*
	for (it_enemy = m_vEnemies.begin(); it_enemy != m_vEnemies.end();)
	{
		(*it_enemy)->setPositionY((*it_enemy)->getPositionY() - (*it_enemy)->getSpeed());
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
	*/
	vector<Enemy*> removeableEnemies;
	for (auto enemy : m_vEnemies)
	{
		enemy->setPositionY(enemy->getPositionY() - enemy->getSpeed());
		if (enemy->getPositionY() + enemy->getContentSize().height / 2 <= 0)
		{
			removeableEnemies.push_back(enemy);
			this->removeChild(enemy);
		}
	}

	/*
	//====道具出边界====
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
	*/

	//====敌机碰撞====
	/*
	for (it_enemy = m_vEnemies.begin(); it_enemy != m_vEnemies.end(); it_enemy++)
	{
		bool isHit = false;
		for (it_bullet = m_vBullets.begin(); it_bullet != m_vBullets.end();)
		{
			if ((*it_bullet)->getBoundingBox().intersectsRect((*it_enemy)->getBoundingBox()))  //两者矩形框有重叠即为相碰
			{
				(*it_enemy)->loseHP((*it_bullet)->getATK());
				(*it_enemy)->hit();
				if ((*it_enemy)->isDestroy())
				{
					m_totalScore += (*it_enemy)->getScore();
					auto str_score = StringUtils::format("%d", m_totalScore);
					lbl_score->setString(str_score);

					(*it_enemy)->destroy();
					this->removeChild((*it_enemy));
					it_enemy = m_vEnemies.erase(it_enemy);
				}

				this->removeChild((*it_bullet));
				it_bullet = m_vBullets.erase(it_bullet);

				isHit = true;
			}
			else
			{
				it_bullet++;
			}
		}
		if (isHit)
		{
			(*it_enemy)->hit();
			if ((*it_enemy)->isDestroy())
			{
				m_totalScore += (*it_enemy)->getScore();
				auto str_score = StringUtils::format("%d", m_totalScore);
				lbl_score->setString(str_score);

				(*it_enemy)->destroy();
				this->removeChild((*it_enemy));
				it_enemy = m_vEnemies.erase(it_enemy);
			}
		}
		else
		{
			it_enemy++;
		}
	}
	*/
	for (auto enemy : m_vEnemies)
	{
		//敌机与子弹碰撞
		for (auto bullet : m_vBullets)
		{
			if (enemy->getBoundingBox().intersectsRect(bullet->getBoundingBox()))
			{
				removeableBullets.push_back(bullet);
				this->removeChild(bullet);

				enemy->hitAM();
				enemy->loseHP(bullet->getATK());

				if (enemy->isDestroy())
				{
					enemy->destroyAM();
					m_totalScore += enemy->getScore();
					auto str_score = StringUtils::format("%d", m_totalScore);
					lbl_score->setString(str_score);

					removeableEnemies.push_back(enemy);
					//this->removeChild(enemy);
				}
			}
		}
		//敌机与英雄碰撞
		if (m_planeActive && enemy->getBoundingBox().intersectsRect(hero->getBoundingBox()))
		{
			//不再参与碰撞检测，若仍有血量进入“闪烁”的不受攻击状态，否则死亡
			m_planeActive = false;

			m_canMove = false;

			enemy->hitAM();
			enemy->loseHP(HERO_HIT_ATTACK);

			if (enemy->isDestroy())
			{
				enemy->destroyAM();
				removeableEnemies.push_back(enemy);
			}

			auto animation = Animation::create();
			for (int i = 0; i < 4; i++)
			{
				auto png_file = StringUtils::format("hero_blowup_n%d.png", i + 1);
				auto sprite_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(png_file);
				animation->addSpriteFrame(sprite_frame);
			}
			animation->setDelayPerUnit(HERO_DESTROY_DELAY);
			auto animate = Animate::create(animation);
			auto CallFunc = CallFunc::create([=]()
			{
				auto scene = GameOverScene::createScene(m_totalScore);
				Director::getInstance()->replaceScene(scene);
			});
			hero->stopAllActions();
			hero->runAction(Sequence::create(animate, CallFunc, NULL));
		}
	}

	//====道具与英雄碰撞====
	/*
	for (it_item = this->m_vItems.begin(); it_item != m_vItems.end();)
	{
		bool isGet = false;
		ItemTpye item_type;
		if ((*it_item)->getBoundingBox().intersectsRect(hero->getBoundingBox()))
		{
			item_type = (*it_item)->getType();

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
			switch (item_type)
			{
			case BulletLevelUp:
				this->m_isDoubleBullet = true;
				this->m_doubleBulletNumber = DOUBLE_BULLETS_NUMBER;
				break;
			case Bomb:
				if (m_bombCount < 10)
				{
  					m_bombCount++;
					//this->updateBombCount();
				}
				break;
			}
		}
	}
	*/
	if (m_planeActive)
	{
		vector<Item*> removeableItems;
		for (auto item : m_vItems)
		{
			if (item->getBoundingBox().intersectsRect(hero->getBoundingBox()))
			{
				removeableItems.push_back(item);
				this->removeChild(item);

				switch (item->getType())
				{
				case BulletLevelUp:
					SimpleAudioEngine::getInstance()->playEffect("get_double_laser.mp3");
					m_isDoubleBullet = true;
					m_doubleBulletNumber = DOUBLE_BULLETS_NUMBER;
					break;
				case Bomb:
					SimpleAudioEngine::getInstance()->playEffect("get_bomb.mp3");
					if (m_bombCount < MAX_BOMB_NUM)
					{
						m_bombCount++;
					}
					updateBombCount();
					break;
				default:
					break;
				}
			}
		}
		for (auto item : removeableItems)
		{
			m_vItems.eraseObject(item);
		}
		removeableItems.clear();
		removeableItems.shrink_to_fit();
	}
	

	for (auto bullet : removeableBullets)
	{
		m_vBullets.eraseObject(bullet);
	}
	removeableBullets.clear();
	removeableBullets.shrink_to_fit();

	for (auto enemy : removeableEnemies)
	{
		m_vEnemies.eraseObject(enemy);
	}
	removeableEnemies.clear();
	removeableEnemies.shrink_to_fit();
}

void GameScene::createBullet(float)
{
	SimpleAudioEngine::getInstance()->playEffect("bullet.mp3");
	auto hero = this->getChildByTag(HERO_TAG);

	if (m_isDoubleBullet)  //双重子弹
	{
		//左侧子弹
		auto leftBullet = Bullet::create(Twofold);
		leftBullet->setPosition(hero->getPosition() + Point(-hero->getContentSize().width / 3 + 2, hero->getContentSize().height / 5 + 5));
		this->addChild(leftBullet, BULLET_LAYER);
		m_vBullets.pushBack(leftBullet);

		//右侧子弹
		auto rightBullet = Bullet::create(Twofold);
		rightBullet->setPosition(hero->getPosition() + Point(hero->getContentSize().width / 3 - 2, hero->getContentSize().height / 5 + 5));
		this->addChild(rightBullet, BULLET_LAYER);
		m_vBullets.pushBack(rightBullet);

		m_doubleBulletNumber--;

		if (this->m_doubleBulletNumber <= 0)
		{
			this->m_isDoubleBullet = false;
		}
	}
	else  //单重子弹
	{
		auto bullet = Bullet::create(Onefold);
		bullet->setPosition(hero->getPosition() + Point(0, hero->getContentSize().height / 2 + 10));
		this->addChild(bullet, BULLET_LAYER);
		m_vBullets.pushBack(bullet);
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
	m_vEnemies.pushBack(enemy);
	this->addChild(enemy, ENEMY_LAYER);
}

void GameScene::createSmallEnemmy(float)
{
	this->createEnemy(Small);
}

void GameScene::createMiddleEnemy(float)
{
	this->createEnemy(Middle);
}

void GameScene::createBigEnemy(float)
{
	this->createEnemy(Big);
}

void GameScene::createItems(float)
{
	SimpleAudioEngine::getInstance()->playEffect("out_porp.mp3");
	//创建道具
	int itemType = rand() % 2;
	Item* item;

	switch (itemType)
	{
	case 0:
		item = Item::create(BulletLevelUp);
		break;
	case 1:
		item = Item::create(Bomb);
		break;
	default:
		break;
	}
	
	float minX = item->getContentSize().width / 2;
	float maxX = visSize.width - minX;
	float x = rand() % (int)(maxX - minX + 1) + minX;
	float y = visSize.height + item->getContentSize().height / 2;
	item->setPosition(x, y);
	this->addChild(item, ITEM_LAYER, ITEM_TAG);
	this->m_vItems.pushBack(item);

	//移动道具
	auto move1 = MoveBy::create(1, Point(0, -300));  //进入屏幕
	auto move2 = MoveBy::create(1, Point(0, 200));
	auto move3 = MoveTo::create(2, Point(item->getPositionX(), -item->getContentSize().height / 2));  //移出下边界
	
	auto callFunc = CallFuncN::create([=](Node* node)
	{
		auto item = (Item*)node;
		item->removeFromParentAndCleanup(true);
		this->m_vItems.eraseObject(item);
	});

	item->runAction(Sequence::create(move1, move2, move3, callFunc, NULL));
}

void GameScene::bomb(Ref*)
{
	if (Director::getInstance()->isPaused() || !m_bombCount)
	{
		return;
	}

	SimpleAudioEngine::getInstance()->playEffect("use_bomb.mp3");
	auto lbl_score = (Label*)this->getChildByTag(LABEL_SCORE_TAG);

	m_bombCount--;
	updateBombCount();

	/*
	for (it_enemy = m_vEnemies.begin(); it_enemy != m_vEnemies.end(); it_enemy++)
	{
		(*it_enemy)->destroy();
		(*it_enemy)->loseHP(BOMB_ATTACK);
		if ((*it_enemy)->isDestroy())
		{
			m_totalScore += (*it_enemy)->getScore();
			auto str_score = StringUtils::format("%d", m_totalScore);
			lbl_score->setString(str_score);

			this->removeChild((*it_enemy));
			it_enemy = m_vEnemies.erase((it_enemy));
		}
	}
	*/
	vector<Enemy*> removeableEnemies;
	for (auto enemy : m_vEnemies)
	{
		enemy->loseHP(BOMB_ATTACK);
		enemy->destroyAM();
		if (enemy->isDestroy())
		{
			m_totalScore += enemy->getScore();
			auto str_score = StringUtils::format("%d", m_totalScore);
			lbl_score->setString(str_score);

			removeableEnemies.push_back(enemy);
			//this->removeChild(enemy);
		}
	}

	for (auto enemy : removeableEnemies)
	{
		m_vEnemies.eraseObject(enemy);
	}
	removeableEnemies.clear();
	removeableEnemies.shrink_to_fit();
}

void GameScene::updateBombCount()
{
	auto menu = this->getChildByTag(MENU_TAG);
	auto menu_item_bomb = menu->getChildByTag(MENU_ITEM_BOMB_TAG);
	auto lbl_bomb = (Label*)this->getChildByTag(LABEL_BOMB_TAG);

	if (m_bombCount <= 0)
	{
		menu_item_bomb->setVisible(false);
		lbl_bomb->setVisible(false);
	}
	else if (m_bombCount == 1)
	{
		menu_item_bomb->setVisible(true);
		lbl_bomb->setVisible(false);
	}
	else
	{
		menu_item_bomb->setVisible(true);
		lbl_bomb->setVisible(true);
		lbl_bomb->setString(StringUtils::format("X%d", this->m_bombCount));
	}
}

void GameScene::pauseAndResume(Ref* ref)
{
	auto toggle = (MenuItemToggle*)ref;  //通过Toggle菜单项的SelectedIndex可以判断现在切换到第几项
	if (toggle->getSelectedIndex() == 0)
	{
		SimpleAudioEngine::getInstance()->playEffect("button.mp3");
		m_canMove = true;
		SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
		Director::getInstance()->resume();
	} 
	else
	{
		SimpleAudioEngine::getInstance()->playEffect("button.mp3");
		m_canMove = false;
		SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
		Director::getInstance()->pause();
	}
}