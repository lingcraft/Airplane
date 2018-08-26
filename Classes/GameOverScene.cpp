#include "GameOverScene.h"

Scene* GameOverScene::createScene(int score)
{
	auto scene = Scene::create();
	auto layer = GameOverScene::create(score);
	scene->addChild(layer);

	return scene;
}

GameOverScene* GameOverScene::create(int score)
{
	auto game_over = new GameOverScene();
	if (game_over && game_over->init(score))
	{
		game_over->autorelease();
		return game_over;
	}
	delete game_over;
	game_over = NULL;
	return NULL;
}

bool GameOverScene::init(int score)
{
	if (!Layer::init()) { return false; }

	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	SimpleAudioEngine::getInstance()->playEffect("game_over.mp3");

	auto bg = Sprite::createWithSpriteFrameName("gameover.png");
	bg->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
	bg->setPosition(visSize.width / 2, visSize.height);
	this->addChild(bg, BG_LAYER);

	auto lbl_score = Label::createWithBMFont("num.fnt",StringUtils::format("%d", score));
	lbl_score->setPosition(visSize.width / 2, visSize.height - 400);
	this->addChild(lbl_score);
	lbl_score->setColor(Color3B(120, 120, 120));

	auto high_score = UserDefault::getInstance()->getIntegerForKey("HIGHSCORE", 0);
	auto lbl_high_score = Label::createWithBMFont("num.fnt", StringUtils::format("%d", high_score));
	lbl_high_score->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
	lbl_high_score->setPosition(145, visSize.height - 20);
	this->addChild(lbl_high_score);
	lbl_high_score->setColor(Color3B(120, 120, 120));

	//当前分高于最高分时重置最高分
	if (score > high_score)
	{
		SimpleAudioEngine::getInstance()->playEffect("achievement.mp3");

		high_score = score;
		UserDefault::getInstance()->setIntegerForKey("HIGHSCORE", high_score);

		lbl_high_score->setString(StringUtils::format("%d", high_score));

		auto scaleIn = ScaleTo::create(0.5f, 0.5f);
		auto scaleOut = ScaleTo::create(0.5f, 1);
		lbl_high_score->runAction(Sequence::create(scaleIn, scaleOut, NULL));
	}

	//重新游戏
	auto back = Sprite::createWithSpriteFrameName("btn_finish.png");
	auto menu_item_back = MenuItemSprite::create(back, back, [=](Ref*)
	{	
		SimpleAudioEngine::getInstance()->playEffect("button.mp3");

		auto bg = Sprite::createWithSpriteFrameName("background.png");
		bg->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
		this->addChild(bg);

		auto loading = Sprite::createWithSpriteFrameName("game_loading4.png");
		loading->setPosition(visSize.width - 115, 40);
		this->addChild(loading);

		auto animation = AnimationCache::getInstance()->getAnimation("loading");

		auto animate = Animate::create(animation);
		auto callFunc = CallFunc::create([=]()
		{

			auto scene = GameScene::createScene();
			Director::getInstance()->replaceScene(scene);
		});
		loading->runAction(Sequence::create(animate, callFunc, NULL));

	});

	float x = visSize.width - back->getContentSize().width / 2 - 20;
	menu_item_back->setPosition(x, back->getContentSize().height / 2 + 20);

	auto menu = Menu::create(menu_item_back, NULL);
	menu->setPosition(Point::ZERO);
	this->addChild(menu);

	return true;

}