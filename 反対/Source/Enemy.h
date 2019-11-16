#pragma once

#include <memory>
#include <DirectXMath.h>
#include <Skinned_mesh.h>

#include "../MyLib/Audio.h"

class Enemy
{

public:
	static int ENEMY1_WAIT_COUNT;
	static int ENEMY2_WAIT_COUNT;
	static int ENEMY3_WAIT_COUNT;
	static int ERASE_COUNT;
	enum class EnemyType
	{
		TotemPole,	// Enemy3
		Ghost,		// Enemy2
		Daruma,		// Enemy1
	};
	enum Point : int
	{
		R_Point1,
		R_Point2,
		R_Point3,
		R_Point4,
		R_Point5,
		L_Point1,
		L_Point2,
		L_Point3,
		L_Point4,
		L_Point5,
	};

	DirectX::XMFLOAT3 pos;
private:
	DirectX::XMFLOAT3 scale;

	EnemyType	type;
	Point		nowPoint;
	int			moveTime;
	int			eraseCount;
	bool		isEnable;
	bool		isPreparationErase;
	bool		isHitLight;


public:
	Enemy(){}
	~Enemy(){}

	void Init();
	void Update();
	void Render(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P);
	void Uninit();

	void Move();
	void MoveEnemy1();
	void MoveEnemy2();
	void MoveEnemy3();
	void MoveCross();

	void MoveRightPoint();
	void MoveLeftPoint();
	void JudgeIsHitLight();
	void ErasePreparation();
	void EraseThisInstance();

	void Generate(EnemyType _type, Point _generatePoint);
	bool GetEnable() { return isEnable; }
};


class EnemyManager
{
public:
	enum SoundType
	{
		MoveDaruma,
		MoveGhost,
		MovePole,
		Max,
	};

	Audio* sound[SoundType::Max];
	float  soundVol[SoundType::Max];

private:
	static constexpr int ENEMY_MAX = 50;
	Enemy enemy[ENEMY_MAX];

public:
	std::unique_ptr<SkinnedMesh> totemPoleModel;
	std::unique_ptr<SkinnedMesh> ghostModel;
	std::unique_ptr<SkinnedMesh> darumaModel;

private:
	EnemyManager() {}
public:
	~EnemyManager() { Uninit(); }

	void Init();
	void Update();
	void Render(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P);
	void Uninit();


	void GenerateEnemy(
		Enemy::EnemyType _type, Enemy::Point _generatePoint);


	static EnemyManager& Get()
	{
		static EnemyManager instance;
		return instance;
	}
};
