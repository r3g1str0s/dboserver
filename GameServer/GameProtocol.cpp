#include "GameNetwork.h"
#include "GameProtocol.h"


bool GameClient::PacketControl(Packet* pPacket)
{
	LPPACKETDATA data = (LPPACKETDATA)pPacket->GetPacketData();

	switch (data->wOpCode)
	{
	case UG_PING: break;
	case UG_GAME_ENTER_REQ: {
		SendGameEnterRes((sUG_GAME_ENTER_REQ*)data);
		SendCharInfo();
		SendCharItemInfo();
		SendCharSkillInfo();
		SendCharBuffsInfo();
		SendCharHTBInfo();
		// Quest List
		SendCharQuickSlotInfo();
		SendAvatarWarFogInfo();
		SendCharInfoEnd();
	} break;
	case UG_GAME_LEAVE_REQ: SendGameLeaveRes(); break;
	case UG_AUTH_KEY_FOR_COMMUNITY_SERVER_REQ: SendAuthkeyCommSrvRes(); break;
	case UG_ENTER_WORLD: {
		SendCharWorldInfo();
		SendCharWorldInfoEnd();
	} break;
	case UG_SERVER_COMMAND: CheckCommand((sUG_SERVER_COMMAND*)data); break;
	case UG_CHAR_READY_FOR_COMMUNITY_SERVER_NFY: break;
	case UG_CHAR_READY_TO_SPAWN: SendCharReadySpawnReq(); break;
	case UG_CHAR_READY: SendCharReadyRes((sUG_CHAR_READY*)data); break;
	case UG_CHAR_FOLLOW_MOVE: SendCharFollowMove((sUG_CHAR_FOLLOW_MOVE*)data); break;
	case UG_CHAR_MOVE: SendCharMove((sUG_CHAR_MOVE*)data); break;
	case UG_CHAR_DEST_MOVE: SendCharDestMove((sUG_CHAR_DEST_MOVE*)data); break;
	case UG_CHAR_CHANGE_HEADING: SendCharChangeHeading((sUG_CHAR_CHANGE_HEADING*)data); break;
	case UG_CHAR_CHANGE_DIRECTION_ON_FLOATING: SendCharChangeDirectionFloating((sUG_CHAR_CHANGE_DIRECTION_ON_FLOATING*)data); break;
	case UG_CHAR_MOVE_SYNC: SendCharMoveSync((sUG_CHAR_MOVE_SYNC*)data); break;
	case UG_CHAR_JUMP: SendCharJump((sUG_CHAR_JUMP*)data); break;
	case UG_CHAR_JUMP_END: SendCharJumpEnd(); break;
	case UG_CHAR_DIRECT_PLAY_ACK: SendCharDirectPlay((sUG_CHAR_DIRECT_PLAY_ACK*)data); break;
	case UG_CHAR_TOGG_FIGHTING: SendToggleFightMode((sUG_CHAR_TOGG_FIGHTING*)data); break;
	case UG_CHAR_TOGG_SITDOWN:{
		sUG_CHAR_TOGG_SITDOWN* test = (sUG_CHAR_TOGG_SITDOWN*)data;
		if (test->bSitDown)
		{
			SendToggleSitDown();
		}
		else
		{
			SendToggleStandUp();
		}
	}break;
	case UG_CHAR_TARGET_SELECT: SendTargetSelect((sUG_CHAR_TARGET_SELECT*)data); break;
	case UG_CHAR_TARGET_INFO: SendTargetInfo((sUG_CHAR_TARGET_INFO*)data); break;
	case UG_CHAR_ATTACK_BEGIN: SendCharAttackBegin((sUG_CHAR_ATTACK_BEGIN*)data); break;
	case UG_CHAR_ATTACK_END: SendCharAttackEnd((sUG_CHAR_ATTACK_END*)data); break;
	case UG_CHAR_EXIT_REQ: SendCharExitRes(); break;
	case UG_GAME_EXIT_REQ: SendGameExitRes(); break;
	case UG_CHAR_SKILL_REQ: SendCharSkillRes((sUG_CHAR_SKILL_REQ*)data); break;
	case UG_ITEM_MOVE_REQ: SendIemMoveRes((sUG_ITEM_MOVE_REQ*)data); break;
	case UG_SOCIAL_ACTION: SendSocialAction((sUG_SOCIAL_ACTION*)data); break;
	case UG_TS_CONFIRM_STEP_REQ: SendTSConfirmRes((sUG_TS_CONFIRM_STEP_REQ*)data); break;
	case UG_TS_EXCUTE_TRIGGER_OBJECT: SendTSExecObjectRes((sUG_TS_EXCUTE_TRIGGER_OBJECT*)data); break;
	case UG_TUTORIAL_HINT_UPDATE_REQ: SendTutoHintUpdateRes((sUG_TUTORIAL_HINT_UPDATE_REQ*)data); break;
	case UG_WAR_FOG_UPDATE_REQ: SendWarFogUpdateRes((sUG_WAR_FOG_UPDATE_REQ*)data); break;

		//CASH/EVENT SHOPS (not working)
	case UG_SHOP_EVENTITEM_START_REQ: SendEventItemStartRes(); break;
	case UG_SHOP_EVENTITEM_END_REQ: SendEventItemEndRes(); break;
	case UG_SHOP_NETPYITEM_START_REQ: SendNetpyItemStartRes(); break;
	case UG_SHOP_NETPYITEM_END_REQ: SendNetpyItemEndRes(); break;
	case UG_PRIVATESHOP_CREATE_REQ: SendPrivateShopCreate((sUG_PRIVATESHOP_CREATE_REQ*)data); break;

		//PORTAL PACKETS
	case UG_PORTAL_START_REQ: SendPortalStart((sUG_PORTAL_START_REQ*)data); break;
	case UG_PORTAL_ADD_REQ: SendPortalAdd((sUG_PORTAL_ADD_REQ*)data); break;
	case UG_PORTAL_REQ: SendPortalReq((sUG_PORTAL_REQ*)data); break;
		// SYS PACKETS
	case SYS_ALIVE: { ResetAliveTime(); } break;
	case SYS_PING:	{
		// use the ping response to check
		//pServer->GetObjectManager()->SpawnToClient(this);
	} break;
	default:
		Logger::Log("Received Opcode: %s\n", NtlGetPacketName_UG(data->wOpCode));
		return false;
		break;
	}
	return true;
}

void GameClient::SendCharSkillRes(sUG_CHAR_SKILL_REQ* pData)
{
	TBLIDX skillId = INVALID_TBLIDX;
	WORD Result = GAME_SUCCESS;
	for (int i = 0; i < NTL_MAX_PC_HAVE_SKILL; i++)
	{
		if (pProfile->asSkillInfo[i].bySlotId == pData->bySlotIndex)
		{
			skillId = pProfile->asSkillInfo[i].tblidx;
		}
	}

	if (skillId != INVALID_TBLIDX)
	{
		sSKILL_TBLDAT* pTbl = (sSKILL_TBLDAT*)pServer->GetTableContainer()->GetSkillTable()->FindData(skillId);

		if (pTbl)
		{
			sGU_CHAR_ACTION_SKILL sSkill;
			memset(&sSkill, 0, sizeof(sSkill));
			sSkill.wOpCode = GU_CHAR_ACTION_SKILL;
			sSkill.dwLpEpEventId = skillId;
			sSkill.skillId = skillId;
			sSkill.handle = pProfile->GetSerialID();
			sSkill.hAppointedTarget = pData->hTarget;
			sSkill.wResultCode = Result;
			sSkill.byRpBonusType = pData->byRpBonusType;
			sSkill.bySkillResultCount = pData->byApplyTargetCount;

			for (int i = 0; i < sSkill.bySkillResultCount; i++)
			{
				sSkill.aSkillResult[i].hTarget = pData->ahApplyTarget[i];
				sSkill.aSkillResult[i].effectResult1.fResultValue = pTbl->fSkill_Effect_Value[0];
				sSkill.aSkillResult[i].effectResult2.fResultValue = pTbl->fSkill_Effect_Value[1];
				sSkill.aSkillResult[i].byAttackResult = BATTLE_ATTACK_RESULT_HIT;
				sSkill.aSkillResult[i].byBlockedAction = 255;

				WORD wDamage = (WORD)(sSkill.aSkillResult[i].effectResult1.fResultValue + sSkill.aSkillResult[i].effectResult2.fResultValue);

				DamagetoTarget(sSkill.aSkillResult[i].hTarget, wDamage);
			}

			pServer->GetClientManager()->SendAll2(&sSkill, sizeof(sSkill), this);
		}
	}
	else
	{
		Result = eRESULTCODE::GAME_SKILL_ID_NULL;
	}

	sGU_CHAR_SKILL_RES sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_CHAR_SKILL_RES;
	sPkt.wResultCode = Result;
	Send(&sPkt, sizeof(sPkt));
}

