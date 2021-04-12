#pragma once
#include "Wanderer.h"
#include "Application/Game/Main/Stage.h"
#include "Application/Component/ModelComponent.h"


void Wanderer::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);
	status = wanderer;
	//敵にマップ方法注入
	MapOnly=Stage::GetInstance().MapOnly;
	Adjust = Stage::GetInstance().Adjust;
	voiceSE = voiceSE->Deserialize(Track::Music::EnemyVoice);
	ChoiceSpeed();
	dhisolve = 1;
}

void Wanderer::Update()
{
	GetModelComponent()->SetDissolveThreshold(dhisolve);
	dhisolve -= 0.010;
	GameObject::Update();
	if(Scene::GetInstance().EnemyUpdate)
	Do_Move();
	
	//
	if (ChaseFlg)
	{
		if (Scene::GetInstance().m_EditorCameraEnable)
		{
			for (auto& AllNode : AnsNode)
			{
				Vec3 startL;
				startL.x = AllNode->start.x;
				startL.y = 0;
				startL.z = AllNode->start.y;

				Vec3 endL;
				endL.x = AllNode->end.x;
				endL.y = 0;
				endL.z = AllNode->end.y;

				Scene::GetInstance().AddDebugLine(startL, endL, { 0,1,1,1 });

			}
		}
	}
	voiceSE->SetPos(m_mWorld.GetTranslation());
}

void Wanderer::Do_Move()
{
	//主人公の位置を実数値ではなくタイル換算で読み込む
	Matrix mTrans, mScale;
	float PlayerX, PlayerY;
	Vec3 pos = m_mWorld.GetTranslation();
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		if ((obj->GetTag() & TAG_Player))
		{
			Grid a = obj->GetNowTile();
			if (obj->GetNowTile().x != -9999)
			{
				lastPlayerTile = obj->GetNowTile();
			}
			nowPlayerTile = obj->GetNowTile();
			PlayerX = obj->GetMatrix().GetTranslation().x;
			PlayerY = obj->GetMatrix().GetTranslation().z;
			break;
		}
	}
	if (status == wanderer)
	{
		//タイル間のどこを走っているのかで分岐


		if (NowPos.x < NextPos.x)
			pos.x += moveSpeed;
		else if ((NowPos.x > NextPos.x))
			pos.x -= moveSpeed;

		if (NowPos.y < NextPos.y)
			pos.z += moveSpeed;
		else if (NowPos.y > NextPos.y)
			pos.z -= moveSpeed;



		//次に行く予定のタイルを選択
		//到着予定のタイルの真ん中付近でのみ次のタイルを選択する
		float Bottom_e, Height_e, Hypotenuse_e;
		Bottom_e = (float)NextPos.x - pos.x;
		Height_e = (float)NextPos.y - pos.z;
		Hypotenuse_e = sqrt(Bottom_e * Bottom_e + Height_e * Height_e);
		if (Hypotenuse_e < 0.1)
		{
			//主人公の近くでのみ追跡処理を開始する
			float Buttom_p, Height_p, Hypotenuse_p;
			Buttom_p = PlayerX - NextPos.x;
			Height_p = PlayerY - NextPos.y;
			Hypotenuse_p = sqrt(Buttom_p * Buttom_p + Height_p * Height_p);

			if (Hypotenuse_p < reach)
			{
				Game0ver = true;
				Grid NowTile = GetNowTile();
				bool floor = true;
				if (nowPlayerTile.x == -9999)
				{
					if (lastPlayerTile.x == NowTile.x)
					{
						if (lastPlayerTile.y == NowTile.y)
						{
							for (auto list : Stage::GetInstance().DoorAroundList)
							{
								if (list.Before.x == NowTile.x)
								{
									if (list.Before.y == NowTile.y)
									{
										//NowPos = NextPos;
										DoorFront.x = list.Before.x;
										DoorFront.z = list.Before.y;
										floor = false;
										status = hunt;
										//MomentRot({ PlayerX,0,PlayerY });
										TargetPos = { (float)list.After.x,0,(float)list.After.y };
									}
								}
							}
						}
					}
				}
				if (floor)
				{
					//追跡
					Do_Chase(lastPlayerTile.x, lastPlayerTile.y);
				}
			}
			else
			{
				if (Hypotenuse_p < reach + 5)
				{
					//遠くないところでしか音を鳴らさない
					if (!voiceSE->IsPlay())
					{
						voiceSE->Play3D(m_mWorld.GetTranslation(), true, true);
					}
				}
				else
				{
					if (!voiceSE->IsPause())
					{
						voiceSE->Pause();
					}
				}
				Game0ver = false;
				//次のタイルを選択
				Do_Choice();
			}
			ChoiceSpeed();
		}




		//回転処理
		Do_Rotate();

		mTrans.CreateTranslation(pos.x, pos.y, pos.z);
		m_mWorld = mScale * mRotate * mTrans;

	}
	if (status == hunt)
	{
		if (nowPlayerTile.x == -9999)
		{
			TargetPos.x = PlayerX;
			TargetPos.z = PlayerY;
		}
		else
		{

			TargetPos.x=DoorFront.x;
			TargetPos.z=DoorFront.z;
		}
		Hunt(TargetPos);
	}


}

