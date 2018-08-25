#include "WelcomeScene.h"
#include "GameScene.h"

Scene* WelcomeScene::createScene()
{
	auto scene = Scene::create();
	auto layer = WelcomeScene::create();
	scene->addChild(layer);
	return scene;
}

bool WelcomeScene::init()
{
	if (!Layer::init()) { return false; }

	srand(time(NULL));
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("shoot_background.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("shoot.plist");

	//====爆炸动画====
	//小敌机
	auto animation = Animation::create();
	for (int i = 0; i < 4; i++)
	{
		auto png_file = StringUtils::format("enemy1_down%d.png", i + 1);
		auto sprite_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(png_file);
		animation->addSpriteFrame(sprite_frame);
	}
	animation->setDelayPerUnit(ENEMY_DESTROY_DELAY);
	AnimationCache::getInstance()->addAnimation(animation, "small_enemy_destroy");  //缓存起来
	//中敌机
	animation = Animation::create();
	for (int i = 0; i < 4 ; i++)
	{
		auto png_file = StringUtils::format("enemy2_down%d.png", i + 1);
		auto sprite_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(png_file);
		animation->addSpriteFrame(sprite_frame);
	}
	animation->setDelayPerUnit(ENEMY_DESTROY_DELAY);
	AnimationCache::getInstance()->addAnimation(animation, "middle_enemy_destroy");
	//大敌机
	animation = Animation::create();
	for (int i = 0; i < 6 ; i++)
	{
		auto png_file = StringUtils::format("enemy3_down%d.png", i + 1);
		auto sprite_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(png_file);
		animation->addSpriteFrame(sprite_frame);
	}
	animation->setDelayPerUnit(ENEMY_DESTROY_DELAY);
	AnimationCache::getInstance()->addAnimation(animation, "big_enemy_destroy");
	//英雄
	animation = Animation::create();
	for (int i = 0; i < 4; i++)
	{
		auto png_file = StringUtils::format("hero_blowup_n%d.png", i + 1);
		auto sprite_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(png_file);
		animation->addSpriteFrame(sprite_frame);
	}
	animation->setDelayPerUnit(HERO_DESTROY_DELAY);
	AnimationCache::getInstance()->addAnimation(animation, "hero_destroy");

	//====受伤动画====
	//中敌机
	animation = Animation::create();
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy2_hit.png"));
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy2.png"));
	animation->setDelayPerUnit(0.2f);
	AnimationCache::getInstance()->addAnimation(animation, "middle_enemy_hit");
	//大敌机
	animation = Animation::create();
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_hit.png"));
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_n1.png"));
	animation->setDelayPerUnit(0.2f);
	AnimationCache::getInstance()->addAnimation(animation, "big_enemy_hit");

	//====飞行动画====
	//英雄
	animation = Animation::create();
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("hero1.png"));
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("hero2.png"));
	animation->setDelayPerUnit(PLANE_INTERVAL_DELAY);  //切换时间
	animation->setLoops(REPEAT_FOREVER);  //循环次数:无限
	AnimationCache::getInstance()->addAnimation(animation, "hero_fly");
	//大敌机
	animation = Animation::create();
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_n1.png"));
	animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_n2.png"));
	animation->setDelayPerUnit(BIG_ENEMY_FLY_DELAY);  //切换时间
	animation->setLoops(REPEAT_FOREVER);  //循环次数:无限
	AnimationCache::getInstance()->addAnimation(animation, "big_enemy_fly");

	//====声音文件====
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("game_music.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("achievement.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("big_spaceship_flying.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("bullet.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("button.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("enemy1_down.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("enemy2_down.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("enemy3_down.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("game_over.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("get_bomb.mp3");		
	SimpleAudioEngine::getInstance()->preloadEffect("get_double_laser.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("out_porp.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("use_bomb.mp3");

	//====加载界面====
	//背景
	auto bg = Sprite::createWithSpriteFrameName("background.png");
	bg->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
	this->addChild(bg, BG_LAYER);
	//logo
	auto logo = Sprite::createWithSpriteFrameName("shoot_copyright.png");
	logo->setPosition(Point(visSize / 2));
	this->addChild(logo);

	//加载动画
	auto loading = Sprite::createWithSpriteFrameName("game_loading4.png");
	loading->setPosition(visSize.width / 2, visSize. height / 2 + 120);
	this->addChild(loading);

	animation = Animation::create();
	for (int i = 0; i < 4; i++)
	{
		auto png_file = StringUtils::format("game_loading%d.png", i + 1);
		auto sprite_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(png_file);
		animation->addSpriteFrame(sprite_frame);
	}
	animation->setDelayPerUnit(LOADING_DELAY);
	animation->setLoops(2);
	AnimationCache::getInstance()->addAnimation(animation, "loading");

	auto animate = Animate::create(animation);
	auto callFunc = CallFunc::create([]()
	{
		auto scene = GameScene::createScene();
		Director::getInstance()->replaceScene(scene);
	});
	loading->runAction(Sequence::create(animate, callFunc, NULL));

	return true;
}