void GameClient::SendGiveExp(DWORD dwExp)
{
	DWORD mobExp = dwExp * pServer->nMobExpMulti;
	pProfile->sPcProfile.dwCurExp += mobExp;

	sGU_UPDATE_CHAR_EXP sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_UPDATE_CHAR_EXP;
	sPkt.handle = pProfile->GetSerialID();
	sPkt.dwIncreasedExp = mobExp;
	sPkt.dwAcquisitionExp = dwExp;
	sPkt.dwBonusExp = mobExp - dwExp;
	sPkt.dwCurExp = pProfile->sPcProfile.dwCurExp;
	Send(&sPkt, sizeof(sPkt));

	pProfile->UpdateCharExp();
}

void GameClient::SendMobGiveExp(TBLIDX mobId)
{
	sMOB_TBLDAT* pTbl = (sMOB_TBLDAT*)pServer->GetTableContainer()->GetMobTable()->FindData(mobId);

	if (pTbl)
	{
		DWORD mobExp = pTbl->wExp * pServer->nMobExpMulti;
		pProfile->sPcProfile.dwCurExp += mobExp;

		sGU_UPDATE_CHAR_EXP sPkt;
		memset(&sPkt, 0, sizeof(sPkt));
		sPkt.wOpCode = GU_UPDATE_CHAR_EXP;
		sPkt.handle = pProfile->GetSerialID();
		sPkt.dwIncreasedExp = mobExp;
		sPkt.dwAcquisitionExp = pTbl->wExp;
		sPkt.dwBonusExp = mobExp - pTbl->wExp;
		sPkt.dwCurExp = pProfile->sPcProfile.dwCurExp;
		Send(&sPkt, sizeof(sPkt));

		pProfile->UpdateCharExp();
	}
}

void GameClient::SendToggleFightMode(sUG_CHAR_TOGG_FIGHTING* pData)
{
	if (pData->byAvatarType == pProfile->GetAvatartype())
	{
		pProfile->sCharState.sCharStateBase.bFightMode = pData->bFightMode;

		sGU_CHAR_FIGHTMODE sPkt;
		memset(&sPkt, 0, sizeof(sPkt));
		sPkt.bFightMode = pProfile->sCharState.sCharStateBase.bFightMode;
		sPkt.handle = pProfile->GetSerialID();
		sPkt.wOpCode = GU_CHAR_FIGHTMODE;
		pServer->GetClientManager()->SendAll2(&sPkt, sizeof(sPkt),this);
	}
}

void GameClient::DamagetoTarget(HOBJECT hTarget, WORD wDamage)
{
	CharacterProfile* pPcProfile = NULL;
	MobProfile* pMobProfile = NULL;
	NpcProfile* pNpcProfile = NULL;

	if (pServer->GetObjectManager()->FindObject(hTarget, eOBJTYPE::OBJTYPE_PC))
	{
		pPcProfile = pServer->GetObjectManager()->pcList.find(pProfile->GetTarget())->second;

		pPcProfile->sPcProfile.wCurLP -= wDamage;
		if ((pPcProfile->sPcProfile.wCurLP - wDamage) < 0)
			pPcProfile->sPcProfile.wCurLP = 0;

		Logger::Log(" PC Attack [%u] LP: %u\n", pPcProfile->GetSerialID(), pPcProfile->sPcProfile.wCurLP);
	}
	else if (pServer->GetObjectManager()->FindObject(hTarget, eOBJTYPE::OBJTYPE_MOB))
	{
		pMobProfile = pServer->GetObjectManager()->mobList.find(pProfile->GetTarget())->second;

		int result = pMobProfile->sBrief.wCurLP;
		result-= wDamage;
		if (result <= 0)
			pMobProfile->sBrief.wCurLP = 0;
		else
			pMobProfile->sBrief.wCurLP = result;

		Logger::Log(" MOB Attack [%u] LP: %u\n", pMobProfile->GetSerialID(), pMobProfile->sBrief.wCurLP);//just to use in console
		
		if (pMobProfile->sBrief.wCurLP == 0)
		{
			pMobProfile->sCharState.sCharStateBase.byStateID = CHARSTATE_FAINTING;
			SendCharStateUpdate(pMobProfile->GetSerialID(), pMobProfile->sCharState);
			pMobProfile->sCharState.sCharStateBase.byStateID = CHARSTATE_STANDING;

			sGU_CHAR_FIGHTMODE sPkt;
			memset(&sPkt, 0, sizeof(sPkt));
			sPkt.bFightMode = pProfile->sCharState.sCharStateBase.bFightMode = false;
			sPkt.handle = pProfile->GetSerialID();
			sPkt.wOpCode = GU_CHAR_FIGHTMODE;
			pServer->GetClientManager()->SendAll2(&sPkt, sizeof(sPkt),this);

			SendMobGiveExp(pMobProfile->GetMobID());
		}
	}
}

void GameClient::SendCharAttack(DWORD dwCurrTick)
{
	WORD wDamage = pProfile->sPcProfile.avatarAttribute.wLastPhysicalOffence;
	wDamage += 2 * wDamage;

	if (pProfile->sCharState.sCharStateBase.bFightMode)
	{
		if (pProfile->dwLastAttack != 0 && (dwCurrTick - pProfile->dwLastAttack) >= 2100)
		{
			sGU_CHAR_ACTION_ATTACK sPkt;
			memset(&sPkt, 0, sizeof(sPkt));
			sPkt.wOpCode = GU_CHAR_ACTION_ATTACK;
			sPkt.dwLpEpEventId = 255;
			sPkt.byBlockedAction = 255;
			sPkt.hSubject = pProfile->GetSerialID();
			sPkt.hTarget = pProfile->GetTarget();
			sPkt.wAttackResultValue = wDamage;
			sPkt.bChainAttack = false;
			sPkt.byAttackSequence = 1;
			sPkt.vShift = pProfile->sCharState.sCharStateBase.vCurLoc;
			pServer->GetClientManager()->SendAll2(&sPkt, sizeof(sPkt),this);
			pProfile->dwLastAttack = dwCurrTick;

			DamagetoTarget(pProfile->GetTarget(), wDamage);
		}
	}
}

void GameClient::SendCharAttackEnd(sUG_CHAR_ATTACK_END* pData)
{
	if (pData->byAvatarType == pProfile->GetAvatartype())
	{
		pProfile->dwLastAttack = 0;
	}
}

void GameClient::SendCharAttackBegin(sUG_CHAR_ATTACK_BEGIN* pData)
{
	if (pData->byAvatarType == pProfile->GetAvatartype())
	{
		pProfile->dwLastAttack = GetTickCount();
	}
}

void GameClient::SendLPEPUpdate(WORD wCurLp, WORD wMaxLp, WORD wCurEp, WORD wMaxEp, HOBJECT hTarget)
{
	sGU_UPDATE_CHAR_LP_EP sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_UPDATE_CHAR_LP_EP;
	sPkt.handle = hTarget;
	sPkt.wCurLP = wCurLp;
	sPkt.wMaxLP = wMaxLp;
	sPkt.wCurEP = wCurEp;
	sPkt.wMaxEP = wMaxEp;
	sPkt.dwLpEpEventId = 255;
	pServer->GetClientManager()->SendAll2(&sPkt, sizeof(sPkt),this);
}

void GameClient::SendEPUpdate(WORD wCurEp, WORD wMaxEp, HOBJECT hTarget)
{
	sGU_UPDATE_CHAR_EP sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_UPDATE_CHAR_EP;
	sPkt.handle = hTarget;
	sPkt.wCurEP = wCurEp;
	sPkt.wMaxEP = wMaxEp;
	sPkt.dwLpEpEventId = 255;
	pServer->GetClientManager()->SendAll2(&sPkt, sizeof(sPkt),this);
}

void GameClient::SendLPUpdate(WORD wCurLp, WORD wMaxLp, HOBJECT hTarget)
{
	sGU_UPDATE_CHAR_LP sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_UPDATE_CHAR_LP;
	sPkt.handle = hTarget;
	sPkt.wCurLP = wCurLp;
	sPkt.wMaxLP = wMaxLp;
	sPkt.dwLpEpEventId = 255;
	pServer->GetClientManager()->SendAll2(&sPkt, sizeof(sPkt),this);
}