void Wanderer::Do_Rotate()
{
	//進行方向に向かって角度変更
	mRotate = FormermRotate;
	if (NowPos.x < NextPos.x)
		mRotate.RotateY(90 * ToRadians);
	else if (NowPos.x > NextPos.x)
		mRotate.RotateY(270 * ToRadians);

	else if (NowPos.y < NextPos.y)
		mRotate.RotateY(0 * ToRadians);
	else if (NowPos.y > NextPos.y)
		mRotate.RotateY(180 * ToRadians);

}
//進行方向選択
void Wanderer::Do_Choice()
{
	ChaseFlg = false;
	//進める方向リスト
	std::vector<int> houkou;

	//進行方向との逆の方向を記憶（いきなり振り返らないため）
	int gyakuDir = (nowDir + 2) % 4;

	//次のタイルへ更新
	NowPos = NextPos;

	//マップの終端
	int Xend = NextPos.x+Adjust.x;
	int Yend = NextPos.y+Adjust.y;

	//進める方向リストに積んでいく
	if (MapOnly.size() > Xend + 1)
	{
		if (MapOnly[Xend + 1][Yend] && gyakuDir != UP)
		{
			houkou.push_back(UP);
		}
	}
	if (MapOnly[0].size() > Yend + 1)
	{
		if (MapOnly[Xend][NextPos.y + Adjust.y + 1] && gyakuDir != LEFT)
		{
			houkou.push_back(LEFT);
		}
	}
	if (0 < Yend)
		if (MapOnly[Xend][Yend - 1] && gyakuDir != RIGHT)
		{
			houkou.push_back(RIGHT);
		}
	if (0 < Xend)
		if (MapOnly[Xend - 1][Yend] && gyakuDir != DOWN)
		{
			houkou.push_back(DOWN);
		}

	//進める方向リストの中からランダムで選択
	//進む方向が無かったら反対方向に戻る
	if (houkou.size())
	{
		nowDir = houkou[rand() % houkou.size()];
	}
	else
	{
		nowDir = gyakuDir;
	}
	//次に進むタイルの座標を更新
	switch (nowDir)
	{
	case 0:
		NextPos.x += 1;

		break;
	case 1:
		NextPos.y += 1;

		break;
	case 2:
		NextPos.x -= 1;

		break;
	case 3:
		NextPos.y -= 1;

		break;
	default:
		break;
	}

}
//追跡プログラム
void Wanderer::Do_Chase(int PlayerX, int PlayerY)
{
	//スタックしていくノードリスト
	std::vector<std::shared_ptr<Astar>> NodeSave;
	//たどるためのポインタ
	std::weak_ptr<Astar> wpNode;

	//出発地点のノード制作
	std::shared_ptr<Astar> node = std::make_shared<Astar>();
	node->Pos.x = NextPos.x;
	node->Pos.y = NextPos.y;

	node->Open = true;
	node->Rcost = 0;
	node->Ecost = abs(node->Pos.x - PlayerX) + abs(node->Pos.y - PlayerY);
	node->owner = { 9999,9999 };
	node->Score = node->Rcost + node->Ecost;
	//スタック
	NodeSave.push_back(node);
	ChaseFlg = true;
	//経路探索をあきらめる距離

	if (node->Ecost != 0)
	{
		while (1)
		{

			bool END = false;

			//一番主人公に近いノードを探す
			int minScore = 99999;
			for (auto& nextnode : NodeSave)
			{
				//ターゲット設定
				if ((minScore > nextnode->Score) && nextnode->Open && nextnode->Isarrive)
				{
					minScore = nextnode->Score;
				}
			}

			//探索開始
			//架空のオーナー9999に設定　上下左右にシェアードポインタを待機
			std::shared_ptr<Astar> NewNode1 = std::make_shared<Astar>();
			NewNode1->owner.x = 9999;
			std::shared_ptr<Astar> NewNode2 = std::make_shared<Astar>();
			NewNode2->owner.x = 9999;
			std::shared_ptr<Astar> NewNode3 = std::make_shared<Astar>();
			NewNode3->owner.x = 9999;
			std::shared_ptr<Astar> NewNode4 = std::make_shared<Astar>();
			NewNode4->owner.x = 9999;
			bool kai = false;//この周回で新しく追加されたかどうか

			for (auto& next1Node : NodeSave)
			{
				std::weak_ptr<Astar> next2 = next1Node;
				auto nextNode = next2.lock();
				if (nextNode->Score == minScore && nextNode->Open)
				{
					//ノードを閉じる
					nextNode->Open = false;
					if (nextNode->Pos.x + 1 + Adjust.x < MapOnly.size())
						if (MapOnly[nextNode->Pos.x + 1 + Adjust.x][nextNode->Pos.y + Adjust.y])
						{
							NewNode1->Pos.x = nextNode->Pos.x + 1;
							NewNode1->Pos.y = nextNode->Pos.y;

							NewNode1->Open = true;

							NewNode1->Rcost = nextNode->Rcost + 1;

							NewNode1->Ecost = abs(NewNode1->Pos.x - PlayerX) + abs(NewNode1->Pos.y - PlayerY);

							NewNode1->owner = nextNode->Pos;

							NewNode1->Isarrive = true;

							NewNode1->Score = NewNode1->Rcost + NewNode1->Ecost;
						}
					if (nextNode->Pos.y + 1 + Adjust.y < MapOnly[0].size())
						if (MapOnly[nextNode->Pos.x + Adjust.x][nextNode->Pos.y + 1 + Adjust.y])
						{
							NewNode2->Pos.x = nextNode->Pos.x;
							NewNode2->Pos.y = nextNode->Pos.y + 1;

							NewNode2->Open = true;

							NewNode2->Rcost = nextNode->Rcost + 1;

							NewNode2->Ecost = abs(NewNode2->Pos.x - PlayerX) + abs(NewNode2->Pos.y - PlayerY);

							NewNode2->owner = nextNode->Pos;

							NewNode2->Isarrive = true;

							NewNode2->Score = NewNode2->Rcost + NewNode2->Ecost;
						}
					if (nextNode->Pos.y - 1 + Adjust.y >= 0)
						if (MapOnly[nextNode->Pos.x + Adjust.x][nextNode->Pos.y - 1 + Adjust.y])
						{
							NewNode3->Pos.x = nextNode->Pos.x;
							NewNode3->Pos.y = nextNode->Pos.y - 1;

							NewNode3->Open = true;

							NewNode3->Rcost = nextNode->Rcost + 1;

							NewNode3->Ecost = abs(NewNode3->Pos.x - PlayerX) + abs(NewNode3->Pos.y - PlayerY);

							NewNode3->owner = nextNode->Pos;

							NewNode3->Isarrive = true;

							NewNode3->Score = NewNode3->Rcost + NewNode3->Ecost;
						}
					if (nextNode->Pos.x - 1 + Adjust.x >= 0)
						if (MapOnly[nextNode->Pos.x - 1 + Adjust.x][nextNode->Pos.y + Adjust.y])
						{
							NewNode4->Pos.x = nextNode->Pos.x - 1;
							NewNode4->Pos.y = nextNode->Pos.y;

							NewNode4->Open = true;

							NewNode4->Rcost = nextNode->Rcost + 1;

							NewNode4->Ecost = abs(NewNode4->Pos.x - PlayerX) + abs(NewNode4->Pos.y - PlayerY);

							NewNode4->owner = nextNode->Pos;

							NewNode4->Isarrive = true;

							NewNode4->Score = NewNode4->Rcost + NewNode4->Ecost;
						}
					kai = true;

				}
				//追加
				if (kai)
					break;
			}

			//ノードを追加していく
			if (NewNode1->owner.x != 9999)
				NodeSave.push_back(NewNode1);

			if (NewNode2->owner.x != 9999)
				NodeSave.push_back(NewNode2);

			if (NewNode3->owner.x != 9999)
				NodeSave.push_back(NewNode3);

			if (NewNode4->owner.x != 9999)
				NodeSave.push_back(NewNode4);

			//被り消去
			for (auto& nextnode : NodeSave)
			{
				for (auto& DelNode : NodeSave)
				{
					if (nextnode->Pos.x == DelNode->Pos.x && nextnode->Pos.y == DelNode->Pos.y && DelNode->Open && DelNode->owner.x != 9999)
					{
						if (nextnode == DelNode)
							continue;
						if (DelNode->owner.x == 9999)
							int a = 0;

						DelNode->Isarrive = false;
					}
				}
			}
			//消去実行
			for (auto spObjectItr = NodeSave.begin(); spObjectItr != NodeSave.end();)
			{

				if (!(*spObjectItr)->Isarrive)
				{
					spObjectItr = NodeSave.erase(spObjectItr);
				}
				else
				{
					++spObjectItr;
				}
			}
			//終了条件
			for (auto& AllNode : NodeSave)
			{
				if (AllNode->Ecost == 0)
				{
					END = true;
					wpNode = AllNode;
				}
			}
			if (END)
				break;
	
		}


		//経路を戻って方向に応じて返し値をだす
		//std::vector<std::shared_ptr<GridLINE>> AnsNode;//答えに行くためのノード配列
		AnsNode.clear();

		while (1)
		{
			bool END = false;
			for (auto& AllNode : NodeSave)
			{
				auto selectNode = wpNode.lock();
				if (selectNode)
				{
					if (AllNode->Pos.x == selectNode->owner.x && AllNode->Pos.y == selectNode->owner.y && AllNode->Open != true)
					{
						std::shared_ptr<GridLINE> AddNode = std::make_shared<GridLINE>();
						AddNode->start = AllNode->Pos;
						AddNode->end = selectNode->Pos;
						AnsNode.push_back(AddNode);
						wpNode = AllNode;
					}
					if (selectNode->owner.x == 9999)
						END = true;
				}
			}
			//経路のデバッグライン表示
			if (END)
			{

				break;
			}
		}

		//方向選択
		auto itr = AnsNode.end() - 1;
		std::shared_ptr<GridLINE> LOOK = *itr;
		NowPos = NextPos;

		if (LOOK->end.x > LOOK->start.x)
		{
			NextPos.x += 1;
		}
		if (LOOK->end.x < LOOK->start.x)
		{
			NextPos.x -= 1;
		}
		if (LOOK->end.y > LOOK->start.y)
		{
			NextPos.y += 1;
		}
		if (LOOK->end.y < LOOK->start.y)
		{
			NextPos.y -= 1;
		}
	}
}

