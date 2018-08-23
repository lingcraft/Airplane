#pragma once

//标志
#define	BG_TAG						1
#define	BGNEXT_TAG					2
#define HERO_TAG					3
#define SCORE_TAG					4
#define ITEM_TAG					5
#define STABLE_LABEL_TAG			6

//层
#define BG_LAYER					-1
#define PLANE_LAYER					0
#define	BULLET_LAYER				1
#define ENEMY_LAYER					2
#define SCORE_LAYER					3
#define ITEM_LAYER					4

//得分
#define DESTROY_SAMLL_SCORE			10
#define DESTROY_MIDDLE_SCORE		50
#define DESTROY_BIG_SCORE			100

//速度单位：像素
#define BG_SPEED					1
#define BULLETS_SPEED				5
#define SMALL_ENEMY_SPEED			4
#define MIDDLE_ENEMY_SPEED			2
#define BIG_ENEMY_SPEED				0.5f

//血量
#define SMALL_ENEMY_HP				1
#define MIDDLE_ENEMY_HP				10
#define BIG_ENEMY_HP				30

//子弹
#define	BULLET1_ATTACK				1
#define BULLET2_ATTACK				2
#define DOUBLE_BULLETS_NUMBER		20

//延迟单位：秒
#define PLANE_INTERVAL_DELAY		0.2f
#define BULLET_INTERVAL_DELAY		0.2f
#define SMALL_ENEMY_INTERVAL_DELAY	0.8f
#define MIDDLE_ENEMY_INTERVAL_DELAY	10
#define BIG_ENEMY_INTERVAL_DELAY	20
#define ITEM_INTERVAL_DELAY			5
#define SMALL_ENEMY_APPEAR_DELAY	2
#define MIDDLE_ENEMY_APPEAR_DELAY	8
#define BIG_ENEMY_APPEAR_DELAY		15
#define ITEM_APPEAR_DELAY			1
#define ENEMY_HIT_DELAY				0.2f
#define ENEMY_DESTROY_DELAY			0.1f

#define REPEAT_FOREVER				-1
#define visSize Director::getInstance()->getVisibleSize()