void GameClient::SendCharStateUpdate(HOBJECT hObject, sCHARSTATE sCharState)
{
	sGU_UPDATE_CHAR_STATE sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.handle = hObject;
	sPkt.wOpCode = GU_UPDATE_CHAR_STATE;
	sPkt.sCharState = sCharState;
	pServer->GetClientManager()->SendAll2(&sPkt, sizeof(sPkt), this);
}

void GameClient::SendCharLevelUp(BYTE byToUp)
{
	sEXP_TBLDAT* pExpTbl = (sEXP_TBLDAT*)pServer->GetTableContainer()->GetExpTable()->FindData(pProfile->sPcProfile.byLevel + byToUp);

	if (pExpTbl)
	{
		pProfile->sPcProfile.byLevel += byToUp;
		sGU_UPDATE_CHAR_LEVEL sPkt;
		memset(&sPkt, 0, sizeof(sPkt));
		sPkt.wOpCode = GU_UPDATE_CHAR_LEVEL;
		sPkt.byCurLevel = pProfile->sPcProfile.byLevel;
		sPkt.byPrevLevel = pProfile->sPcProfile.byLevel - byToUp;
		sPkt.dwMaxExpInThisLevel = pExpTbl->dwNeed_Exp;
		sPkt.handle = pProfile->GetSerialID();
		pServer->GetClientManager()->SendAll2(&sPkt, sizeof(sPkt), this);
		pProfile->UpdateCharLevel();
	}
}

void GameClient::SendTargetSelect(sUG_CHAR_TARGET_SELECT* pData)
{
	pProfile->SetTarget(pData->hTarget);
}
//By Luiz45 - Execute trigger Object
void GameClient::SendTSExecObjectRes(sUG_TS_EXCUTE_TRIGGER_OBJECT* pData)
{
	SendSystemText("hSrc[%u] hTgt[%u] EvtType[%u] uiParam[%u]",
		pData->hSource, pData->hTarget, pData->byEvtGenType, pData->uiParam);
	sGU_TS_EXCUTE_TRIGGER_OBJECT_RES pExecuteRes;
	memset(&pExecuteRes, 0, sizeof(pExecuteRes));
	pExecuteRes.wOpCode = GU_TS_EXCUTE_TRIGGER_OBJECT_RES;
	pExecuteRes.wResultCode = GAME_TS_WARNING_BEGIN;
	pExecuteRes.hTriggerObject = pData->hTarget;
	Send(&pExecuteRes, sizeof(pExecuteRes));
}

void GameClient::SendWarFogUpdateRes(sUG_WAR_FOG_UPDATE_REQ* pData)
{
	sGU_WAR_FOG_UPDATE_RES sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_WAR_FOG_UPDATE_RES;
	sPkt.handle = pData->hObject;

	if (pProfile->AddWarFogFlags(pData->hObject))
	{
		sPkt.wResultCode = eRESULTCODE::GAME_SUCCESS;
	}
	else
	{
		sPkt.wResultCode = eRESULTCODE::GAME_WARFOG_ARLEADY_ADDED;
	}
	Send(&sPkt, sizeof(sPkt));
}

void GameClient::SendEventItemStartRes()
{
	sGU_SHOP_EVENTITEM_START_RES sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_SHOP_NETPYITEM_START_RES;
	sPkt.byType = 0; // 0 Normal - 1 Especial Event NPC
	sPkt.wResultCode = eRESULTCODE::GAME_SUCCESS;
	Send(&sPkt, sizeof(sPkt));
}

void GameClient::SendEventItemEndRes()
{
	sGU_SHOP_EVENTITEM_END_RES sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_SHOP_NETPYITEM_START_RES;
	sPkt.wResultCode = eRESULTCODE::GAME_SUCCESS;
	Send(&sPkt, sizeof(sPkt));
}

void GameClient::SendNetpyItemStartRes()
{
	sGU_SHOP_NETPYITEM_START_RES sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_SHOP_NETPYITEM_START_RES;
	sPkt.byType = 0; // 0 Normal - 1 Especial Event NPC
	sPkt.wResultCode = eRESULTCODE::GAME_SUCCESS;
	Send(&sPkt, sizeof(sPkt));
}

void GameClient::SendNetpyItemEndRes()
{
	sGU_SHOP_NETPYITEM_END_RES sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_SHOP_NETPYITEM_START_RES;
	sPkt.wResultCode = eRESULTCODE::GAME_SUCCESS;
	Send(&sPkt, sizeof(sPkt));
}

void GameClient::SendTutoHintUpdateRes(sUG_TUTORIAL_HINT_UPDATE_REQ* pData)
{
	pProfile->sPcProfile.dwTutorialHint |= pData->dwTutorialHint;
	pServer->ServerDB->ExecuteQuery("UPDATE `character` SET `TutorialHint` = '%u' WHERE `ID` = '%u';", pProfile->sPcProfile.dwTutorialHint, pProfile->GetCharID());
	sGU_TUTORIAL_HINT_UPDATE_RES sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_TUTORIAL_HINT_UPDATE_RES;
	sPkt.wResultCode = GAME_SUCCESS;
	sPkt.dwTutorialHint = pData->dwTutorialHint;
	Send(&sPkt, sizeof(sPkt));
}

void GameClient::SendSocialAction(sUG_SOCIAL_ACTION* pData)
{
	sGU_SOCIAL_ACTION sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_SOCIAL_ACTION;
	sPkt.hSubject = pProfile->GetSerialID();
	sPkt.socialActionId = pData->socialActionId;
	pServer->GetClientManager()->SendOthers(&sPkt, sizeof(sPkt), this);
}

void GameClient::SendTSConfirmRes(sUG_TS_CONFIRM_STEP_REQ* pData)
{
	sGU_TS_CONFIRM_STEP_RES sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_TS_CONFIRM_STEP_RES;
	sPkt.byTsType = pData->byTsType;
	sPkt.dwParam = pData->dwParam;
	sPkt.tId = pData->tId;
	sPkt.tcCurId = pData->tcCurId;
	sPkt.tcNextId = pData->tcNextId;
	sPkt.wResultCode = RESULT_SUCCESS;
	
	Send(&sPkt, sizeof(sPkt));
	/*DungeonTable* table = pServer->GetTableContainer()->GetDungeonTable();
	WorldTable* wolrTabl = pServer->GetTableContainer()->GetWorldTable();
	WorldMapTable* world = pServer->GetTableContainer()->GetWorldMapTable();
	if (pData->tcNextId == 2)
	{
		sCHARSTATE pState = pProfile->sCharState;
		pState.sCharStateBase.byStateID = CHARSTATE_TELEPORTING;
		pState.sCharStateDetail.sCharStateTeleporting.byTeleportType = TELEPORT_TYPE_NPC_PORTAL;//dont know if is dungeont or npc however we progress
		SendCharStateUpdate(pProfile->GetSerialID(), pState);//just relax
	}
	if (pData->tcNextId == 254)
	{
		sWORLD_TBLDAT* pTablDat = reinterpret_cast<sWORLD_TBLDAT*>(wolrTabl->FindData(pData->tId));
		SendCharTeleport(pTablDat->tblidx, TELEPORT_TYPE_NPC_PORTAL);
	}*/
}

void GameClient::UpdateCharObjEquips(HOBJECT hItem, BYTE byPos)
{
	sGU_UPDATE_ITEM_EQUIP sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_UPDATE_ITEM_EQUIP;
	sPkt.byPos = byPos;
	sPkt.handle = pProfile->GetSerialID();
	pProfile->GetItemBrief(sPkt.sItemBrief, hItem);
	pServer->GetClientManager()->SendOthers(&sPkt, sizeof(sPkt), this);
}

