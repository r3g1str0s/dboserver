#ifndef _OBJECTMANAGER
#define _OBJECTMANAGER

#include <Base.h>
#include <Def.h>
#include <Thread.h>

#include <iostream>
#include <memory>
#include <map>

#include "GameProtocol.h"
#include "ObjectTable.h"
#include "DynamicObjectTable.h"

class GameServer;
class GameClient;

class CharacterProfile;
class NpcProfile;
class MobProfile;

class ObjectManager : public RunObject
{
public:
	ObjectManager();
	~ObjectManager();

	void Init();
	void Release();
	void CreateThread();
	void Run();

	bool AddObject(HOBJECT hObject, void* pObj, eOBJTYPE eType);
	void RemoveObject(HOBJECT hObject, eOBJTYPE eType);
	bool FindObject(HOBJECT hObject, eOBJTYPE eType);

	void UpdateCharState(HOBJECT hObject, sCHARSTATE CharState);
	void SpawnToClient(GameClient* pClient);

	bool HasPcs() { return pcList.size() > 0; }

	typedef std::map<HOBJECT, NpcProfile*> OBJNPCLIST;
	typedef OBJNPCLIST::iterator OBJNPCLISTIT;
	typedef OBJNPCLIST::value_type OBJNPCLISTVAL;
	OBJNPCLIST npcList;

	typedef std::map<HOBJECT, MobProfile*> OBJMOBLIST;
	typedef OBJMOBLIST::iterator OBJMOBLISTIT;
	typedef OBJMOBLIST::value_type OBJMOBLISTVAL;
	OBJMOBLIST mobList;

	typedef std::map<HOBJECT, CharacterProfile*> OBJPCLIST;
	typedef OBJPCLIST::iterator OBJPCLISTIT;
	typedef OBJPCLIST::value_type OBJPCLISTVAL;
	OBJPCLIST pcList;

	typedef std::map<HOBJECT, sOBJECT_TBLDAT*> OBJMAPLIST;
	typedef OBJMAPLIST::iterator OBJMAPLISTIT;
	typedef OBJMAPLIST::value_type OBJMAPLISTVAL;
	OBJMAPLIST objMapList;

	typedef std::map<HOBJECT, sDYNAMIC_OBJECT_TBLDAT*> OBJDYNAMICLIST;
	typedef OBJDYNAMICLIST::iterator OBJDYNAMICLISTIT;
	typedef OBJDYNAMICLIST::value_type OBJDYNAMICLISTVAL;
	OBJDYNAMICLIST objDynamicList;

private:
	Thread* pThread;
	GameServer* pServer;
};

#endif