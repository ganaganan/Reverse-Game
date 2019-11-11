#pragma once

#include <memory>
#include <DirectXMath.h>
#include <Skinned_mesh.h>

class Enemy
{
public:
	static constexpr int ENEMY_WAIT_COUNT = 300;
	static constexpr int ERASE_COUNT = 60;
	enum class EnemyType
	{
		TotemPole,
		Ghost,
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
	void MoveRightPoint();
	void MoveLeftPoint();
	bool JudgeIsHitLight();
	void ErasePreparation();
	void EraseThisInstance();

	void Generate(DirectX::XMFLOAT3 _scale, EnemyType _type, Point _generatePoint);
	bool GetEnable() { return isEnable; }
};


class EnemyManager
{
	static constexpr int ENEMY_MAX = 50;
	Enemy enemy[ENEMY_MAX];

public:
	std::unique_ptr<SkinnedMesh> totemPoleModel;
	std::unique_ptr<SkinnedMesh> ghostModel;

private:
	EnemyManager() {}
public:
	~EnemyManager(){}

	void Init();
	void Update();
	void Render(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P);
	void Uninit();


	void GenerateEnemy(
		DirectX::XMFLOAT3 _scale,
		Enemy::EnemyType _type, Enemy::Point _generatePoint);


	static EnemyManager& Get()
	{
		static EnemyManager instance;
		return instance;
	}
};