void GameClient::SendIemMoveRes(sUG_ITEM_MOVE_REQ* pData)
{
	WORD wResult = eRESULTCODE::GAME_SUCCESS;
	sITEM_TBLDAT* sSrcTbldat = NULL;
	sITEM_TBLDAT* sDestTbldat = NULL;

	sPC_TBLDAT pcdata = *(sPC_TBLDAT*)pServer->GetTableContainer()->GetPcTable()->GetPcTbldat(pProfile->GetRace(), pProfile->GetClass(), pProfile->GetGender());

	HOBJECT hSrcItem = pProfile->GetInventoryItemSerialID(pData->bySrcPlace, pData->bySrcPos);
	HOBJECT hDestItem = pProfile->GetInventoryItemSerialID(pData->byDestPlace, pData->byDestPos);
	TBLIDX SrcTblidx = pProfile->GetInventoryItemID(pData->bySrcPlace, pData->bySrcPos);
	TBLIDX DestTblidx = pProfile->GetInventoryItemID(pData->byDestPlace, pData->byDestPos);
	sITEM_BRIEF pSrcItemData;
	pProfile->GetItemBrief(pSrcItemData, hSrcItem);
	sITEM_BRIEF pDestItemData;
	pProfile->GetItemBrief(pDestItemData, hDestItem);

	if (SrcTblidx != INVALID_TBLIDX)
		sSrcTbldat = (sITEM_TBLDAT*)pServer->GetTableContainer()->GetItemTable()->FindData(SrcTblidx);
	if (DestTblidx != INVALID_TBLIDX)
		sDestTbldat = (sITEM_TBLDAT*)pServer->GetTableContainer()->GetItemTable()->FindData(DestTblidx);

	// Check equipment requeriments
	if ((pData->byDestPlace == CONTAINER_TYPE_EQUIP) &&
		((pData->byDestPos >= EQUIP_SLOT_TYPE_FIRST) && (pData->byDestPos <= EQUIP_SLOT_TYPE_LAST)))
	{
		if (sSrcTbldat)
		{
			if ((sSrcTbldat->byClass_Special != pProfile->GetClass() && sSrcTbldat->byClass_Special != INVALID_BYTE) ||
				(sSrcTbldat->byRace_Special != pProfile->GetRace() && sSrcTbldat->byRace_Special != INVALID_BYTE))
			{
				if (sSrcTbldat->dwNeed_Class_Bit_Flag != pcdata.dwClass_Bit_Flag && sSrcTbldat->dwNeed_Class_Bit_Flag != INVALID_DWORD)
					wResult = eRESULTCODE::GAME_ITEM_CLASS_FAIL;
			}
			else if (sSrcTbldat->byNeed_Level > pProfile->sPcProfile.byLevel)
				wResult = eRESULTCODE::GAME_ITEM_NEED_MORE_LEVEL;
			else if (sSrcTbldat->byNeed_Con > pProfile->sPcProfile.avatarAttribute.byBaseCon && sSrcTbldat->byNeed_Con != INVALID_BYTE)
				wResult = eRESULTCODE::GAME_ITEM_NEED_MORE_PARAMETER;
			else if (sSrcTbldat->byNeed_Dex > pProfile->sPcProfile.avatarAttribute.byBaseDex && sSrcTbldat->byNeed_Dex != INVALID_BYTE)
				wResult = eRESULTCODE::GAME_ITEM_NEED_MORE_PARAMETER;
			else if (sSrcTbldat->byNeed_Eng > pProfile->sPcProfile.avatarAttribute.byBaseEng && sSrcTbldat->byNeed_Eng != INVALID_BYTE)
				wResult = eRESULTCODE::GAME_ITEM_NEED_MORE_PARAMETER;
			else if (sSrcTbldat->byNeed_Foc > pProfile->sPcProfile.avatarAttribute.byBaseFoc && sSrcTbldat->byNeed_Foc != INVALID_BYTE)
				wResult = eRESULTCODE::GAME_ITEM_NEED_MORE_PARAMETER;
			else if (sSrcTbldat->byNeed_Sol > pProfile->sPcProfile.avatarAttribute.byBaseSol && sSrcTbldat->byNeed_Sol != INVALID_BYTE)
				wResult = eRESULTCODE::GAME_ITEM_NEED_MORE_PARAMETER;
			else if (sSrcTbldat->byNeed_Str > pProfile->sPcProfile.avatarAttribute.byBaseStr && sSrcTbldat->byNeed_Str != INVALID_BYTE)
				wResult = eRESULTCODE::GAME_ITEM_NEED_MORE_PARAMETER;
		}
	}

	if (wResult == eRESULTCODE::GAME_SUCCESS)
	{
		// Update CHARACTER Obj Model Equiped Item (Other Players Char Object Spawn)
		if ((pData->byDestPlace == CONTAINER_TYPE_EQUIP) &&
			((pData->byDestPos >= EQUIP_SLOT_TYPE_FIRST) && (pData->byDestPos <= EQUIP_SLOT_TYPE_LAST)))
		{
			UpdateCharObjEquips(hSrcItem, pData->byDestPos);
			if ((pData->byDestPos == EQUIP_SLOT_TYPE_EARRING_1) ||
				(pData->byDestPos == EQUIP_SLOT_TYPE_EARRING_2) ||
				(pData->byDestPos == EQUIP_SLOT_TYPE_SUB_WEAPON) ||
				(pData->byDestPos == EQUIP_SLOT_TYPE_HAND) ||
				(pData->byDestPos == EQUIP_SLOT_TYPE_JACKET) ||
				(pData->byDestPos == EQUIP_SLOT_TYPE_PANTS) ||
				(pData->byDestPos == EQUIP_SLOT_TYPE_BOOTS) ||
				(pData->byDestPos == EQUIP_SLOT_TYPE_NECKLACE) ||
				(pData->byDestPos == EQUIP_SLOT_TYPE_RING_1) ||
				(pData->byDestPos == EQUIP_SLOT_TYPE_RING_2))
			{
				pProfile->GetAttributeManager()->LoadAttribute(&pProfile->sPcProfile.avatarAttribute);
				if (pDestItemData.tblidx != INVALID_TBLIDX)
					pProfile->GetAttributeManager()->UpdateWithEquipment(sDestTbldat, false, pDestItemData.byGrade);
				else
					pProfile->GetAttributeManager()->UpdateWithEquipment(sSrcTbldat, false, pSrcItemData.byGrade);
				sGU_AVATAR_ATTRIBUTE_UPDATE pPacket = pProfile->GetAttributeManager()->PrepareUpdatePacket(pProfile->GetSerialID());
				Send(&pPacket, sizeof(pPacket));
				pProfile->sPcProfile.avatarAttribute = pProfile->GetAttributeManager()->GetAvatarAttribute();
			}
		}

		if ((pData->bySrcPlace == CONTAINER_TYPE_EQUIP) &&
			((pData->bySrcPos >= EQUIP_SLOT_TYPE_FIRST) && (pData->bySrcPos <= EQUIP_SLOT_TYPE_LAST)))
		{
			UpdateCharObjEquips(hDestItem, pData->bySrcPos);
			if ((pData->bySrcPos == EQUIP_SLOT_TYPE_EARRING_1) ||
				(pData->bySrcPos == EQUIP_SLOT_TYPE_EARRING_2) ||
				(pData->bySrcPos == EQUIP_SLOT_TYPE_SUB_WEAPON) ||
				(pData->bySrcPos == EQUIP_SLOT_TYPE_HAND) ||
				(pData->bySrcPos == EQUIP_SLOT_TYPE_JACKET) ||
				(pData->bySrcPos == EQUIP_SLOT_TYPE_PANTS) ||
				(pData->bySrcPos == EQUIP_SLOT_TYPE_BOOTS) ||
				(pData->bySrcPos == EQUIP_SLOT_TYPE_NECKLACE) ||
				(pData->bySrcPos == EQUIP_SLOT_TYPE_RING_1) ||
				(pData->bySrcPos == EQUIP_SLOT_TYPE_RING_2))
			{
				pProfile->GetAttributeManager()->LoadAttribute(&pProfile->sPcProfile.avatarAttribute);
				if (pDestItemData.tblidx != INVALID_TBLIDX)
					pProfile->GetAttributeManager()->UpdateWithEquipment(sDestTbldat, true, pDestItemData.byGrade);
				else
					pProfile->GetAttributeManager()->UpdateWithEquipment(sSrcTbldat, true, pSrcItemData.byGrade);
				sGU_AVATAR_ATTRIBUTE_UPDATE pPacket = pProfile->GetAttributeManager()->PrepareUpdatePacket(pProfile->GetSerialID());
				Send(&pPacket, sizeof(pPacket));
				pProfile->sPcProfile.avatarAttribute = pProfile->GetAttributeManager()->GetAvatarAttribute();
			}
		}

		if ((pData->byDestPlace == CONTAINER_TYPE_SCOUT) &&
			(pData->byDestPos == EQUIP_SLOT_TYPE_SCOUTER))
		{
			UpdateCharObjEquips(hSrcItem, pData->byDestPos);
			//Do here
		}
		if ((pData->bySrcPlace == CONTAINER_TYPE_SCOUT) &&
			(pData->bySrcPos == EQUIP_SLOT_TYPE_SCOUTER))
		{
			UpdateCharObjEquips(hDestItem, pData->bySrcPos);
			//Do here
		}
		

		// Update DB
		if (hSrcItem != INVALID_HOBJECT)
		{
			pProfile->UpdateItemInventoryPosition(hSrcItem, pData->byDestPlace, pData->byDestPos);
		}
		if (hDestItem != INVALID_HOBJECT)
		{
			pProfile->UpdateItemInventoryPosition(hDestItem, pData->bySrcPlace, pData->bySrcPos);
		}
	}

	sGU_ITEM_MOVE_RES sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_ITEM_MOVE_RES;
	sPkt.byDestPlace = pData->byDestPlace;
	sPkt.byDestPos = pData->byDestPos;
	sPkt.bySrcPlace = pData->bySrcPlace;
	sPkt.bySrcPos = pData->bySrcPos;
	sPkt.hSrcItem = hSrcItem;
	sPkt.hDestItem = hDestItem;
	sPkt.wResultCode = wResult;
	Send(&sPkt, sizeof(sPkt));
}

