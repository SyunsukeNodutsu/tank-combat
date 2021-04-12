//-----------------------------------------------------------------------------
// File: GenerateGameObject.cpp
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#include "GameObject.h"

#include "AI/Character/AutomaticTank.h"
#include "AI/Character/TitleTank.h"

#include "Ambient/SkySphere.h"
#include "Ambient/Tree.h"

#include "Building/House.h"

#include "Other/EditorCamera.h"

#include "Player/Human.h"
#include "Player/PlayerTank.h"

#include "Scene/Process/CreditProcess.h"
#include "Scene/Process/OptionProcess.h"
#include "Scene/Process/ResultProcess.h"
#include "Scene/Process/StageSelectProcess.h"
#include "Scene/Process/TankGameProcess.h"
#include "Scene/Process/TitleAnimationProcess.h"
#include "Scene/Process/TitleProcess.h"
#include "Scene/Process/TutorialProcess.h"

//-----------------------------------------------------------------------------
// Name: CreateGameObject()
// Desc: クラスの名前を元にGameObjectを生成
//-----------------------------------------------------------------------------
std::shared_ptr<GameObject> CreateGameObject(const std::string& class_name)
{
	// ObjectBase
	if (class_name == "GameObject")				{ return std::make_shared<GameObject>(); }
	// AI
	if (class_name == "AutomaticTank")			{ return std::make_shared<AutomaticTank>(); }
	if (class_name == "TitleTank")				{ return std::make_shared<TitleTank>(); }
	// Ambient
	if (class_name == "SkySphere")				{ return std::make_shared<SkySphere>(); }
	if (class_name == "Tree")					{ return std::make_shared<Tree>(); }
	// Building
	if (class_name == "House")					{ return std::make_shared<House>(); }
	// Other
	if (class_name == "EditorCamera")			{ return std::make_shared<EditorCamera>(); }
	// Player
	if (class_name == "Human")					{ return std::make_shared<Human>(); }
	if (class_name == "PlayerTank")				{ return std::make_shared<PlayerTank>(); }
	// Process
	if (class_name == "CreditProcess")			{ return std::make_shared<CreditProcess>(); }
	if (class_name == "OptionProcess")			{ return std::make_shared<OptionProcess>(); }
	if (class_name == "ResultProcess")			{ return std::make_shared<ResultProcess>(); }
	if (class_name == "StageSelectProcess")		{ return std::make_shared<StageSelectProcess>(); }
	if (class_name == "TankGameProcess")		{ return std::make_shared<TankGameProcess>(); }
	if (class_name == "TitleAnimationProcess")	{ return std::make_shared<TitleAnimationProcess>(); }
	if (class_name == "TitleProcess")			{ return std::make_shared<TitleProcess>(); }
	if (class_name == "TutorialProcess")		{ return std::make_shared<TutorialProcess>(); }

	// Error
	assert(0 && "class not found");
	return nullptr;
}