#ifndef _CHARACTERPROFILE
#define _CHARACTERPROFILE

#include "Base.h"
#include "Def.h"
#include "ObjectManager.h"
#include <TableAll.h>

class CharacterProfile
{
public:
	CharacterProfile();
	~CharacterProfile();

	void Init();

	void InsertOnlineData();
	void RemoveOnlineData();
	void LoadCharacterData();
	void LoadWorldInfoData();
	unsigned int GetPCTblidx(BYTE Race, BYTE Gender, BYTE Class);
	void UpdatePositions(sVECTOR2 Dir, sVECTOR3 Loc);
	void UpdatePositions(sVECTOR3 Dir, sVECTOR3 Loc);
	int LoadItemData();
	int LoadSkillData();
	int LoadQuickslotData();
	void CalculateAtributes(sPC_TBLDAT* pcdata);
	ObjectInfo GetCharSpawnData();
	sGU_ITEM_CREATE InsertNextBagSlot(ITEMID item, BYTE qtd = 1);
	HOBJECT GetInventoryItemSerialID(BYTE byPlace, BYTE byPos);
	TBLIDX GetInventoryItemID(BYTE byPlace, BYTE byPos);
	void UpdateItemInventoryPosition(HOBJECT hItem, BYTE byPlace, BYTE byPos);
	void GetItemBrief(sITEM_BRIEF& sBrief, HOBJECT hItem);

	// Set
	void SetAvatartype(BYTE byAvatarType) { this->byAvatarType = byAvatarType; }
	void SetAccountid(ACCOUNTID AccountID) { this->AccountID = AccountID; }
	void SetCharid(HOBJECT hCharID) { this->hCharID = hCharID; }
	void SetTutorialmode(bool bTutorialMode) { this->bTutorialMode = bTutorialMode; }
	void SetMovedirection(BYTE byMoveDirection) { this->byMoveDirection = byMoveDirection; }

	// Get
	WCHAR* GetUsername() { return wszUserName; }
	WCHAR* GetPassword() { return wszPassWord; }
	WCHAR* GetCharname() { return wszCharName; }
	WCHAR* GetGuildname() { return wszGuildName; }
	BYTE* GetAuthkey() { return abyAuthKey; }
	ACCOUNTID GetAccountid() { return AccountID; }
	BYTE GetLastserverid() { return byLastServerID; }
	DWORD GetAclevel() { return dwAcLevel; }
	BYTE GetChannelid() { return byChannelID; }
	HOBJECT GetSerialID() { return hSerialID; }
	HOBJECT GetCharID() { return hCharID; }
	BYTE GetServerID() { return byServerID; }
	bool GetTutorialMode() { return bTutorialMode; }
	bool GetGameMaster() { return bIsGameMaster; }
	BYTE GetRace() { return byRace; }
	BYTE GetClass() { return byClass; }
	BYTE GetGender() { return byGender; }
	BYTE GetAvatartype() { return byAvatarType; }
	BYTE GetMovedirection() { return byMoveDirection; }

	// Structs
	sPC_PROFILE sPcProfile;
	sCHARSTATE sCharState;
	sWORLD_INFO sWorldInfo;
	sITEM_PROFILE asItemProfile[NTL_MAX_COUNT_USER_HAVE_INVEN_ITEM];
	sSKILL_INFO asSkillInfo[NTL_MAX_PC_HAVE_SKILL];
	sQUICK_SLOT_PROFILE asQuickSlotData[NTL_CHAR_QUICK_SLOT_MAX_COUNT];

private:
	WCHAR wszUserName[NTL_MAX_SIZE_USERID_UNICODE + 1];
	WCHAR wszPassWord[NTL_MAX_SIZE_USERPW_UNICODE + 1];
	WCHAR wszCharName[NTL_MAX_SIZE_CHAR_NAME_UNICODE + 1];
	WCHAR wszGuildName[NTL_MAX_SIZE_GUILD_NAME_IN_UNICODE + 1];
	BYTE abyAuthKey[NTL_MAX_SIZE_AUTH_KEY];
	ACCOUNTID AccountID;
	BYTE byLastServerID;
	DWORD dwAcLevel;
	BYTE byServerID;
	BYTE byChannelID;
	HOBJECT hCharID;
	bool bIsGameMaster;
	bool bTutorialMode;
	HOBJECT hSerialID;
	BYTE byMoveDirection;
	BYTE byAvatarType;
	BYTE byRace;
	BYTE byClass;
	BYTE byGender;

	GameServer* pServer;
};

#endif