void GameClient::SendGameExitRes()
{
	sGU_GAME_EXIT_RES sRes;
	memset(&sRes, 0, sizeof(sRes));
	sRes.wOpCode = GU_CHAR_EXIT_RES;
	Send(&sRes, sizeof(sRes));
}

void GameClient::SendCharExitRes()
{
	sGU_CHAR_EXIT_RES sRes;
	memset(&sRes, 0, sizeof(sRes));
	sRes.wOpCode = GU_CHAR_EXIT_RES;
	sRes.wResultCode = eRESULTCODE::GAME_SUCCESS;
	memcpy(sRes.achAuthKey, pProfile->GetAuthkey(), NTL_MAX_SIZE_AUTH_KEY);

	// servers
	sRes.byServerInfoCount = pServer->ServerCfg->GetInt("CharServerList", "Count");
	for (int x = 0; x < sRes.byServerInfoCount; x++)
	{
		char snode[20];
		sprintf_s(snode, "CharServer%d", x + 1);
		memcpy(sRes.aServerInfo[x].szCharacterServerIP, pServer->ServerCfg->GetChildStr("CharServerList", snode, "IP"), NTL_MAX_LENGTH_OF_IP);
		sRes.aServerInfo[x].wCharacterServerPortForClient = pServer->ServerCfg->GetChildInt("CharServerList", snode, "Port");
		sRes.aServerInfo[x].dwLoad = 0;
	}
	Send(&sRes, sizeof(sRes));
}

void GameClient::SendGameLeaveRes()
{
	Logger::Log("Client[%u] exit game\n", this);
	sGU_GAME_LEAVE_RES glRes;
	memset(&glRes, 0, sizeof(glRes));
	glRes.wOpCode = GU_GAME_LEAVE_RES;
	glRes.wResultCode = eRESULTCODE::GAME_SUCCESS;
	Send(&glRes, sizeof(glRes));
}

void GameClient::SendAuthkeyCommSrvRes()
{
	sGU_AUTH_KEY_FOR_COMMUNITY_SERVER_RES auRes;
	memset(&auRes, 0, sizeof(auRes));
	auRes.wOpCode = GU_AUTH_KEY_FOR_COMMUNITY_SERVER_RES;
	memcpy(auRes.abyAuthKey, pProfile->GetAuthkey(), NTL_MAX_SIZE_AUTH_KEY);
	auRes.wResultCode = eRESULTCODE::GAME_SUCCESS;
	Send(&auRes, sizeof(auRes));
}

void GameClient::SendAvatarWarFogInfo()
{
	sGU_WAR_FOG_INFO sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_WAR_FOG_INFO;
	pProfile->LoadWarFogFlags();
	memcpy(sPkt.abyWarFogInfo, pProfile->acWarFogFlag, NTL_MAX_SIZE_WAR_FOG);
	Send(&sPkt, sizeof(sPkt));
}

void GameClient::SendCharBuffsInfo()
{
	sGU_AVATAR_BUFF_INFO bInfo;
	memset(&bInfo, 0, sizeof(bInfo));
	bInfo.wOpCode = GU_AVATAR_BUFF_INFO;
	bInfo.byBuffCount = 0;
	Send(&bInfo, sizeof(bInfo));
}

void GameClient::SpawnTesteMob(unsigned int id)
{
	MobProfile* mob = new MobProfile(pProfile->sWorldInfo.tblidx, 0, id, true);
	mob->Init();

	mob->sCharState.sCharStateBase.vCurLoc = pProfile->sCharState.sCharStateBase.vCurLoc;
	mob->sCharState.sCharStateBase.vCurLoc.x = +(float)(rand() % 5);
	mob->sCharState.sCharStateBase.vCurLoc.z = +(float)(rand() % 5);

	mob->sCharState.sCharStateBase.vCurDir.x = 0.0f;
	mob->sCharState.sCharStateBase.vCurDir.y = 0.0f;
	mob->sCharState.sCharStateBase.vCurDir.z = 1.0f;

	if (false == pServer->GetObjectManager()->FindObject(mob->GetSerialID(), eOBJTYPE::OBJTYPE_MOB))
	{
		pServer->GetObjectManager()->AddObject(mob->GetSerialID(), mob, eOBJTYPE::OBJTYPE_MOB);
	}
}

void GameClient::CheckCommand(sUG_SERVER_COMMAND* pData)
{
	GameString command(pData->awchCommand);

	Logger::Log("%s typed command: %s\n", GameString(pProfile->GetCharname()).c_str(), command.c_str());

	std::vector<std::string> tok;
	command.GetToken(tok, ' ');

	if (tok.size() != 0 && pProfile->GetGameMaster())
	{
		if (strcmp(tok[0].c_str(), "@teste") == 0)
		{
			// TEST COMMANDS
		}
		if (strcmp(tok[0].c_str(), "@addcon") == 0)
		{
			if (tok.size() >= 1)
			{
				BYTE byToUp = 1;
				if (tok.size() == 2) byToUp = atoi(tok[1].c_str());
				pProfile->sPcProfile.avatarAttribute.byLastCon += byToUp;
				pProfile->GetAttributeManager()->LoadAttribute(&pProfile->sPcProfile.avatarAttribute);
				sGU_AVATAR_ATTRIBUTE_UPDATE pPacket = pProfile->GetAttributeManager()->PrepareUpdatePacket(pProfile->GetSerialID());
				Send(&pPacket, sizeof(pPacket));
			}
		}
		if (strcmp(tok[0].c_str(), "@questcomplete") == 0)
		{
			if (tok.size() > 1)
			{
				QUESTID qId = atoi(tok[1].c_str());
				sGU_QUEST_FORCED_COMPLETION_NFY sPkt;
				memset(&sPkt, 0, sizeof(sPkt));
				sPkt.wOpCode = GU_QUEST_FORCED_COMPLETION_NFY;
				sPkt.questId = qId;
				Send(&sPkt, sizeof(sPkt));
			}
		}
		if (strcmp(tok[0].c_str(), "@levelup") == 0)
		{
			if (tok.size() >= 1)
			{
				BYTE byToUp = 1;
				if (tok.size() == 2) byToUp = atoi(tok[1].c_str());
				SendCharLevelUp(byToUp);
			}
		}
		if (strcmp(tok[0].c_str(), "@additem") == 0)
		{
			if (tok.size() > 1)
			{
				BYTE qtd = 1;
				ITEMID item = atoi(tok[1].c_str());
				if (tok.size() == 3) qtd = atoi(tok[2].c_str());
				sGU_ITEM_CREATE sPkt;
				if (pProfile->InsertNextBagSlot(sPkt, item, qtd))
				{
					Send(&sPkt, sizeof(sPkt));
				}
				else
				{
					SendSystemText("Invalid Item Index");
				}
			}
		}
		if (strcmp(tok[0].c_str(), "@setspeed") == 0)
		{
			if (tok.size() > 1)
			{
				float speed = (float)atof(tok[1].c_str());
				pProfile->sPcProfile.avatarAttribute.fLastRunSpeed = speed;
				sGU_UPDATE_CHAR_SPEED sPkt;
				memset(&sPkt, 0, sizeof(sPkt));
				sPkt.wOpCode = GU_UPDATE_CHAR_SPEED;
				sPkt.handle = pProfile->GetSerialID();
				sPkt.fLastRunningSpeed = sPkt.fLastWalkingSpeed = speed;
				pServer->GetClientManager()->SendAll2(&sPkt, sizeof(sPkt), this);
			}
		}
		if (strcmp(tok[0].c_str(), "@spawnmob") == 0)
		{
			if (tok.size() > 1)
			{
				SpawnTesteMob(atoi(tok[1].c_str()));
			}
		}
		if (strcmp(tok[0].c_str(), "@notice") == 0)
		{
			if (tok.size() > 1)
			{
				std::string msg = "";
				for (unsigned int i = 1; i < tok.size(); i++)
				{
					msg.append(tok[i] + " ");
				}
				SendSystemText(GameString(msg.c_str()), SERVER_TEXT_SYSNOTICE);
			}
		}
		if (strcmp(tok[0].c_str(), "@emergency") == 0)
		{
			if (tok.size() > 1)
			{
				std::string msg = "";
				for (unsigned int i = 1; i < tok.size(); i++)
				{
					msg.append(tok[i] + " ");
				}
				SendSystemText(GameString(msg.c_str()), SERVER_TEXT_EMERGENCY);
			}
		}
		if (strcmp(tok[0].c_str(), "@cashshop") == 0)
		{
			SendNetpyItemStartRes();
		}
		if (strcmp(tok[0].c_str(), "@eventshop") == 0)
		{
			SendEventItemStartRes();
		}
	}
}