void Wanderer::ChoiceSpeed()
{

	if (Scene::GetInstance().OperateEnemy)
	{
		moveSpeed = 0.03125;
		int Speed = Scene::GetInstance().EnemySpeed;

		for (int i = 1; i < Speed; i++)
		{
			moveSpeed *= 2;
		}
	}

}

void Wanderer::Hunt(Vec3 target)
{
	Vec3 TargetVec = target - m_mWorld.GetTranslation();

	TargetVec.Normalize();

	Vec3 vZ = m_mWorld.GetAxisZ();

	vZ.Normalize();

	Vec3 vRotAxis = Vec3::Cross(vZ, TargetVec);
	if (vRotAxis.LengthSquared() != 0)
	{
		float d = Vec3::Dot(vZ, TargetVec);

		if (d > 1)d = 1.0f;
		else if (d < 1.0f)d = -1.0f;

		float radian = acos(d);
		radian *= ToRadians;
		//if (radian > 3.0f * ToRadians)
		//{
		//	radian = 3.0f * ToRadians;
		//}
		Matrix mRot;
		mRot.CreateRotationAxis(vRotAxis, radian);
		auto pos = m_mWorld.GetTranslation();
		m_mWorld.SetTranslation({ 0,0,0 });
		m_mWorld *= mRot;
		m_mWorld.SetTranslation(pos);
		Vec3 move = m_mWorld.GetAxisZ();
		move.Normalize();

		move *= (float)0.01;

		m_mWorld.Move(move);
	}
	if (!(nowPlayerTile.x == -9999))
	{
		float Bottom_e, Height_e, Hypotenuse_e;
		Bottom_e = DoorFront.x - m_mWorld.GetTranslation().x;
		Height_e = DoorFront.z - m_mWorld.GetTranslation().z;
		Hypotenuse_e = sqrt(Bottom_e * Bottom_e + Height_e * Height_e);

		if (Hypotenuse_e < 0.1)
		{
			status = wanderer;
			//追跡
			//MomentRot(target);
			m_mWorld.SetTranslation(DoorFront);
			Do_Choice();
		}
	}
}

void Wanderer::MomentRot(Vec3 target)
{
	Vec3 TargetVec = target - m_mWorld.GetTranslation();

	TargetVec.Normalize();

	Vec3 vZ = m_mWorld.GetAxisZ();

	vZ.Normalize();

	Vec3 vRotAxis = Vec3::Cross(vZ, TargetVec);
	if (vRotAxis.LengthSquared() != 0)
	{
		float d = Vec3::Dot(vZ, TargetVec);

		if (d > 1)d = 1.0f;
		else if (d < 1.0f)d = -1.0f;

		float radian = acos(d);

		if (radian > 3.0f * ToRadians)
		{
			radian = 3.0f * ToRadians;
		}
		Matrix mRot;
		mRot.CreateRotationAxis(vRotAxis, acos(d));
		auto pos = m_mWorld.GetTranslation();
		m_mWorld.SetTranslation({ 0,0,0 });
		m_mWorld *= mRot;
		m_mWorld.SetTranslation(pos);
	}
}
