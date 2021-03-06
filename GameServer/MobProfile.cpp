#include "MobProfile.h"
#include "GameNetwork.h"

MobProfile::MobProfile(TBLIDX worldTblidx, TBLIDX spawnTblidx, TBLIDX mobTblidx, bool bCustom)
{
	pServer = (GameServer*)_GetApp();
	worldTblIdx = worldTblidx;
	spawnTblIdx = spawnTblidx;
	mobTblIdx = mobTblidx;
	bCustomSpawn = bCustom;
}

MobProfile::~MobProfile()
{
}

bool MobProfile::Init()
{
	memset(&sBrief, 0, sizeof(sBrief));
	memset(&sCharState, 0, sizeof(sCharState));

	TBLIDX mobId = mobTblIdx;

	if (false == bCustomSpawn)
	{
		sSPAWN_TBLDAT* pSpawnData = (sSPAWN_TBLDAT*)pServer->GetTableContainer()->GetMobSpawnTable(worldTblIdx)->FindData(spawnTblIdx);
		mobId = pSpawnData->mob_Tblidx;

		sCharState.sCharStateBase.vCurLoc.x = pSpawnData->vSpawn_Loc.x;
		sCharState.sCharStateBase.vCurLoc.y = pSpawnData->vSpawn_Loc.y;
		sCharState.sCharStateBase.vCurLoc.z = pSpawnData->vSpawn_Loc.z;

		sCharState.sCharStateBase.vCurDir.x = pSpawnData->vSpawn_Dir.x;
		sCharState.sCharStateBase.vCurDir.y = pSpawnData->vSpawn_Dir.y;
		sCharState.sCharStateBase.vCurDir.z = pSpawnData->vSpawn_Dir.z;
	}

	sCharState.sCharStateBase.byStateID = CHARSTATE_STANDING;
	sCharState.sCharStateBase.bFightMode = false;

	sMOB_TBLDAT* pTblData = (sMOB_TBLDAT*)pServer->GetTableContainer()->GetMobTable()->FindData(mobId);

	if (pTblData)
	{
		hSerialID = pServer->AcquireSerialID();
		sBrief.tblidx = pTblData->tblidx;
		sBrief.wCurLP = sBrief.wMaxLP = pTblData->wBasic_LP;
		sBrief.wCurEP = sBrief.wMaxEP = pTblData->wBasic_EP;
		sBrief.fLastWalkingSpeed = pTblData->fWalk_Speed;
		sBrief.fLastRunningSpeed = pTblData->fRun_Speed;
		return true;
	}
	return false;
}

void MobProfile::GetObjectCreate(sGU_OBJECT_CREATE& sPacket)
{
	memset(&sPacket, 0, sizeof(sPacket));
	sPacket.wOpCode = GU_OBJECT_CREATE;
	sPacket.handle = hSerialID;
	sPacket.sObjectInfo.objType = OBJTYPE_MOB;
	sPacket.sObjectInfo.mobBrief = sBrief;
	sPacket.sObjectInfo.mobState = sCharState;
}