void GameClient::SendCharReadyRes(sUG_CHAR_READY* pData)
{
	pProfile->SetAvatartype(pData->byAvatarType);
	pProfile->RemoveOnlineData();
	pProfile->InsertOnlineData();

	bIsSpawnReady = true;
	bIsReadyToUpdate = true;
}

void GameClient::SendCharReadySpawnReq()
{
	if (pServer->GetClientManager()->AddClient(this))
	{
		pServer->GetObjectManager()->AddObject(pProfile->GetSerialID(), pProfile, eOBJTYPE::OBJTYPE_PC);
	}
}

void GameClient::SendGameEnterRes(sUG_GAME_ENTER_REQ* data)
{
	pProfile->Init();
	pProfile->SetAccountid(data->accountId);
	pProfile->SetCharid(data->charId);
	pProfile->SetTutorialmode(data->bTutorialMode);
	memcpy(pProfile->GetAuthkey(), data->abyAuthKey, NTL_MAX_SIZE_AUTH_KEY);
	pServer->ServerDB->ExecuteQuery("UPDATE `character` SET `TutorialFlag` = '1' WHERE `ID` = '%u';", pProfile->GetCharID());

	Logger::Log("Client[%d] entering game (%d|%d) handle[%d]\n", this, pProfile->GetAccountid(), pProfile->GetCharID(), pProfile->GetSerialID());

	sGU_GAME_ENTER_RES geRes;
	memset(&geRes, 0, sizeof(geRes));
	geRes.wOpCode = GU_GAME_ENTER_RES;
	memcpy(geRes.achCommunityServerIP, pServer->chatServerIP, NTL_MAX_LENGTH_OF_IP);
	geRes.wCommunityServerPort = pServer->chatServerPort;
	geRes.timeDBOEnter = time(NULL);
	geRes.wResultCode = eRESULTCODE::GAME_SUCCESS;
	Send((unsigned char*)&geRes, sizeof(geRes));
}

void GameClient::SendCharInfoEnd()
{
	sGU_AVATAR_INFO_END iEnd;
	memset(&iEnd, 0, sizeof(iEnd));
	iEnd.wOpCode = GU_AVATAR_INFO_END;
	Send((unsigned char*)&iEnd, sizeof(iEnd));
}

void GameClient::SendCharInfo()
{
	sGU_AVATAR_CHAR_INFO charInfo;
	memset(&charInfo, 0, sizeof(charInfo));
	charInfo.wOpCode = GU_AVATAR_CHAR_INFO;

	pProfile->LoadCharacterData();
	pProfile->CalculateAtributes();

	// Current LP/EP (TODO: Load from DB)
	pProfile->sPcProfile.wCurLP = pProfile->sPcProfile.avatarAttribute.wBaseMaxLP;
	pProfile->sPcProfile.wCurEP = pProfile->sPcProfile.avatarAttribute.wBaseMaxEP;

	charInfo.handle = pProfile->GetSerialID();
	memcpy(&charInfo.sPcProfile, &pProfile->sPcProfile, sizeof(pProfile->sPcProfile));
	memcpy(&charInfo.sCharState, &pProfile->sCharState, sizeof(pProfile->sCharState));
	charInfo.wCharStateSize = sizeof(pProfile->sCharState);
	Send((unsigned char*)&charInfo, sizeof(charInfo));
}

void GameClient::SendCharWorldInfo()
{
	sGU_AVATAR_WORLD_INFO wInfo;
	memset(&wInfo, 0, sizeof(wInfo));
	wInfo.wOpCode = GU_AVATAR_WORLD_INFO;
	pProfile->LoadWorldInfoData();
	memcpy(&wInfo.worldInfo, &pProfile->sWorldInfo, sizeof(pProfile->sWorldInfo));

	// TO DO: TUTOTIAL MODE
	/*if (TutorialMode)
	{
	sNEWBIE_TBLDAT* pTbldat = (sNEWBIE_TBLDAT*)pServer->GetTableContainer()->GetNewbieTable()->GetNewbieTbldat(byRace, byClass);
	wInfo.worldInfo.tblidx = pTbldat->tutorialWorld;
	wInfo.worldInfo.worldID = pTbldat->world_Id;
	CharState.sCharStateBase.vCurLoc.x = (-1)*78.90;
	CharState.sCharStateBase.vCurLoc.y = 46.95;
	CharState.sCharStateBase.vCurLoc.z = (-1) * 168.35;
	CharState.sCharStateBase.vCurDir.x = (-1) * 0.95;
	CharState.sCharStateBase.vCurDir.y = 0;
	CharState.sCharStateBase.vCurDir.z = 0.30;
	}*/

	wInfo.vCurDir = pProfile->sCharState.sCharStateBase.vCurDir;
	wInfo.vCurLoc = pProfile->sCharState.sCharStateBase.vCurLoc;
	Send((unsigned char*)&wInfo, sizeof(wInfo));
}

void GameClient::SendCharWorldInfoEnd()
{
	sGU_ENTER_WORLD_COMPLETE entWorld;
	memset(&entWorld, 0, sizeof(entWorld));
	entWorld.wOpCode = GU_ENTER_WORLD_COMPLETE;
	Send((unsigned char*)&entWorld, sizeof(entWorld));
}

void GameClient::SendCharJump(sUG_CHAR_JUMP* pData)
{
	sGU_CHAR_JUMP pJump;
	memset(&pJump, 0, sizeof(pJump));
	pJump.wOpCode = GU_CHAR_JUMP;
	pJump.handle = pProfile->GetSerialID();
	pJump.byMoveDirection = pProfile->GetMovedirection();
	pJump.vCurrentHeading = pData->vCurrentHeading;
	pJump.vJumpDir = pProfile->sCharState.sCharStateBase.vCurDir;
	pServer->GetClientManager()->SendOthers(&pJump, sizeof(pJump), this);
}

void GameClient::SendCharJumpEnd()
{
	sGU_CHAR_JUMP_END pJEnd;
	pJEnd.handle = pProfile->GetSerialID();
	pJEnd.wOpCode = GU_CHAR_JUMP_END;
	pServer->GetClientManager()->SendOthers(&pJEnd, sizeof(pJEnd), this);
}

void GameClient::SendCharMove(sUG_CHAR_MOVE* data)
{
	sGU_CHAR_MOVE mData;
	memset(&mData, 0, sizeof(mData));
	mData.wOpCode = GU_CHAR_MOVE;
	if (data->byAvatarType == pProfile->GetAvatartype())
	{
		pProfile->bIsMoving = true;
		pProfile->UpdatePositions(data->vCurDir, data->vCurLoc);
		mData.vCurDir.x = data->vCurDir.x;
		mData.vCurDir.y = 0.0f;
		mData.vCurDir.z = data->vCurDir.z;
		mData.vCurLoc.x = data->vCurLoc.x;
		mData.vCurLoc.y = data->vCurLoc.y;
		mData.vCurLoc.z = data->vCurLoc.z;
		mData.handle = pProfile->GetSerialID();
		mData.byMoveDirection = data->byMoveDirection;
		pProfile->SetMovedirection(data->byMoveDirection);
	}
	pServer->GetClientManager()->SendOthers(&mData, sizeof(mData), this);
}

void GameClient::SendCharDestMove(sUG_CHAR_DEST_MOVE* pData)
{
	sGU_CHAR_DEST_MOVE dMove;
	memset(&dMove, 0, sizeof(dMove));
	dMove.wOpCode = GU_CHAR_DEST_MOVE;
	if (pData->byAvatarType == pProfile->GetAvatartype())
	{
		pProfile->bIsMoving = false;
		dMove.vCurLoc = pData->vCurLoc;
		dMove.handle = pProfile->GetSerialID();
		dMove.byDestLocCount = 1;
		dMove.avDestLoc[0] = pData->vDestLoc;
		dMove.dwTimeStamp = pData->dwTimeStamp;
		dMove.byMoveFlag = 0; // to do run or not
	}
	pServer->GetClientManager()->SendOthers(&dMove, sizeof(dMove), this);
}

