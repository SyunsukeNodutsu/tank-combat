#pragma once
#include "../GameObject.h"
#include"../../Component/AudioComponent.h"
#include "Application/Game/Scene.h"
#include "Application/Game/Main/Stage.h"

enum Status
{
	wanderer = 0,

	hunt = 1,

};

//タイル間をつなぐ線
struct GridLINE
{
	Grid start;
	Grid end;
};

class Wanderer :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;

	int nowDir = UP;

	void Do_Move();
	void Do_Rotate();
	void Do_Choice();
	void Do_Chase(int playerX,int playerY);//追跡

	void ChoiceSpeed();

	Status status;

	struct Astar
	{
		bool Open = true;
		Grid Pos;
		int Rcost;
		int Ecost;
		int Score;
		Grid owner;

		bool Isarrive = true;
	};

	Vec3 TargetPos;
	void Hunt(Vec3 target);
	void MomentRot(Vec3 target);
	Vec3 DoorFront;

	std::vector<std::shared_ptr<GridLINE>> AnsNode;


private:
	std::vector<std::vector<MapData>> MapData;
	
	Grid NowPos = {0,0};
	Grid NextPos={NowPos.x+1,0};
	
	float moveSpeed = 0.03125/2;

	float reach = 8;

	bool ChaseFlg = false;

	Matrix mRotate;
	Matrix FormermRotate=mRotate;

	Grid lastPlayerTile = {0,0};
	Grid nowPlayerTile = {0,0};

	std::shared_ptr<SoundInstance> voiceSE;
};