void GameClient::SendCharMoveSync(sUG_CHAR_MOVE_SYNC* pData)
{
	pProfile->UpdatePositions(pData->vCurDir, pData->vCurLoc);
	pServer->GetObjectManager()->UpdateCharState(pProfile->GetSerialID(), pProfile->sCharState);
	sGU_CHAR_MOVE_SYNC cmSync;
	memset(&cmSync, 0, sizeof(cmSync));
	cmSync.wOpCode = GU_CHAR_MOVE_SYNC;
	if (pData->byAvatarType == pProfile->GetAvatartype())
	{
		cmSync.handle = pProfile->GetSerialID();
		cmSync.vCurDir = pData->vCurDir;
		cmSync.vCurLoc = pData->vCurLoc;
		cmSync.dwTimeStamp = pData->dwTimeStamp;
	}
	pServer->GetClientManager()->SendOthers(&cmSync, sizeof(cmSync), this);
}

void GameClient::SendCharChangeHeading(sUG_CHAR_CHANGE_HEADING* pData)
{
	sGU_CHAR_CHANGE_HEADING pCHead;
	memset(&pCHead, 0, sizeof(pCHead));
	pCHead.wOpCode = GU_CHAR_CHANGE_HEADING;
	pCHead.handle = pProfile->GetSerialID();
	pCHead.dwTimeStamp = pData->dwTimeStamp;
	pCHead.vNewHeading = pData->vCurrentHeading;
	pCHead.vNewLoc = pData->vCurrentPosition;
	pServer->GetClientManager()->SendOthers(&pCHead, sizeof(pCHead), this);
}

void GameClient::SendCharItemInfo()
{
	sGU_AVATAR_ITEM_INFO iInfo;
	memset(&iInfo, 0, sizeof(iInfo));
	iInfo.wOpCode = GU_AVATAR_ITEM_INFO;
	iInfo.byItemCount = pProfile->LoadItemData();
	memcpy(iInfo.aItemProfile, pProfile->asItemProfile, sizeof(pProfile->asItemProfile));
	int psize = 3;
	psize += sizeof(sITEM_PROFILE) * iInfo.byItemCount;
	Send(&iInfo, psize);
}

void GameClient::SendCharSkillInfo()
{
	sGU_AVATAR_SKILL_INFO sInfo;
	memset(&sInfo, 0, sizeof(sInfo));
	sInfo.wOpCode = GU_AVATAR_SKILL_INFO;
	sInfo.bySkillCount = pProfile->LoadSkillData();
	memcpy(sInfo.aSkillInfo, pProfile->asSkillInfo, sizeof(pProfile->asSkillInfo));
	int psize = 3;
	psize += sizeof(sSKILL_INFO) * sInfo.bySkillCount;
	Send(&sInfo, psize);
}
//By Luiz45 - HTB Info Loader
void GameClient::SendCharHTBInfo()
{
	sGU_AVATAR_HTB_INFO sInfo;
	memset(&sInfo, 0, sizeof(sInfo));
	sInfo.wOpCode = GU_AVATAR_HTB_INFO;
	sInfo.byHTBSkillCount = pProfile->LoadHTBData();
	memcpy(sInfo.aHTBSkillnfo, pProfile->asHTBInfo, sizeof(pProfile->asHTBInfo));
	int psize = 3;
	psize += sizeof(sHTB_SKILL_INFO)*sInfo.byHTBSkillCount;
	Send(&sInfo, psize);
}

void GameClient::SendCharQuickSlotInfo()
{
	sGU_QUICK_SLOT_INFO qsInfo;
	memset(&qsInfo, 0, sizeof(qsInfo));
	qsInfo.wOpCode = GU_QUICK_SLOT_INFO;
	qsInfo.byQuickSlotCount = pProfile->LoadQuickslotData();
	memcpy(qsInfo.asQuickSlotData, pProfile->asQuickSlotData, sizeof(pProfile->asQuickSlotData));
	int psize = 3;
	psize += sizeof(sQUICK_SLOT_PROFILE) * qsInfo.byQuickSlotCount;
	Send(&qsInfo, psize);
}

void GameClient::SendSystemText(char* szText, ...)
{
	va_list args;
	va_start(args, szText);
	char text[NTL_MAX_LENGTH_OF_CHAT_MESSAGE_UNICODE];
	vsprintf_s(text, szText, args);
	SendSystemText(GameString(text), SERVER_TEXT_SYSTEM);
	va_end(args);
}

void GameClient::SendSystemText(GameString msg, eSERVER_TEXT_TYPE type)
{
	sGU_SYSTEM_DISPLAY_TEXT sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.wOpCode = GU_SYSTEM_DISPLAY_TEXT;
	memcpy(sPkt.awGMChar, pProfile->GetCharname(), NTL_MAX_SIZE_CHAR_NAME_UNICODE);
	sPkt.byDisplayType = type;
	sPkt.wMessageLengthInUnicode = NTL_MAX_LENGTH_OF_CHAT_MESSAGE_UNICODE;
	memcpy(sPkt.awchMessage, msg.wc_str(), sPkt.wMessageLengthInUnicode);
	pServer->GetClientManager()->SendAll2(&sPkt, sizeof(sPkt), this);
}
//By Luiz45 sitDown
void GameClient::SendToggleSitDown()
{
	sGU_CHAR_SITDOWN sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.handle = pProfile->GetSerialID();
	sPkt.wOpCode = GU_CHAR_SITDOWN;
	Send(&sPkt, sizeof(sPkt));
	sCHARSTATE pSitState = pProfile->sCharState;
	pSitState.sCharStateBase.byStateID = CHARSTATE_SITTING;
	SendCharStateUpdate(pProfile->GetSerialID(), pSitState);
}
//By Luiz45 StantUp
void GameClient::SendToggleStandUp()
{
	sGU_CHAR_STANDUP sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.handle = pProfile->GetSerialID();
	sPkt.wOpCode = GU_CHAR_STANDUP;
	Send(&sPkt, sizeof(sPkt));
	sCHARSTATE pStandUpState = pProfile->sCharState;
	pStandUpState.sCharStateBase.byStateID = CHARSTATE_STANDING;
	SendCharStateUpdate(pProfile->GetSerialID(), pStandUpState);
}
//By Luiz45 ToggRunning(AutoRun)
void GameClient::SendToggleRunning(sUG_CHAR_TOGG_RUNNING* pData)
{
	//Who Know's what gonna here?
}
//By Luiz45 - Target Info
void GameClient::SendTargetInfo(sUG_CHAR_TARGET_INFO* pData)
{
	sGU_UPDATE_TARGET_INFO sPkt;
}
//By Luiz45 - Direct Play ACK(Acknologement), in general this will be used in Tutorial and in some parts of TLQ/TMQ
void GameClient::SendCharDirectPlay(sUG_CHAR_DIRECT_PLAY_ACK* pData)
{
	sGU_CHAR_DIRECT_PLAY sPkt;
	memset(&sPkt, 0, sizeof(sPkt));
	sPkt.bSynchronize = false;//Sync between Server-Client(Maybe this will be not necessary)
	sPkt.byPlayMode = true;
	sPkt.hSubject = pProfile->GetSerialID();
	sPkt.directTblidx = 1;
	sPkt.wOpCode = GU_CHAR_DIRECT_PLAY;
	Send(&sPkt, sizeof(sPkt));
}
//By Luiz45 - Private shop Create
void GameClient::SendPrivateShopCreate(sUG_PRIVATESHOP_CREATE_REQ* pData)
{
	sGU_PRIVATESHOP_CREATE_RES sPktReply;
	sGU_PRIVATESHOP_CREATE_NFY sPktReplyNFY;//This will be notify all connected clients
	//Client Answer
	sPktReply.sPrivateShopData.hOwner = pProfile->GetSerialID();
	sPktReply.sPrivateShopData.sSummaryPrivateShopData.bIsOwnerEmpty = false;
	sPktReply.sPrivateShopData.sSummaryPrivateShopData.byShopState = ePRIVATESHOP_STATE::PRIVATESHOP_STATE_BUSINESS_WAIT;
	sPktReply.wResultCode = GAME_SUCCESS;
	sPktReply.wOpCode = GU_PRIVATESHOP_CREATE_RES;
	Send(&sPktReply, sizeof(sPktReply));
	//Notify
	sPktReplyNFY.sSummaryPrivateShopData.byShopState = ePRIVATESHOP_STATE::PRIVATESHOP_STATE_CLOSE;
	sPktReplyNFY.hOwner = pProfile->GetSerialID();
	sPktReplyNFY.wOpCode = GU_PRIVATESHOP_CREATE_NFY;
	pServer->GetClientManager()->SendAll2(&sPktReplyNFY, sizeof(sPktReplyNFY), this);
}
//By Luiz45 - Char Follow Move
void GameClient::SendCharFollowMove(sUG_CHAR_FOLLOW_MOVE* pData)
{
	sGU_CHAR_FOLLOW_MOVE pMove;
	memset(&pMove, 0, sizeof(pMove));
	pMove.byMoveFlag = ENtlMovementFlag::NTL_MOVE_FLAG_RUN;
	pMove.byMovementReason = pData->byMovementReason;
	pMove.dwTimeStamp = pData->dwTimeStamp;
	pMove.handle = pProfile->GetSerialID();
	pMove.hTarget = pData->hTarget;
	pMove.fDistance = pData->fDistance;
	pMove.wOpCode = GU_CHAR_FOLLOW_MOVE;
	pServer->GetClientManager()->SendOthers(&pMove, sizeof(pMove), this);
}
//By Luiz45 - Char Change Direction on Floating
void GameClient::SendCharChangeDirectionFloating(sUG_CHAR_CHANGE_DIRECTION_ON_FLOATING* pData)
{
	sGU_CHAR_CHANGE_DIRECTION_ON_FLOATING pRes;
	memset(&pRes, 0, sizeof(pRes));
	pRes.byMoveDirection = pData->byMoveDirection;
	pRes.dwTimeStamp = pData->byMoveDirection;
	pRes.vCurDir = pData->vCurDir;
	pRes.wOpCode = GU_CHAR_CHANGE_DIRECTION_ON_FLOATING;
	pRes.hSubject = pProfile->GetSerialID();
	pServer->GetClientManager()->SendOthers(&pRes, sizeof(pRes), this);
}
//By Luiz45 - Char Teleport
void GameClient::SendPortalStart(sUG_PORTAL_START_REQ* pData)
{
	sGU_PORTAL_START_RES pRes;
	memset(&pRes, 0, sizeof(pRes));
	pRes.hNpcHandle = pData->handle;
	pRes.wOpCode = GU_PORTAL_START_RES;
	pRes.wResultCode = GAME_SUCCESS;
	Send(&pRes, sizeof(pRes));

	sGU_PORTAL_INFO pRes2;
	memset(&pRes2, 0, sizeof(pRes2));
	PortalTable* pPortalTbl = pServer->GetTableContainer()->GetPortalTable();
	int i = 0;
	for (Table::TABLEIT itPortal = pPortalTbl->Begin(); itPortal != pPortalTbl->End(); ++itPortal)
	{
		sPORTAL_TBLDAT* pPortalTblData = (sPORTAL_TBLDAT*)itPortal->second;
		pRes2.aPortalID[i] = pPortalTblData->tblidx;
		pRes2.byCount = i;
		i++;
	}
	pRes2.wOpCode = GU_PORTAL_INFO;
	Send(&pRes2, sizeof(pRes2));
}
//By Luiz45 - Char Add Teleport
void GameClient::SendPortalAdd(sUG_PORTAL_ADD_REQ* pData)
{
	sGU_PORTAL_ADD_RES pRes;
	memset(&pRes, 0, sizeof(pRes));
	pRes.hNpcHandle = pData->handle;
	pRes.PortalID = 255;
	pRes.wOpCode = GU_PORTAL_ADD_RES;
	pRes.wResultCode = GAME_SUCCESS;
	Send(&pRes, sizeof(pRes));
}
//By Luiz45 - Char Portal Req
void GameClient::SendPortalReq(sUG_PORTAL_REQ* pData)
{
	sGU_PORTAL_RES pRes;
	memset(&pRes, 0, sizeof(pRes));
	pRes.byPoint = pData->byPoint;
	pRes.hNpcHandle = pData->handle;
	pRes.wOpCode = GU_PORTAL_RES;
	int iMyPortalID = pData->byPoint;
	iMyPortalID += 1;
	PortalTable* myPortalTbl = pServer->GetTableContainer()->GetPortalTable();
	sPORTAL_TBLDAT* pPortalTblData = reinterpret_cast<sPORTAL_TBLDAT*>(myPortalTbl->FindData(iMyPortalID));
	if (pPortalTblData)
	{
		pRes.vDir = pPortalTblData->vDir;
		pRes.vLoc = pPortalTblData->vLoc;
		pRes.worldID = pPortalTblData->worldId;
		pRes.wResultCode = GAME_SUCCESS;
		Send(&pRes, sizeof(pRes));
		SendCharTeleport(pPortalTblData->worldId, TELEPORT_TYPE_NPC_PORTAL, iMyPortalID);
	}
	else
	{
		pRes.wResultCode = GAME_PORTAL_NOT_EXIST;
		Send(&pRes, sizeof(pRes));
	}

}
//By Luiz45 - Char Teleport
void GameClient::SendCharTeleport(TBLIDX worldID, eTELEPORT_TYPE teleportType, TBLIDX portalIDX)
{
	sGU_CHAR_TELEPORT_RES pRes;
	memset(&pRes, 0, sizeof(pRes));
	pRes.bIsToMoveAnotherServer = false;
	pRes.wResultCode = GAME_SUCCESS;
	if (eTELEPORT_TYPE::TELEPORT_TYPE_DUNGEON == teleportType)
	{
		WorldTable* pWorldTable = pServer->GetTableContainer()->GetWorldTable();
		sWORLD_TBLDAT* pWorldTbldat = reinterpret_cast<sWORLD_TBLDAT*>(pWorldTable->FindData(worldID));
		pRes.sWorldInfo.sRuleInfo.byRuleType = pWorldTbldat->worldRuleTbldx;
		pRes.sWorldInfo.tblidx = pWorldTbldat->tblidx;
		pRes.sWorldInfo.worldID = worldID;
		pRes.vNewDir.x = pWorldTbldat->vStart1Dir.x;
		pRes.vNewDir.y = 0;
		pRes.vNewDir.z = pWorldTbldat->vStart1Dir.z;
		pRes.vNewLoc.x = pWorldTbldat->vStart1Loc.x;
		pRes.vNewLoc.y = pWorldTbldat->vStart1Loc.y;
		pRes.vNewLoc.z = pWorldTbldat->vStart1Loc.z;

	}
	if (eTELEPORT_TYPE::TELEPORT_TYPE_NPC_PORTAL == teleportType)
	{
		if (portalIDX == 0)
		{
			WorldTable* pWorldTable = pServer->GetTableContainer()->GetWorldTable();
			sWORLD_TBLDAT* pWorldTbldat = reinterpret_cast<sWORLD_TBLDAT*>(pWorldTable->FindData(worldID));
			pRes.sWorldInfo.sRuleInfo.byRuleType = pWorldTbldat->worldRuleTbldx;
			pRes.sWorldInfo.tblidx = pWorldTbldat->tblidx;
			pRes.sWorldInfo.worldID = worldID;
			pRes.vNewDir.x = pWorldTbldat->vStart1Dir.x;
			pRes.vNewDir.y = 0;
			pRes.vNewDir.z = pWorldTbldat->vStart1Dir.z;
			pRes.vNewLoc.x = pWorldTbldat->vStart1Loc.x;
			pRes.vNewLoc.y = pWorldTbldat->vStart1Loc.y;
			pRes.vNewLoc.z = pWorldTbldat->vStart1Loc.z;
		}
		else
		{
			PortalTable* myPortalTbl = pServer->GetTableContainer()->GetPortalTable();
			sPORTAL_TBLDAT* pPortalTblData = reinterpret_cast<sPORTAL_TBLDAT*>(myPortalTbl->FindData(portalIDX));
			pRes.vNewDir.x = pPortalTblData->vDir.x;
			pRes.vNewDir.y = pPortalTblData->vDir.y;
			pRes.vNewDir.z = pPortalTblData->vDir.z;
			pRes.vNewLoc.x = pPortalTblData->vLoc.x;
			pRes.vNewLoc.y = pPortalTblData->vLoc.y;
			pRes.vNewLoc.z = pPortalTblData->vLoc.z;
		}		
	}
	pRes.wOpCode = GU_CHAR_TELEPORT_RES;
	Send(&pRes, sizeof(pRes));
	pProfile->UpdatePositions(pRes.vNewDir, pRes.vNewLoc);
	sCHARSTATE pCharState = pProfile->sCharState;
	pCharState.sCharStateBase.vCurDir = pRes.vNewDir;
	pCharState.sCharStateBase.vCurLoc = pRes.vNewLoc;
	pCharState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
	pCharState.sCharStateDetail.sCharStateTeleporting.byTeleportType = teleportType;
	SendCharStateUpdate(pProfile->GetSerialID(), pCharState);
}