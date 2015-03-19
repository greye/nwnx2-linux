/***************************************************************************
    lua_int.c - Lua interface to nwscript VM.
    (c) 2010 pardik (d.paroli@tiscali.it)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ***************************************************************************/

#include "FunctionHooks.h"
#include "NWNStructures.h"
#include "nwscript.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

static int luaL_checkboolean(lua_State *L, int narg) {
  int d = lua_toboolean(L, narg);
  if (d == 0 && !lua_isboolean(L, narg))  /* avoid extra test when d is not 0 */
		luaL_typerror(L, narg, lua_typename(L, LUA_TBOOLEAN));
  return d;
}

static int luaL_optboolean(lua_State *L, int narg, int def) {
  return luaL_opt(L, luaL_checkboolean, narg, def);
}

static void *
nwn_checkptr(lua_State *L, int index, const char *tname) {
  void *pt = lua_touserdata(L, index);
  if (pt == NULL) luaL_typerror(L, index, tname);
  return pt;
}

static Vector *
lua_nwnx_pushvector(lua_State *L) {
	Vector *vec = (Vector *) lua_newuserdata(L, sizeof(*vec));

	luaL_getmetatable(L, VECTOR);
	lua_setmetatable(L, -2);

	return vec;
}

static CScriptLocation *
lua_nwn_checkloc(lua_State *L, int index) {
	return (CScriptLocation *) nwn_checkptr(L, index, LOCATION);
}

static void
send_exostring_to_lua(lua_State *L) {
    size_t len;
    char *str;

    StackPopString(&str, &len);
    lua_pushlstring(L, str, (len != 0) ? len - 1 : 0);

    free(str);
}

static int NWScript_Random(lua_State *L)
{
	int nMaxInteger = luaL_checkint(L, 1);

	StackPushInteger(nMaxInteger);
	VM_ExecuteCommand(0, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_PrintString(lua_State *L)
{
	char *sString = (char *)luaL_checkstring(L, 1);

	StackPushString(sString);
	VM_ExecuteCommand(1, 1);
	return 0;
}

static int NWScript_PrintFloat(lua_State *L)
{
	double fFloat = luaL_checknumber(L, 1);
	int nWidth = luaL_optint(L, 2, 18);
	int nDecimals = luaL_optint(L, 3, 9);

	StackPushInteger(nDecimals);
	StackPushInteger(nWidth);
	StackPushFloat(fFloat);
	VM_ExecuteCommand(2, 3);
	return 0;
}

static int NWScript_FloatToString(lua_State *L)
{
	double fFloat = luaL_checknumber(L, 1);
	int nWidth = luaL_optint(L, 2, 18);
	int nDecimals = luaL_optint(L, 3, 9);

	StackPushInteger(nDecimals);
	StackPushInteger(nWidth);
	StackPushFloat(fFloat);
	VM_ExecuteCommand(3, 3);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_PrintInteger(lua_State *L)
{
	int nInteger = luaL_checkint(L, 1);

	StackPushInteger(nInteger);
	VM_ExecuteCommand(4, 1);
	return 0;
}

static int NWScript_PrintObject(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(5, 1);
	return 0;
}
/*
static int NWScript_AssignCommand(VALUE oActionSubject, VALUE aActionToAssign)
{
	//ERROR: Undefined variable type: action
	StackPushObject(oActionSubject));
	VM_ExecuteCommand(6, 2);
	return 0;
}

static int NWScript_DelayCommand(VALUE fSeconds, VALUE aActionToDelay)
{
	//ERROR: Undefined variable type: action
	StackPushFloat(luaL_checkint(L, 2));
	VM_ExecuteCommand(7, 2);
	return 0;
}
*/
static int NWScript_ExecuteScript(lua_State *L)
{
	char *sScript = (char *)luaL_checkstring(L, 1);
	dword oTarget = luaL_checkint(L, 2);

	StackPushObject(oTarget);
	StackPushString(sScript);
	VM_ExecuteCommand(8, 2);
	return 0;
}

static int NWScript_ClearAllActions(lua_State *L)
{
	int nClearCombatState = luaL_optboolean (L, 1, FALSE);

	StackPushInteger(nClearCombatState);
	VM_ExecuteCommand(9, 1);
	return 0;
}

static int NWScript_SetFacing(lua_State *L)
{
	double fDirection = luaL_checknumber(L, 1);

	StackPushFloat(fDirection);
	VM_ExecuteCommand(10, 1);
	return 0;
}

static int NWScript_SetCalendar(lua_State *L)
{
	int nYear = luaL_checkint(L, 1);
	int nMonth = luaL_checkint(L, 2);
	int nDay = luaL_checkint(L, 3);

	StackPushInteger(nDay);
	StackPushInteger(nMonth);
	StackPushInteger(nYear);
	VM_ExecuteCommand(11, 3);
	return 0;
}

static int NWScript_SetTime(lua_State *L)
{
	int nHour = luaL_checkint(L, 1);
	int nMinute = luaL_checkint(L, 2);
	int nSecond = luaL_checkint(L, 3);
	int nMillisecond = luaL_checkint(L, 4);

	StackPushInteger(nMillisecond);
	StackPushInteger(nSecond);
	StackPushInteger(nMinute);
	StackPushInteger(nHour);
	VM_ExecuteCommand(12, 4);
	return 0;
}

static int NWScript_GetCalendarYear(lua_State *L)
{
	VM_ExecuteCommand(13, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetCalendarMonth(lua_State *L)
{
	VM_ExecuteCommand(14, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetCalendarDay(lua_State *L)
{
	VM_ExecuteCommand(15, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetTimeHour(lua_State *L)
{
	VM_ExecuteCommand(16, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetTimeMinute(lua_State *L)
{
	VM_ExecuteCommand(17, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetTimeSecond(lua_State *L)
{
	VM_ExecuteCommand(18, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetTimeMillisecond(lua_State *L)
{
	VM_ExecuteCommand(19, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_ActionRandomWalk(lua_State *L)
{
	VM_ExecuteCommand(20, 0);
	return 1;
}

static int NWScript_ActionMoveToLocation(lua_State *L)
{
	void *lDestination = lua_nwn_checkloc(L, 1);
	int bRun = luaL_optboolean (L, 2, FALSE);

	StackPushInteger(bRun);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lDestination);
	VM_ExecuteCommand(21, 2);
	return 0;
}

static int NWScript_ActionMoveToObject(lua_State *L)
{
	dword oMoveTo = luaL_checkint(L, 1);
	int bRun = luaL_optboolean (L, 2, FALSE);
	double fRange = luaL_optnumber (L, 3, 1.0);

	StackPushFloat(fRange);
	StackPushInteger(bRun);
	StackPushObject(oMoveTo);
	VM_ExecuteCommand(22, 3);
	return 0;
}

static int NWScript_ActionMoveAwayFromObject(lua_State *L)
{
	dword oFleeFrom = luaL_checkint(L, 1);
	int bRun = luaL_optboolean (L, 2, FALSE);
	double fMoveAwayRange = luaL_optnumber (L, 3, 40.0);

	StackPushFloat(fMoveAwayRange);
	StackPushInteger(bRun);
	StackPushObject(oFleeFrom);
	VM_ExecuteCommand(23, 3);
	return 0;
}

static int NWScript_GetArea(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);

	StackPushObject(oTarget);
	VM_ExecuteCommand(24, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetEnteringObject(lua_State *L)
{
	VM_ExecuteCommand(25, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetExitingObject(lua_State *L)
{
	VM_ExecuteCommand(26, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetPosition(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);

	StackPushObject(oTarget);
	VM_ExecuteCommand(27, 1);

	Vector *vec = lua_nwnx_pushvector(L);
	StackPopVector(vec);

	return 1;
}

static int NWScript_GetFacing(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);

	StackPushObject(oTarget);
	VM_ExecuteCommand(28, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_GetItemPossessor(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(29, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetItemPossessedBy(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	char *sItemTag = (char *)luaL_checkstring(L, 2);

	StackPushString(sItemTag);
	StackPushObject(oCreature);
	VM_ExecuteCommand(30, 2);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_CreateItemOnObject(lua_State *L)
{
	char *sItemTemplate = (char *)luaL_checkstring(L, 1);
	dword oTarget = luaL_optint (L, 2, OBJECT_SELF);
	int nStackSize = luaL_optint (L, 3, 1);
	char *sNewTag = (char *)luaL_optstring (L,  4, "");

	StackPushString(sNewTag);
	StackPushInteger(nStackSize);
	StackPushObject(oTarget);
	StackPushString(sItemTemplate);
	VM_ExecuteCommand(31, 4);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_ActionEquipItem(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	int nInventorySlot = luaL_checkint(L, 2);

	StackPushInteger(nInventorySlot);
	StackPushObject(oItem);
	VM_ExecuteCommand(32, 2);
	return 0;
}

static int NWScript_ActionUnequipItem(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(33, 1);
	return 0;
}

static int NWScript_ActionPickUpItem(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(34, 1);
	return 0;
}

static int NWScript_ActionPutDownItem(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(35, 1);
	return 0;
}

static int NWScript_GetLastAttacker(lua_State *L)
{
	dword oAttackee = luaL_optint (L, 1, OBJECT_SELF);

	StackPushObject(oAttackee);
	VM_ExecuteCommand(36, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_ActionAttack(lua_State *L)
{
	dword oAttackee = luaL_checkint(L, 1);
	int bPassive = luaL_optboolean (L, 2, FALSE);

	StackPushInteger(bPassive);
	StackPushObject(oAttackee);
	VM_ExecuteCommand(37, 2);
	return 0;
}

static int NWScript_GetNearestCreature(lua_State *L)
{
	int nFirstCriteriaType = luaL_checkint(L, 1);
	int nFirstCriteriaValue = luaL_checkint(L, 2);
	dword oTarget = luaL_optint (L, 3, OBJECT_SELF);
	int nNth = luaL_optint (L, 4, 1);
	int nSecondCriteriaType = luaL_optint (L, 5, -1);
	int nSecondCriteriaValue = luaL_optint (L, 6, -1);
	int nThirdCriteriaType = luaL_optint (L, 7, -1);
	int nThirdCriteriaValue = luaL_optint (L, 8, -1);

	StackPushInteger(nThirdCriteriaValue);
	StackPushInteger(nThirdCriteriaType);
	StackPushInteger(nSecondCriteriaValue);
	StackPushInteger(nSecondCriteriaType);
	StackPushInteger(nNth);
	StackPushObject(oTarget);
	StackPushInteger(nFirstCriteriaValue);
	StackPushInteger(nFirstCriteriaType);
	VM_ExecuteCommand(38, 8);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_ActionSpeakString(lua_State *L)
{
	char *sStringToSpeak = (char *)luaL_checkstring(L, 1);
	int nTalkVolume = luaL_optint (L, 2, TALKVOLUME_TALK);

	StackPushInteger(nTalkVolume);
	StackPushString(sStringToSpeak);
	VM_ExecuteCommand(39, 2);
	return 0;
}

static int NWScript_ActionPlayAnimation(lua_State *L)
{
	int nAnimation = luaL_checkint(L, 1);
	double fSpeed = luaL_optnumber (L, 2, 1.0);
	double fDurationSeconds = luaL_optnumber (L, 3, 1.0);

	StackPushFloat(fDurationSeconds);
	StackPushFloat(fSpeed);
	StackPushInteger(nAnimation);
	VM_ExecuteCommand(40, 3);
	return 0;
}

static int NWScript_GetDistanceToObject(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(41, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_GetIsObjectValid(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(42, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
	return 1;
}

static int NWScript_ActionOpenDoor(lua_State *L)
{
	dword oDoor = luaL_checkint(L, 1);

	StackPushObject(oDoor);
	VM_ExecuteCommand(43, 1);
	return 0;
}

static int NWScript_ActionCloseDoor(lua_State *L)
{
	dword oDoor = luaL_checkint(L, 1);

	StackPushObject(oDoor);
	VM_ExecuteCommand(44, 1);
	return 0;
}

static int NWScript_SetCameraFacing(lua_State *L)
{
	double fDirection = luaL_checknumber(L, 1);
	double fDistance  = luaL_optnumber(L, 2, -1.0);
	double fPitch  = luaL_optnumber(L, 3, -1.0);
	int nTransitionType = luaL_optint(L, 4, CAMERA_TRANSITION_TYPE_SNAP);

	StackPushInteger(nTransitionType);
	StackPushFloat(fPitch);
	StackPushFloat(fDistance );
	StackPushFloat(fDirection);
	VM_ExecuteCommand(45, 4);
	return 0;
}

static int NWScript_PlaySound(lua_State *L)
{
	char *sSoundName = (char *)luaL_checkstring(L, 1);

	StackPushString(sSoundName);
	VM_ExecuteCommand(46, 1);
	return 0;
}

static int NWScript_GetSpellTargetObject(lua_State *L)
{
	VM_ExecuteCommand(47, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_ActionCastSpellAtObject(lua_State *L)
{
	int nSpell = luaL_checkint(L, 1);
	dword oTarget = luaL_checkint(L, 2);
	int nMetaMagic = luaL_optint(L, 3, METAMAGIC_ANY);
	int bCheat = luaL_optboolean(L, 4, FALSE);
	int nDomainLevel = luaL_optint(L, 5, 0);
	int nProjectilePathType = luaL_optint(L, 6, PROJECTILE_PATH_TYPE_DEFAULT);
	int bInstantSpell = luaL_optboolean(L, 7, FALSE);

	StackPushInteger(bInstantSpell);
	StackPushInteger(nProjectilePathType);
	StackPushInteger(nDomainLevel);
	StackPushInteger(bCheat);
	StackPushInteger(nMetaMagic);
	StackPushObject(oTarget);
	StackPushInteger(nSpell);
	VM_ExecuteCommand(48, 7);
	return 0;
}

static int NWScript_GetCurrentHitPoints(lua_State *L)
{
	dword oObject = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oObject);
	VM_ExecuteCommand(49, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetMaxHitPoints(lua_State *L)
{
	dword oObject = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oObject);
	VM_ExecuteCommand(50, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetLocalInt(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);

	StackPushString(sVarName);
	StackPushObject(oObject);
	VM_ExecuteCommand(51, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetLocalFloat(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);

	StackPushString(sVarName);
	StackPushObject(oObject);
	VM_ExecuteCommand(52, 2);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_GetLocalString(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);

	StackPushString(sVarName);
	StackPushObject(oObject);
	VM_ExecuteCommand(53, 2);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetLocalObject(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);

	StackPushString(sVarName);
	StackPushObject(oObject);
	VM_ExecuteCommand(54, 2);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_SetLocalInt(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	int nValue = luaL_checkint(L, 3);

	StackPushInteger(nValue);
	StackPushString(sVarName);
	StackPushObject(oObject);
	VM_ExecuteCommand(55, 3);
	return 0;
}

static int NWScript_SetLocalFloat(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	double fValue = luaL_checknumber(L, 3);

	StackPushFloat(fValue);
	StackPushString(sVarName);
	StackPushObject(oObject);
	VM_ExecuteCommand(56, 3);
	return 0;
}

static int NWScript_SetLocalString(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	char *sValue = (char *)luaL_checkstring(L, 3);

	StackPushString(sValue);
	StackPushString(sVarName);
	StackPushObject(oObject);
	VM_ExecuteCommand(57, 3);
	return 0;
}

static int NWScript_SetLocalObject(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	dword oValue = luaL_checkint(L, 3);

	StackPushObject(oValue);
	StackPushString(sVarName);
	StackPushObject(oObject);
	VM_ExecuteCommand(58, 3);
	return 0;
}

static int NWScript_GetStringLength(lua_State *L)
{
	char *sString = (char *)luaL_checkstring(L, 1);

	StackPushString(sString);
	VM_ExecuteCommand(59, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetStringUpperCase(lua_State *L)
{
	char *sString = (char *)luaL_checkstring(L, 1);

	StackPushString(sString);
	VM_ExecuteCommand(60, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetStringLowerCase(lua_State *L)
{
	char *sString = (char *)luaL_checkstring(L, 1);

	StackPushString(sString);
	VM_ExecuteCommand(61, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetStringRight(lua_State *L)
{
	char *sString = (char *)luaL_checkstring(L, 1);
	int nCount = luaL_checkint(L, 2);

	StackPushInteger(nCount);
	StackPushString(sString);
	VM_ExecuteCommand(62, 2);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetStringLeft(lua_State *L)
{
	char *sString = (char *)luaL_checkstring(L, 1);
	int nCount = luaL_checkint(L, 2);

	StackPushInteger(nCount);
	StackPushString(sString);
	VM_ExecuteCommand(63, 2);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_InsertString(lua_State *L)
{
	char *sDestination = (char *)luaL_checkstring(L, 1);
	char *sString = (char *)luaL_checkstring(L, 2);
	int nPosition = luaL_checkint(L, 3);

	StackPushInteger(nPosition);
	StackPushString(sString);
	StackPushString(sDestination);
	VM_ExecuteCommand(64, 3);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetSubString(lua_State *L)
{
	char *sString = (char *)luaL_checkstring(L, 1);
	int nStart = luaL_checkint(L, 2);
	int nCount = luaL_checkint(L, 3);

	StackPushInteger(nCount);
	StackPushInteger(nStart);
	StackPushString(sString);
	VM_ExecuteCommand(65, 3);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_FindSubString(lua_State *L)
{
	char *sString = (char *)luaL_checkstring(L, 1);
	char *sSubString = (char *)luaL_checkstring(L, 2);
	int nStart = luaL_optint(L, 3, 0);

	StackPushInteger(nStart);
	StackPushString(sSubString);
	StackPushString(sString);
	VM_ExecuteCommand(66, 3);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_fabs(lua_State *L)
{
	double fValue = luaL_checknumber(L, 1);

	StackPushFloat(fValue);
	VM_ExecuteCommand(67, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_cos(lua_State *L)
{
	double fValue = luaL_checknumber(L, 1);

	StackPushFloat(fValue);
	VM_ExecuteCommand(68, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_sin(lua_State *L)
{
	double fValue = luaL_checknumber(L, 1);

	StackPushFloat(fValue);
	VM_ExecuteCommand(69, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_tan(lua_State *L)
{
	double fValue = luaL_checknumber(L, 1);

	StackPushFloat(fValue);
	VM_ExecuteCommand(70, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_acos(lua_State *L)
{
	double fValue = luaL_checknumber(L, 1);

	StackPushFloat(fValue);
	VM_ExecuteCommand(71, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_asin(lua_State *L)
{
	double fValue = luaL_checknumber(L, 1);

	StackPushFloat(fValue);
	VM_ExecuteCommand(72, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_atan(lua_State *L)
{
	double fValue = luaL_checknumber(L, 1);

	StackPushFloat(fValue);
	VM_ExecuteCommand(73, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_log(lua_State *L)
{
	double fValue = luaL_checknumber(L, 1);

	StackPushFloat(fValue);
	VM_ExecuteCommand(74, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_pow(lua_State *L)
{
	double fValue = luaL_checknumber(L, 1);
	double fExponent = luaL_checknumber(L, 2);

	StackPushFloat(fExponent);
	StackPushFloat(fValue);
	VM_ExecuteCommand(75, 2);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_sqrt(lua_State *L)
{
	double fValue = luaL_checknumber(L, 1);

	StackPushFloat(fValue);
	VM_ExecuteCommand(76, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_abs(lua_State *L)
{
	int nValue = luaL_checkint(L, 1);

	StackPushInteger(nValue);
	VM_ExecuteCommand(77, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_EffectHeal(lua_State *L)
{
	int nDamageToHeal = luaL_checkint(L, 1);

	StackPushInteger(nDamageToHeal);
	VM_ExecuteCommand(78, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_EffectDamage(lua_State *L)
{
	int nDamageAmount = luaL_checkint(L, 1);
	int nDamageType = luaL_optint(L, 2, DAMAGE_TYPE_MAGICAL);
	int nDamagePower = luaL_optint(L, 3, DAMAGE_POWER_NORMAL);

	StackPushInteger(nDamagePower);
	StackPushInteger(nDamageType);
	StackPushInteger(nDamageAmount);
	VM_ExecuteCommand(79, 3);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_EffectAbilityIncrease(lua_State *L)
{
	int nAbilityToIncrease = luaL_checkint(L, 1);
	int nModifyBy = luaL_checkint(L, 2);

	StackPushInteger(nModifyBy);
	StackPushInteger(nAbilityToIncrease);
	VM_ExecuteCommand(80, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_EffectDamageResistance(lua_State *L)
{
	int nDamageType = luaL_checkint(L, 1);
	int nAmount = luaL_checkint(L, 2);
	int nLimit = luaL_optint(L, 3, 0);

	StackPushInteger(nLimit);
	StackPushInteger(nAmount);
	StackPushInteger(nDamageType);
	VM_ExecuteCommand(81, 3);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_EffectResurrection(lua_State *L)
{
	VM_ExecuteCommand(82, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_EffectSummonCreature(lua_State *L)
{
	char *sCreatureResref = (char *)luaL_checkstring(L, 1);
	int nVisualEffectId = luaL_optint(L, 2, VFX_NONE);
	double fDelaySeconds = luaL_optnumber(L, 3, 0.0);
	int nUseAppearAnimation = luaL_optboolean(L, 4, FALSE);

	StackPushInteger(nUseAppearAnimation);
	StackPushFloat(fDelaySeconds);
	StackPushInteger(nVisualEffectId);
	StackPushString(sCreatureResref);
	VM_ExecuteCommand(83, 4);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_GetCasterLevel(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(84, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetFirstEffect(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(85, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_GetNextEffect(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(86, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_RemoveEffect(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	void *eEffect = nwn_checkptr(L, 2, EFFECT);

	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eEffect);
	StackPushObject(oCreature);
	VM_ExecuteCommand(87, 2);
	return 0;
}

static int NWScript_GetIsEffectValid(lua_State *L)
{
	void *eEffect = nwn_checkptr(L, 1, EFFECT);

	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eEffect);
	VM_ExecuteCommand(88, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
	return 1;
}

static int NWScript_GetEffectDurationType(lua_State *L)
{
	void *eEffect = nwn_checkptr(L, 1, EFFECT);

	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eEffect);
	VM_ExecuteCommand(89, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetEffectSubType(lua_State *L)
{
	void *eEffect = nwn_checkptr(L, 1, EFFECT);

	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eEffect);
	VM_ExecuteCommand(90, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetEffectCreator(lua_State *L)
{
	void *eEffect = nwn_checkptr(L, 1, EFFECT);

	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eEffect);
	VM_ExecuteCommand(91, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_IntToString(lua_State *L)
{
	int nInteger = luaL_checkint(L, 1);

	StackPushInteger(nInteger);
	VM_ExecuteCommand(92, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetFirstObjectInArea(lua_State *L)
{
	dword oArea = luaL_optint(L, 1, OBJECT_INVALID);

	StackPushObject(oArea);
	VM_ExecuteCommand(93, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetNextObjectInArea(lua_State *L)
{
	dword oArea = luaL_optint(L, 1, OBJECT_INVALID);

	StackPushObject(oArea);
	VM_ExecuteCommand(94, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_d2(lua_State *L)
{
	int nNumDice = luaL_optint(L, 1, 1);

	StackPushInteger(nNumDice);
	VM_ExecuteCommand(95, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_d3(lua_State *L)
{
	int nNumDice = luaL_optint(L, 1, 1);

	StackPushInteger(nNumDice);
	VM_ExecuteCommand(96, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_d4(lua_State *L)
{
	int nNumDice = luaL_optint(L, 1, 1);

	StackPushInteger(nNumDice);
	VM_ExecuteCommand(97, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_d6(lua_State *L)
{
	int nNumDice = luaL_optint(L, 1, 1);

	StackPushInteger(nNumDice);
	VM_ExecuteCommand(98, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_d8(lua_State *L)
{
	int nNumDice = luaL_optint(L, 1, 1);

	StackPushInteger(nNumDice);
	VM_ExecuteCommand(99, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_d10(lua_State *L)
{
	int nNumDice = luaL_optint(L, 1, 1);

	StackPushInteger(nNumDice);
	VM_ExecuteCommand(100, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_d12(lua_State *L)
{
	int nNumDice = luaL_optint(L, 1, 1);

	StackPushInteger(nNumDice);
	VM_ExecuteCommand(101, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_d20(lua_State *L)
{
	int nNumDice = luaL_optint(L, 1, 1);

	StackPushInteger(nNumDice);
	VM_ExecuteCommand(102, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_d100(lua_State *L)
{
	int nNumDice = luaL_optint(L, 1, 1);

	StackPushInteger(nNumDice);
	VM_ExecuteCommand(103, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_VectorMagnitude(lua_State *L)
{
	Vector *vec = (Vector *) luaL_checkudata(L, 1, VECTOR);
	double len = sqrt(vec->X * vec->X + vec->Y * vec->Y + vec->Z * vec->Z);

	lua_pushnumber(L, len);

	return 1;
}

static int NWScript_GetMetaMagicFeat(lua_State *L)
{
	VM_ExecuteCommand(105, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetObjectType(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);

	StackPushObject(oTarget);
	VM_ExecuteCommand(106, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetRacialType(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(107, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_FortitudeSave(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	int nDC = luaL_checkint(L, 2);
	int nSaveType = luaL_optint(L, 3, SAVING_THROW_TYPE_NONE);
	dword oSaveVersus = luaL_optint(L, 4, OBJECT_SELF);

	StackPushObject(oSaveVersus);
	StackPushInteger(nSaveType);
	StackPushInteger(nDC);
	StackPushObject(oCreature);
	VM_ExecuteCommand(108, 4);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_ReflexSave(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	int nDC = luaL_checkint(L, 2);
	int nSaveType = luaL_optint(L, 3, SAVING_THROW_TYPE_NONE);
	dword oSaveVersus = luaL_optint(L, 4, OBJECT_SELF);

	StackPushObject(oSaveVersus);
	StackPushInteger(nSaveType);
	StackPushInteger(nDC);
	StackPushObject(oCreature);
	VM_ExecuteCommand(109, 4);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_WillSave(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	int nDC = luaL_checkint(L, 2);
	int nSaveType = luaL_optint(L, 3, SAVING_THROW_TYPE_NONE);
	dword oSaveVersus = luaL_optint(L, 4, OBJECT_SELF);

	StackPushObject(oSaveVersus);
	StackPushInteger(nSaveType);
	StackPushInteger(nDC);
	StackPushObject(oCreature);
	VM_ExecuteCommand(110, 4);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetSpellSaveDC(lua_State *L)
{
	VM_ExecuteCommand(111, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_MagicalEffect(lua_State *L)
{
	void *eEffect = nwn_checkptr(L, 1, EFFECT);

	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eEffect);
	VM_ExecuteCommand(112, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_SupernaturalEffect(lua_State *L)
{
	void *eEffect = nwn_checkptr(L, 1, EFFECT);

	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eEffect);
	VM_ExecuteCommand(113, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ExtraordinaryEffect(lua_State *L)
{
	void *eEffect = nwn_checkptr(L, 1, EFFECT);

	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eEffect);
	VM_ExecuteCommand(114, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_EffectACIncrease(lua_State *L)
{
	int nValue = luaL_checkint(L, 1);
	int nModifyType = luaL_optint(L, 2, AC_DODGE_BONUS);
	int nDamageType = luaL_optint(L, 3, AC_VS_DAMAGE_TYPE_ALL);

	StackPushInteger(nDamageType);
	StackPushInteger(nModifyType);
	StackPushInteger(nValue);
	VM_ExecuteCommand(115, 3);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_GetAC(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	int nForFutureUse = luaL_optint(L, 2, 0);

	StackPushInteger(nForFutureUse);
	StackPushObject(oObject);
	VM_ExecuteCommand(116, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_EffectSavingThrowIncrease(lua_State *L)
{
    int nSave = luaL_checkint(L, 1);
    int nValue = luaL_checkint(L, 2);
    int nSaveType = luaL_optint(L, 3, SAVING_THROW_TYPE_ALL);

    StackPushInteger(nSaveType);
    StackPushInteger(nValue);
    StackPushInteger(nSave);
    VM_ExecuteCommand(117, 3);
    void *pRetVal;
    StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
    lua_pushlightuserdata(L, pRetVal);
    return 1;
}

static int NWScript_EffectAttackIncrease(lua_State *L)
{
	int nBonus = luaL_checkint(L, 1);
	int nModifierType = luaL_optint(L, 2, ATTACK_BONUS_MISC);

	StackPushInteger(nModifierType);
	StackPushInteger(nBonus);
	VM_ExecuteCommand(118, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_EffectDamageReduction(lua_State *L)
{
	int nAmount = luaL_checkint(L, 1);
	int nDamagePower = luaL_checkint(L, 2);
	int nLimit = luaL_optint(L, 3, 0);

	StackPushInteger(nLimit);
	StackPushInteger(nDamagePower);
	StackPushInteger(nAmount);
	VM_ExecuteCommand(119, 3);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_EffectDamageIncrease(lua_State *L)
{
	int nBonus = luaL_checkint(L, 1);
	int nDamageType = luaL_optint(L, 2, DAMAGE_TYPE_MAGICAL);

	StackPushInteger(nDamageType);
	StackPushInteger(nBonus);
	VM_ExecuteCommand(120, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_RoundsToSeconds(lua_State *L)
{
	int nRounds = luaL_checkint(L, 1);

	StackPushInteger(nRounds);
	VM_ExecuteCommand(121, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_HoursToSeconds(lua_State *L)
{
	int nHours = luaL_checkint(L, 1);

	StackPushInteger(nHours);
	VM_ExecuteCommand(122, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_TurnsToSeconds(lua_State *L)
{
	int nTurns = luaL_checkint(L, 1);

	StackPushInteger(nTurns);
	VM_ExecuteCommand(123, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_GetLawChaosValue(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(124, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetGoodEvilValue(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(125, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetAlignmentLawChaos(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(126, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetAlignmentGoodEvil(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(127, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_EffectEntangle(lua_State *L)
{
	VM_ExecuteCommand(130, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_SignalEvent(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	void *eEvent = nwn_checkptr(L, 2, EVENT);
  StackPushEngineStructure(ENGINE_STRUCTURE_EVENT, eEvent);
	StackPushObject(oObject);
	VM_ExecuteCommand(131, 2);
	return 0;
}

static int NWScript_EventUserDefined(lua_State *L)
{
	int nUserDefinedEventNumber = luaL_checkint(L, 1);
  StackPushInteger(nUserDefinedEventNumber);
	VM_ExecuteCommand(132, 1);
  void *pRetVal;
  StackPopEngineStructure(ENGINE_STRUCTURE_EVENT, &pRetVal);
  lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectDeath(lua_State *L)
{
	int nSpectacularDeath = luaL_optboolean(L, 1, FALSE);
	int nDisplayFeedback = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(nDisplayFeedback);
	StackPushInteger(nSpectacularDeath);
	VM_ExecuteCommand(133, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_EffectKnockdown(lua_State *L)
{
	VM_ExecuteCommand(134, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ActionGiveItem(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	dword oGiveTo = luaL_checkint(L, 2);

	StackPushObject(oGiveTo);
	StackPushObject(oItem);
	VM_ExecuteCommand(135, 2);
	return 0;
}

static int NWScript_ActionTakeItem(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	dword oTakeFrom = luaL_checkint(L, 2);

	StackPushObject(oTakeFrom);
	StackPushObject(oItem);
	VM_ExecuteCommand(136, 2);
	return 0;
}

static int NWScript_VectorNormalize(lua_State *L)
{
	Vector *vec = (Vector *) luaL_checkudata(L, 1, VECTOR);
	double invLen = 1 / sqrt(vec->X * vec->X + vec->Y * vec->Y + vec->Z * vec->Z);
	Vector *norm = lua_nwnx_pushvector(L);
	norm->X = vec->X * invLen;
	norm->Y = vec->Y * invLen;
	norm->Z = vec->Z * invLen;

	return 1;
}

static int NWScript_EffectCurse(lua_State *L)
{
	int nStrMod = luaL_optint(L, 1, 1);
	int nDexMod = luaL_optint(L, 2, 1);
	int nConMod = luaL_optint(L, 3, 1);
	int nIntMod = luaL_optint(L, 4, 1);
	int nWisMod = luaL_optint(L, 5, 1);
	int nChaMod = luaL_optint(L, 6, 1);

	StackPushInteger(nChaMod);
	StackPushInteger(nWisMod);
	StackPushInteger(nIntMod);
	StackPushInteger(nConMod);
	StackPushInteger(nDexMod);
	StackPushInteger(nStrMod);
	VM_ExecuteCommand(138, 6);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_GetAbilityScore(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	int nAbilityType = luaL_checkint(L, 2);
	int nBaseAbilityScore = luaL_optboolean(L, 3, FALSE);

	StackPushInteger(nBaseAbilityScore);
	StackPushInteger(nAbilityType);
	StackPushObject(oCreature);
	VM_ExecuteCommand(139, 3);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetIsDead(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(140, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
	return 1;
}

static int NWScript_PrintVector(lua_State *L)
{
	Vector *vVector = (Vector *)luaL_checkudata(L, 1, VECTOR);
	int bPrepend = luaL_checkboolean(L, 2);

	StackPushInteger(bPrepend);
	StackPushVector(*vVector);
	VM_ExecuteCommand(141, 2);
	return 0;
}

static int NWScript_Vector(lua_State *L)
{
	double x = luaL_optnumber(L, 1, 0.0);
	double y = luaL_optnumber(L, 2, 0.0);
	double z = luaL_optnumber(L, 3, 0.0);

	Vector *vec = lua_nwnx_pushvector(L);
	vec->X = (float) x;
	vec->Y = (float) y;
	vec->Z = (float) z;

	return 1;
}

static int NWScript_SetFacingPoint(lua_State *L)
{
	Vector *vTarget = (Vector *)luaL_checkudata(L, 1, VECTOR);

	StackPushVector(*vTarget);
	VM_ExecuteCommand(143, 1);
	return 0;
}

static int NWScript_AngleToVector(lua_State *L)
{
	double fAngle = luaL_checknumber(L, 1);

	StackPushFloat(fAngle);
	VM_ExecuteCommand(144, 1);

	Vector *vec = lua_nwnx_pushvector(L);
	StackPopVector(vec);

	return 1;
}

static int NWScript_VectorToAngle(lua_State *L)
{
	Vector *vVector = (Vector *)luaL_checkudata(L, 1, VECTOR);

	StackPushVector(*vVector);
	VM_ExecuteCommand(145, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_TouchAttackMelee(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	int bDisplayFeedback = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bDisplayFeedback);
	StackPushObject(oTarget);
	VM_ExecuteCommand(146, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_TouchAttackRanged(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	int bDisplayFeedback = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bDisplayFeedback);
	StackPushObject(oTarget);
	VM_ExecuteCommand(147, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_EffectParalyze(lua_State *L)
{
	VM_ExecuteCommand(148, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_EffectSpellImmunity(lua_State *L)
{
	int nImmunityToSpell = luaL_optint(L, 1, SPELL_ALL_SPELLS);

	StackPushInteger(nImmunityToSpell);
	VM_ExecuteCommand(149, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_EffectDeaf(lua_State *L)
{
	VM_ExecuteCommand(150, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_GetDistanceBetween(lua_State *L)
{
	dword oObjectA = luaL_checkint(L, 1);
	dword oObjectB = luaL_checkint(L, 2);

	StackPushObject(oObjectB);
	StackPushObject(oObjectA);
	VM_ExecuteCommand(151, 2);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
	return 1;
}

static int NWScript_SetLocalLocation(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	void *lValue = lua_nwn_checkloc(L, 3);

	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lValue);
	StackPushString(sVarName);
	StackPushObject(oObject);
	VM_ExecuteCommand(152, 3);
	return 0;
}

static int NWScript_GetLocalLocation(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);

	StackPushString(sVarName);
	StackPushObject(oObject);
	VM_ExecuteCommand(153, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_LOCATION, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectSleep(lua_State *L)
{
	VM_ExecuteCommand(154, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetItemInSlot(lua_State *L)
{
	int nInventorySlot = luaL_checkint(L, 1);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nInventorySlot);
	VM_ExecuteCommand(155, 2);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_EffectCharmed(lua_State *L)
{
	VM_ExecuteCommand(156, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectConfused(lua_State *L)
{
	VM_ExecuteCommand(157, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectFrightened(lua_State *L)
{
	VM_ExecuteCommand(158, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectDominated(lua_State *L)
{
	VM_ExecuteCommand(159, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectDazed(lua_State *L)
{
	VM_ExecuteCommand(160, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectStunned(lua_State *L)
{
	VM_ExecuteCommand(161, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_SetCommandable(lua_State *L)
{
	int bCommandable = luaL_checkboolean(L, 1);
	dword oTarget = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oTarget);
	StackPushInteger(bCommandable);
	VM_ExecuteCommand(162, 2);
	return 0;
}

static int NWScript_GetCommandable(lua_State *L)
{
	dword oTarget = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oTarget);
	VM_ExecuteCommand(163, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_EffectRegenerate(lua_State *L)
{
	int nAmount = luaL_checkint(L, 1);
	double fIntervalSeconds = luaL_checknumber(L, 2);

	StackPushFloat(fIntervalSeconds);
	StackPushInteger(nAmount);
	VM_ExecuteCommand(164, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_EffectMovementSpeedIncrease(lua_State *L)
{
	int nPercentChange = luaL_checkint(L, 1);

	StackPushInteger(nPercentChange);
	VM_ExecuteCommand(165, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_GetHitDice(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(166, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_ActionForceFollowObject(lua_State *L)
{
	dword oFollow = luaL_checkint(L, 1);
	double fFollowDistance = luaL_optnumber(L, 2, 0.0);

	StackPushFloat(fFollowDistance);
	StackPushObject(oFollow);
	VM_ExecuteCommand(167, 2);
	return 0;
}

static int NWScript_GetTag(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(168, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_ResistSpell(lua_State *L)
{
	dword oCaster = luaL_checkint(L, 1);
	dword oTarget = luaL_checkint(L, 2);

	StackPushObject(oTarget);
	StackPushObject(oCaster);
	VM_ExecuteCommand(169, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetEffectType(lua_State *L)
{
	void *eEffect = nwn_checkptr(L, 1, EFFECT);

	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eEffect);
	VM_ExecuteCommand(170, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_EffectAreaOfEffect(lua_State *L)
{
	int nAreaEffectId = luaL_checkint(L, 1);
	char *sOnEnterScript = (char *)luaL_optstring(L,  2, "");
	char *sHeartbeatScript = (char *)luaL_optstring(L,  3, "");
	char *sOnExitScript = (char *)luaL_optstring(L,  4, "");

	StackPushString(sOnExitScript);
	StackPushString(sHeartbeatScript);
	StackPushString(sOnEnterScript);
	StackPushInteger(nAreaEffectId);
	VM_ExecuteCommand(171, 4);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_GetFactionEqual(lua_State *L)
{
	dword oFirstObject = luaL_checkint(L, 1);
	dword oSecondObject = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oSecondObject);
	StackPushObject(oFirstObject);
	VM_ExecuteCommand(172, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
	return 1;
}

static int NWScript_ChangeFaction(lua_State *L)
{
	dword oObjectToChangeFaction = luaL_checkint(L, 1);
	dword oMemberOfFactionToJoin = luaL_checkint(L, 2);

	StackPushObject(oMemberOfFactionToJoin);
	StackPushObject(oObjectToChangeFaction);
	VM_ExecuteCommand(173, 2);
	return 0;
}

static int NWScript_GetIsListening(lua_State *L)
{
  dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(174, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
	return 1;
}

static int NWScript_SetListening(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
  int  bValue = luaL_checkboolean(L, 2);

  StackPushInteger(bValue);
	StackPushObject(oObject);
	VM_ExecuteCommand(175, 2);
	return 0;
}

static int NWScript_SetListenPattern(lua_State *L)
{
	dword oObject = luaL_checkboolean(L, 1);
	char *sPattern = (char *)luaL_checkstring(L, 2);
	int nNumber = luaL_optint(L, 3, 0);

	StackPushInteger(nNumber);
	StackPushString(sPattern);
	StackPushObject(oObject);
	VM_ExecuteCommand(176, 3);
	return 0;
}

static int NWScript_TestStringAgainstPattern(lua_State *L)
{
  char *sPattern = (char *)luaL_checkstring(L, 1);
  char *sStringToTest = (char *)luaL_checkstring(L, 2);

	StackPushString(sStringToTest);
	StackPushString(sPattern);
	VM_ExecuteCommand(177, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetMatchedSubstring(lua_State *L)
{
	int nString = luaL_checkint(L, 1);

	StackPushInteger(nString);
	VM_ExecuteCommand(178, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetMatchedSubstringsCount(lua_State *L)
{
	VM_ExecuteCommand(179, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_EffectVisualEffect(lua_State *L)
{
	int nVisualEffectId = luaL_checkint(L, 1);
	int nMissEffect = luaL_optboolean(L, 2, FALSE);

	StackPushInteger(nMissEffect);
	StackPushInteger(nVisualEffectId);
	VM_ExecuteCommand(180, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_GetFactionWeakestMember(lua_State *L)
{
	dword oFactionMember = luaL_optint(L, 1, OBJECT_SELF);
  int bMustBeVisible = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bMustBeVisible);
	StackPushObject(oFactionMember);
	VM_ExecuteCommand(181, 2);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetFactionStrongestMember(lua_State *L)
{
	dword oFactionMember = luaL_optint(L, 1, OBJECT_SELF);
  int bMustBeVisible = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bMustBeVisible);
	StackPushObject(oFactionMember);
	VM_ExecuteCommand(182, 2);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetFactionMostDamagedMember(lua_State *L)
{
	dword oFactionMember = luaL_optint(L, 1, OBJECT_SELF);
  int bMustBeVisible = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bMustBeVisible);
	StackPushObject(oFactionMember);
	VM_ExecuteCommand(183, 2);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
    return 1;
}

static int NWScript_GetFactionLeastDamagedMember(lua_State *L)
{
	dword oFactionMember = luaL_optint(L, 1, OBJECT_SELF);
  int bMustBeVisible = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bMustBeVisible);
	StackPushObject(oFactionMember);
	VM_ExecuteCommand(184, 2);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetFactionGold(lua_State *L)
{
	dword oFactionMember = luaL_checkint(L, 1);

  StackPushObject(oFactionMember);
	VM_ExecuteCommand(185, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetFactionAverageReputation(lua_State *L)
{
	dword oSourceFactionMember = luaL_checkint(L, 1);
  dword oTarget = luaL_checkint(L, 2);

  StackPushObject(oTarget);
	StackPushObject(oSourceFactionMember);
	VM_ExecuteCommand(186, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetFactionAverageGoodEvilAlignment(lua_State *L)
{
	dword oFactionMember = luaL_checkint(L, 1);

  StackPushObject(oFactionMember);
	VM_ExecuteCommand(187, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetFactionAverageLawChaosAlignment(lua_State *L)
{
	dword oFactionMember = luaL_checkint(L, 1);

  StackPushObject(oFactionMember);
	VM_ExecuteCommand(188, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetFactionAverageLevel(lua_State *L)
{
	dword oFactionMember = luaL_checkint(L, 1);

  StackPushObject(oFactionMember);
	VM_ExecuteCommand(189, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetFactionAverageXP(lua_State *L)
{
	dword oFactionMember = luaL_checkint(L, 1);

  StackPushObject(oFactionMember);
	VM_ExecuteCommand(190, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetFactionMostFrequentClass(lua_State *L)
{
	dword oFactionMember = luaL_checkint(L, 1);

  StackPushObject(oFactionMember);
	VM_ExecuteCommand(191, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetFactionWorstAC(lua_State *L)
{
	dword oFactionMember = luaL_optint(L, 1, OBJECT_SELF);
  int bMustBeVisible = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bMustBeVisible);
	StackPushObject(oFactionMember);
	VM_ExecuteCommand(192, 2);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetFactionBestAC(lua_State *L)
{
	dword oFactionMember = luaL_optint(L, 1, OBJECT_SELF);
  int bMustBeVisible = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bMustBeVisible);
	StackPushObject(oFactionMember);
	VM_ExecuteCommand(193, 2);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_ActionSit(lua_State *L)
{
	dword oChair = luaL_checkint(L, 1);

  StackPushObject(oChair);
	VM_ExecuteCommand(194, 1);
	return 0;
}

static int NWScript_GetListenPatternNumber(lua_State *L)
{
	VM_ExecuteCommand(195, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_ActionJumpToObject(lua_State *L)
{
	dword oToJumpTo = luaL_checkint(L, 1);
	int bWalkStraightLineToPoint = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bWalkStraightLineToPoint);
	StackPushObject(oToJumpTo);
	VM_ExecuteCommand(196, 2);
	return 0;
}

static int NWScript_GetWaypointByTag(lua_State *L)
{
	char *sWaypointTag = (char *)luaL_checkstring(L, 1);

  StackPushString(sWaypointTag);
	VM_ExecuteCommand(197, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetTransitionTarget(lua_State *L)
{
	dword oTransition = luaL_checkint(L, 1);

  StackPushObject(oTransition);
	VM_ExecuteCommand(198, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_EffectLinkEffects(lua_State *L)
{
	void *eChildEffect = nwn_checkptr(L, 1, EFFECT);
  void *eParentEffect = nwn_checkptr(L, 2, EFFECT);

  StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eParentEffect);
	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eChildEffect);
	VM_ExecuteCommand(199, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetObjectByTag(lua_State *L)
{
  char *sTag = (char *)luaL_checkstring(L, 1);
	int nNth = luaL_optint(L, 2, 0);

	StackPushInteger(nNth);
	StackPushString(sTag);
	VM_ExecuteCommand(200, 2);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_AdjustAlignment(lua_State *L)
{
  dword oSubject = luaL_checkint(L, 1);
  int nAlignment = luaL_checkint(L, 2);
  int nShift = luaL_checkint(L, 3);
	int bAllPartyMembers = luaL_optboolean(L, 4, TRUE);

	StackPushInteger(bAllPartyMembers);
	StackPushInteger(nShift);
	StackPushInteger(nAlignment);
	StackPushObject(oSubject);
	VM_ExecuteCommand(201, 4);
	return 0;
}

static int NWScript_ActionWait(lua_State *L)
{
	double fSeconds = luaL_checknumber(L, 1);

  StackPushFloat(fSeconds);
	VM_ExecuteCommand(202, 1);
	return 0;
}

static int NWScript_SetAreaTransitionBMP(lua_State *L)
{
  int nPredefinedAreaTransition = luaL_checkint(L, 1);
	char *sCustomAreaTransitionBMP = (char *)luaL_optstring(L, 2, "");

	StackPushString(sCustomAreaTransitionBMP);
	StackPushInteger(nPredefinedAreaTransition);
	VM_ExecuteCommand(203, 2);
	return 0;
}

static int NWScript_ActionStartConversation(lua_State *L)
{
  dword oObjectToConverseWith = luaL_checkint(L, 1);
	char *sDialogResRef = (char *)luaL_optstring(L, 2, "");
  int bPrivateConversation = luaL_optboolean(L, 3, FALSE);
  int bPlayHello = luaL_optboolean(L, 4, TRUE);

	StackPushInteger(bPlayHello);
	StackPushInteger(bPrivateConversation);
	StackPushString(sDialogResRef);
	StackPushObject(oObjectToConverseWith);
	VM_ExecuteCommand(204, 4);
	return 0;
}

static int NWScript_ActionPauseConversation(lua_State *L)
{
	VM_ExecuteCommand(205, 0);
	return 0;
}

static int NWScript_ActionResumeConversation(lua_State *L)
{
	VM_ExecuteCommand(206, 0);
	return 0;
}

static int NWScript_EffectBeam(lua_State *L)
{
  int nBeamVisualEffect = luaL_checkint(L, 1);
  dword oEffector = luaL_checkint(L, 2);
  int nBodyPart = luaL_checkint(L, 3);
	int bMissEffect = luaL_optboolean(L, 4, FALSE);

	StackPushInteger(bMissEffect);
	StackPushInteger(nBodyPart);
	StackPushObject(oEffector);
	StackPushInteger(nBeamVisualEffect);
	VM_ExecuteCommand(207, 4);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetReputation(lua_State *L)
{
	dword oSource = luaL_checkint(L, 1);
  dword oTarget = luaL_checkint(L, 2);

  StackPushObject(oTarget);
	StackPushObject(oSource);
	VM_ExecuteCommand(208, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_AdjustReputation(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
  dword oSourceFactionMember = luaL_checkint(L, 2);
  int nAdjustment = luaL_checkint(L, 3);

  StackPushInteger(nAdjustment);
	StackPushObject(oSourceFactionMember);
	StackPushObject(oTarget);
	VM_ExecuteCommand(209, 3);
	return 0;
}

static int NWScript_GetSittingCreature(lua_State *L)
{
	dword oChair = luaL_checkint(L, 1);

	StackPushObject(oChair);
	VM_ExecuteCommand(210, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetGoingToBeAttackedBy(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);

	StackPushObject(oTarget);
	VM_ExecuteCommand(211, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_EffectSpellResistanceIncrease(lua_State *L)
{
	int nValue = luaL_checkint(L, 1);

	StackPushInteger(nValue);
	VM_ExecuteCommand(212, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetLocation(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(213, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_LOCATION, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_ActionJumpToLocation(lua_State *L)
{
	void *lLocation = lua_nwn_checkloc(L, 1);

	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lLocation);
	VM_ExecuteCommand(214, 1);
	return 0;
}

static int NWScript_Location(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);
	Vector *vPosition = (Vector *)luaL_checkudata(L, 2, VECTOR);
	double fOrientation = luaL_checknumber(L, 3);

	StackPushFloat(fOrientation);
	StackPushVector(*vPosition);
	StackPushObject(oArea);
	VM_ExecuteCommand(215, 3);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_LOCATION, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_ApplyEffectAtLocation(lua_State *L)
{
	int nDurationType = luaL_checkint(L, 1);
	void *eEffect = nwn_checkptr(L, 2, EFFECT);
	void *lLocation = lua_nwn_checkloc(L, 3);
	double fDuration = luaL_optnumber(L, 4, 0.0);

	StackPushFloat(fDuration);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lLocation);
	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eEffect);
	StackPushInteger(nDurationType);
	VM_ExecuteCommand(216, 4);
	return 0;
}

static int NWScript_GetIsPC(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(217, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_FeetToMeters(lua_State *L)
{
	double fFeet = luaL_checknumber(L, 1);

	StackPushFloat(fFeet);
	VM_ExecuteCommand(218, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
  return 1;
}

static int NWScript_YardsToMeters(lua_State *L)
{
	double fYards = luaL_checknumber(L, 1);

	StackPushFloat(fYards);
	VM_ExecuteCommand(219, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
  return 1;
}

static int NWScript_ApplyEffectToObject(lua_State *L)
{
	int nDurationType = luaL_checkint(L, 1);
	void *eEffect = nwn_checkptr(L, 2, EFFECT);
	dword oTarget = luaL_checkint(L, 3);
	double fDuration = luaL_optnumber(L, 4, 0.0);

	StackPushFloat(fDuration);
	StackPushObject(oTarget);
	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eEffect);
	StackPushInteger(nDurationType);
	VM_ExecuteCommand(220, 4);
	return 0;
}

static int NWScript_SpeakString(lua_State *L)
{
	char *sStringToSpeak = (char *)luaL_checkstring(L, 1);
	int nTalkVolume = luaL_optint(L, 2, TALKVOLUME_TALK);

	StackPushInteger(nTalkVolume);
	StackPushString(sStringToSpeak);
	VM_ExecuteCommand(221, 2);
	return 0;
}

static int NWScript_GetSpellTargetLocation(lua_State *L)
{
	VM_ExecuteCommand(222, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_LOCATION, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetPositionFromLocation(lua_State *L)
{
	CScriptLocation *loc = lua_nwn_checkloc(L, 1);
	Vector *vec = lua_nwnx_pushvector(L);
	vec->X = loc->X;
	vec->Y = loc->Y;
	vec->Z = loc->Z;

	return 1;
}

static int NWScript_GetAreaFromLocation(lua_State *L)
{
	CScriptLocation *loc = lua_nwn_checkloc(L, 1);
	lua_pushinteger(L, loc->AreaID);

	return 1;
}

static int NWScript_GetFacingFromLocation(lua_State *L)
{
	CScriptLocation *loc = lua_nwn_checkloc(L, 1);
	lua_pushnumber(L, loc->OrientationX);

	return 1;
}

static int NWScript_GetNearestCreatureToLocation(lua_State *L)
{
	int nFirstCriteriaType = luaL_checkint(L, 1);
	int nFirstCriteriaValue = luaL_checkint(L, 2);
	void *lLocation = lua_nwn_checkloc(L, 3);
	int nNth = luaL_optint(L, 4, 1);
	int nSecondCriteriaType = luaL_optint(L, 5, -1);
	int nSecondCriteriaValue = luaL_optint(L, 6, -1);
	int nThirdCriteriaType = luaL_optint(L, 7, -1);
	int nThirdCriteriaValue = luaL_optint(L, 8, -1);

	StackPushInteger(nThirdCriteriaValue);
	StackPushInteger(nThirdCriteriaType);
	StackPushInteger(nSecondCriteriaValue);
	StackPushInteger(nSecondCriteriaType);
	StackPushInteger(nNth);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lLocation);
	StackPushInteger(nFirstCriteriaValue);
	StackPushInteger(nFirstCriteriaType);
	VM_ExecuteCommand(226, 8);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetNearestObject(lua_State *L)
{
	int nObjectType = luaL_optint(L, 1, OBJECT_TYPE_ALL);
	dword oTarget = luaL_optint(L, 2, OBJECT_SELF);
	int nNth = luaL_optint(L, 3, 1);

	StackPushInteger(nNth);
	StackPushObject(oTarget);
	StackPushInteger(nObjectType);
	VM_ExecuteCommand(227, 3);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetNearestObjectToLocation(lua_State *L)
{
	int nObjectType = luaL_checkint(L, 1);
	void *lLocation = lua_nwn_checkloc(L, 2);
	int nNth = luaL_optint(L, 3, 1);

	StackPushInteger(nNth);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lLocation);
	StackPushInteger(nObjectType);
	VM_ExecuteCommand(228, 3);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetNearestObjectByTag(lua_State *L)
{
	char *sTag = (char *)luaL_checkstring(L, 1);
	dword oTarget = luaL_optint(L, 2, OBJECT_SELF);
	int nNth = luaL_optint(L, 3, 1);

	StackPushInteger(nNth);
	StackPushObject(oTarget);
	StackPushString(sTag);
	VM_ExecuteCommand(229, 3);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_IntToFloat(lua_State *L)
{
	int nInteger = luaL_checkint(L, 1);

	StackPushInteger(nInteger);
	VM_ExecuteCommand(230, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
  return 1;
}

static int NWScript_FloatToInt(lua_State *L)
{
	double fFloat = luaL_checknumber(L, 1);

	StackPushFloat(fFloat);
	VM_ExecuteCommand(231, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_StringToInt(lua_State *L)
{
	char *sNumber = (char *)luaL_checkstring(L, 1);

	StackPushString(sNumber);
	VM_ExecuteCommand(232, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_StringToFloat(lua_State *L)
{
	char *sNumber = (char *)luaL_checkstring(L, 1);

	StackPushString(sNumber);
	VM_ExecuteCommand(233, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
    return 1;
}

static int NWScript_ActionCastSpellAtLocation(lua_State *L)
{
	int nSpell = luaL_checkint(L, 1);
	void *lTargetLocation = lua_nwn_checkloc(L, 2);
	int nMetaMagic = luaL_optint(L, 3, METAMAGIC_ANY);
	int bCheat = luaL_optboolean(L, 4, FALSE);
	int nProjectilePathType = luaL_optint(L, 5, PROJECTILE_PATH_TYPE_DEFAULT);
	int bInstantSpell = luaL_optboolean(L, 6, FALSE);

	StackPushInteger(bInstantSpell);
	StackPushInteger(nProjectilePathType);
	StackPushInteger(bCheat);
	StackPushInteger(nMetaMagic);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lTargetLocation);
	StackPushInteger(nSpell);
	VM_ExecuteCommand(234, 6);
	return 0;
}

static int NWScript_GetIsEnemy(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	dword oSource = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oSource);
	StackPushObject(oTarget);
	VM_ExecuteCommand(235, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetIsFriend(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	dword oSource = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oSource);
	StackPushObject(oTarget);
	VM_ExecuteCommand(236, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetIsNeutral(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	dword oSource = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oSource);
	StackPushObject(oTarget);
	VM_ExecuteCommand(237, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetPCSpeaker(lua_State *L)
{
	VM_ExecuteCommand(238, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetStringByStrRef(lua_State *L)
{
	int nStrRef = luaL_checkint(L, 1);
	int nGender = luaL_optint(L, 2, GENDER_MALE);

	StackPushInteger(nGender);
	StackPushInteger(nStrRef);
	VM_ExecuteCommand(239, 2);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_ActionSpeakStringByStrRef(lua_State *L)
{
	int nStrRef = luaL_checkint(L, 1);
	int nTalkVolume = luaL_optint(L, 2, TALKVOLUME_TALK);

	StackPushInteger(nTalkVolume);
	StackPushInteger(nStrRef);
	VM_ExecuteCommand(240, 2);
	return 0;
}

static int NWScript_DestroyObject(lua_State *L)
{
	dword oDestroy = luaL_checkint(L, 1);
	double fDelay = luaL_optnumber(L, 2, 0.0);

	StackPushFloat(fDelay);
	StackPushObject(oDestroy);
	VM_ExecuteCommand(241, 2);
	return 0;
}

static int NWScript_GetModule(lua_State *L)
{
	VM_ExecuteCommand(242, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_CreateObject(lua_State *L)
{
	int nObjectType = luaL_checkint(L, 1);
	char *sTemplate = (char *)luaL_checkstring(L, 2);
	void *lLocation = lua_nwn_checkloc(L, 3);
	int bUseAppearAnimation = luaL_optboolean(L, 4, FALSE);
	char *sNewTag = (char *)luaL_optstring(L, 5, "");

	StackPushString(sNewTag);
	StackPushInteger(bUseAppearAnimation);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lLocation);
	StackPushString(sTemplate);
	StackPushInteger(nObjectType);
	VM_ExecuteCommand(243, 5);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_EventSpellCastAt(lua_State *L)
{
	dword oCaster = luaL_checkint(L, 1);
	int nSpell = luaL_checkint(L, 2);
	int bHarmful = luaL_optboolean(L, 3, TRUE);

	StackPushInteger(bHarmful);
	StackPushInteger(nSpell);
	StackPushObject(oCaster);
	VM_ExecuteCommand(244, 3);
  void *pRetVal;
  StackPopEngineStructure(ENGINE_STRUCTURE_EVENT, &pRetVal);
  lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetLastSpellCaster(lua_State *L)
{
	VM_ExecuteCommand(245, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastSpell(lua_State *L)
{
	VM_ExecuteCommand(246, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetUserDefinedEventNumber(lua_State *L)
{
	VM_ExecuteCommand(247, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetSpellId(lua_State *L)
{
	VM_ExecuteCommand(248, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_RandomName(lua_State *L)
{
	int nNameType = luaL_optint(L, 1, NAME_FIRST_GENERIC_MALE);

	StackPushInteger(nNameType);
	VM_ExecuteCommand(249, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_EffectPoison(lua_State *L)
{
	int nPoisonType = luaL_checkint(L, 1);

	StackPushInteger(nPoisonType);
	VM_ExecuteCommand(250, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectDisease(lua_State *L)
{
	int nDiseaseType = luaL_checkint(L, 1);

	StackPushInteger(nDiseaseType);
	VM_ExecuteCommand(251, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectSilence(lua_State *L)
{
	VM_ExecuteCommand(252, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetName(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	int bOriginalName = luaL_optboolean(L, 2, FALSE);

	StackPushInteger(bOriginalName);
	StackPushObject(oObject);
	VM_ExecuteCommand(253, 2);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetLastSpeaker(lua_State *L)
{
	VM_ExecuteCommand(254, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_BeginConversation(lua_State *L)
{
	char *sResRef = (char *)luaL_optstring(L, 1, "");
	int oObjectToDialog = luaL_optint(L, 2, OBJECT_INVALID);

	StackPushObject(oObjectToDialog);
	StackPushString(sResRef);
	VM_ExecuteCommand(255, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastPerceived(lua_State *L)
{
	VM_ExecuteCommand(256, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastPerceptionHeard(lua_State *L)
{
	VM_ExecuteCommand(257, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastPerceptionInaudible(lua_State *L)
{
	VM_ExecuteCommand(258, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastPerceptionSeen(lua_State *L)
{
	VM_ExecuteCommand(259, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastClosedBy(lua_State *L)
{
	VM_ExecuteCommand(260, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastPerceptionVanished(lua_State *L)
{
	VM_ExecuteCommand(261, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetFirstInPersistentObject(lua_State *L)
{
	dword oPersistentObject = luaL_optint(L, 1, OBJECT_SELF);
	int nResidentObjectType = luaL_optint(L, 2, OBJECT_TYPE_CREATURE);
	int nPersistentZone = luaL_optint(L, 3, PERSISTENT_ZONE_ACTIVE);

	StackPushInteger(nPersistentZone);
	StackPushInteger(nResidentObjectType);
	StackPushObject(oPersistentObject);
	VM_ExecuteCommand(262, 3);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetNextInPersistentObject(lua_State *L)
{
	dword oPersistentObject = luaL_optint(L, 1, OBJECT_SELF);
	int nResidentObjectType = luaL_optint(L, 2, OBJECT_TYPE_CREATURE);
	int nPersistentZone = luaL_optint(L, 3, PERSISTENT_ZONE_ACTIVE);

	StackPushInteger(nPersistentZone);
	StackPushInteger(nResidentObjectType);
	StackPushObject(oPersistentObject);
	VM_ExecuteCommand(263, 3);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetAreaOfEffectCreator(lua_State *L)
{
	dword oAreaOfEffectObject = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oAreaOfEffectObject);
	VM_ExecuteCommand(264, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_DeleteLocalInt(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);

	StackPushString(sVarName);
	StackPushObject(oObject);
	VM_ExecuteCommand(265, 2);
	return 0;
}

static int NWScript_DeleteLocalFloat(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);

	StackPushString(sVarName);
	StackPushObject(oObject);
	VM_ExecuteCommand(266, 2);
	return 0;
}

static int NWScript_DeleteLocalString(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);

	StackPushString(sVarName);
	StackPushObject(oObject);
	VM_ExecuteCommand(267, 2);
	return 0;
}

static int NWScript_DeleteLocalObject(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);

	StackPushString(sVarName);
	StackPushObject(oObject);
	VM_ExecuteCommand(268, 2);
	return 0;
}

static int NWScript_DeleteLocalLocation(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);

	StackPushString(sVarName);
	StackPushObject(oObject);
	VM_ExecuteCommand(269, 2);
	return 0;
}

static int NWScript_EffectHaste(lua_State *L)
{
	VM_ExecuteCommand(270, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectSlow(lua_State *L)
{
	VM_ExecuteCommand(271, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_ObjectToString(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(272, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_EffectImmunity(lua_State *L)
{
	int nImmunityType = luaL_checkint(L, 1);

	StackPushInteger(nImmunityType);
	VM_ExecuteCommand(273, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetIsImmune(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	int nImmunityType = luaL_checkint(L, 2);
	dword oVersus = luaL_optint(L, 3, OBJECT_INVALID);

	StackPushObject(oVersus);
	StackPushInteger(nImmunityType);
	StackPushObject(oCreature);
	VM_ExecuteCommand(274, 3);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_EffectDamageImmunityIncrease(lua_State *L)
{
	int nDamageType = luaL_checkint(L, 1);
	int nPercentImmunity = luaL_checkint(L, 2);

	StackPushInteger(nPercentImmunity);
	StackPushInteger(nDamageType);
	VM_ExecuteCommand(275, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_GetEncounterActive(lua_State *L)
{
	dword oEncounter = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oEncounter);
	VM_ExecuteCommand(276, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_SetEncounterActive(lua_State *L)
{
	int nNewValue = luaL_checkboolean(L, 1);
	dword oEncounter = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oEncounter);
	StackPushInteger(nNewValue);
	VM_ExecuteCommand(277, 2);
	return 0;
}

static int NWScript_GetEncounterSpawnsMax(lua_State *L)
{
	dword oEncounter = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oEncounter);
	VM_ExecuteCommand(278, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetEncounterSpawnsMax(lua_State *L)
{
	int nNewValue = luaL_checkint(L, 1);
	dword oEncounter = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oEncounter);
	StackPushInteger(nNewValue);
	VM_ExecuteCommand(279, 2);
	return 0;
}

static int NWScript_GetEncounterSpawnsCurrent(lua_State *L)
{
	dword oEncounter = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oEncounter);
	VM_ExecuteCommand(280, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetEncounterSpawnsCurrent(lua_State *L)
{
	int nNewValue = luaL_checkint(L, 1);
	dword oEncounter = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oEncounter);
	StackPushInteger(nNewValue);
	VM_ExecuteCommand(281, 2);
	return 0;
}

static int NWScript_GetModuleItemAcquired(lua_State *L)
{
	VM_ExecuteCommand(282, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetModuleItemAcquiredFrom(lua_State *L)
{
	VM_ExecuteCommand(283, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetCustomToken(lua_State *L)
{
	int nCustomTokenNumber = luaL_checkint(L, 1);
	char *sTokenValue = (char *)luaL_checkstring(L, 2);

	StackPushString(sTokenValue);
	StackPushInteger(nCustomTokenNumber);
	VM_ExecuteCommand(284, 2);
	return 0;
}

static int NWScript_GetHasFeat(lua_State *L)
{
	int nFeat = luaL_checkint(L, 1);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nFeat);
	VM_ExecuteCommand(285, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetHasSkill(lua_State *L)
{
	int nSkill = luaL_checkint(L, 1);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nSkill);
	VM_ExecuteCommand(286, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_ActionUseFeat(lua_State *L)
{
	int nFeat = luaL_checkint(L, 1);
	dword oTarget = luaL_checkint(L, 2);

	StackPushObject(oTarget);
	StackPushInteger(nFeat);
	VM_ExecuteCommand(287, 2);
	return 0;
}

static int NWScript_ActionUseSkill(lua_State *L)
{
	int nSkill = luaL_checkint(L, 1);
	dword oTarget = luaL_checkint(L, 2);
	int nSubSkill = luaL_optint(L, 3, 0);
	dword oItemUsed = luaL_optint(L, 4, OBJECT_INVALID);

	StackPushObject(oItemUsed);
	StackPushInteger(nSubSkill);
	StackPushObject(oTarget);
	StackPushInteger(nSkill);
	VM_ExecuteCommand(288, 4);
	return 0;
}

static int NWScript_GetObjectSeen(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	dword oSource = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oSource);
	StackPushObject(oTarget);
	VM_ExecuteCommand(289, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetObjectHeard(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	dword oSource = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oSource);
	StackPushObject(oTarget);
	VM_ExecuteCommand(290, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetLastPlayerDied(lua_State *L)
{
	VM_ExecuteCommand(291, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetModuleItemLost(lua_State *L)
{
	VM_ExecuteCommand(292, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetModuleItemLostBy(lua_State *L)
{
	VM_ExecuteCommand(293, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}
/*
static int NWScript_ActionDoCommand(VALUE aActionToDo)
{
	//ERROR: Undefined variable type: action
	VM_ExecuteCommand(294, 1);
	return 0;
}
*/
static int NWScript_EventConversation(lua_State *L)
{
	VM_ExecuteCommand(295, 0);
  void *pRetVal;
  StackPopEngineStructure(ENGINE_STRUCTURE_EVENT, &pRetVal);
  lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_SetEncounterDifficulty(lua_State *L)
{
	int nEncounterDifficulty = luaL_checkint(L, 1);
	dword oEncounter = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oEncounter);
	StackPushInteger(nEncounterDifficulty);
	VM_ExecuteCommand(296, 2);
	return 0;
}

static int NWScript_GetEncounterDifficulty(lua_State *L)
{
	dword oEncounter = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oEncounter);
	VM_ExecuteCommand(297, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetDistanceBetweenLocations(lua_State *L)
{
	void *lLocationA = lua_nwn_checkloc(L, 1);
	void *lLocationB = lua_nwn_checkloc(L, 2);

	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lLocationB);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lLocationA);
	VM_ExecuteCommand(298, 2);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
  return 1;
}

static int NWScript_GetReflexAdjustedDamage(lua_State *L)
{
	int nDamage = luaL_checkint(L, 1);
	dword oTarget = luaL_checkint(L, 2);
	int nDC = luaL_checkint(L, 3);
	int nSaveType = luaL_optint(L, 4, SAVING_THROW_TYPE_NONE);
	dword oSaveVersus = luaL_optint(L, 5, OBJECT_SELF);

	StackPushObject(oSaveVersus);
	StackPushInteger(nSaveType);
	StackPushInteger(nDC);
	StackPushObject(oTarget);
	StackPushInteger(nDamage);
	VM_ExecuteCommand(299, 5);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_PlayAnimation(lua_State *L)
{
	int nAnimation = luaL_checkint(L, 1);
	double fSpeed = luaL_optnumber(L, 2, 1.0);
	double fSeconds = luaL_optnumber(L, 3, 0.0);

	StackPushFloat(fSeconds);
	StackPushFloat(fSpeed);
	StackPushInteger(nAnimation);
	VM_ExecuteCommand(300, 3);
	return 0;
}

static int NWScript_TalentSpell(lua_State *L)
{
	int nSpell = luaL_checkint(L, 1);

  StackPushInteger(nSpell);
	VM_ExecuteCommand(301, 1);
  void *pRetVal;
  StackPopEngineStructure(ENGINE_STRUCTURE_TALENT, &pRetVal);
  lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_TalentFeat(lua_State *L)
{
	int nFeat = luaL_checkint(L, 1);

  StackPushInteger(nFeat);
	VM_ExecuteCommand(302, 1);
  void *pRetVal;
  StackPopEngineStructure(ENGINE_STRUCTURE_TALENT, &pRetVal);
  lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_TalentSkill(lua_State *L)
{
	int nSkill = luaL_checkint(L, 1);

  StackPushInteger(nSkill);
	VM_ExecuteCommand(303, 1);
  void *pRetVal;
  StackPopEngineStructure(ENGINE_STRUCTURE_TALENT, &pRetVal);
  lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetHasSpellEffect(lua_State *L)
{
	int nSpell = luaL_checkint(L, 1);
	dword oObject = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oObject);
	StackPushInteger(nSpell);
	VM_ExecuteCommand(304, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetEffectSpellId(lua_State *L)
{
	void *eSpellEffect = nwn_checkptr(L, 1, EFFECT);

	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eSpellEffect);
	VM_ExecuteCommand(305, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetCreatureHasTalent(lua_State *L)
{
	void *tTalent = nwn_checkptr(L, 1, TALENT);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushEngineStructure(ENGINE_STRUCTURE_TALENT, tTalent);
	VM_ExecuteCommand(306, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetCreatureTalentRandom(lua_State *L)
{
  int nCategory = luaL_checkint(L, 1);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nCategory);
	VM_ExecuteCommand(307, 2);
  void *pRetVal;
  StackPopEngineStructure(ENGINE_STRUCTURE_TALENT, &pRetVal);
  lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetCreatureTalentBest(lua_State *L)
{
	int nCategory = luaL_checkint(L, 1);
  int nCRMax = luaL_checkint(L, 2);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nCRMax);
	StackPushInteger(nCategory);
	VM_ExecuteCommand(308, 3);
  void *pRetVal;
  StackPopEngineStructure(ENGINE_STRUCTURE_TALENT, &pRetVal);
  lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_ActionUseTalentOnObject(lua_State *L)
{
	void *tChosenTalent = nwn_checkptr(L, 1, TALENT);
  dword oTarget = luaL_checkint(L, 2);

  StackPushObject(oTarget);
	StackPushEngineStructure(ENGINE_STRUCTURE_TALENT, tChosenTalent);
	VM_ExecuteCommand(309, 2);
	return 0;
}

static int NWScript_ActionUseTalentAtLocation(lua_State *L)
{
	void *tChosenTalent = nwn_checkptr(L, 1, TALENT);
	void *lTargetLocation = lua_nwn_checkloc(L, 2);

	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lTargetLocation);
	StackPushEngineStructure(ENGINE_STRUCTURE_TALENT, tChosenTalent);
	VM_ExecuteCommand(310, 2);
	return 0;
}

static int NWScript_GetGoldPieceValue(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(311, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetIsPlayableRacialType(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(312, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_JumpToLocation(lua_State *L)
{
	void *lDestination = lua_nwn_checkloc(L, 1);

	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lDestination);
	VM_ExecuteCommand(313, 1);
	return 0;
}

static int NWScript_EffectTemporaryHitpoints(lua_State *L)
{
	int nHitPoints = luaL_checkint(L, 1);

	StackPushInteger(nHitPoints);
	VM_ExecuteCommand(314, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetSkillRank(lua_State *L)
{
	int nSkill = luaL_checkint(L, 1);
	dword oTarget = luaL_optint(L, 2, OBJECT_SELF);
	int nBaseSkillRank = luaL_optboolean(L, 3, FALSE);

	StackPushInteger(nBaseSkillRank);
	StackPushObject(oTarget);
	StackPushInteger(nSkill);
	VM_ExecuteCommand(315, 3);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetAttackTarget(lua_State *L)
{
	dword oCreature = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oCreature);
	VM_ExecuteCommand(316, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastAttackType(lua_State *L)
{
	dword oCreature = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oCreature);
	VM_ExecuteCommand(317, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastAttackMode(lua_State *L)
{
	dword oCreature = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oCreature);
	VM_ExecuteCommand(318, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetMaster(lua_State *L)
{
	dword oAssociate = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oAssociate);
	VM_ExecuteCommand(319, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetIsInCombat(lua_State *L)
{
	dword oCreature = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oCreature);
	VM_ExecuteCommand(320, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetLastAssociateCommand(lua_State *L)
{
	dword oAssociate = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oAssociate);
	VM_ExecuteCommand(321, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GiveGoldToCreature(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	int nGP = luaL_checkint(L, 2);
	StackPushInteger(nGP);
	StackPushObject(oCreature);
	VM_ExecuteCommand(322, 2);
	return 0;
}

static int NWScript_SetIsDestroyable(lua_State *L)
{
	int bDestroyable = luaL_checkboolean(L, 1);
	int bRaiseable = luaL_optboolean(L, 2, TRUE);
	int bSelectableWhenDead = luaL_optboolean(L, 3, FALSE);

	StackPushInteger(bSelectableWhenDead);
	StackPushInteger(bRaiseable);
	StackPushInteger(bDestroyable);
	VM_ExecuteCommand(323, 3);
	return 0;
}

static int NWScript_SetLocked(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	int bLocked = luaL_checkboolean(L, 2);

	StackPushInteger(bLocked);
	StackPushObject(oTarget);
	VM_ExecuteCommand(324, 2);
	return 0;
}

static int NWScript_GetLocked(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);

	StackPushObject(oTarget);
	VM_ExecuteCommand(325, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetClickingObject(lua_State *L)
{
	VM_ExecuteCommand(326, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetAssociateListenPatterns(lua_State *L)
{
	dword oTarget = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oTarget);
	VM_ExecuteCommand(327, 1);
	return 0;
}

static int NWScript_GetLastWeaponUsed(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(328, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_ActionInteractObject(lua_State *L)
{
	dword oPlaceable = luaL_checkint(L, 1);

	StackPushObject(oPlaceable);
	VM_ExecuteCommand(329, 1);
	return 0;
}

static int NWScript_GetLastUsedBy(lua_State *L)
{
	VM_ExecuteCommand(330, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetAbilityModifier(lua_State *L)
{
	int nAbility = luaL_checkint(L, 1);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nAbility);
	VM_ExecuteCommand(331, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetIdentified(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(332, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_SetIdentified(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	int bIdentified = luaL_checkboolean(L, 2);

	StackPushInteger(bIdentified);
	StackPushObject(oItem);
	VM_ExecuteCommand(333, 2);
	return 0;
}

static int NWScript_SummonAnimalCompanion(lua_State *L)
{
	dword oMaster = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oMaster);
	VM_ExecuteCommand(334, 1);
	return 0;
}

static int NWScript_SummonFamiliar(lua_State *L)
{
	dword oMaster = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oMaster);
	VM_ExecuteCommand(335, 1);
	return 0;
}

static int NWScript_GetBlockingDoor(lua_State *L)
{
	VM_ExecuteCommand(336, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetIsDoorActionPossible(lua_State *L)
{
	dword oTargetDoor = luaL_checkint(L, 1);
	int nDoorAction = luaL_checkint(L, 2);

	StackPushInteger(nDoorAction);
	StackPushObject(oTargetDoor);
	VM_ExecuteCommand(337, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_DoDoorAction(lua_State *L)
{
	dword oTargetDoor = luaL_checkint(L, 1);
	int nDoorAction = luaL_checkint(L, 2);

	StackPushInteger(nDoorAction);
	StackPushObject(oTargetDoor);
	VM_ExecuteCommand(338, 2);
	return 0;
}

static int NWScript_GetFirstItemInInventory(lua_State *L)
{
	dword oTarget = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oTarget);
	VM_ExecuteCommand(339, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetNextItemInInventory(lua_State *L)
{
	dword oTarget = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oTarget);
	VM_ExecuteCommand(340, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetClassByPosition(lua_State *L)
{
	int nClassPosition = luaL_checkint(L, 1);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nClassPosition);
	VM_ExecuteCommand(341, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLevelByPosition(lua_State *L)
{
	int nClassPosition = luaL_checkint(L, 1);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nClassPosition);
	VM_ExecuteCommand(342, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLevelByClass(lua_State *L)
{
	int nClassType = luaL_checkint(L, 1);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nClassType);
	VM_ExecuteCommand(343, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetDamageDealtByType(lua_State *L)
{
	int nDamageType = luaL_checkint(L, 1);

	StackPushInteger(nDamageType);
	VM_ExecuteCommand(344, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetTotalDamageDealt(lua_State *L)
{
	VM_ExecuteCommand(345, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastDamager(lua_State *L)
{
	dword oObject = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oObject);
	VM_ExecuteCommand(346, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastDisarmed(lua_State *L)
{
	VM_ExecuteCommand(347, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastDisturbed(lua_State *L)
{
	VM_ExecuteCommand(348, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastLocked(lua_State *L)
{
	VM_ExecuteCommand(349, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastUnlocked(lua_State *L)
{
	VM_ExecuteCommand(350, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_EffectSkillIncrease(lua_State *L)
{
	int nSkill = luaL_checkint(L, 1);
	int nValue = luaL_checkint(L, 2);

	StackPushInteger(nValue);
	StackPushInteger(nSkill);
	VM_ExecuteCommand(351, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetInventoryDisturbType(lua_State *L)
{
	VM_ExecuteCommand(352, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetInventoryDisturbItem(lua_State *L)
{
	VM_ExecuteCommand(353, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetHenchman(lua_State *L)
{
	dword oMaster = luaL_optint(L, 1, OBJECT_SELF);
	int nNth = luaL_optint(L, 2, 1);

	StackPushInteger(nNth);
	StackPushObject(oMaster);
	VM_ExecuteCommand(354, 2);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_VersusAlignmentEffect(lua_State *L)
{
	void *eEffect = nwn_checkptr(L, 1, EFFECT);
	int nLawChaos = luaL_optint(L, 2, ALIGNMENT_ALL);
	int nGoodEvil = luaL_optint(L, 3, ALIGNMENT_ALL);

	StackPushInteger(nGoodEvil);
	StackPushInteger(nLawChaos);
	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eEffect);
	VM_ExecuteCommand(355, 3);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_VersusRacialTypeEffect(lua_State *L)
{
	void *eEffect = nwn_checkptr(L, 1, EFFECT);
	int nRacialType = luaL_checkint(L, 2);

	StackPushInteger(nRacialType);
	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eEffect);
	VM_ExecuteCommand(356, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_VersusTrapEffect(lua_State *L)
{
	void *eEffect = nwn_checkptr(L, 1, EFFECT);

	StackPushEngineStructure(ENGINE_STRUCTURE_EFFECT, eEffect);
	VM_ExecuteCommand(357, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetGender(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(358, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
	return 1;
}

static int NWScript_GetIsTalentValid(lua_State *L)
{
	void *tTalent = nwn_checkptr(L, 1, TALENT);

  StackPushEngineStructure(ENGINE_STRUCTURE_TALENT, tTalent);
	VM_ExecuteCommand(359, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_ActionMoveAwayFromLocation(lua_State *L)
{
	void *lMoveAwayFrom = lua_nwn_checkloc(L, 1);
	int bRun = luaL_optboolean(L, 2, FALSE);
	double fMoveAwayRange = luaL_optnumber(L, 3, 40.0);

	StackPushFloat(fMoveAwayRange);
	StackPushInteger(bRun);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lMoveAwayFrom);
	VM_ExecuteCommand(360, 3);
	return 0;
}

static int NWScript_GetAttemptedAttackTarget(lua_State *L)
{
	VM_ExecuteCommand(361, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetTypeFromTalent(lua_State *L)
{
	void *tTalent = nwn_checkptr(L, 1, TALENT);

  StackPushEngineStructure(ENGINE_STRUCTURE_TALENT, tTalent);
	VM_ExecuteCommand(362, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetIdFromTalent(lua_State *L)
{
	void *tTalent = nwn_checkptr(L, 1, TALENT);

  StackPushEngineStructure(ENGINE_STRUCTURE_TALENT, tTalent);
	VM_ExecuteCommand(363, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetAssociate(lua_State *L)
{
	int nAssociateType = luaL_checkint(L, 1);
	dword oMaster = luaL_optint(L, 2, OBJECT_SELF);
	int nTh = luaL_optint(L, 3, 1);

	StackPushInteger(nTh);
	StackPushObject(oMaster);
	StackPushInteger(nAssociateType);
	VM_ExecuteCommand(364, 3);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_AddHenchman(lua_State *L)
{
	dword oMaster = luaL_checkint(L, 1);
	dword oHenchman = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oHenchman);
	StackPushObject(oMaster);
	VM_ExecuteCommand(365, 2);
	return 0;
}

static int NWScript_RemoveHenchman(lua_State *L)
{
	dword oMaster = luaL_checkint(L, 1);
	dword oHenchman = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oHenchman);
	StackPushObject(oMaster);
	VM_ExecuteCommand(366, 2);
	return 0;
}

static int NWScript_AddJournalQuestEntry(lua_State *L)
{
	char *szPlotID = (char *)luaL_checkstring(L, 1);
	int nState = luaL_checkint(L, 2);
	dword oCreature = luaL_checkint(L, 3);
	int bAllPartyMembers = luaL_optboolean(L, 4, TRUE);
	int bAllPlayers = luaL_optboolean(L, 5, FALSE);
	int bAllowOverrideHigher = luaL_optboolean(L, 6, FALSE);

	StackPushInteger(bAllowOverrideHigher);
	StackPushInteger(bAllPlayers);
	StackPushInteger(bAllPartyMembers);
	StackPushObject(oCreature);
	StackPushInteger(nState);
	StackPushString(szPlotID);
	VM_ExecuteCommand(367, 6);
	return 0;
}

static int NWScript_RemoveJournalQuestEntry(lua_State *L)
{
	char *szPlotID = (char *)luaL_checkstring(L, 1);
	dword oCreature = luaL_checkint(L, 2);
	int bAllPartyMembers = luaL_optboolean(L, 3, TRUE);
	int bAllPlayers = luaL_optboolean(L, 4, FALSE);

	StackPushInteger(bAllPlayers);
	StackPushInteger(bAllPartyMembers);
	StackPushObject(oCreature);
	StackPushString(szPlotID);
	VM_ExecuteCommand(368, 4);
	return 0;
}

static int NWScript_GetPCPublicCDKey(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);
	int nSinglePlayerCDKey = luaL_optboolean(L, 2, FALSE);

	StackPushInteger(nSinglePlayerCDKey);
	StackPushObject(oPlayer);
	VM_ExecuteCommand(369, 2);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetPCIPAddress(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);

	StackPushObject(oPlayer);
	VM_ExecuteCommand(370, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetPCPlayerName(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);

	StackPushObject(oPlayer);
	VM_ExecuteCommand(371, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_SetPCLike(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);
	dword oTarget = luaL_checkint(L, 2);

	StackPushObject(oTarget);
	StackPushObject(oPlayer);
	VM_ExecuteCommand(372, 2);
	return 0;
}

static int NWScript_SetPCDislike(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);
	dword oTarget = luaL_checkint(L, 2);

	StackPushObject(oTarget);
	StackPushObject(oPlayer);
	VM_ExecuteCommand(373, 2);
	return 0;
}

static int NWScript_SendMessageToPC(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);
	char *sMessage = (char *)luaL_checkstring(L, 2);

	StackPushString(sMessage);
	StackPushObject(oPlayer);
	VM_ExecuteCommand(374, 2);
	return 0;
}

static int NWScript_GetAttemptedSpellTarget(lua_State *L)
{
	VM_ExecuteCommand(375, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastOpenedBy(lua_State *L)
{
	VM_ExecuteCommand(376, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetHasSpell(lua_State *L)
{
	int nSpell = luaL_checkint(L, 1);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nSpell);
	VM_ExecuteCommand(377, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_OpenStore(lua_State *L)
{
	dword oStore = luaL_checkint(L, 1);
	dword oPC = luaL_checkint(L, 2);
	int nBonusMarkUp = luaL_optint(L, 3, 0);
	int nBonusMarkDown = luaL_optint(L, 4, 0);

	StackPushInteger(nBonusMarkDown);
	StackPushInteger(nBonusMarkUp);
	StackPushObject(oPC);
	StackPushObject(oStore);
	VM_ExecuteCommand(378, 4);
	return 0;
}

static int NWScript_EffectTurned(lua_State *L)
{
	VM_ExecuteCommand(379, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetFirstFactionMember(lua_State *L)
{
	dword oMemberOfFaction = luaL_checkint(L, 1);
	int bPCOnly = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bPCOnly);
	StackPushObject(oMemberOfFaction);
	VM_ExecuteCommand(380, 2);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetNextFactionMember(lua_State *L)
{
	dword oMemberOfFaction = luaL_checkint(L, 1);
	int bPCOnly = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bPCOnly);
	StackPushObject(oMemberOfFaction);
	VM_ExecuteCommand(381, 2);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_ActionForceMoveToLocation(lua_State *L)
{
	void *lDestination = lua_nwn_checkloc(L, 1);
	int bRun = luaL_optboolean(L, 2, FALSE);
	double fTimeout = luaL_optnumber(L, 3, 30.0);

	StackPushFloat(fTimeout);
	StackPushInteger(bRun);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lDestination);
	VM_ExecuteCommand(382, 3);
	return 0;
}

static int NWScript_ActionForceMoveToObject(lua_State *L)
{
	dword oMoveTo = luaL_checkint(L, 1);
	int bRun = luaL_optboolean(L, 2, FALSE);
	double fRange = luaL_optnumber(L, 3, 1.0);
	double fTimeout = luaL_optnumber(L, 4, 30.0);

	StackPushFloat(fTimeout);
	StackPushFloat(fRange);
	StackPushInteger(bRun);
	StackPushObject(oMoveTo);
	VM_ExecuteCommand(383, 4);
	return 0;
}

static int NWScript_GetJournalQuestExperience(lua_State *L)
{
	char *szPlotID = (char *)luaL_checkstring(L, 1);

	StackPushString(szPlotID);
	VM_ExecuteCommand(384, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_JumpToObject(lua_State *L)
{
	dword oToJumpTo = luaL_checkint(L, 1);
	int nWalkStraightLineToPoint = luaL_optint(L, 2, 1);

	StackPushInteger(nWalkStraightLineToPoint);
	StackPushObject(oToJumpTo);
	VM_ExecuteCommand(385, 2);
	return 0;
}

static int NWScript_SetMapPinEnabled(lua_State *L)
{
	dword oMapPin = luaL_checkint(L, 1);
	int nEnabled = luaL_checkboolean(L, 2);

	StackPushInteger(nEnabled);
	StackPushObject(oMapPin);
	VM_ExecuteCommand(386, 2);
	return 0;
}

static int NWScript_EffectHitPointChangeWhenDying(lua_State *L)
{
	double fHitPointChangePerRound = luaL_checknumber(L, 1);

	StackPushFloat(fHitPointChangePerRound);
	VM_ExecuteCommand(387, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_PopUpGUIPanel(lua_State *L)
{
	dword oPC = luaL_checkint(L, 1);
	int nGUIPanel = luaL_checkint(L, 2);

	StackPushInteger(nGUIPanel);
	StackPushObject(oPC);
	VM_ExecuteCommand(388, 2);
	return 0;
}

static int NWScript_ClearPersonalReputation(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	dword oSource = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oSource);
	StackPushObject(oTarget);
	VM_ExecuteCommand(389, 2);
	return 0;
}

static int NWScript_SetIsTemporaryFriend(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	dword oSource = luaL_optint(L, 2, OBJECT_SELF);
	int bDecays = luaL_optboolean(L, 3, FALSE);
	double fDurationInSeconds = luaL_optnumber(L, 4, 180.0);

	StackPushFloat(fDurationInSeconds);
	StackPushInteger(bDecays);
	StackPushObject(oSource);
	StackPushObject(oTarget);
	VM_ExecuteCommand(390, 4);
	return 0;
}

static int NWScript_SetIsTemporaryEnemy(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	dword oSource = luaL_optint(L, 2, OBJECT_SELF);
	int bDecays = luaL_optboolean(L, 3, FALSE);
	double fDurationInSeconds = luaL_optnumber(L, 4, 180.0);

	StackPushFloat(fDurationInSeconds);
	StackPushInteger(bDecays);
	StackPushObject(oSource);
	StackPushObject(oTarget);
	VM_ExecuteCommand(391, 4);
	return 0;
}

static int NWScript_SetIsTemporaryNeutral(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	dword oSource = luaL_optint(L, 2, OBJECT_SELF);
	int bDecays = luaL_optboolean(L, 3, FALSE);
	double fDurationInSeconds = luaL_optnumber(L, 4, 180.0);

	StackPushFloat(fDurationInSeconds);
	StackPushInteger(bDecays);
	StackPushObject(oSource);
	StackPushObject(oTarget);
	VM_ExecuteCommand(392, 4);
	return 0;
}

static int NWScript_GiveXPToCreature(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	int nXpAmount = luaL_checkint(L, 2);

	StackPushInteger(nXpAmount);
	StackPushObject(oCreature);
	VM_ExecuteCommand(393, 2);
	return 0;
}

static int NWScript_SetXP(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	int nXpAmount = luaL_checkint(L, 2);

	StackPushInteger(nXpAmount);
	StackPushObject(oCreature);
	VM_ExecuteCommand(394, 2);
	return 0;
}

static int NWScript_GetXP(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(395, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_IntToHexString(lua_State *L)
{
	int nInteger = luaL_checkint(L, 1);

	StackPushInteger(nInteger);
	VM_ExecuteCommand(396, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetBaseItemType(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(397, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetItemHasItemProperty(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	int nProperty = luaL_checkint(L, 2);

	StackPushInteger(nProperty);
	StackPushObject(oItem);
	VM_ExecuteCommand(398, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_ActionEquipMostDamagingMelee(lua_State *L)
{
	dword oVersus = luaL_optint(L, 1, OBJECT_INVALID);
	int bOffHand = luaL_optboolean(L, 2, FALSE);

	StackPushInteger(bOffHand);
	StackPushObject(oVersus);
	VM_ExecuteCommand(399, 2);
	return 0;
}

static int NWScript_ActionEquipMostDamagingRanged(lua_State *L)
{
	dword oVersus = luaL_optint(L, 1, OBJECT_INVALID);

	StackPushObject(oVersus);
	VM_ExecuteCommand(400, 1);
	return 0;
}

static int NWScript_GetItemACValue(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(401, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_ActionRest(lua_State *L)
{
	int bCreatureToEnemyLineOfSightCheck = luaL_optboolean(L, 1, FALSE);

	StackPushInteger(bCreatureToEnemyLineOfSightCheck);
	VM_ExecuteCommand(402, 1);
	return 0;
}

static int NWScript_ExploreAreaForPlayer(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);
	dword oPlayer = luaL_checkint(L, 2);
	int bExplored = luaL_optboolean(L, 3, TRUE);

	StackPushInteger(bExplored);
	StackPushObject(oPlayer);
	StackPushObject(oArea);
	VM_ExecuteCommand(403, 3);
	return 0;
}

static int NWScript_ActionEquipMostEffectiveArmor(lua_State *L)
{
	VM_ExecuteCommand(404, 0);
	return 0;
}

static int NWScript_GetIsDay(lua_State *L)
{
	VM_ExecuteCommand(405, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetIsNight(lua_State *L)
{
	VM_ExecuteCommand(406, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetIsDawn(lua_State *L)
{
	VM_ExecuteCommand(407, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetIsDusk(lua_State *L)
{
	VM_ExecuteCommand(408, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetIsEncounterCreature(lua_State *L)
{
	dword oCreature = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oCreature);
	VM_ExecuteCommand(409, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetLastPlayerDying(lua_State *L)
{
	VM_ExecuteCommand(410, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetStartingLocation(lua_State *L)
{
	VM_ExecuteCommand(411, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_LOCATION, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_ChangeToStandardFaction(lua_State *L)
{
	dword oCreatureToChange = luaL_checkint(L, 1);
	int nStandardFaction = luaL_checkint(L, 2);

	StackPushInteger(nStandardFaction);
	StackPushObject(oCreatureToChange);
	VM_ExecuteCommand(412, 2);
	return 0;
}

static int NWScript_SoundObjectPlay(lua_State *L)
{
	dword oSound = luaL_checkint(L, 1);

	StackPushObject(oSound);
	VM_ExecuteCommand(413, 1);
	return 0;
}

static int NWScript_SoundObjectStop(lua_State *L)
{
	dword oSound = luaL_checkint(L, 1);

	StackPushObject(oSound);
	VM_ExecuteCommand(414, 1);
	return 0;
}

static int NWScript_SoundObjectSetVolume(lua_State *L)
{
	dword oSound = luaL_checkint(L, 1);
	int nVolume = luaL_checkint(L, 2);

	StackPushInteger(nVolume);
	StackPushObject(oSound);
	VM_ExecuteCommand(415, 2);
	return 0;
}

static int NWScript_SoundObjectSetPosition(lua_State *L)
{
	dword oSound = luaL_checkint(L, 1);
	Vector *vPosition = (Vector *)luaL_checkudata(L, 2, VECTOR);

	StackPushVector(*vPosition);
	StackPushObject(oSound);
	VM_ExecuteCommand(416, 2);
	return 0;
}

static int NWScript_SpeakOneLinerConversation(lua_State *L)
{
	char *sDialogResRef = (char *)luaL_optstring(L, 1, "");
	dword oTokenTarget = luaL_optint(L, 2, OBJECT_TYPE_INVALID);

	StackPushObject(oTokenTarget);
	StackPushString(sDialogResRef);
	VM_ExecuteCommand(417, 2);
	return 0;
}

static int NWScript_GetGold(lua_State *L)
{
	dword oTarget = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oTarget);
	VM_ExecuteCommand(418, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLastRespawnButtonPresser(lua_State *L)
{
	VM_ExecuteCommand(419, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetIsDM(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(420, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_PlayVoiceChat(lua_State *L)
{
	int nVoiceChatID = luaL_checkint(L, 1);
	dword oTarget = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oTarget);
	StackPushInteger(nVoiceChatID);
	VM_ExecuteCommand(421, 2);
	return 0;
}

static int NWScript_GetIsWeaponEffective(lua_State *L)
{
	dword oVersus = luaL_optint(L, 1, OBJECT_INVALID);
	int bOffHand = luaL_optboolean(L, 2, FALSE);

	StackPushInteger(bOffHand);
	StackPushObject(oVersus);
	VM_ExecuteCommand(422, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetLastSpellHarmful(lua_State *L)
{
	VM_ExecuteCommand(423, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_EventActivateItem(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	void *lTarget = lua_nwn_checkloc(L, 2);
	dword oTarget = luaL_optint(L, 3, OBJECT_INVALID);

	StackPushObject(oTarget);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lTarget);
	StackPushObject(oItem);
	VM_ExecuteCommand(424, 3);
  void *pRetVal;
  StackPopEngineStructure(ENGINE_STRUCTURE_EVENT, &pRetVal);
  lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_MusicBackgroundPlay(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);

	StackPushObject(oArea);
	VM_ExecuteCommand(425, 1);
	return 0;
}

static int NWScript_MusicBackgroundStop(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);

	StackPushObject(oArea);
	VM_ExecuteCommand(426, 1);
	return 0;
}

static int NWScript_MusicBackgroundSetDelay(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);
	int nDelay = luaL_checkint(L, 2);

	StackPushInteger(nDelay);
	StackPushObject(oArea);
	VM_ExecuteCommand(427, 2);
	return 0;
}

static int NWScript_MusicBackgroundChangeDay(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);
	int nTrack = luaL_checkint(L, 2);

	StackPushInteger(nTrack);
	StackPushObject(oArea);
	VM_ExecuteCommand(428, 2);
	return 0;
}

static int NWScript_MusicBackgroundChangeNight(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);
	int nTrack = luaL_checkint(L, 2);

	StackPushInteger(nTrack);
	StackPushObject(oArea);
	VM_ExecuteCommand(429, 2);
	return 0;
}

static int NWScript_MusicBattlePlay(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);

	StackPushObject(oArea);
	VM_ExecuteCommand(430, 1);
	return 0;
}

static int NWScript_MusicBattleStop(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);

	StackPushObject(oArea);
	VM_ExecuteCommand(431, 1);
	return 0;
}

static int NWScript_MusicBattleChange(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);
	int nTrack = luaL_checkint(L, 2);

	StackPushInteger(nTrack);
	StackPushObject(oArea);
	VM_ExecuteCommand(432, 2);
	return 0;
}

static int NWScript_AmbientSoundPlay(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);

	StackPushObject(oArea);
	VM_ExecuteCommand(433, 1);
	return 0;
}

static int NWScript_AmbientSoundStop(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);

	StackPushObject(oArea);
	VM_ExecuteCommand(434, 1);
	return 0;
}

static int NWScript_AmbientSoundChangeDay(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);
	int nTrack = luaL_checkint(L, 2);

	StackPushInteger(nTrack);
	StackPushObject(oArea);
	VM_ExecuteCommand(435, 2);
	return 0;
}

static int NWScript_AmbientSoundChangeNight(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);
	int nTrack = luaL_checkint(L, 2);

	StackPushInteger(nTrack);
	StackPushObject(oArea);
	VM_ExecuteCommand(436, 2);
	return 0;
}

static int NWScript_GetLastKiller(lua_State *L)
{
	VM_ExecuteCommand(437, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetSpellCastItem(lua_State *L)
{
	VM_ExecuteCommand(438, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetItemActivated(lua_State *L)
{
	VM_ExecuteCommand(439, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetItemActivator(lua_State *L)
{
	VM_ExecuteCommand(440, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetItemActivatedTargetLocation(lua_State *L)
{
	VM_ExecuteCommand(441, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_LOCATION, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetItemActivatedTarget(lua_State *L)
{
	VM_ExecuteCommand(442, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetIsOpen(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(443, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_TakeGoldFromCreature(lua_State *L)
{
	int nAmount = luaL_checkint(L, 1);
	dword oCreatureToTakeFrom = luaL_checkint(L, 2);
	int bDestroy = luaL_optboolean(L, 3, FALSE);

	StackPushInteger(bDestroy);
	StackPushObject(oCreatureToTakeFrom);
	StackPushInteger(nAmount);
	VM_ExecuteCommand(444, 3);
	return 0;
}

static int NWScript_IsInConversation(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(445, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_EffectAbilityDecrease(lua_State *L)
{
	int nAbility = luaL_checkint(L, 1);
	int nModifyBy = luaL_checkint(L, 2);

	StackPushInteger(nModifyBy);
	StackPushInteger(nAbility);
	VM_ExecuteCommand(446, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectAttackDecrease(lua_State *L)
{
	int nPenalty = luaL_checkint(L, 1);
	int nModifierType = luaL_optint(L, 2, ATTACK_BONUS_MISC);

	StackPushInteger(nModifierType);
	StackPushInteger(nPenalty);
	VM_ExecuteCommand(447, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectDamageDecrease(lua_State *L)
{
	int nPenalty = luaL_checkint(L, 1);
	int nDamageType = luaL_optint(L, 2, DAMAGE_TYPE_MAGICAL);

	StackPushInteger(nDamageType);
	StackPushInteger(nPenalty);
	VM_ExecuteCommand(448, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectDamageImmunityDecrease(lua_State *L)
{
	int nDamageType = luaL_checkint(L, 1);
	int nPercentImmunity = luaL_checkint(L, 2);

	StackPushInteger(nPercentImmunity);
	StackPushInteger(nDamageType);
	VM_ExecuteCommand(449, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectACDecrease(lua_State *L)
{
	int nValue = luaL_checkint(L, 1);
	int nModifyType = luaL_optint(L, 2, AC_DODGE_BONUS);
	int nDamageType = luaL_optint(L, 3, AC_VS_DAMAGE_TYPE_ALL);

	StackPushInteger(nDamageType);
	StackPushInteger(nModifyType);
	StackPushInteger(nValue);
	VM_ExecuteCommand(450, 3);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectMovementSpeedDecrease(lua_State *L)
{
	int nPercentChange = luaL_checkint(L, 1);

	StackPushInteger(nPercentChange);
	VM_ExecuteCommand(451, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectSavingThrowDecrease(lua_State *L)
{
	int nSave = luaL_checkint(L, 1);
	int nValue = luaL_checkint(L, 2);
	int nSaveType = luaL_optint(L, 3, SAVING_THROW_TYPE_ALL);

	StackPushInteger(nSaveType);
	StackPushInteger(nValue);
	StackPushInteger(nSave);
	VM_ExecuteCommand(452, 3);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectSkillDecrease(lua_State *L)
{
	int nSkill = luaL_checkint(L, 1);
	int nValue = luaL_checkint(L, 2);

	StackPushInteger(nValue);
	StackPushInteger(nSkill);
	VM_ExecuteCommand(453, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectSpellResistanceDecrease(lua_State *L)
{
	int nValue = luaL_checkint(L, 1);

	StackPushInteger(nValue);
	VM_ExecuteCommand(454, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetPlotFlag(lua_State *L)
{
	dword oTarget = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oTarget);
	VM_ExecuteCommand(455, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_SetPlotFlag(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	int nPlotFlag = luaL_checkboolean(L, 2);

	StackPushInteger(nPlotFlag);
	StackPushObject(oTarget);
	VM_ExecuteCommand(456, 2);
	return 0;
}

static int NWScript_EffectInvisibility(lua_State *L)
{
	int nInvisibilityType = luaL_checkint(L, 1);

	StackPushInteger(nInvisibilityType);
	VM_ExecuteCommand(457, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectConcealment(lua_State *L)
{
	int nPercentage = luaL_checkint(L, 1);
	int nMissType = luaL_optint(L, 2, MISS_CHANCE_TYPE_NORMAL);

	StackPushInteger(nMissType);
	StackPushInteger(nPercentage);
	VM_ExecuteCommand(458, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectDarkness(lua_State *L)
{
	VM_ExecuteCommand(459, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectDispelMagicAll(lua_State *L)
{
	int nCasterLevel = luaL_optint(L, 1, USE_CREATURE_LEVEL);

	StackPushInteger(nCasterLevel);
	VM_ExecuteCommand(460, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectUltravision(lua_State *L)
{
	VM_ExecuteCommand(461, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectNegativeLevel(lua_State *L)
{
	int nNumLevels = luaL_checkint(L, 1);
	int bHPBonus = luaL_optboolean(L, 2, FALSE);

	StackPushInteger(bHPBonus);
	StackPushInteger(nNumLevels);
	VM_ExecuteCommand(462, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectPolymorph(lua_State *L)
{
	int nPolymorphSelection = luaL_checkint(L, 1);
	int nLocked = luaL_optboolean(L, 2, FALSE);

	StackPushInteger(nLocked);
	StackPushInteger(nPolymorphSelection);
	VM_ExecuteCommand(463, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectSanctuary(lua_State *L)
{
	int nDifficultyClass = luaL_checkint(L, 1);

	StackPushInteger(nDifficultyClass);
	VM_ExecuteCommand(464, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectTrueSeeing(lua_State *L)
{
	VM_ExecuteCommand(465, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectSeeInvisible(lua_State *L)
{
	VM_ExecuteCommand(466, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectTimeStop(lua_State *L)
{
	VM_ExecuteCommand(467, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectBlindness(lua_State *L)
{
	VM_ExecuteCommand(468, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetIsReactionTypeFriendly(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	dword oSource = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oSource);
	StackPushObject(oTarget);
	VM_ExecuteCommand(469, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetIsReactionTypeNeutral(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	dword oSource = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oSource);
	StackPushObject(oTarget);
	VM_ExecuteCommand(470, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetIsReactionTypeHostile(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	dword oSource = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oSource);
	StackPushObject(oTarget);
	VM_ExecuteCommand(471, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_EffectSpellLevelAbsorption(lua_State *L)
{
	int nMaxSpellLevelAbsorbed = luaL_checkint(L, 1);
	int nTotalSpellLevelsAbsorbed = luaL_optint(L, 2, 0);
	int nSpellSchool = luaL_optint(L, 3, SPELL_SCHOOL_GENERAL);

	StackPushInteger(nSpellSchool);
	StackPushInteger(nTotalSpellLevelsAbsorbed);
	StackPushInteger(nMaxSpellLevelAbsorbed);
	VM_ExecuteCommand(472, 3);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectDispelMagicBest(lua_State *L)
{
	int nCasterLevel = luaL_optint(L, 1, USE_CREATURE_LEVEL);

	StackPushInteger(nCasterLevel);
	VM_ExecuteCommand(473, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_ActivatePortal(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	char *sIPaddress = (char *)luaL_optstring(L, 2, "");
	char *sPassword = (char *)luaL_optstring(L, 3, "");
	char *sWaypointTag = (char *)luaL_optstring(L, 4, "");
	int bSeemless = luaL_optboolean(L, 5, FALSE);

	StackPushInteger(bSeemless);
	StackPushString(sWaypointTag);
	StackPushString(sPassword);
	StackPushString(sIPaddress);
	StackPushObject(oTarget);
	VM_ExecuteCommand(474, 5);
	return 0;
}

static int NWScript_GetNumStackedItems(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(475, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SurrenderToEnemies(lua_State *L)
{
	VM_ExecuteCommand(476, 0);
	return 0;
}

static int NWScript_EffectMissChance(lua_State *L)
{
	int nPercentage = luaL_checkint(L, 1);
	int nMissChanceType = luaL_optint(L, 2, MISS_CHANCE_TYPE_NORMAL);

	StackPushInteger(nMissChanceType);
	StackPushInteger(nPercentage);
	VM_ExecuteCommand(477, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetTurnResistanceHD(lua_State *L)
{
	dword oUndead = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oUndead);
	VM_ExecuteCommand(478, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetCreatureSize(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(479, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_EffectDisappearAppear(lua_State *L)
{
	void *lLocation = lua_nwn_checkloc(L, 1);
	int nAnimation = luaL_optint(L, 2, 1);

	StackPushInteger(nAnimation);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lLocation);
	VM_ExecuteCommand(480, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectDisappear(lua_State *L)
{
	int nAnimation = luaL_optint(L, 1, 1);

	StackPushInteger(nAnimation);
	VM_ExecuteCommand(481, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectAppear(lua_State *L)
{
	int nAnimation = luaL_optint(L, 1, 1);

	StackPushInteger(nAnimation);
	VM_ExecuteCommand(482, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_ActionUnlockObject(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);

	StackPushObject(oTarget);
	VM_ExecuteCommand(483, 1);
	return 0;
}

static int NWScript_ActionLockObject(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);

	StackPushObject(oTarget);
	VM_ExecuteCommand(484, 1);
	return 0;
}

static int NWScript_EffectModifyAttacks(lua_State *L)
{
	int nAttacks = luaL_checkint(L, 1);

	StackPushInteger(nAttacks);
	VM_ExecuteCommand(485, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetLastTrapDetected(lua_State *L)
{
	dword oTarget = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oTarget);
	VM_ExecuteCommand(486, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_EffectDamageShield(lua_State *L)
{
	int nDamageAmount = luaL_checkint(L, 1);
  int nRandomAmount = luaL_checkint(L, 2);
  int nDamageType = luaL_checkint(L, 3);

  StackPushInteger(nDamageType);
	StackPushInteger(nRandomAmount);
	StackPushInteger(nDamageAmount);
	VM_ExecuteCommand(487, 3);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetNearestTrapToObject(lua_State *L)
{
	dword oTarget = luaL_optint(L, 1, OBJECT_SELF);
	int nTrapDetected = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(nTrapDetected);
	StackPushObject(oTarget);
	VM_ExecuteCommand(488, 2);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetDeity(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(489, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetSubRace(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);

	StackPushObject(oTarget);
	VM_ExecuteCommand(490, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetFortitudeSavingThrow(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);

	StackPushObject(oTarget);
	VM_ExecuteCommand(491, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetWillSavingThrow(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);

	StackPushObject(oTarget);
	VM_ExecuteCommand(492, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetReflexSavingThrow(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);

	StackPushObject(oTarget);
	VM_ExecuteCommand(493, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetChallengeRating(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(494, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
  return 1;
}

static int NWScript_GetAge(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(495, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetMovementRate(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(496, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetFamiliarCreatureType(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(497, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetAnimalCompanionCreatureType(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(498, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetFamiliarName(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(499, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetAnimalCompanionName(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);

	StackPushObject(oTarget);
	VM_ExecuteCommand(500, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_ActionCastFakeSpellAtObject(lua_State *L)
{
	int nSpell = luaL_checkint(L, 1);
	dword oTarget = luaL_checkint(L, 2);
	int nProjectilePathType = luaL_optint(L, 3, PROJECTILE_PATH_TYPE_DEFAULT);

	StackPushInteger(nProjectilePathType);
	StackPushObject(oTarget);
	StackPushInteger(nSpell);
	VM_ExecuteCommand(501, 3);
	return 0;
}

static int NWScript_ActionCastFakeSpellAtLocation(lua_State *L)
{
	int nSpell = luaL_checkint(L, 1);
	void *lTarget = lua_nwn_checkloc(L, 2);
	int nProjectilePathType = luaL_optint(L, 3, PROJECTILE_PATH_TYPE_DEFAULT);

	StackPushInteger(nProjectilePathType);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lTarget);
	StackPushInteger(nSpell);
	VM_ExecuteCommand(502, 3);
	return 0;
}

static int NWScript_RemoveSummonedAssociate(lua_State *L)
{
	dword oMaster = luaL_checkint(L, 1);
	dword oAssociate = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oAssociate);
	StackPushObject(oMaster);
	VM_ExecuteCommand(503, 2);
	return 0;
}

static int NWScript_SetCameraMode(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);
	int nCameraMode = luaL_checkint(L, 2);

	StackPushInteger(nCameraMode);
	StackPushObject(oPlayer);
	VM_ExecuteCommand(504, 2);
	return 0;
}

static int NWScript_GetIsResting(lua_State *L)
{
	dword oCreature = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oCreature);
	VM_ExecuteCommand(505, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetLastPCRested(lua_State *L)
{
	VM_ExecuteCommand(506, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetWeather(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	int nWeather = luaL_checkint(L, 2);

	StackPushInteger(nWeather);
	StackPushObject(oTarget);
	VM_ExecuteCommand(507, 2);
	return 0;
}

static int NWScript_GetLastRestEventType(lua_State *L)
{
	VM_ExecuteCommand(508, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_StartNewModule(lua_State *L)
{
	char *sModuleName = (char *)luaL_checkstring(L, 1);

	StackPushString(sModuleName);
	VM_ExecuteCommand(509, 1);
	return 0;
}

static int NWScript_EffectSwarm(lua_State *L)
{
	int nLooping = luaL_checkint(L, 1);
	char *sCreatureTemplate1 = (char *)luaL_checkstring(L, 2);
	char *sCreatureTemplate2 = (char *)luaL_optstring(L, 3, "");
	char *sCreatureTemplate3 = (char *)luaL_optstring(L, 4, "");
	char *sCreatureTemplate4 = (char *)luaL_optstring(L, 5, "");

	StackPushString(sCreatureTemplate4);
	StackPushString(sCreatureTemplate3);
	StackPushString(sCreatureTemplate2);
	StackPushString(sCreatureTemplate1);
	StackPushInteger(nLooping);
	VM_ExecuteCommand(510, 5);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetWeaponRanged(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(511, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_DoSinglePlayerAutoSave(lua_State *L)
{
	VM_ExecuteCommand(512, 0);
	return 0;
}

static int NWScript_GetGameDifficulty(lua_State *L)
{
	VM_ExecuteCommand(513, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetTileMainLightColor(lua_State *L)
{
	void *lTileLocation = lua_nwn_checkloc(L, 1);
	int nMainLight1Color = luaL_checkint(L, 2);
	int nMainLight2Color = luaL_checkint(L, 3);

	StackPushInteger(nMainLight2Color);
	StackPushInteger(nMainLight1Color);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lTileLocation);
	VM_ExecuteCommand(514, 3);
	return 0;
}

static int NWScript_SetTileSourceLightColor(lua_State *L)
{
	void *lTileLocation = lua_nwn_checkloc(L, 1);
	int nSourceLight1Color = luaL_checkint(L, 2);
	int nSourceLight2Color = luaL_checkint(L, 3);

	StackPushInteger(nSourceLight2Color);
	StackPushInteger(nSourceLight1Color);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lTileLocation);
	VM_ExecuteCommand(515, 3);
	return 0;
}

static int NWScript_RecomputeStaticLighting(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);

	StackPushObject(oArea);
	VM_ExecuteCommand(516, 1);
	return 0;
}

static int NWScript_GetTileMainLight1Color(lua_State *L)
{
	void *lTile = lua_nwn_checkloc(L, 1);

	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lTile);
	VM_ExecuteCommand(517, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetTileMainLight2Color(lua_State *L)
{
	void *lTile = lua_nwn_checkloc(L, 1);

	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lTile);
	VM_ExecuteCommand(518, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetTileSourceLight1Color(lua_State *L)
{
	void *lTile = lua_nwn_checkloc(L, 1);

	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lTile);
	VM_ExecuteCommand(519, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetTileSourceLight2Color(lua_State *L)
{
	void *lTile = lua_nwn_checkloc(L, 1);

	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lTile);
	VM_ExecuteCommand(520, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetPanelButtonFlash(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);
	int nButton = luaL_checkint(L, 2);
	int nEnableFlash = luaL_checkint(L, 3);

	StackPushInteger(nEnableFlash);
	StackPushInteger(nButton);
	StackPushObject(oPlayer);
	VM_ExecuteCommand(521, 3);
	return 0;
}

static int NWScript_GetCurrentAction(lua_State *L)
{
	dword oObject = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oObject);
	VM_ExecuteCommand(522, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetStandardFactionReputation(lua_State *L)
{
	int nStandardFaction = luaL_checkint(L, 1);
	int nNewReputation = luaL_checkint(L, 2);
	dword oCreature = luaL_optint(L, 3, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nNewReputation);
	StackPushInteger(nStandardFaction);
	VM_ExecuteCommand(523, 3);
	return 0;
}

static int NWScript_GetStandardFactionReputation(lua_State *L)
{
	int nStandardFaction = luaL_checkint(L, 1);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nStandardFaction);
	VM_ExecuteCommand(524, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_FloatingTextStrRefOnCreature(lua_State *L)
{
	int nStrRefToDisplay = luaL_checkint(L, 1);
	dword oCreatureToFloatAbove = luaL_checkint(L, 2);
	int bBroadcastToFaction = luaL_optboolean(L, 3, TRUE);

	StackPushInteger(bBroadcastToFaction);
	StackPushObject(oCreatureToFloatAbove);
	StackPushInteger(nStrRefToDisplay);
	VM_ExecuteCommand(525, 3);
	return 0;
}

static int NWScript_FloatingTextStringOnCreature(lua_State *L)
{
	char *sStringToDisplay = (char *)luaL_checkstring(L, 1);
	dword oCreatureToFloatAbove = luaL_checkint(L, 2);
	int bBroadcastToFaction = luaL_optboolean(L, 3, TRUE);

	StackPushInteger(bBroadcastToFaction);
	StackPushObject(oCreatureToFloatAbove);
	StackPushString(sStringToDisplay);
	VM_ExecuteCommand(526, 3);
	return 0;
}

static int NWScript_GetTrapDisarmable(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);

	StackPushObject(oTrapObject);
	VM_ExecuteCommand(527, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetTrapDetectable(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);

	StackPushObject(oTrapObject);
	VM_ExecuteCommand(528, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetTrapDetectedBy(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);
	dword oCreature = luaL_checkint(L, 2);

	StackPushObject(oCreature);
	StackPushObject(oTrapObject);
	VM_ExecuteCommand(529, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetTrapFlagged(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);

	StackPushObject(oTrapObject);
	VM_ExecuteCommand(530, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetTrapBaseType(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);

	StackPushObject(oTrapObject);
	VM_ExecuteCommand(531, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetTrapOneShot(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);

	StackPushObject(oTrapObject);
	VM_ExecuteCommand(532, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetTrapCreator(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);

	StackPushObject(oTrapObject);
	VM_ExecuteCommand(533, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetTrapKeyTag(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);

	StackPushObject(oTrapObject);
	VM_ExecuteCommand(534, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetTrapDisarmDC(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);

	StackPushObject(oTrapObject);
	VM_ExecuteCommand(535, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetTrapDetectDC(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);

	StackPushObject(oTrapObject);
	VM_ExecuteCommand(536, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLockKeyRequired(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(537, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetLockKeyTag(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(538, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetLockLockable(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(539, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetLockUnlockDC(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(540, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetLockLockDC(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(541, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetPCLevellingUp(lua_State *L)
{
	VM_ExecuteCommand(542, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetHasFeatEffect(lua_State *L)
{
	int nFeat = luaL_checkint(L, 1);
	dword oObject = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oObject);
	StackPushInteger(nFeat);
	VM_ExecuteCommand(543, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_SetPlaceableIllumination(lua_State *L)
{
	dword oPlaceable = luaL_optint(L, 1, OBJECT_SELF);
	int bIlluminate = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bIlluminate);
	StackPushObject(oPlaceable);
	VM_ExecuteCommand(544, 2);
	return 0;
}

static int NWScript_GetPlaceableIllumination(lua_State *L)
{
	dword oPlaceable = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oPlaceable);
	VM_ExecuteCommand(545, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetIsPlaceableObjectActionPossible(lua_State *L)
{
	dword oPlaceable = luaL_checkint(L, 1);
	int nPlaceableAction = luaL_checkint(L, 2);

	StackPushInteger(nPlaceableAction);
	StackPushObject(oPlaceable);
	VM_ExecuteCommand(546, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_DoPlaceableObjectAction(lua_State *L)
{
	dword oPlaceable = luaL_checkint(L, 1);
	int nPlaceableAction = luaL_checkint(L, 2);

	StackPushInteger(nPlaceableAction);
	StackPushObject(oPlaceable);
	VM_ExecuteCommand(547, 2);
	return 0;
}

static int NWScript_GetFirstPC(lua_State *L)
{
	VM_ExecuteCommand(548, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetNextPC(lua_State *L)
{
	VM_ExecuteCommand(549, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetTrapDetectedBy(lua_State *L)
{
	dword oTrap = luaL_checkint(L, 1);
	dword oDetector = luaL_checkint(L, 2);
	int bDetected = luaL_optboolean(L, 3, TRUE);

	StackPushInteger(bDetected);
	StackPushObject(oDetector);
	StackPushObject(oTrap);
	VM_ExecuteCommand(550, 3);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetIsTrapped(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(551, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_EffectTurnResistanceDecrease(lua_State *L)
{
	int nHitDice = luaL_checkint(L, 1);

	StackPushInteger(nHitDice);
	VM_ExecuteCommand(552, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_EffectTurnResistanceIncrease(lua_State *L)
{
	int nHitDice = luaL_checkint(L, 1);

	StackPushInteger(nHitDice);
	VM_ExecuteCommand(553, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_PopUpDeathGUIPanel(lua_State *L)
{
	dword oPC = luaL_checkint(L, 1);
	int bRespawnButtonEnabled = luaL_optboolean(L, 2, TRUE);
	int bWaitForHelpButtonEnabled = luaL_optboolean(L, 3, TRUE);
	int nHelpStringReference = luaL_optint(L, 4, 0);
	char *sHelpString = (char *)luaL_optstring(L, 5, "");

	StackPushString(sHelpString);
	StackPushInteger(nHelpStringReference);
	StackPushInteger(bWaitForHelpButtonEnabled);
	StackPushInteger(bRespawnButtonEnabled);
	StackPushObject(oPC);
	VM_ExecuteCommand(554, 5);
	return 0;
}

static int NWScript_SetTrapDisabled(lua_State *L)
{
	dword oTrap = luaL_checkint(L, 1);

	StackPushObject(oTrap);
	VM_ExecuteCommand(555, 1);
	return 0;
}

static int NWScript_GetLastHostileActor(lua_State *L)
{
	dword oVictim = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oVictim);
	VM_ExecuteCommand(556, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_ExportAllCharacters(lua_State *L)
{
	VM_ExecuteCommand(557, 0);
	return 0;
}

static int NWScript_MusicBackgroundGetDayTrack(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);

	StackPushObject(oArea);
	VM_ExecuteCommand(558, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_MusicBackgroundGetNightTrack(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);

	StackPushObject(oArea);
	VM_ExecuteCommand(559, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_WriteTimestampedLogEntry(lua_State *L)
{
	char *sLogEntry = (char *)luaL_checkstring(L, 1);

	StackPushString(sLogEntry);
	VM_ExecuteCommand(560, 1);
	return 0;
}

static int NWScript_GetModuleName(lua_State *L)
{
	VM_ExecuteCommand(561, 0);
	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetFactionLeader(lua_State *L)
{
	dword oMemberOfFaction = luaL_checkint(L, 1);

	StackPushObject(oMemberOfFaction);
	VM_ExecuteCommand(562, 1);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SendMessageToAllDMs(lua_State *L)
{
	char *szMessage = (char *)luaL_checkstring(L, 1);

  StackPushString(szMessage);
	VM_ExecuteCommand(563, 1);
	return 0;
}

static int NWScript_EndGame(lua_State *L)
{
	char *sEndMovie = (char *)luaL_checkstring(L, 1);

  StackPushString(sEndMovie);
	VM_ExecuteCommand(564, 1);
	return 0;
}

static int NWScript_BootPC(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);

  StackPushObject(oPlayer);
	VM_ExecuteCommand(565, 1);
	return 0;
}

static int NWScript_ActionCounterSpell(lua_State *L)
{
	dword oCounterSpellTarget = luaL_checkint(L, 1);

  StackPushObject(oCounterSpellTarget);
	VM_ExecuteCommand(566, 1);
	return 0;
}

static int NWScript_AmbientSoundSetDayVolume(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);
  int nVolume = luaL_checkint(L, 2);

  StackPushInteger(nVolume);
	StackPushObject(oArea);
	VM_ExecuteCommand(567, 2);
	return 0;
}

static int NWScript_AmbientSoundSetNightVolume(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);
  int nVolume = luaL_checkint(L, 2);

  StackPushInteger(nVolume);
	StackPushObject(oArea);
	VM_ExecuteCommand(568, 2);
	return 0;
}

static int NWScript_MusicBackgroundGetBattleTrack(lua_State *L)
{
  dword oArea = luaL_checkint(L, 1);

  StackPushObject(oArea);
	VM_ExecuteCommand(569, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetHasInventory(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

  StackPushObject(oObject);
	VM_ExecuteCommand(570, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
	return 1;
}

static int NWScript_GetStrRefSoundDuration(lua_State *L)
{
	int nStrRef = luaL_checkint(L, 1);

  StackPushInteger(nStrRef);
	VM_ExecuteCommand(571, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
  return 1;
}

static int NWScript_AddToParty(lua_State *L)
{
	dword oPC = luaL_checkint(L, 1);
  dword oPartyLeader = luaL_checkint(L, 2);

  StackPushObject(oPartyLeader);
	StackPushObject(oPC);
	VM_ExecuteCommand(572, 2);
	return 0;
}

static int NWScript_RemoveFromParty(lua_State *L)
{
	dword oPC = luaL_checkint(L, 1);

  StackPushObject(oPC);
	VM_ExecuteCommand(573, 1);
	return 0;
}

static int NWScript_GetStealthMode(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

  StackPushObject(oCreature);
	VM_ExecuteCommand(574, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetDetectMode(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

  StackPushObject(oCreature);
	VM_ExecuteCommand(575, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetDefensiveCastingMode(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

  StackPushObject(oCreature);
	VM_ExecuteCommand(576, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetAppearanceType(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

  StackPushObject(oCreature);
	VM_ExecuteCommand(577, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SpawnScriptDebugger(lua_State *L)
{
	VM_ExecuteCommand(578, 0);
	return 0;
}

static int NWScript_GetModuleItemAcquiredStackSize(lua_State *L)
{
	VM_ExecuteCommand(579, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_DecrementRemainingFeatUses(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
  int nFeat = luaL_checkint(L, 2);

  StackPushInteger(nFeat);
	StackPushObject(oCreature);
	VM_ExecuteCommand(580, 2);
	return 0;
}

static int NWScript_DecrementRemainingSpellUses(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
  int nSpell = luaL_checkint(L, 2);

  StackPushInteger(nSpell);
	StackPushObject(oCreature);
	VM_ExecuteCommand(581, 2);
	return 0;
}

static int NWScript_GetResRef(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(582, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_EffectPetrify(lua_State *L)
{
	VM_ExecuteCommand(583, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_CopyItem(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
  dword oTargetInventory = luaL_optint(L, 2, OBJECT_INVALID);
  int bCopyVars = luaL_optboolean(L, 3, FALSE);

	StackPushInteger(bCopyVars);
	StackPushObject(oTargetInventory);
	StackPushObject(oItem);
	VM_ExecuteCommand(584, 3);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_EffectCutsceneParalyze(lua_State *L)
{
	VM_ExecuteCommand(585, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetDroppableFlag(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

  StackPushObject(oItem);
	VM_ExecuteCommand(586, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetUseableFlag(lua_State *L)
{
  dword oObject = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oObject);
	VM_ExecuteCommand(587, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetStolenFlag(lua_State *L)
{
	dword oStolen = luaL_checkint(L, 1);

  StackPushObject(oStolen);
	VM_ExecuteCommand(588, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_SetCampaignFloat(lua_State *L)
{
	char *sCampaignName = (char *)luaL_checkstring(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	double flFloat = luaL_checknumber(L, 3);
  dword oPlayer = luaL_optint(L, 4, OBJECT_INVALID);

	StackPushObject(oPlayer);
	StackPushFloat(flFloat);
	StackPushString(sVarName);
	StackPushString(sCampaignName);
	VM_ExecuteCommand(589, 4);
	return 0;
}

static int NWScript_SetCampaignInt(lua_State *L)
{
	char *sCampaignName = (char *)luaL_checkstring(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	int nInt = luaL_checkint(L, 3);
	dword oPlayer = luaL_optint(L, 4, OBJECT_INVALID);

	StackPushObject(oPlayer);
	StackPushInteger(nInt);
	StackPushString(sVarName);
	StackPushString(sCampaignName);
	VM_ExecuteCommand(590, 4);
	return 0;
}

static int NWScript_SetCampaignVector(lua_State *L)
{
	char *sCampaignName = (char *)luaL_checkstring(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	Vector *vVector = (Vector *)luaL_checkudata(L, 3, VECTOR);
	dword oPlayer = luaL_optint(L, 4, OBJECT_INVALID);

	StackPushObject(oPlayer);
	StackPushVector(*vVector);
	StackPushString(sVarName);
	StackPushString(sCampaignName);
	VM_ExecuteCommand(591, 4);
	return 0;
}

static int NWScript_SetCampaignLocation(lua_State *L)
{
	char *sCampaignName = (char *)luaL_checkstring(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	void *locLocation = lua_nwn_checkloc(L, 3);
	dword oPlayer = luaL_optint(L, 4, OBJECT_INVALID);

	StackPushObject(oPlayer);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, locLocation);
	StackPushString(sVarName);
	StackPushString(sCampaignName);
	VM_ExecuteCommand(592, 4);
	return 0;
}

static int NWScript_SetCampaignString(lua_State *L)
{
	char *sCampaignName = (char *)luaL_checkstring(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	char *sString = (char *)luaL_checkstring(L, 3);
	dword oPlayer = luaL_optint(L, 4, OBJECT_INVALID);

	StackPushObject(oPlayer);
	StackPushString(sString);
	StackPushString(sVarName);
	StackPushString(sCampaignName);
	VM_ExecuteCommand(593, 4);
	return 0;
}

static int NWScript_DestroyCampaignDatabase(lua_State *L)
{
	char *sCampaignName = (char *)luaL_checkstring(L, 1);

	StackPushString(sCampaignName);
	VM_ExecuteCommand(594, 1);
	return 0;
}

static int NWScript_GetCampaignFloat(lua_State *L)
{
	char *sCampaignName = (char *)luaL_checkstring(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	dword oPlayer = luaL_optint(L, 3, OBJECT_INVALID);

	StackPushObject(oPlayer);
	StackPushString(sVarName);
	StackPushString(sCampaignName);
	VM_ExecuteCommand(595, 3);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
  return 1;
}

static int NWScript_GetCampaignInt(lua_State *L)
{
	char *sCampaignName = (char *)luaL_checkstring(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	dword oPlayer = luaL_optint(L, 3, OBJECT_INVALID);

	StackPushObject(oPlayer);
	StackPushString(sVarName);
	StackPushString(sCampaignName);
	VM_ExecuteCommand(596, 3);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetCampaignVector(lua_State *L)
{
	char *sCampaignName = (char *)luaL_checkstring(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	dword oPlayer = luaL_optint(L, 3, OBJECT_INVALID);

	StackPushObject(oPlayer);
	StackPushString(sVarName);
	StackPushString(sCampaignName);
	VM_ExecuteCommand(597, 3);
	Vector *vRetVal = (Vector *)lua_newuserdata(L, sizeof(Vector));
	StackPopVector(vRetVal);
	luaL_getmetatable(L, VECTOR);
  lua_setmetatable(L, -2);
  return 1;
}

static int NWScript_GetCampaignLocation(lua_State *L)
{
	char *sCampaignName = (char *)luaL_checkstring(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	dword oPlayer = luaL_optint(L, 3, OBJECT_INVALID);

	StackPushObject(oPlayer);
	StackPushString(sVarName);
	StackPushString(sCampaignName);
	VM_ExecuteCommand(598, 3);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_LOCATION, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetCampaignString(lua_State *L)
{
	char *sCampaignName = (char *)luaL_checkstring(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	dword oPlayer = luaL_optint(L, 3, OBJECT_INVALID);

	StackPushObject(oPlayer);
	StackPushString(sVarName);
	StackPushString(sCampaignName);
	VM_ExecuteCommand(599, 3);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_CopyObject(lua_State *L)
{
	dword oSource = luaL_checkint(L, 1);
	void *locLocation = lua_nwn_checkloc(L, 2);
	dword oOwner  = luaL_optint(L, 3, OBJECT_INVALID);
	char *sNewTag  = (char *)luaL_optstring(L, 4, "");

	StackPushString(sNewTag);
	StackPushObject(oOwner);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, locLocation);
	StackPushObject(oSource);
	VM_ExecuteCommand(600, 4);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_DeleteCampaignVariable(lua_State *L)
{
	char *sCampaignName = (char *)luaL_checkstring(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	dword oPlayer = luaL_optint(L, 3, OBJECT_INVALID);

	StackPushObject(oPlayer);
	StackPushString(sVarName);
	StackPushString(sCampaignName);
	VM_ExecuteCommand(601, 3);
	return 0;
}

static int NWScript_StoreCampaignObject(lua_State *L)
{
	char *sCampaignName = (char *)luaL_checkstring(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	dword oObject = luaL_checkint(L, 3);
	dword oPlayer = luaL_optint(L, 4, OBJECT_INVALID);

	StackPushObject(oPlayer);
	StackPushObject(oObject);
	StackPushString(sVarName);
	StackPushString(sCampaignName);
	VM_ExecuteCommand(602, 4);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_RetrieveCampaignObject(lua_State *L)
{
	char *sCampaignName = (char *)luaL_checkstring(L, 1);
	char *sVarName = (char *)luaL_checkstring(L, 2);
	void *locLocation = lua_nwn_checkloc(L, 3);
	dword oOwner  = luaL_optint(L, 4, OBJECT_INVALID);
	dword oPlayer = luaL_optint(L, 5, OBJECT_INVALID);

	StackPushObject(oPlayer);
	StackPushObject(oOwner);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, locLocation);
	StackPushString(sVarName);
	StackPushString(sCampaignName);
	VM_ExecuteCommand(603, 5);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_EffectCutsceneDominated(lua_State *L)
{
	VM_ExecuteCommand(604, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetItemStackSize(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(605, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetItemStackSize(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	int nSize = luaL_checkint(L, 2);

	StackPushInteger(nSize);
	StackPushObject(oItem);
	VM_ExecuteCommand(606, 2);
	return 0;
}

static int NWScript_GetItemCharges(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(607, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetItemCharges(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	int nCharges = luaL_checkint(L, 2);

	StackPushInteger(nCharges);
	StackPushObject(oItem);
	VM_ExecuteCommand(608, 2);
	return 0;
}

static int NWScript_AddItemProperty(lua_State *L)
{
	int nDurationType = luaL_checkint(L, 1);
	void *ipProperty = nwn_checkptr(L, 2, ITEMPROPERTY);
	dword oItem = luaL_checkint(L, 3);
	double fDuration = luaL_optnumber(L, 4, 0.0);

	StackPushFloat(fDuration);
	StackPushObject(oItem);
	StackPushEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, ipProperty);
	StackPushInteger(nDurationType);
	VM_ExecuteCommand(609, 4);
	return 0;
}

static int NWScript_RemoveItemProperty(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	void *ipProperty = nwn_checkptr(L, 2, ITEMPROPERTY);

	StackPushEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, ipProperty);
	StackPushObject(oItem);
	VM_ExecuteCommand(610, 2);
	return 0;
}

static int NWScript_GetIsItemPropertyValid(lua_State *L)
{
	void *ipProperty = nwn_checkptr(L, 1, ITEMPROPERTY);

	StackPushEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, ipProperty);
	VM_ExecuteCommand(611, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetFirstItemProperty(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(612, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetNextItemProperty(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(613, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetItemPropertyType(lua_State *L)
{
	void *ip = nwn_checkptr(L, 1, ITEMPROPERTY);

	StackPushEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, ip);
	VM_ExecuteCommand(614, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetItemPropertyDurationType(lua_State *L)
{
	void *ip = nwn_checkptr(L, 1, ITEMPROPERTY);

	StackPushEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, ip);
	VM_ExecuteCommand(615, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_ItemPropertyAbilityBonus(lua_State *L)
{
	int nAbility = luaL_checkint(L, 1);
	int nBonus = luaL_checkint(L, 2);

	StackPushInteger(nBonus);
	StackPushInteger(nAbility);
	VM_ExecuteCommand(616, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_ItemPropertyACBonus(lua_State *L)
{
	int nBonus = luaL_checkint(L, 1);

  StackPushInteger(nBonus);
	VM_ExecuteCommand(617, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
 	return 1;
}

static int NWScript_ItemPropertyACBonusVsAlign(lua_State *L)
{
	int nAlignGroup = luaL_checkint(L, 1);
  int nACBonus = luaL_checkint(L, 2);

  StackPushInteger(nACBonus);
	StackPushInteger(nAlignGroup);
	VM_ExecuteCommand(618, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyACBonusVsDmgType(lua_State *L)
{
	int nDamageType = luaL_checkint(L, 1);
  int nACBonus = luaL_checkint(L, 2);

  StackPushInteger(nACBonus);
	StackPushInteger(nDamageType);
	VM_ExecuteCommand(619, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyACBonusVsRace(lua_State *L)
{
	int nRace = luaL_checkint(L, 1);
  int nACBonus = luaL_checkint(L, 2);

  StackPushInteger(nACBonus);
	StackPushInteger(nRace);
	VM_ExecuteCommand(620, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyACBonusVsSAlign(lua_State *L)
{
	int nAlign = luaL_checkint(L, 1);
  int nACBonus = luaL_checkint(L, 2);

  StackPushInteger(nACBonus);
	StackPushInteger(nAlign);
	VM_ExecuteCommand(621, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyEnhancementBonus(lua_State *L)
{
	int nEnhancementBonus = luaL_checkint(L, 1);
  StackPushInteger(nEnhancementBonus);

	VM_ExecuteCommand(622, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyEnhancementBonusVsAlign(lua_State *L)
{
	int nAlignGroup = luaL_checkint(L, 1);
  int nBonus = luaL_checkint(L, 2);

  StackPushInteger(nBonus);
	StackPushInteger(nAlignGroup);
	VM_ExecuteCommand(623, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyEnhancementBonusVsRace(lua_State *L)
{
	int nRace = luaL_checkint(L, 1);
  int nBonus = luaL_checkint(L, 2);

  StackPushInteger(nBonus);
	StackPushInteger(nRace);
	VM_ExecuteCommand(624, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyEnhancementBonusVsSAlign(lua_State *L)
{
	int nAlign = luaL_checkint(L, 1);
  int nBonus = luaL_checkint(L, 2);

  StackPushInteger(nBonus);
	StackPushInteger(nAlign);
	VM_ExecuteCommand(625, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyEnhancementPenalty(lua_State *L)
{
	int nPenalty = luaL_checkint(L, 1);

  StackPushInteger(nPenalty);
	VM_ExecuteCommand(626, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyWeightReduction(lua_State *L)
{
	int nReduction = luaL_checkint(L, 1);

  StackPushInteger(nReduction);
	VM_ExecuteCommand(627, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyBonusFeat(lua_State *L)
{
	int nFeat = luaL_checkint(L, 1);

  StackPushInteger(nFeat);
	VM_ExecuteCommand(628, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyBonusLevelSpell(lua_State *L)
{
	int nClass = luaL_checkint(L, 1);
  int nSpellLevel = luaL_checkint(L, 2);

  StackPushInteger(nSpellLevel);
	StackPushInteger(nClass);
	VM_ExecuteCommand(629, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyCastSpell(lua_State *L)
{
	int nSpell = luaL_checkint(L, 1);
  int nNumUses = luaL_checkint(L, 2);

  StackPushInteger(nNumUses);
	StackPushInteger(nSpell);
	VM_ExecuteCommand(630, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyDamageBonus(lua_State *L)
{
	int nDamageType = luaL_checkint(L, 1);
  int nDamage = luaL_checkint(L, 2);

  StackPushInteger(nDamage);
	StackPushInteger(nDamageType);
	VM_ExecuteCommand(631, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyDamageBonusVsAlign(lua_State *L)
{
	int nAlignGroup = luaL_checkint(L, 1);
  int nDamageType = luaL_checkint(L, 2);
  int nDamage = luaL_checkint(L, 3);

  StackPushInteger(nDamage);
	StackPushInteger(nDamageType);
	StackPushInteger(nAlignGroup);
	VM_ExecuteCommand(632, 3);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyDamageBonusVsRace(lua_State *L)
{
	int nRace = luaL_checkint(L, 1);
  int nDamageType = luaL_checkint(L, 2);
  int nDamage = luaL_checkint(L, 3);

  StackPushInteger(nDamage);
	StackPushInteger(nDamageType);
	StackPushInteger(nRace);
	VM_ExecuteCommand(633, 3);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyDamageBonusVsSAlign(lua_State *L)
{
	int nAlign = luaL_checkint(L, 1);
  int nDamageType = luaL_checkint(L, 2);
  int nDamage = luaL_checkint(L, 3);

  StackPushInteger(nDamage);
	StackPushInteger(nDamageType);
	StackPushInteger(nAlign);
	VM_ExecuteCommand(634, 3);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyDamageImmunity(lua_State *L)
{
	int nImmuneBonus = luaL_checkint(L, 1);
  int nDamageType = luaL_checkint(L, 2);

  StackPushInteger(nImmuneBonus);
	StackPushInteger(nDamageType);
	VM_ExecuteCommand(635, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyDamagePenalty(lua_State *L)
{
	int nPenalty = luaL_checkint(L, 1);

  StackPushInteger(nPenalty);
	VM_ExecuteCommand(636, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyDamageReduction(lua_State *L)
{
	int nEnhancement = luaL_checkint(L, 1);
  int nHPSoak = luaL_checkint(L, 2);

  StackPushInteger(nHPSoak);
	StackPushInteger(nEnhancement);
	VM_ExecuteCommand(637, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyDamageResistance(lua_State *L)
{
	int nDamageType = luaL_checkint(L, 1);
  int nHPResist = luaL_checkint(L, 2);

  StackPushInteger(nHPResist);
	StackPushInteger(nDamageType);
	VM_ExecuteCommand(638, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyDamageVulnerability(lua_State *L)
{
	int nDamageType = luaL_checkint(L, 1);
  int nVulnerability = luaL_checkint(L, 2);

  StackPushInteger(nVulnerability);
	StackPushInteger(nDamageType);
	VM_ExecuteCommand(639, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyDarkvision(lua_State *L)
{
	VM_ExecuteCommand(640, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyDecreaseAbility(lua_State *L)
{
	int nAbility = luaL_checkint(L, 1);
  int nModifier = luaL_checkint(L, 2);

  StackPushInteger(nModifier);
	StackPushInteger(nAbility);
	VM_ExecuteCommand(641, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyDecreaseAC(lua_State *L)
{
	int nModifierType = luaL_checkint(L, 1);
  int nPenalty = luaL_checkint(L, 2);

  StackPushInteger(nPenalty);
	StackPushInteger(nModifierType);
	VM_ExecuteCommand(642, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyDecreaseSkill(lua_State *L)
{
	int nSkill = luaL_checkint(L, 1);
  int nPenalty = luaL_checkint(L, 2);

  StackPushInteger(nPenalty);
	StackPushInteger(nSkill);
	VM_ExecuteCommand(643, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyContainerReducedWeight(lua_State *L)
{
	int nContainerType = luaL_checkint(L, 1);

  StackPushInteger(nContainerType);
	VM_ExecuteCommand(644, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyExtraMeleeDamageType(lua_State *L)
{
	int nDamageType = luaL_checkint(L, 1);

  StackPushInteger(nDamageType);
	VM_ExecuteCommand(645, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyExtraRangeDamageType(lua_State *L)
{
	int nDamageType = luaL_checkint(L, 1);

  StackPushInteger(nDamageType);
	VM_ExecuteCommand(646, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyHaste(lua_State *L)
{
	VM_ExecuteCommand(647, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyHolyAvenger(lua_State *L)
{
	VM_ExecuteCommand(648, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyImmunityMisc(lua_State *L)
{
	int nImmunityType = luaL_checkint(L, 1);

  StackPushInteger(nImmunityType);
	VM_ExecuteCommand(649, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyImprovedEvasion(lua_State *L)
{
	VM_ExecuteCommand(650, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyBonusSpellResistance(lua_State *L)
{
	int nBonus = luaL_checkint(L, 1);

  StackPushInteger(nBonus);
	VM_ExecuteCommand(651, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyBonusSavingThrowVsX(lua_State *L)
{
	int nBonusType = luaL_checkint(L, 1);
  int nBonus = luaL_checkint(L, 2);

  StackPushInteger(nBonus);
	StackPushInteger(nBonusType);
	VM_ExecuteCommand(652, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyBonusSavingThrow(lua_State *L)
{
	int nBaseSaveType = luaL_checkint(L, 1);
  int nBonus = luaL_checkint(L, 2);

  StackPushInteger(nBonus);
	StackPushInteger(nBaseSaveType);
	VM_ExecuteCommand(653, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyKeen(lua_State *L)
{
	VM_ExecuteCommand(654, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyLight(lua_State *L)
{
	int nBrightness = luaL_checkint(L, 1);
  int nColor = luaL_checkint(L, 2);

  StackPushInteger(nColor);
	StackPushInteger(nBrightness);
	VM_ExecuteCommand(655, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyMaxRangeStrengthMod(lua_State *L)
{
	int nModifier = luaL_checkint(L, 1);

  StackPushInteger(nModifier);
	VM_ExecuteCommand(656, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyNoDamage(lua_State *L)
{
	VM_ExecuteCommand(657, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyOnHitProps(lua_State *L)
{
	int nProperty = luaL_checkint(L, 1);
	int nSaveDC = luaL_checkint(L, 2);
  int nSpecial = luaL_optint(L, 3, 0);

	StackPushInteger(nSpecial);
	StackPushInteger(nSaveDC);
	StackPushInteger(nProperty);
	VM_ExecuteCommand(658, 3);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyReducedSavingThrowVsX(lua_State *L)
{
	int nBaseSaveType = luaL_checkint(L, 1);
  int nPenalty = luaL_checkint(L, 2);

  StackPushInteger(nPenalty);
	StackPushInteger(nBaseSaveType);
	VM_ExecuteCommand(659, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyReducedSavingThrow(lua_State *L)
{
	int nBonusType = luaL_checkint(L, 1);
  int nPenalty = luaL_checkint(L, 2);

  StackPushInteger(nPenalty);
	StackPushInteger(nBonusType);
	VM_ExecuteCommand(660, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyRegeneration(lua_State *L)
{
	int nRegenAmount = luaL_checkint(L, 1);

  StackPushInteger(nRegenAmount);
	VM_ExecuteCommand(661, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertySkillBonus(lua_State *L)
{
	int nSkill = luaL_checkint(L, 1);
  int nBonus = luaL_checkint(L, 2);

  StackPushInteger(nBonus);
	StackPushInteger(nSkill);
	VM_ExecuteCommand(662, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertySpellImmunitySpecific(lua_State *L)
{
	int nSpell = luaL_checkint(L, 1);

  StackPushInteger(nSpell);
	VM_ExecuteCommand(663, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertySpellImmunitySchool(lua_State *L)
{
	int nSchool = luaL_checkint(L, 1);

  StackPushInteger(nSchool);
	VM_ExecuteCommand(664, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyThievesTools(lua_State *L)
{
	int nModifier = luaL_checkint(L, 1);

  StackPushInteger(nModifier);
	VM_ExecuteCommand(665, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyAttackBonus(lua_State *L)
{
	int nBonus = luaL_checkint(L, 1);

  StackPushInteger(nBonus);
	VM_ExecuteCommand(666, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyAttackBonusVsAlign(lua_State *L)
{
	int nAlignGroup = luaL_checkint(L, 1);
  int nBonus = luaL_checkint(L, 2);

  StackPushInteger(nBonus);
	StackPushInteger(nAlignGroup);
	VM_ExecuteCommand(667, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyAttackBonusVsRace(lua_State *L)
{
	int nRace = luaL_checkint(L, 1);
  int nBonus = luaL_checkint(L, 2);

  StackPushInteger(nBonus);
	StackPushInteger(nRace);
	VM_ExecuteCommand(668, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyAttackBonusVsSAlign(lua_State *L)
{
	int nAlignment = luaL_checkint(L, 1);
  int nBonus = luaL_checkint(L, 2);

  StackPushInteger(nBonus);
	StackPushInteger(nAlignment);
	VM_ExecuteCommand(669, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyAttackPenalty(lua_State *L)
{
	int nPenalty = luaL_checkint(L, 1);

  StackPushInteger(nPenalty);
	VM_ExecuteCommand(670, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyUnlimitedAmmo(lua_State *L)
{
  int nAmmoDamage = luaL_optint(L, 1, IP_CONST_UNLIMITEDAMMO_BASIC);

	StackPushInteger(nAmmoDamage);
	VM_ExecuteCommand(671, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyLimitUseByAlign(lua_State *L)
{
	int nAlignGroup = luaL_checkint(L, 1);

  StackPushInteger(nAlignGroup);
	VM_ExecuteCommand(672, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyLimitUseByClass(lua_State *L)
{
	int nClass = luaL_checkint(L, 1);

  StackPushInteger(nClass);
	VM_ExecuteCommand(673, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyLimitUseByRace(lua_State *L)
{
	int nRace = luaL_checkint(L, 1);

  StackPushInteger(nRace);
	VM_ExecuteCommand(674, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyLimitUseBySAlign(lua_State *L)
{
	int nAlignment = luaL_checkint(L, 1);

  StackPushInteger(nAlignment);
	VM_ExecuteCommand(675, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_BadBadReplaceMeThisDoesNothing(lua_State *L)
{
	VM_ExecuteCommand(676, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyVampiricRegeneration(lua_State *L)
{
	int nRegenAmount = luaL_checkint(L, 1);

  StackPushInteger(nRegenAmount);
	VM_ExecuteCommand(677, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyTrap(lua_State *L)
{
	int nTrapLevel = luaL_checkint(L, 1);
  int nTrapType = luaL_checkint(L, 2);

  StackPushInteger(nTrapType);
	StackPushInteger(nTrapLevel);
	VM_ExecuteCommand(678, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyTrueSeeing(lua_State *L)
{
	VM_ExecuteCommand(679, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyOnMonsterHitProperties(lua_State *L)
{
	int nProperty = luaL_checkint(L, 1);
  int nSpecial = luaL_optint(L, 2, 0);

	StackPushInteger(nSpecial);
	StackPushInteger(nProperty);
	VM_ExecuteCommand(680, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyTurnResistance(lua_State *L)
{
	int nModifier = luaL_checkint(L, 1);

  StackPushInteger(nModifier);
	VM_ExecuteCommand(681, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyMassiveCritical(lua_State *L)
{
	int nDamage = luaL_checkint(L, 1);

  StackPushInteger(nDamage);
	VM_ExecuteCommand(682, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyFreeAction(lua_State *L)
{
	VM_ExecuteCommand(683, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyMonsterDamage(lua_State *L)
{
	int nDamage = luaL_checkint(L, 1);

  StackPushInteger(nDamage);
	VM_ExecuteCommand(684, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyImmunityToSpellLevel(lua_State *L)
{
	int nLevel = luaL_checkint(L, 1);

  StackPushInteger(nLevel);
	VM_ExecuteCommand(685, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertySpecialWalk(lua_State *L)
{
  int nWalkType = luaL_optint(L, 1, 0);

	StackPushInteger(nWalkType);
	VM_ExecuteCommand(686, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyHealersKit(lua_State *L)
{
	int nModifier = luaL_checkint(L, 1);

  StackPushInteger(nModifier);
	VM_ExecuteCommand(687, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyWeightIncrease(lua_State *L)
{
	int nWeight = luaL_checkint(L, 1);

  StackPushInteger(nWeight);
	VM_ExecuteCommand(688, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_GetIsSkillSuccessful(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
  int nSkill = luaL_checkint(L, 2);
  int nDifficulty = luaL_checkint(L, 3);

  StackPushInteger(nDifficulty);
	StackPushInteger(nSkill);
	StackPushObject(oTarget);
	VM_ExecuteCommand(689, 3);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_EffectSpellFailure(lua_State *L)
{
  int nPercent = luaL_optint(L, 1, 100);
  int nSpellSchool = luaL_optint(L, 2, SPELL_SCHOOL_GENERAL);

	StackPushInteger(nSpellSchool);
	StackPushInteger(nPercent);
	VM_ExecuteCommand(690, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_SpeakStringByStrRef(lua_State *L)
{
	int nStrRef = luaL_checkint(L, 1);
  int nTalkVolume = luaL_optint(L, 2, TALKVOLUME_TALK);

	StackPushInteger(nTalkVolume);
	StackPushInteger(nStrRef);
	VM_ExecuteCommand(691, 2);
	return 0;
}

static int NWScript_SetCutsceneMode(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
  int nInCutscene = luaL_optboolean(L, 2, TRUE);
  int nLeftClickingEnabled = luaL_optboolean(L, 3, FALSE);

	StackPushInteger(nLeftClickingEnabled);
	StackPushInteger(nInCutscene);
	StackPushObject(oCreature);
	VM_ExecuteCommand(692, 3);
	return 0;
}

static int NWScript_GetLastPCToCancelCutscene(lua_State *L)
{
	VM_ExecuteCommand(693, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetDialogSoundLength(lua_State *L)
{
	int nStrRef = luaL_checkint(L, 1);

  StackPushInteger(nStrRef);
	VM_ExecuteCommand(694, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
  return 1;
}

static int NWScript_FadeFromBlack(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
  double fSpeed = luaL_optnumber(L, 2, FADE_SPEED_MEDIUM);

	StackPushFloat(fSpeed);
	StackPushObject(oCreature);
	VM_ExecuteCommand(695, 2);
	return 0;
}

static int NWScript_FadeToBlack(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
  double fSpeed = luaL_optnumber(L, 2, FADE_SPEED_MEDIUM);

	StackPushFloat(fSpeed);
	StackPushObject(oCreature);
	VM_ExecuteCommand(696, 2);
	return 0;
}

static int NWScript_StopFade(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

  StackPushObject(oCreature);
	VM_ExecuteCommand(697, 1);
	return 0;
}

static int NWScript_BlackScreen(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

  StackPushObject(oCreature);
	VM_ExecuteCommand(698, 1);
	return 0;
}

static int NWScript_GetBaseAttackBonus(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

  StackPushObject(oCreature);
	VM_ExecuteCommand(699, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetImmortal(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
  int bImmortal = luaL_checkboolean(L, 2);

  StackPushInteger(bImmortal);
	StackPushObject(oCreature);
	VM_ExecuteCommand(700, 2);
	return 0;
}

static int NWScript_OpenInventory(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
  dword oPlayer = luaL_checkint(L, 2);

  StackPushObject(oPlayer);
	StackPushObject(oCreature);
	VM_ExecuteCommand(701, 2);
	return 0;
}

static int NWScript_StoreCameraFacing(lua_State *L)
{
	VM_ExecuteCommand(702, 0);
	return 0;
}

static int NWScript_RestoreCameraFacing(lua_State *L)
{
	VM_ExecuteCommand(703, 0);
	return 0;
}

static int NWScript_LevelUpHenchman(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	int nClass  = luaL_optint(L, 2, CLASS_TYPE_INVALID);
	int bReadyAllSpells  = luaL_optboolean(L, 3, FALSE);
	int nPackage  = luaL_optint(L, 4, PACKAGE_INVALID);

	StackPushInteger(nPackage );
	StackPushInteger(bReadyAllSpells );
	StackPushInteger(nClass );
	StackPushObject(oCreature);
	VM_ExecuteCommand(704, 4);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetDroppableFlag(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	int bDroppable = luaL_checkboolean(L, 2);

	StackPushInteger(bDroppable);
	StackPushObject(oItem);
	VM_ExecuteCommand(705, 2);
	return 0;
}

static int NWScript_GetWeight(lua_State *L)
{
	dword oTarget = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oTarget);
	VM_ExecuteCommand(706, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetModuleItemAcquiredBy(lua_State *L)
{
	VM_ExecuteCommand(707, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetImmortal(lua_State *L)
{
	dword oTarget = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oTarget);
	VM_ExecuteCommand(708, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_DoWhirlwindAttack(lua_State *L)
{
	int bDisplayFeedback = luaL_optboolean(L, 1, TRUE);
	int bImproved = luaL_optboolean(L, 2, FALSE);

	StackPushInteger(bImproved);
	StackPushInteger(bDisplayFeedback);
	VM_ExecuteCommand(709, 2);
	return 0;
}

static int NWScript_Get2DAString(lua_State *L)
{
	char *s2DA = (char *)luaL_checkstring(L, 1);
	char *sColumn = (char *)luaL_checkstring(L, 2);
	int nRow = luaL_checkint(L, 3);

	StackPushInteger(nRow);
	StackPushString(sColumn);
	StackPushString(s2DA);
	VM_ExecuteCommand(710, 3);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_EffectEthereal(lua_State *L)
{
	VM_ExecuteCommand(711, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetAILevel(lua_State *L)
{
	dword oTarget = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oTarget);
	VM_ExecuteCommand(712, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetAILevel(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	int nAILevel = luaL_checkint(L, 2);

	StackPushInteger(nAILevel);
	StackPushObject(oTarget);
	VM_ExecuteCommand(713, 2);
	return 0;
}

static int NWScript_GetIsPossessedFamiliar(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(714, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_UnpossessFamiliar(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	StackPushObject(oCreature);
	VM_ExecuteCommand(715, 1);
	return 0;
}

static int NWScript_GetIsAreaInterior(lua_State *L)
{
	dword oArea  = luaL_optint(L, 1, OBJECT_INVALID);

	StackPushObject(oArea );
	VM_ExecuteCommand(716, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_SendMessageToPCByStrRef(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);
	int nStrRef = luaL_checkint(L, 2);

	StackPushInteger(nStrRef);
	StackPushObject(oPlayer);
	VM_ExecuteCommand(717, 2);
	return 0;
}

static int NWScript_IncrementRemainingFeatUses(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	int nFeat = luaL_checkint(L, 2);

	StackPushInteger(nFeat);
	StackPushObject(oCreature);
	VM_ExecuteCommand(718, 2);
	return 0;
}

static int NWScript_ExportSingleCharacter(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);

	StackPushObject(oPlayer);
	VM_ExecuteCommand(719, 1);
	return 0;
}

static int NWScript_PlaySoundByStrRef(lua_State *L)
{
	int nStrRef = luaL_checkint(L, 1);
	int nRunAsAction  = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(nRunAsAction);
	StackPushInteger(nStrRef);
	VM_ExecuteCommand(720, 2);
	return 0;
}

static int NWScript_SetSubRace(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	char *sSubRace = (char *)luaL_checkstring(L, 2);

	StackPushString(sSubRace);
	StackPushObject(oCreature);
	VM_ExecuteCommand(721, 2);
	return 0;
}

static int NWScript_SetDeity(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	char *sDeity = (char *)luaL_checkstring(L, 2);

	StackPushString(sDeity);
	StackPushObject(oCreature);
	VM_ExecuteCommand(722, 2);
	return 0;
}

static int NWScript_GetIsDMPossessed(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(723, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetWeather(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);

	StackPushObject(oArea);
	VM_ExecuteCommand(724, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetIsAreaNatural(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);

	StackPushObject(oArea);
	VM_ExecuteCommand(725, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetIsAreaAboveGround(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);

	StackPushObject(oArea);
	VM_ExecuteCommand(726, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetPCItemLastEquipped(lua_State *L)
{
	VM_ExecuteCommand(727, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetPCItemLastEquippedBy(lua_State *L)
{
	VM_ExecuteCommand(728, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetPCItemLastUnequipped(lua_State *L)
{
	VM_ExecuteCommand(729, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
   	return 1;
}

static int NWScript_GetPCItemLastUnequippedBy(lua_State *L)
{
	VM_ExecuteCommand(730, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_CopyItemAndModify(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	int nType = luaL_checkint(L, 2);
	int nIndex = luaL_checkint(L, 3);
	int nNewValue = luaL_checkint(L, 4);
	int bCopyVars = luaL_optboolean(L, 5, FALSE);

	StackPushInteger(bCopyVars);
	StackPushInteger(nNewValue);
	StackPushInteger(nIndex);
	StackPushInteger(nType);
	StackPushObject(oItem);
	VM_ExecuteCommand(731, 5);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetItemAppearance(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	int nType = luaL_checkint(L, 2);
	int nIndex = luaL_checkint(L, 3);

	StackPushInteger(nIndex);
	StackPushInteger(nType);
	StackPushObject(oItem);
	VM_ExecuteCommand(732, 3);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_ItemPropertyOnHitCastSpell(lua_State *L)
{
	int nSpell = luaL_checkint(L, 1);
	int nLevel = luaL_checkint(L, 2);

	StackPushInteger(nLevel);
	StackPushInteger(nSpell);
	VM_ExecuteCommand(733, 2);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_GetItemPropertySubType(lua_State *L)
{
	void *iProperty = nwn_checkptr(L, 1, ITEMPROPERTY);

	StackPushEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, iProperty);
	VM_ExecuteCommand(734, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetActionMode(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	int nMode = luaL_checkint(L, 2);

	StackPushInteger(nMode);
	StackPushObject(oCreature);
	VM_ExecuteCommand(735, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_SetActionMode(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	int nMode = luaL_checkint(L, 2);
	int nStatus = luaL_checkboolean(L, 3);

	StackPushInteger(nStatus);
	StackPushInteger(nMode);
	StackPushObject(oCreature);
	VM_ExecuteCommand(736, 3);
	return 0;
}

static int NWScript_GetArcaneSpellFailure(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(737, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_ActionExamine(lua_State *L)
{
	dword oExamine = luaL_checkint(L, 1);

	StackPushObject(oExamine);
	VM_ExecuteCommand(738, 1);
	return 0;
}

static int NWScript_ItemPropertyVisualEffect(lua_State *L)
{
	int nEffect = luaL_checkint(L, 1);

	StackPushInteger(nEffect);
	VM_ExecuteCommand(739, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_SetLootable(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	int bLootable = luaL_checkboolean(L, 2);

	StackPushInteger(bLootable);
	StackPushObject(oCreature);
	VM_ExecuteCommand(740, 2);
	return 0;
}

static int NWScript_GetLootable(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(741, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetCutsceneCameraMoveRate(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(742, 1);
	float fRetVal;
	StackPopFloat(&fRetVal);
	lua_pushnumber(L, fRetVal);
  return 1;
}

static int NWScript_SetCutsceneCameraMoveRate(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	double fRate = luaL_checknumber(L, 2);

	StackPushFloat(fRate);
	StackPushObject(oCreature);
	VM_ExecuteCommand(743, 2);
	return 0;
}

static int NWScript_GetItemCursedFlag(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(744, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}
static int NWScript_SetItemCursedFlag(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	int nCursed = luaL_checkint(L, 2);

	StackPushInteger(nCursed);
	StackPushObject(oItem);
	VM_ExecuteCommand(745, 2);
	return 0;
}

static int NWScript_SetMaxHenchmen(lua_State *L)
{
	int nNumHenchmen = luaL_checkint(L, 1);

	StackPushInteger(nNumHenchmen);
	VM_ExecuteCommand(746, 1);
	return 0;
}

static int NWScript_GetMaxHenchmen(lua_State *L)
{
	VM_ExecuteCommand(747, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetAssociateType(lua_State *L)
{
	dword oAssociate = luaL_checkint(L, 1);

	StackPushObject(oAssociate);
	VM_ExecuteCommand(748, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetSpellResistance(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(749, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_DayToNight(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);
	double fTransitionTime = luaL_optnumber(L, 2, 0.0);

	StackPushFloat(fTransitionTime);
	StackPushObject(oPlayer);
	VM_ExecuteCommand(750, 2);
	return 0;
}

static int NWScript_NightToDay(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);
	double fTransitionTime = luaL_optnumber(L, 2, 0.0);

	StackPushFloat(fTransitionTime);
	StackPushObject(oPlayer);
	VM_ExecuteCommand(751, 2);
	return 0;
}

static int NWScript_LineOfSightObject(lua_State *L)
{
	dword oSource = luaL_checkint(L, 1);
	dword oTarget = luaL_checkint(L, 2);

	StackPushObject(oTarget);
	StackPushObject(oSource);
	VM_ExecuteCommand(752, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_LineOfSightVector(lua_State *L)
{
	Vector *vSource = (Vector *)luaL_checkudata(L, 1, VECTOR);
	Vector *vTarget = (Vector *)luaL_checkudata(L, 2, VECTOR);

	StackPushVector(*vTarget);
	StackPushVector(*vSource);
	VM_ExecuteCommand(753, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetLastSpellCastClass(lua_State *L)
{
	VM_ExecuteCommand(754, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetBaseAttackBonus(lua_State *L)
{
	int nBaseAttackBonus = luaL_checkint(L, 1);
	dword oCreature  = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nBaseAttackBonus);
	VM_ExecuteCommand(755, 2);
	return 0;
}

static int NWScript_RestoreBaseAttackBonus(lua_State *L)
{
	dword oCreature = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oCreature);
	VM_ExecuteCommand(756, 1);
	return 0;
}

static int NWScript_EffectCutsceneGhost(lua_State *L)
{
	VM_ExecuteCommand(757, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_ItemPropertyArcaneSpellFailure(lua_State *L)
{
	int nModLevel = luaL_checkint(L, 1);

	StackPushInteger(nModLevel);
	VM_ExecuteCommand(758, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_GetStoreGold(lua_State *L)
{
	dword oidStore = luaL_checkint(L, 1);

	StackPushObject(oidStore);
	VM_ExecuteCommand(759, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetStoreGold(lua_State *L)
{
	dword oidStore = luaL_checkint(L, 1);
	int nGold = luaL_checkint(L, 2);

	StackPushInteger(nGold);
	StackPushObject(oidStore);
	VM_ExecuteCommand(760, 2);
	return 0;
}

static int NWScript_GetStoreMaxBuyPrice(lua_State *L)
{
	dword oidStore = luaL_checkint(L, 1);

	StackPushObject(oidStore);
	VM_ExecuteCommand(761, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetStoreMaxBuyPrice(lua_State *L)
{
	dword oidStore = luaL_checkint(L, 1);
	int nMaxBuy = luaL_checkint(L, 2);

	StackPushInteger(nMaxBuy);
	StackPushObject(oidStore);
	VM_ExecuteCommand(762, 2);
	return 0;
}

static int NWScript_GetStoreIdentifyCost(lua_State *L)
{
	dword oidStore = luaL_checkint(L, 1);

	StackPushObject(oidStore);
	VM_ExecuteCommand(763, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetStoreIdentifyCost(lua_State *L)
{
	dword oidStore = luaL_checkint(L, 1);
	int nCost = luaL_checkint(L, 2);

	StackPushInteger(nCost);
	StackPushObject(oidStore);
	VM_ExecuteCommand(764, 2);
	return 0;
}

static int NWScript_SetCreatureAppearanceType(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	int nAppearanceType = luaL_checkint(L, 2);

	StackPushInteger(nAppearanceType);
	StackPushObject(oCreature);
	VM_ExecuteCommand(765, 2);
	return 0;
}

static int NWScript_GetCreatureStartingPackage(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(766, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_EffectCutsceneImmobilize(lua_State *L)
{
	VM_ExecuteCommand(767, 0);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_EFFECT, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
  return 1;
}

static int NWScript_GetIsInSubArea(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);
	dword oSubArea = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oSubArea);
	StackPushObject(oCreature);
	VM_ExecuteCommand(768, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_GetItemPropertyCostTable(lua_State *L)
{
	void *iProp = nwn_checkptr(L, 1, ITEMPROPERTY);

	StackPushEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, iProp);
	VM_ExecuteCommand(769, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetItemPropertyCostTableValue(lua_State *L)
{
	void *iProp = nwn_checkptr(L, 1, ITEMPROPERTY);

	StackPushEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, iProp);
	VM_ExecuteCommand(770, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetItemPropertyParam1(lua_State *L)
{
	void *iProp = nwn_checkptr(L, 1, ITEMPROPERTY);

	StackPushEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, iProp);
	VM_ExecuteCommand(771, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetItemPropertyParam1Value(lua_State *L)
{
	void *iProp = nwn_checkptr(L, 1, ITEMPROPERTY);

	StackPushEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, iProp);
	VM_ExecuteCommand(772, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetIsCreatureDisarmable(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(773, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_SetStolenFlag(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	int nStolenFlag = luaL_checkboolean(L, 2);

	StackPushInteger(nStolenFlag);
	StackPushObject(oItem);
	VM_ExecuteCommand(774, 2);
	return 0;
}

static int NWScript_ForceRest(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(775, 1);
	return 0;
}

static int NWScript_SetCameraHeight(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);
	double fHeight = luaL_optnumber(L, 2, 0.0);

	StackPushFloat(fHeight);
	StackPushObject(oPlayer);
	VM_ExecuteCommand(776, 2);
	return 0;
}

static int NWScript_SetSkyBox(lua_State *L)
{
	int nSkyBox = luaL_checkint(L, 1);
	dword oArea = luaL_optint(L, 2, OBJECT_INVALID);

	StackPushObject(oArea);
	StackPushInteger(nSkyBox);
	VM_ExecuteCommand(777, 2);
	return 0;
}

static int NWScript_GetPhenoType(lua_State *L)
{
	dword oCreature = luaL_checkint(L, 1);

	StackPushObject(oCreature);
	VM_ExecuteCommand(778, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetPhenoType(lua_State *L)
{
	int nPhenoType = luaL_checkint(L, 1);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nPhenoType);
	VM_ExecuteCommand(779, 2);
	return 0;
}

static int NWScript_SetFogColor(lua_State *L)
{
	int nFogType = luaL_checkint(L, 1);
	int nFogColor = luaL_checkint(L, 2);
	dword oArea = luaL_optint(L, 3, OBJECT_INVALID);

	StackPushObject(oArea);
	StackPushInteger(nFogColor);
	StackPushInteger(nFogType);
	VM_ExecuteCommand(780, 3);
	return 0;
}

static int NWScript_GetCutsceneMode(lua_State *L)
{
	dword oCreature = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oCreature);
	VM_ExecuteCommand(781, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetSkyBox(lua_State *L)
{
	dword oArea = luaL_optint(L, 1, OBJECT_INVALID);

	StackPushObject(oArea);
	VM_ExecuteCommand(782, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetFogColor(lua_State *L)
{
	int nFogType = luaL_checkint(L, 1);
	dword oArea = luaL_optint(L, 2, OBJECT_INVALID);

	StackPushObject(oArea);
	StackPushInteger(nFogType);
	VM_ExecuteCommand(783, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetFogAmount(lua_State *L)
{
	int nFogType = luaL_checkint(L, 1);
	int nFogAmount = luaL_checkint(L, 2);
	dword oArea = luaL_optint(L, 3, OBJECT_INVALID);

	StackPushObject(oArea);
	StackPushInteger(nFogAmount);
	StackPushInteger(nFogType);
	VM_ExecuteCommand(784, 3);
	return 0;
}

static int NWScript_GetFogAmount(lua_State *L)
{
	int nFogType = luaL_checkint(L, 1);
	dword oArea = luaL_optint(L, 2, OBJECT_INVALID);

	StackPushObject(oArea);
	StackPushInteger(nFogType);
	VM_ExecuteCommand(785, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetPickpocketableFlag(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(786, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_SetPickpocketableFlag(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	int bPickpocketable = luaL_checkboolean(L, 2);

	StackPushInteger(bPickpocketable);
	StackPushObject(oItem);
	VM_ExecuteCommand(787, 2);
	return 0;
}

static int NWScript_GetFootstepType(lua_State *L)
{
	dword oCreature = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oCreature);
	VM_ExecuteCommand(788, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetFootstepType(lua_State *L)
{
	int nFootstepType = luaL_checkint(L, 1);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nFootstepType);
	VM_ExecuteCommand(789, 2);
	return 0;
}

static int NWScript_GetCreatureWingType(lua_State *L)
{
	dword oCreature = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oCreature);
	VM_ExecuteCommand(790, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetCreatureWingType(lua_State *L)
{
	int nWingType = luaL_checkint(L, 1);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nWingType);
	VM_ExecuteCommand(791, 2);
	return 0;
}

static int NWScript_GetCreatureBodyPart(lua_State *L)
{
	int nPart = luaL_checkint(L, 1);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nPart);
	VM_ExecuteCommand(792, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetCreatureBodyPart(lua_State *L)
{
	int nPart = luaL_checkint(L, 1);
	int nModelNumber = luaL_checkint(L, 2);
	dword oCreature = luaL_optint(L, 3, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nModelNumber);
	StackPushInteger(nPart);
	VM_ExecuteCommand(793, 3);
	return 0;
}

static int NWScript_GetCreatureTailType(lua_State *L)
{
	dword oCreature = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oCreature);
	VM_ExecuteCommand(794, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetCreatureTailType(lua_State *L)
{
	int nTailType = luaL_checkint(L, 1);
	dword oCreature = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oCreature);
	StackPushInteger(nTailType);
	VM_ExecuteCommand(795, 2);
	return 0;
}

static int NWScript_GetHardness(lua_State *L)
{
	dword oObject = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oObject);
	VM_ExecuteCommand(796, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetHardness(lua_State *L)
{
	int nHardness = luaL_checkint(L, 1);
	dword oObject = luaL_optint(L, 2, OBJECT_SELF);

	StackPushObject(oObject);
	StackPushInteger(nHardness);
	VM_ExecuteCommand(797, 2);
	return 0;
}

static int NWScript_SetLockKeyRequired(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	int nKeyRequired = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(nKeyRequired);
	StackPushObject(oObject);
	VM_ExecuteCommand(798, 2);
	return 0;
}

static int NWScript_SetLockKeyTag(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sNewKeyTag = (char *)luaL_checkstring(L, 2);

	StackPushString(sNewKeyTag);
	StackPushObject(oObject);
	VM_ExecuteCommand(799, 2);
	return 0;
}

static int NWScript_SetLockLockable(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	int nLockable = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(nLockable);
	StackPushObject(oObject);
	VM_ExecuteCommand(800, 2);
	return 0;
}

static int NWScript_SetLockUnlockDC(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	int nNewUnlockDC = luaL_checkint(L, 2);

	StackPushInteger(nNewUnlockDC);
	StackPushObject(oObject);
	VM_ExecuteCommand(801, 2);
	return 0;
}

static int NWScript_SetLockLockDC(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	int nNewLockDC = luaL_checkint(L, 2);

	StackPushInteger(nNewLockDC);
	StackPushObject(oObject);
	VM_ExecuteCommand(802, 2);
	return 0;
}

static int NWScript_SetTrapDisarmable(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);
	int nDisarmable = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(nDisarmable);
	StackPushObject(oTrapObject);
	VM_ExecuteCommand(803, 2);
	return 0;
}

static int NWScript_SetTrapDetectable(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);
	int nDetectable = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(nDetectable);
	StackPushObject(oTrapObject);
	VM_ExecuteCommand(804, 2);
	return 0;
}

static int NWScript_SetTrapOneShot(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);
	int nOneShot = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(nOneShot);
	StackPushObject(oTrapObject);
	VM_ExecuteCommand(805, 2);
	return 0;
}

static int NWScript_SetTrapKeyTag(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);
	char *sKeyTag = (char *)luaL_checkstring(L, 2);

	StackPushString(sKeyTag);
	StackPushObject(oTrapObject);
	VM_ExecuteCommand(806, 2);
	return 0;
}

static int NWScript_SetTrapDisarmDC(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);
	int nDisarmDC = luaL_checkint(L, 2);

	StackPushInteger(nDisarmDC);
	StackPushObject(oTrapObject);
	VM_ExecuteCommand(807, 2);
	return 0;
}

static int NWScript_SetTrapDetectDC(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);
	int nDetectDC = luaL_checkint(L, 2);

	StackPushInteger(nDetectDC);
	StackPushObject(oTrapObject);
	VM_ExecuteCommand(808, 2);
	return 0;
}

static int NWScript_CreateTrapAtLocation(lua_State *L)
{
	int nTrapType = luaL_checkint(L, 1);
	void *lLocation = lua_nwn_checkloc(L, 2);
	double fSize = luaL_optnumber(L, 3, 2.0);
	char *sTag = (char *)luaL_optstring(L, 4, "");
	int nFaction = luaL_optint(L, 5, STANDARD_FACTION_HOSTILE);
	char *sOnDisarmScript = (char *)luaL_optstring(L, 6, "");
	char *sOnTrapTriggeredScript = (char *)luaL_optstring(L, 7, "");

	StackPushString(sOnTrapTriggeredScript);
	StackPushString(sOnDisarmScript);
	StackPushInteger(nFaction);
	StackPushString(sTag);
	StackPushFloat(fSize);
	StackPushEngineStructure(ENGINE_STRUCTURE_LOCATION, lLocation);
	StackPushInteger(nTrapType);
	VM_ExecuteCommand(809, 7);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_CreateTrapOnObject(lua_State *L)
{
	int nTrapType = luaL_checkint(L, 1);
	dword oObject = luaL_checkint(L, 2);
	int nFaction = luaL_optint(L, 3, STANDARD_FACTION_HOSTILE);
	char *sOnDisarmScript = (char *)luaL_optstring(L, 4, "");
	char *sOnTrapTriggeredScript = (char *)luaL_optstring(L, 5, "");

	StackPushString(sOnTrapTriggeredScript);
	StackPushString(sOnDisarmScript);
	StackPushInteger(nFaction);
	StackPushObject(oObject);
	StackPushInteger(nTrapType);
	VM_ExecuteCommand(810, 5);
	return 0;
}

static int NWScript_SetWillSavingThrow(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	int nWillSave = luaL_checkint(L, 2);

	StackPushInteger(nWillSave);
	StackPushObject(oObject);
	VM_ExecuteCommand(811, 2);
	return 0;
}

static int NWScript_SetReflexSavingThrow(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	int nReflexSave = luaL_checkint(L, 2);

	StackPushInteger(nReflexSave);
	StackPushObject(oObject);
	VM_ExecuteCommand(812, 2);
	return 0;
}

static int NWScript_SetFortitudeSavingThrow(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	int nFortitudeSave = luaL_checkint(L, 2);

	StackPushInteger(nFortitudeSave);
	StackPushObject(oObject);
	VM_ExecuteCommand(813, 2);
	return 0;
}

static int NWScript_GetTilesetResRef(lua_State *L)
{
	dword oArea = luaL_checkint(L, 1);

	StackPushObject(oArea);
	VM_ExecuteCommand(814, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetTrapRecoverable(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);

	StackPushObject(oTrapObject);
	VM_ExecuteCommand(815, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_SetTrapRecoverable(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);
	int nRecoverable = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(nRecoverable);
	StackPushObject(oTrapObject);
	VM_ExecuteCommand(816, 2);
	return 0;
}

static int NWScript_GetModuleXPScale(lua_State *L)
{
	VM_ExecuteCommand(817, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetModuleXPScale(lua_State *L)
{
	int nXPScale = luaL_checkint(L, 1);

	StackPushInteger(nXPScale);
	VM_ExecuteCommand(818, 1);
	return 0;
}

static int NWScript_GetKeyRequiredFeedback(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);

	StackPushObject(oObject);
	VM_ExecuteCommand(819, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_SetKeyRequiredFeedback(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sFeedbackMessage = (char *)luaL_checkstring(L, 2);

	StackPushString(sFeedbackMessage);
	StackPushObject(oObject);
	VM_ExecuteCommand(820, 2);
	return 0;
}

static int NWScript_GetTrapActive(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);

	StackPushObject(oTrapObject);
	VM_ExecuteCommand(821, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_SetTrapActive(lua_State *L)
{
	dword oTrapObject = luaL_checkint(L, 1);
	int nActive = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(nActive);
	StackPushObject(oTrapObject);
	VM_ExecuteCommand(822, 2);
	return 0;
}

static int NWScript_LockCameraPitch(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);
	int bLocked = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bLocked);
	StackPushObject(oPlayer);
	VM_ExecuteCommand(823, 2);
	return 0;
}

static int NWScript_LockCameraDistance(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);
	int bLocked = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bLocked);
	StackPushObject(oPlayer);
	VM_ExecuteCommand(824, 2);
	return 0;
}

static int NWScript_LockCameraDirection(lua_State *L)
{
	dword oPlayer = luaL_checkint(L, 1);
	int bLocked = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bLocked);
	StackPushObject(oPlayer);
	VM_ExecuteCommand(825, 2);
	return 0;
}

static int NWScript_GetPlaceableLastClickedBy(lua_State *L)
{
	VM_ExecuteCommand(826, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetInfiniteFlag(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);

	StackPushObject(oItem);
	VM_ExecuteCommand(827, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushboolean(L, nRetVal);
  return 1;
}

static int NWScript_SetInfiniteFlag(lua_State *L)
{
	dword oItem = luaL_checkint(L, 1);
	int bInfinite = luaL_optboolean(L, 2, TRUE);

	StackPushInteger(bInfinite);
	StackPushObject(oItem);
	VM_ExecuteCommand(828, 2);
	return 0;
}

static int NWScript_GetAreaSize(lua_State *L)
{
	int nAreaDimension = luaL_checkint(L, 1);
	dword oArea = luaL_optint(L, 2, OBJECT_INVALID);

	StackPushObject(oArea);
	StackPushInteger(nAreaDimension);
	VM_ExecuteCommand(829, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetName(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sNewName = (char *)luaL_optstring(L, 2, "");

	StackPushString(sNewName);
	StackPushObject(oObject);
	VM_ExecuteCommand(830, 2);
	return 0;
}

static int NWScript_GetPortraitId(lua_State *L)
{
	dword oTarget = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oTarget);
	VM_ExecuteCommand(831, 1);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetPortraitId(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	int nPortraitId = luaL_checkint(L, 2);

	StackPushInteger(nPortraitId);
	StackPushObject(oTarget);
	VM_ExecuteCommand(832, 2);
	return 0;
}

static int NWScript_GetPortraitResRef(lua_State *L)
{
	dword oTarget = luaL_optint(L, 1, OBJECT_SELF);

	StackPushObject(oTarget);
	VM_ExecuteCommand(833, 1);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_SetPortraitResRef(lua_State *L)
{
	dword oTarget = luaL_checkint(L, 1);
	char *sPortraitResRef = (char *)luaL_checkstring(L, 2);

	StackPushString(sPortraitResRef);
	StackPushObject(oTarget);
	VM_ExecuteCommand(834, 2);
	return 0;
}

static int NWScript_SetUseableFlag(lua_State *L)
{
	dword oPlaceable = luaL_checkint(L, 1);
	int nUseableFlag = luaL_checkboolean(L, 2);

	StackPushInteger(nUseableFlag);
	StackPushObject(oPlaceable);
	VM_ExecuteCommand(835, 2);
	return 0;
}

static int NWScript_GetDescription(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	int bOriginalDescription = luaL_optboolean(L, 2, FALSE);
	int bIdentifiedDescription = luaL_optboolean(L, 3, TRUE);

	StackPushInteger(bIdentifiedDescription);
	StackPushInteger(bOriginalDescription);
	StackPushObject(oObject);
	VM_ExecuteCommand(836, 3);

	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_SetDescription(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	char *sNewDescription = (char *)luaL_optstring(L, 2, "");
	int bIdentifiedDescription = luaL_optboolean(L, 3, TRUE);

	StackPushInteger(bIdentifiedDescription);
	StackPushString(sNewDescription);
	StackPushObject(oObject);
	VM_ExecuteCommand(837, 3);
	return 0;
}

static int NWScript_GetPCChatSpeaker(lua_State *L)
{
	VM_ExecuteCommand(838, 0);
	dword nRetVal;
	StackPopObject(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_GetPCChatMessage(lua_State *L)
{
	VM_ExecuteCommand(839, 0);
	send_exostring_to_lua(L);
	return 1;
}

static int NWScript_GetPCChatVolume(lua_State *L)
{
	VM_ExecuteCommand(840, 0);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetPCChatMessage(lua_State *L)
{
	char *sNewChatMessage = (char *)luaL_optstring(L, 1, "");

	StackPushString(sNewChatMessage);
	VM_ExecuteCommand(841, 1);
	return 0;
}

static int NWScript_SetPCChatVolume(lua_State *L)
{
	int nTalkVolume = luaL_optint(L, 1, TALKVOLUME_TALK);

	StackPushInteger(nTalkVolume);
	VM_ExecuteCommand(842, 1);
	return 0;
}

static int NWScript_GetColor(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	int nColorChannel = luaL_checkint(L, 2);

	StackPushInteger(nColorChannel);
	StackPushObject(oObject);
	VM_ExecuteCommand(843, 2);
	int nRetVal;
	StackPopInteger(&nRetVal);
	lua_pushinteger(L, nRetVal);
  return 1;
}

static int NWScript_SetColor(lua_State *L)
{
	dword oObject = luaL_checkint(L, 1);
	int nColorChannel = luaL_checkint(L, 2);
	int nColorValue = luaL_checkint(L, 3);

	StackPushInteger(nColorValue);
	StackPushInteger(nColorChannel);
	StackPushObject(oObject);
	VM_ExecuteCommand(844, 3);
	return 0;
}

static int NWScript_ItemPropertyMaterial(lua_State *L)
{
	int nMaterialType = luaL_checkint(L, 1);

	StackPushInteger(nMaterialType);
	VM_ExecuteCommand(845, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyQuality(lua_State *L)
{
	int nQuality = luaL_checkint(L, 1);

	StackPushInteger(nQuality);
	VM_ExecuteCommand(846, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static int NWScript_ItemPropertyAdditional(lua_State *L)
{
	int nAdditionalProperty = luaL_checkint(L, 1);

	StackPushInteger(nAdditionalProperty);
	VM_ExecuteCommand(847, 1);
	void *pRetVal;
	StackPopEngineStructure(ENGINE_STRUCTURE_ITEMPROPERTY, &pRetVal);
	lua_pushlightuserdata(L, pRetVal);
	return 1;
}

static const luaL_Reg nwscriptlib[] = {
	{ "Random", NWScript_Random },
	{ "PrintString", NWScript_PrintString },
	{ "PrintFloat", NWScript_PrintFloat },
	{ "FloatToString", NWScript_FloatToString },
	{ "PrintInteger", NWScript_PrintInteger },
	{ "PrintObject", NWScript_PrintObject },
	{ "ExecuteScript", NWScript_ExecuteScript },
	{ "ClearAllActions", NWScript_ClearAllActions },
	{ "SetFacing", NWScript_SetFacing },
	{ "SetCalendar", NWScript_SetCalendar },
	{ "SetTime", NWScript_SetTime },
	{ "GetCalendarYear", NWScript_GetCalendarYear },
	{ "GetCalendarMonth", NWScript_GetCalendarMonth },
	{ "GetCalendarDay", NWScript_GetCalendarDay },
	{ "GetTimeHour", NWScript_GetTimeHour },
	{ "GetTimeMinute", NWScript_GetTimeMinute },
	{ "GetTimeSecond", NWScript_GetTimeSecond },
	{ "GetTimeMillisecond", NWScript_GetTimeMillisecond },
	{ "ActionRandomWalk", NWScript_ActionRandomWalk },
	{ "ActionMoveToLocation", NWScript_ActionMoveToLocation },
	{ "ActionMoveToObject", NWScript_ActionMoveToObject },
	{ "ActionMoveAwayFromObject", NWScript_ActionMoveAwayFromObject },
	{ "GetArea", NWScript_GetArea },
	{ "GetEnteringObject", NWScript_GetEnteringObject },
	{ "GetExitingObject", NWScript_GetExitingObject },
	{ "GetPosition", NWScript_GetPosition },
	{ "GetFacing", NWScript_GetFacing },
	{ "GetItemPossessor", NWScript_GetItemPossessor },
	{ "GetItemPossessedBy", NWScript_GetItemPossessedBy },
	{ "CreateItemOnObject", NWScript_CreateItemOnObject },
	{ "ActionEquipItem", NWScript_ActionEquipItem },
	{ "ActionUnequipItem", NWScript_ActionUnequipItem },
	{ "ActionPickUpItem", NWScript_ActionPickUpItem },
	{ "ActionPutDownItem", NWScript_ActionPutDownItem },
	{ "GetLastAttacker", NWScript_GetLastAttacker },
	{ "ActionAttack", NWScript_ActionAttack },
	{ "GetNearestCreature", NWScript_GetNearestCreature },
	{ "ActionSpeakString", NWScript_ActionSpeakString },
	{ "ActionPlayAnimation", NWScript_ActionPlayAnimation },
	{ "GetDistanceToObject", NWScript_GetDistanceToObject },
	{ "GetIsObjectValid", NWScript_GetIsObjectValid },
	{ "ActionOpenDoor", NWScript_ActionOpenDoor },
	{ "ActionCloseDoor", NWScript_ActionCloseDoor },
	{ "SetCameraFacing", NWScript_SetCameraFacing },
	{ "PlaySound", NWScript_PlaySound },
	{ "GetSpellTargetObject", NWScript_GetSpellTargetObject },
	{ "ActionCastSpellAtObject", NWScript_ActionCastSpellAtObject },
	{ "GetCurrentHitPoints", NWScript_GetCurrentHitPoints },
	{ "GetMaxHitPoints", NWScript_GetMaxHitPoints },
	{ "GetLocalInt", NWScript_GetLocalInt },
	{ "GetLocalFloat", NWScript_GetLocalFloat },
	{ "GetLocalString", NWScript_GetLocalString },
	{ "GetLocalObject", NWScript_GetLocalObject },
	{ "SetLocalInt", NWScript_SetLocalInt },
	{ "SetLocalFloat", NWScript_SetLocalFloat },
	{ "SetLocalString", NWScript_SetLocalString },
	{ "SetLocalObject", NWScript_SetLocalObject },
	{ "GetStringLength", NWScript_GetStringLength },
	{ "GetStringUpperCase", NWScript_GetStringUpperCase },
	{ "GetStringLowerCase", NWScript_GetStringLowerCase },
	{ "GetStringRight", NWScript_GetStringRight },
	{ "GetStringLeft", NWScript_GetStringLeft },
	{ "InsertString", NWScript_InsertString },
	{ "GetSubString", NWScript_GetSubString },
	{ "FindSubString", NWScript_FindSubString },
	{ "fabs", NWScript_fabs },
	{ "cos", NWScript_cos },
	{ "sin", NWScript_sin },
	{ "tan", NWScript_tan },
	{ "acos", NWScript_acos },
	{ "asin", NWScript_asin },
	{ "atan", NWScript_atan },
	{ "log", NWScript_log },
	{ "pow", NWScript_pow },
	{ "sqrt", NWScript_sqrt },
	{ "abs", NWScript_abs },
	{ "EffectHeal", NWScript_EffectHeal },
	{ "EffectDamage", NWScript_EffectDamage },
	{ "EffectAbilityIncrease", NWScript_EffectAbilityIncrease },
	{ "EffectDamageResistance", NWScript_EffectDamageResistance },
	{ "EffectResurrection", NWScript_EffectResurrection },
	{ "EffectSummonCreature", NWScript_EffectSummonCreature },
	{ "GetCasterLevel", NWScript_GetCasterLevel },
	{ "GetFirstEffect", NWScript_GetFirstEffect },
	{ "GetNextEffect", NWScript_GetNextEffect },
	{ "RemoveEffect", NWScript_RemoveEffect },
	{ "GetIsEffectValid", NWScript_GetIsEffectValid },
	{ "GetEffectDurationType", NWScript_GetEffectDurationType },
	{ "GetEffectSubType", NWScript_GetEffectSubType },
	{ "GetEffectCreator", NWScript_GetEffectCreator },
	{ "IntToString", NWScript_IntToString },
	{ "GetFirstObjectInArea", NWScript_GetFirstObjectInArea },
	{ "GetNextObjectInArea", NWScript_GetNextObjectInArea },
	{ "d2", NWScript_d2 },
	{ "d3", NWScript_d3 },
	{ "d4", NWScript_d4 },
	{ "d6", NWScript_d6 },
	{ "d8", NWScript_d8 },
	{ "d10", NWScript_d10 },
	{ "d12", NWScript_d12 },
	{ "d20", NWScript_d20 },
	{ "d100", NWScript_d100 },
	{ "VectorMagnitude", NWScript_VectorMagnitude },
	{ "GetMetaMagicFeat", NWScript_GetMetaMagicFeat },
	{ "GetObjectType", NWScript_GetObjectType },
	{ "GetRacialType", NWScript_GetRacialType },
	{ "FortitudeSave", NWScript_FortitudeSave },
	{ "ReflexSave", NWScript_ReflexSave },
	{ "WillSave", NWScript_WillSave },
	{ "GetSpellSaveDC", NWScript_GetSpellSaveDC },
	{ "MagicalEffect", NWScript_MagicalEffect },
	{ "SupernaturalEffect", NWScript_SupernaturalEffect },
	{ "ExtraordinaryEffect", NWScript_ExtraordinaryEffect },
	{ "EffectACIncrease", NWScript_EffectACIncrease },
	{ "GetAC", NWScript_GetAC },
	{ "EffectSavingThrowIncrease", NWScript_EffectSavingThrowIncrease },
	{ "EffectAttackIncrease", NWScript_EffectAttackIncrease },
	{ "EffectDamageReduction", NWScript_EffectDamageReduction },
	{ "EffectDamageIncrease", NWScript_EffectDamageIncrease },
	{ "RoundsToSeconds", NWScript_RoundsToSeconds },
	{ "HoursToSeconds", NWScript_HoursToSeconds },
	{ "TurnsToSeconds", NWScript_TurnsToSeconds },
	{ "GetLawChaosValue", NWScript_GetLawChaosValue },
	{ "GetGoodEvilValue", NWScript_GetGoodEvilValue },
	{ "GetAlignmentLawChaos", NWScript_GetAlignmentLawChaos },
	{ "GetAlignmentGoodEvil", NWScript_GetAlignmentGoodEvil },
	{ "EffectEntangle", NWScript_EffectEntangle },
	{ "SignalEvent", NWScript_SignalEvent },
	{ "EventUserDefined", NWScript_EventUserDefined },
	{ "EffectDeath", NWScript_EffectDeath },
	{ "EffectKnockdown", NWScript_EffectKnockdown },
	{ "ActionGiveItem", NWScript_ActionGiveItem },
	{ "ActionTakeItem", NWScript_ActionTakeItem },
	{ "VectorNormalize", NWScript_VectorNormalize },
	{ "EffectCurse", NWScript_EffectCurse },
	{ "GetAbilityScore", NWScript_GetAbilityScore },
	{ "GetIsDead", NWScript_GetIsDead },
	{ "PrintVector", NWScript_PrintVector },
	{ "Vector", NWScript_Vector },
	{ "SetFacingPoint", NWScript_SetFacingPoint },
	{ "AngleToVector", NWScript_AngleToVector },
	{ "VectorToAngle", NWScript_VectorToAngle },
	{ "TouchAttackMelee", NWScript_TouchAttackMelee },
	{ "TouchAttackRanged", NWScript_TouchAttackRanged },
	{ "EffectParalyze", NWScript_EffectParalyze },
	{ "EffectSpellImmunity", NWScript_EffectSpellImmunity },
	{ "EffectDeaf", NWScript_EffectDeaf },
	{ "GetDistanceBetween", NWScript_GetDistanceBetween },
	{ "SetLocalLocation", NWScript_SetLocalLocation },
	{ "GetLocalLocation", NWScript_GetLocalLocation },
	{ "EffectSleep", NWScript_EffectSleep },
	{ "GetItemInSlot", NWScript_GetItemInSlot },
	{ "EffectCharmed", NWScript_EffectCharmed },
	{ "EffectConfused", NWScript_EffectConfused },
	{ "EffectFrightened", NWScript_EffectFrightened },
	{ "EffectDominated", NWScript_EffectDominated },
	{ "EffectDazed", NWScript_EffectDazed },
	{ "EffectStunned", NWScript_EffectStunned },
	{ "SetCommandable", NWScript_SetCommandable },
	{ "GetCommandable", NWScript_GetCommandable },
	{ "EffectRegenerate", NWScript_EffectRegenerate },
	{ "EffectMovementSpeedIncrease", NWScript_EffectMovementSpeedIncrease },
	{ "GetHitDice", NWScript_GetHitDice },
	{ "ActionForceFollowObject", NWScript_ActionForceFollowObject },
	{ "GetTag", NWScript_GetTag },
	{ "ResistSpell", NWScript_ResistSpell },
	{ "GetEffectType", NWScript_GetEffectType },
	{ "EffectAreaOfEffect", NWScript_EffectAreaOfEffect },
	{ "GetFactionEqual", NWScript_GetFactionEqual },
	{ "ChangeFaction", NWScript_ChangeFaction },
	{ "GetIsListening", NWScript_GetIsListening },
	{ "SetListening", NWScript_SetListening },
	{ "SetListenPattern", NWScript_SetListenPattern },
	{ "TestStringAgainstPattern", NWScript_TestStringAgainstPattern },
	{ "GetMatchedSubstring", NWScript_GetMatchedSubstring },
	{ "GetMatchedSubstringsCount", NWScript_GetMatchedSubstringsCount },
	{ "EffectVisualEffect", NWScript_EffectVisualEffect },
	{ "GetFactionWeakestMember", NWScript_GetFactionWeakestMember },
	{ "GetFactionStrongestMember", NWScript_GetFactionStrongestMember },
	{ "GetFactionMostDamagedMember", NWScript_GetFactionMostDamagedMember },
	{ "GetFactionLeastDamagedMember", NWScript_GetFactionLeastDamagedMember },
	{ "GetFactionGold", NWScript_GetFactionGold },
	{ "GetFactionAverageReputation", NWScript_GetFactionAverageReputation },
	{ "GetFactionAverageGoodEvilAlignment", NWScript_GetFactionAverageGoodEvilAlignment },
	{ "GetFactionAverageLawChaosAlignment", NWScript_GetFactionAverageLawChaosAlignment },
	{ "GetFactionAverageLevel", NWScript_GetFactionAverageLevel },
	{ "GetFactionAverageXP", NWScript_GetFactionAverageXP },
	{ "GetFactionMostFrequentClass", NWScript_GetFactionMostFrequentClass },
	{ "GetFactionWorstAC", NWScript_GetFactionWorstAC },
	{ "GetFactionBestAC", NWScript_GetFactionBestAC },
	{ "ActionSit", NWScript_ActionSit },
	{ "GetListenPatternNumber", NWScript_GetListenPatternNumber },
	{ "ActionJumpToObject", NWScript_ActionJumpToObject },
	{ "GetWaypointByTag", NWScript_GetWaypointByTag },
	{ "GetTransitionTarget", NWScript_GetTransitionTarget },
	{ "EffectLinkEffects", NWScript_EffectLinkEffects },
	{ "GetObjectByTag", NWScript_GetObjectByTag },
	{ "AdjustAlignment", NWScript_AdjustAlignment },
	{ "ActionWait", NWScript_ActionWait },
	{ "SetAreaTransitionBMP", NWScript_SetAreaTransitionBMP },
	{ "ActionStartConversation", NWScript_ActionStartConversation },
	{ "ActionPauseConversation", NWScript_ActionPauseConversation },
	{ "ActionResumeConversation", NWScript_ActionResumeConversation },
	{ "EffectBeam", NWScript_EffectBeam },
	{ "GetReputation", NWScript_GetReputation },
	{ "AdjustReputation", NWScript_AdjustReputation },
	{ "GetSittingCreature", NWScript_GetSittingCreature },
	{ "GetGoingToBeAttackedBy", NWScript_GetGoingToBeAttackedBy },
	{ "EffectSpellResistanceIncrease", NWScript_EffectSpellResistanceIncrease },
	{ "GetLocation", NWScript_GetLocation },
	{ "ActionJumpToLocation", NWScript_ActionJumpToLocation },
	{ "Location", NWScript_Location },
	{ "ApplyEffectAtLocation", NWScript_ApplyEffectAtLocation },
	{ "GetIsPC", NWScript_GetIsPC },
	{ "FeetToMeters", NWScript_FeetToMeters },
	{ "YardsToMeters", NWScript_YardsToMeters },
	{ "ApplyEffectToObject", NWScript_ApplyEffectToObject },
	{ "SpeakString", NWScript_SpeakString },
	{ "GetSpellTargetLocation", NWScript_GetSpellTargetLocation },
	{ "GetPositionFromLocation", NWScript_GetPositionFromLocation },
	{ "GetAreaFromLocation", NWScript_GetAreaFromLocation },
	{ "GetFacingFromLocation", NWScript_GetFacingFromLocation },
	{ "GetNearestCreatureToLocation", NWScript_GetNearestCreatureToLocation },
	{ "GetNearestObject", NWScript_GetNearestObject },
	{ "GetNearestObjectToLocation", NWScript_GetNearestObjectToLocation },
	{ "GetNearestObjectByTag", NWScript_GetNearestObjectByTag },
	{ "IntToFloat", NWScript_IntToFloat },
	{ "FloatToInt", NWScript_FloatToInt },
	{ "StringToInt", NWScript_StringToInt },
	{ "StringToFloat", NWScript_StringToFloat },
	{ "ActionCastSpellAtLocation", NWScript_ActionCastSpellAtLocation },
	{ "GetIsEnemy", NWScript_GetIsEnemy },
	{ "GetIsFriend", NWScript_GetIsFriend },
	{ "GetIsNeutral", NWScript_GetIsNeutral },
	{ "GetPCSpeaker", NWScript_GetPCSpeaker },
	{ "GetStringByStrRef", NWScript_GetStringByStrRef },
	{ "ActionSpeakStringByStrRef", NWScript_ActionSpeakStringByStrRef },
	{ "DestroyObject", NWScript_DestroyObject },
	{ "GetModule", NWScript_GetModule },
	{ "CreateObject", NWScript_CreateObject },
	{ "EventSpellCastAt", NWScript_EventSpellCastAt },
	{ "GetLastSpellCaster", NWScript_GetLastSpellCaster },
	{ "GetLastSpell", NWScript_GetLastSpell },
	{ "GetUserDefinedEventNumber", NWScript_GetUserDefinedEventNumber },
	{ "GetSpellId", NWScript_GetSpellId },
	{ "RandomName", NWScript_RandomName },
	{ "EffectPoison", NWScript_EffectPoison },
	{ "EffectDisease", NWScript_EffectDisease },
	{ "EffectSilence", NWScript_EffectSilence },
	{ "GetName", NWScript_GetName },
	{ "GetLastSpeaker", NWScript_GetLastSpeaker },
	{ "BeginConversation", NWScript_BeginConversation },
	{ "GetLastPerceived", NWScript_GetLastPerceived },
	{ "GetLastPerceptionHeard", NWScript_GetLastPerceptionHeard },
	{ "GetLastPerceptionInaudible", NWScript_GetLastPerceptionInaudible },
	{ "GetLastPerceptionSeen", NWScript_GetLastPerceptionSeen },
	{ "GetLastClosedBy", NWScript_GetLastClosedBy },
	{ "GetLastPerceptionVanished", NWScript_GetLastPerceptionVanished },
	{ "GetFirstInPersistentObject", NWScript_GetFirstInPersistentObject },
	{ "GetNextInPersistentObject", NWScript_GetNextInPersistentObject },
	{ "GetAreaOfEffectCreator", NWScript_GetAreaOfEffectCreator },
	{ "DeleteLocalInt", NWScript_DeleteLocalInt },
	{ "DeleteLocalFloat", NWScript_DeleteLocalFloat },
	{ "DeleteLocalString", NWScript_DeleteLocalString },
	{ "DeleteLocalObject", NWScript_DeleteLocalObject },
	{ "DeleteLocalLocation", NWScript_DeleteLocalLocation },
	{ "EffectHaste", NWScript_EffectHaste },
	{ "EffectSlow", NWScript_EffectSlow },
	{ "ObjectToString", NWScript_ObjectToString },
	{ "EffectImmunity", NWScript_EffectImmunity },
	{ "GetIsImmune", NWScript_GetIsImmune },
	{ "EffectDamageImmunityIncrease", NWScript_EffectDamageImmunityIncrease },
	{ "GetEncounterActive", NWScript_GetEncounterActive },
	{ "SetEncounterActive", NWScript_SetEncounterActive },
	{ "GetEncounterSpawnsMax", NWScript_GetEncounterSpawnsMax },
	{ "SetEncounterSpawnsMax", NWScript_SetEncounterSpawnsMax },
	{ "GetEncounterSpawnsCurrent", NWScript_GetEncounterSpawnsCurrent },
	{ "SetEncounterSpawnsCurrent", NWScript_SetEncounterSpawnsCurrent },
	{ "GetModuleItemAcquired", NWScript_GetModuleItemAcquired },
	{ "GetModuleItemAcquiredFrom", NWScript_GetModuleItemAcquiredFrom },
	{ "SetCustomToken", NWScript_SetCustomToken },
	{ "GetHasFeat", NWScript_GetHasFeat },
	{ "GetHasSkill", NWScript_GetHasSkill },
	{ "ActionUseFeat", NWScript_ActionUseFeat },
	{ "ActionUseSkill", NWScript_ActionUseSkill },
	{ "GetObjectSeen", NWScript_GetObjectSeen },
	{ "GetObjectHeard", NWScript_GetObjectHeard },
	{ "GetLastPlayerDied", NWScript_GetLastPlayerDied },
	{ "GetModuleItemLost", NWScript_GetModuleItemLost },
	{ "GetModuleItemLostBy", NWScript_GetModuleItemLostBy },
	{ "EventConversation", NWScript_EventConversation },
	{ "SetEncounterDifficulty", NWScript_SetEncounterDifficulty },
	{ "GetEncounterDifficulty", NWScript_GetEncounterDifficulty },
	{ "GetDistanceBetweenLocations", NWScript_GetDistanceBetweenLocations },
	{ "GetReflexAdjustedDamage", NWScript_GetReflexAdjustedDamage },
	{ "PlayAnimation", NWScript_PlayAnimation },
	{ "TalentSpell", NWScript_TalentSpell },
	{ "TalentFeat", NWScript_TalentFeat },
	{ "TalentSkill", NWScript_TalentSkill },
	{ "GetHasSpellEffect", NWScript_GetHasSpellEffect },
	{ "GetEffectSpellId", NWScript_GetEffectSpellId },
	{ "GetCreatureHasTalent", NWScript_GetCreatureHasTalent },
	{ "GetCreatureTalentRandom", NWScript_GetCreatureTalentRandom },
	{ "GetCreatureTalentBest", NWScript_GetCreatureTalentBest },
	{ "ActionUseTalentOnObject", NWScript_ActionUseTalentOnObject },
	{ "ActionUseTalentAtLocation", NWScript_ActionUseTalentAtLocation },
	{ "GetGoldPieceValue", NWScript_GetGoldPieceValue },
	{ "GetIsPlayableRacialType", NWScript_GetIsPlayableRacialType },
	{ "JumpToLocation", NWScript_JumpToLocation },
	{ "EffectTemporaryHitpoints", NWScript_EffectTemporaryHitpoints },
	{ "GetSkillRank", NWScript_GetSkillRank },
	{ "GetAttackTarget", NWScript_GetAttackTarget },
	{ "GetLastAttackType", NWScript_GetLastAttackType },
	{ "GetLastAttackMode", NWScript_GetLastAttackMode },
	{ "GetMaster", NWScript_GetMaster },
	{ "GetIsInCombat", NWScript_GetIsInCombat },
	{ "GetLastAssociateCommand", NWScript_GetLastAssociateCommand },
	{ "GiveGoldToCreature", NWScript_GiveGoldToCreature },
	{ "SetIsDestroyable", NWScript_SetIsDestroyable },
	{ "SetLocked", NWScript_SetLocked },
	{ "GetLocked", NWScript_GetLocked },
	{ "GetClickingObject", NWScript_GetClickingObject },
	{ "SetAssociateListenPatterns", NWScript_SetAssociateListenPatterns },
	{ "GetLastWeaponUsed", NWScript_GetLastWeaponUsed },
	{ "ActionInteractObject", NWScript_ActionInteractObject },
	{ "GetLastUsedBy", NWScript_GetLastUsedBy },
	{ "GetAbilityModifier", NWScript_GetAbilityModifier },
	{ "GetIdentified", NWScript_GetIdentified },
	{ "SetIdentified", NWScript_SetIdentified },
	{ "SummonAnimalCompanion", NWScript_SummonAnimalCompanion },
	{ "SummonFamiliar", NWScript_SummonFamiliar },
	{ "GetBlockingDoor", NWScript_GetBlockingDoor },
	{ "GetIsDoorActionPossible", NWScript_GetIsDoorActionPossible },
	{ "DoDoorAction", NWScript_DoDoorAction },
	{ "GetFirstItemInInventory", NWScript_GetFirstItemInInventory },
	{ "GetNextItemInInventory", NWScript_GetNextItemInInventory },
	{ "GetClassByPosition", NWScript_GetClassByPosition },
	{ "GetLevelByPosition", NWScript_GetLevelByPosition },
	{ "GetLevelByClass", NWScript_GetLevelByClass },
	{ "GetDamageDealtByType", NWScript_GetDamageDealtByType },
	{ "GetTotalDamageDealt", NWScript_GetTotalDamageDealt },
	{ "GetLastDamager", NWScript_GetLastDamager },
	{ "GetLastDisarmed", NWScript_GetLastDisarmed },
	{ "GetLastDisturbed", NWScript_GetLastDisturbed },
	{ "GetLastLocked", NWScript_GetLastLocked },
	{ "GetLastUnlocked", NWScript_GetLastUnlocked },
	{ "EffectSkillIncrease", NWScript_EffectSkillIncrease },
	{ "GetInventoryDisturbType", NWScript_GetInventoryDisturbType },
	{ "GetInventoryDisturbItem", NWScript_GetInventoryDisturbItem },
	{ "GetHenchman", NWScript_GetHenchman },
	{ "VersusAlignmentEffect", NWScript_VersusAlignmentEffect },
	{ "VersusRacialTypeEffect", NWScript_VersusRacialTypeEffect },
	{ "VersusTrapEffect", NWScript_VersusTrapEffect },
	{ "GetGender", NWScript_GetGender },
	{ "GetIsTalentValid", NWScript_GetIsTalentValid },
	{ "ActionMoveAwayFromLocation", NWScript_ActionMoveAwayFromLocation },
	{ "GetAttemptedAttackTarget", NWScript_GetAttemptedAttackTarget },
	{ "GetTypeFromTalent", NWScript_GetTypeFromTalent },
	{ "GetIdFromTalent", NWScript_GetIdFromTalent },
	{ "GetAssociate", NWScript_GetAssociate },
	{ "AddHenchman", NWScript_AddHenchman },
	{ "RemoveHenchman", NWScript_RemoveHenchman },
	{ "AddJournalQuestEntry", NWScript_AddJournalQuestEntry },
	{ "RemoveJournalQuestEntry", NWScript_RemoveJournalQuestEntry },
	{ "GetPCPublicCDKey", NWScript_GetPCPublicCDKey },
	{ "GetPCIPAddress", NWScript_GetPCIPAddress },
	{ "GetPCPlayerName", NWScript_GetPCPlayerName },
	{ "SetPCLike", NWScript_SetPCLike },
	{ "SetPCDislike", NWScript_SetPCDislike },
	{ "SendMessageToPC", NWScript_SendMessageToPC },
	{ "GetAttemptedSpellTarget", NWScript_GetAttemptedSpellTarget },
	{ "GetLastOpenedBy", NWScript_GetLastOpenedBy },
	{ "GetHasSpell", NWScript_GetHasSpell },
	{ "OpenStore", NWScript_OpenStore },
	{ "EffectTurned", NWScript_EffectTurned },
	{ "GetFirstFactionMember", NWScript_GetFirstFactionMember },
	{ "GetNextFactionMember", NWScript_GetNextFactionMember },
	{ "ActionForceMoveToLocation", NWScript_ActionForceMoveToLocation },
	{ "ActionForceMoveToObject", NWScript_ActionForceMoveToObject },
	{ "GetJournalQuestExperience", NWScript_GetJournalQuestExperience },
	{ "JumpToObject", NWScript_JumpToObject },
	{ "SetMapPinEnabled", NWScript_SetMapPinEnabled },
	{ "EffectHitPointChangeWhenDying", NWScript_EffectHitPointChangeWhenDying },
	{ "PopUpGUIPanel", NWScript_PopUpGUIPanel },
	{ "ClearPersonalReputation", NWScript_ClearPersonalReputation },
	{ "SetIsTemporaryFriend", NWScript_SetIsTemporaryFriend },
	{ "SetIsTemporaryEnemy", NWScript_SetIsTemporaryEnemy },
	{ "SetIsTemporaryNeutral", NWScript_SetIsTemporaryNeutral },
	{ "GiveXPToCreature", NWScript_GiveXPToCreature },
	{ "SetXP", NWScript_SetXP },
	{ "GetXP", NWScript_GetXP },
	{ "IntToHexString", NWScript_IntToHexString },
	{ "GetBaseItemType", NWScript_GetBaseItemType },
	{ "GetItemHasItemProperty", NWScript_GetItemHasItemProperty },
	{ "ActionEquipMostDamagingMelee", NWScript_ActionEquipMostDamagingMelee },
	{ "ActionEquipMostDamagingRanged", NWScript_ActionEquipMostDamagingRanged },
	{ "GetItemACValue", NWScript_GetItemACValue },
	{ "ActionRest", NWScript_ActionRest },
	{ "ExploreAreaForPlayer", NWScript_ExploreAreaForPlayer },
	{ "ActionEquipMostEffectiveArmor", NWScript_ActionEquipMostEffectiveArmor },
	{ "GetIsDay", NWScript_GetIsDay },
	{ "GetIsNight", NWScript_GetIsNight },
	{ "GetIsDawn", NWScript_GetIsDawn },
	{ "GetIsDusk", NWScript_GetIsDusk },
	{ "GetIsEncounterCreature", NWScript_GetIsEncounterCreature },
	{ "GetLastPlayerDying", NWScript_GetLastPlayerDying },
	{ "GetStartingLocation", NWScript_GetStartingLocation },
	{ "ChangeToStandardFaction", NWScript_ChangeToStandardFaction },
	{ "SoundObjectPlay", NWScript_SoundObjectPlay },
	{ "SoundObjectStop", NWScript_SoundObjectStop },
	{ "SoundObjectSetVolume", NWScript_SoundObjectSetVolume },
	{ "SoundObjectSetPosition", NWScript_SoundObjectSetPosition },
	{ "SpeakOneLinerConversation", NWScript_SpeakOneLinerConversation },
	{ "GetGold", NWScript_GetGold },
	{ "GetLastRespawnButtonPresser", NWScript_GetLastRespawnButtonPresser },
	{ "GetIsDM", NWScript_GetIsDM },
	{ "PlayVoiceChat", NWScript_PlayVoiceChat },
	{ "GetIsWeaponEffective", NWScript_GetIsWeaponEffective },
	{ "GetLastSpellHarmful", NWScript_GetLastSpellHarmful },
	{ "EventActivateItem", NWScript_EventActivateItem },
	{ "MusicBackgroundPlay", NWScript_MusicBackgroundPlay },
	{ "MusicBackgroundStop", NWScript_MusicBackgroundStop },
	{ "MusicBackgroundSetDelay", NWScript_MusicBackgroundSetDelay },
	{ "MusicBackgroundChangeDay", NWScript_MusicBackgroundChangeDay },
	{ "MusicBackgroundChangeNight", NWScript_MusicBackgroundChangeNight },
	{ "MusicBattlePlay", NWScript_MusicBattlePlay },
	{ "MusicBattleStop", NWScript_MusicBattleStop },
	{ "MusicBattleChange", NWScript_MusicBattleChange },
	{ "AmbientSoundPlay", NWScript_AmbientSoundPlay },
	{ "AmbientSoundStop", NWScript_AmbientSoundStop },
	{ "AmbientSoundChangeDay", NWScript_AmbientSoundChangeDay },
	{ "AmbientSoundChangeNight", NWScript_AmbientSoundChangeNight },
	{ "GetLastKiller", NWScript_GetLastKiller },
	{ "GetSpellCastItem", NWScript_GetSpellCastItem },
	{ "GetItemActivated", NWScript_GetItemActivated },
	{ "GetItemActivator", NWScript_GetItemActivator },
	{ "GetItemActivatedTargetLocation", NWScript_GetItemActivatedTargetLocation },
	{ "GetItemActivatedTarget", NWScript_GetItemActivatedTarget },
	{ "GetIsOpen", NWScript_GetIsOpen },
	{ "TakeGoldFromCreature", NWScript_TakeGoldFromCreature },
	{ "IsInConversation", NWScript_IsInConversation },
	{ "EffectAbilityDecrease", NWScript_EffectAbilityDecrease },
	{ "EffectAttackDecrease", NWScript_EffectAttackDecrease },
	{ "EffectDamageDecrease", NWScript_EffectDamageDecrease },
	{ "EffectDamageImmunityDecrease", NWScript_EffectDamageImmunityDecrease },
	{ "EffectACDecrease", NWScript_EffectACDecrease },
	{ "EffectMovementSpeedDecrease", NWScript_EffectMovementSpeedDecrease },
	{ "EffectSavingThrowDecrease", NWScript_EffectSavingThrowDecrease },
	{ "EffectSkillDecrease", NWScript_EffectSkillDecrease },
	{ "EffectSpellResistanceDecrease", NWScript_EffectSpellResistanceDecrease },
	{ "GetPlotFlag", NWScript_GetPlotFlag },
	{ "SetPlotFlag", NWScript_SetPlotFlag },
	{ "EffectInvisibility", NWScript_EffectInvisibility },
	{ "EffectConcealment", NWScript_EffectConcealment },
	{ "EffectDarkness", NWScript_EffectDarkness },
	{ "EffectDispelMagicAll", NWScript_EffectDispelMagicAll },
	{ "EffectUltravision", NWScript_EffectUltravision },
	{ "EffectNegativeLevel", NWScript_EffectNegativeLevel },
	{ "EffectPolymorph", NWScript_EffectPolymorph },
	{ "EffectSanctuary", NWScript_EffectSanctuary },
	{ "EffectTrueSeeing", NWScript_EffectTrueSeeing },
	{ "EffectSeeInvisible", NWScript_EffectSeeInvisible },
	{ "EffectTimeStop", NWScript_EffectTimeStop },
	{ "EffectBlindness", NWScript_EffectBlindness },
	{ "GetIsReactionTypeFriendly", NWScript_GetIsReactionTypeFriendly },
	{ "GetIsReactionTypeNeutral", NWScript_GetIsReactionTypeNeutral },
	{ "GetIsReactionTypeHostile", NWScript_GetIsReactionTypeHostile },
	{ "EffectSpellLevelAbsorption", NWScript_EffectSpellLevelAbsorption },
	{ "EffectDispelMagicBest", NWScript_EffectDispelMagicBest },
	{ "ActivatePortal", NWScript_ActivatePortal },
	{ "GetNumStackedItems", NWScript_GetNumStackedItems },
	{ "SurrenderToEnemies", NWScript_SurrenderToEnemies },
	{ "EffectMissChance", NWScript_EffectMissChance },
	{ "GetTurnResistanceHD", NWScript_GetTurnResistanceHD },
	{ "GetCreatureSize", NWScript_GetCreatureSize },
	{ "EffectDisappearAppear", NWScript_EffectDisappearAppear },
	{ "EffectDisappear", NWScript_EffectDisappear },
	{ "EffectAppear", NWScript_EffectAppear },
	{ "ActionUnlockObject", NWScript_ActionUnlockObject },
	{ "ActionLockObject", NWScript_ActionLockObject },
	{ "EffectModifyAttacks", NWScript_EffectModifyAttacks },
	{ "GetLastTrapDetected", NWScript_GetLastTrapDetected },
	{ "EffectDamageShield", NWScript_EffectDamageShield },
	{ "GetNearestTrapToObject", NWScript_GetNearestTrapToObject },
	{ "GetDeity", NWScript_GetDeity },
	{ "GetSubRace", NWScript_GetSubRace },
	{ "GetFortitudeSavingThrow", NWScript_GetFortitudeSavingThrow },
	{ "GetWillSavingThrow", NWScript_GetWillSavingThrow },
	{ "GetReflexSavingThrow", NWScript_GetReflexSavingThrow },
	{ "GetChallengeRating", NWScript_GetChallengeRating },
	{ "GetAge", NWScript_GetAge },
	{ "GetMovementRate", NWScript_GetMovementRate },
	{ "GetFamiliarCreatureType", NWScript_GetFamiliarCreatureType },
	{ "GetAnimalCompanionCreatureType", NWScript_GetAnimalCompanionCreatureType },
	{ "GetFamiliarName", NWScript_GetFamiliarName },
	{ "GetAnimalCompanionName", NWScript_GetAnimalCompanionName },
	{ "ActionCastFakeSpellAtObject", NWScript_ActionCastFakeSpellAtObject },
	{ "ActionCastFakeSpellAtLocation", NWScript_ActionCastFakeSpellAtLocation },
	{ "RemoveSummonedAssociate", NWScript_RemoveSummonedAssociate },
	{ "SetCameraMode", NWScript_SetCameraMode },
	{ "GetIsResting", NWScript_GetIsResting },
	{ "GetLastPCRested", NWScript_GetLastPCRested },
	{ "SetWeather", NWScript_SetWeather },
	{ "GetLastRestEventType", NWScript_GetLastRestEventType },
	{ "StartNewModule", NWScript_StartNewModule },
	{ "EffectSwarm", NWScript_EffectSwarm },
	{ "GetWeaponRanged", NWScript_GetWeaponRanged },
	{ "DoSinglePlayerAutoSave", NWScript_DoSinglePlayerAutoSave },
	{ "GetGameDifficulty", NWScript_GetGameDifficulty },
	{ "SetTileMainLightColor", NWScript_SetTileMainLightColor },
	{ "SetTileSourceLightColor", NWScript_SetTileSourceLightColor },
	{ "RecomputeStaticLighting", NWScript_RecomputeStaticLighting },
	{ "GetTileMainLight1Color", NWScript_GetTileMainLight1Color },
	{ "GetTileMainLight2Color", NWScript_GetTileMainLight2Color },
	{ "GetTileSourceLight1Color", NWScript_GetTileSourceLight1Color },
	{ "GetTileSourceLight2Color", NWScript_GetTileSourceLight2Color },
	{ "SetPanelButtonFlash", NWScript_SetPanelButtonFlash },
	{ "GetCurrentAction", NWScript_GetCurrentAction },
	{ "SetStandardFactionReputation", NWScript_SetStandardFactionReputation },
	{ "GetStandardFactionReputation", NWScript_GetStandardFactionReputation },
	{ "FloatingTextStrRefOnCreature", NWScript_FloatingTextStrRefOnCreature },
	{ "FloatingTextStringOnCreature", NWScript_FloatingTextStringOnCreature },
	{ "GetTrapDisarmable", NWScript_GetTrapDisarmable },
	{ "GetTrapDetectable", NWScript_GetTrapDetectable },
	{ "GetTrapDetectedBy", NWScript_GetTrapDetectedBy },
	{ "GetTrapFlagged", NWScript_GetTrapFlagged },
	{ "GetTrapBaseType", NWScript_GetTrapBaseType },
	{ "GetTrapOneShot", NWScript_GetTrapOneShot },
	{ "GetTrapCreator", NWScript_GetTrapCreator },
	{ "GetTrapKeyTag", NWScript_GetTrapKeyTag },
	{ "GetTrapDisarmDC", NWScript_GetTrapDisarmDC },
	{ "GetTrapDetectDC", NWScript_GetTrapDetectDC },
	{ "GetLockKeyRequired", NWScript_GetLockKeyRequired },
	{ "GetLockKeyTag", NWScript_GetLockKeyTag },
	{ "GetLockLockable", NWScript_GetLockLockable },
	{ "GetLockUnlockDC", NWScript_GetLockUnlockDC },
	{ "GetLockLockDC", NWScript_GetLockLockDC },
	{ "GetPCLevellingUp", NWScript_GetPCLevellingUp },
	{ "GetHasFeatEffect", NWScript_GetHasFeatEffect },
	{ "SetPlaceableIllumination", NWScript_SetPlaceableIllumination },
	{ "GetPlaceableIllumination", NWScript_GetPlaceableIllumination },
	{ "GetIsPlaceableObjectActionPossible", NWScript_GetIsPlaceableObjectActionPossible },
	{ "DoPlaceableObjectAction", NWScript_DoPlaceableObjectAction },
	{ "GetFirstPC", NWScript_GetFirstPC },
	{ "GetNextPC", NWScript_GetNextPC },
	{ "SetTrapDetectedBy", NWScript_SetTrapDetectedBy },
	{ "GetIsTrapped", NWScript_GetIsTrapped },
	{ "EffectTurnResistanceDecrease", NWScript_EffectTurnResistanceDecrease },
	{ "EffectTurnResistanceIncrease", NWScript_EffectTurnResistanceIncrease },
	{ "PopUpDeathGUIPanel", NWScript_PopUpDeathGUIPanel },
	{ "SetTrapDisabled", NWScript_SetTrapDisabled },
	{ "GetLastHostileActor", NWScript_GetLastHostileActor },
	{ "ExportAllCharacters", NWScript_ExportAllCharacters },
	{ "MusicBackgroundGetDayTrack", NWScript_MusicBackgroundGetDayTrack },
	{ "MusicBackgroundGetNightTrack", NWScript_MusicBackgroundGetNightTrack },
	{ "WriteTimestampedLogEntry", NWScript_WriteTimestampedLogEntry },
	{ "GetModuleName", NWScript_GetModuleName },
	{ "GetFactionLeader", NWScript_GetFactionLeader },
	{ "SendMessageToAllDMs", NWScript_SendMessageToAllDMs },
	{ "EndGame", NWScript_EndGame },
	{ "BootPC", NWScript_BootPC },
	{ "ActionCounterSpell", NWScript_ActionCounterSpell },
	{ "AmbientSoundSetDayVolume", NWScript_AmbientSoundSetDayVolume },
	{ "AmbientSoundSetNightVolume", NWScript_AmbientSoundSetNightVolume },
	{ "MusicBackgroundGetBattleTrack", NWScript_MusicBackgroundGetBattleTrack },
	{ "GetHasInventory", NWScript_GetHasInventory },
	{ "GetStrRefSoundDuration", NWScript_GetStrRefSoundDuration },
	{ "AddToParty", NWScript_AddToParty },
	{ "RemoveFromParty", NWScript_RemoveFromParty },
	{ "GetStealthMode", NWScript_GetStealthMode },
	{ "GetDetectMode", NWScript_GetDetectMode },
	{ "GetDefensiveCastingMode", NWScript_GetDefensiveCastingMode },
	{ "GetAppearanceType", NWScript_GetAppearanceType },
	{ "SpawnScriptDebugger", NWScript_SpawnScriptDebugger },
	{ "GetModuleItemAcquiredStackSize", NWScript_GetModuleItemAcquiredStackSize },
	{ "DecrementRemainingFeatUses", NWScript_DecrementRemainingFeatUses },
	{ "DecrementRemainingSpellUses", NWScript_DecrementRemainingSpellUses },
	{ "GetResRef", NWScript_GetResRef },
	{ "EffectPetrify", NWScript_EffectPetrify },
	{ "CopyItem", NWScript_CopyItem },
	{ "EffectCutsceneParalyze", NWScript_EffectCutsceneParalyze },
	{ "GetDroppableFlag", NWScript_GetDroppableFlag },
	{ "GetUseableFlag", NWScript_GetUseableFlag },
	{ "GetStolenFlag", NWScript_GetStolenFlag },
	{ "SetCampaignFloat", NWScript_SetCampaignFloat },
	{ "SetCampaignInt", NWScript_SetCampaignInt },
	{ "SetCampaignVector", NWScript_SetCampaignVector },
	{ "SetCampaignLocation", NWScript_SetCampaignLocation },
	{ "SetCampaignString", NWScript_SetCampaignString },
	{ "DestroyCampaignDatabase", NWScript_DestroyCampaignDatabase },
	{ "GetCampaignFloat", NWScript_GetCampaignFloat },
	{ "GetCampaignInt", NWScript_GetCampaignInt },
	{ "GetCampaignVector", NWScript_GetCampaignVector },
	{ "GetCampaignLocation", NWScript_GetCampaignLocation },
	{ "GetCampaignString", NWScript_GetCampaignString },
	{ "CopyObject", NWScript_CopyObject },
	{ "DeleteCampaignVariable", NWScript_DeleteCampaignVariable },
	{ "StoreCampaignObject", NWScript_StoreCampaignObject },
	{ "RetrieveCampaignObject", NWScript_RetrieveCampaignObject },
	{ "EffectCutsceneDominated", NWScript_EffectCutsceneDominated },
	{ "GetItemStackSize", NWScript_GetItemStackSize },
	{ "SetItemStackSize", NWScript_SetItemStackSize },
	{ "GetItemCharges", NWScript_GetItemCharges },
	{ "SetItemCharges", NWScript_SetItemCharges },
	{ "AddItemProperty", NWScript_AddItemProperty },
	{ "RemoveItemProperty", NWScript_RemoveItemProperty },
	{ "GetIsItemPropertyValid", NWScript_GetIsItemPropertyValid },
	{ "GetFirstItemProperty", NWScript_GetFirstItemProperty },
	{ "GetNextItemProperty", NWScript_GetNextItemProperty },
	{ "GetItemPropertyType", NWScript_GetItemPropertyType },
	{ "GetItemPropertyDurationType", NWScript_GetItemPropertyDurationType },
	{ "ItemPropertyAbilityBonus", NWScript_ItemPropertyAbilityBonus },
	{ "ItemPropertyACBonus", NWScript_ItemPropertyACBonus },
	{ "ItemPropertyACBonusVsAlign", NWScript_ItemPropertyACBonusVsAlign },
	{ "ItemPropertyACBonusVsDmgType", NWScript_ItemPropertyACBonusVsDmgType },
	{ "ItemPropertyACBonusVsRace", NWScript_ItemPropertyACBonusVsRace },
	{ "ItemPropertyACBonusVsSAlign", NWScript_ItemPropertyACBonusVsSAlign },
	{ "ItemPropertyEnhancementBonus", NWScript_ItemPropertyEnhancementBonus },
	{ "ItemPropertyEnhancementBonusVsAlign", NWScript_ItemPropertyEnhancementBonusVsAlign },
	{ "ItemPropertyEnhancementBonusVsRace", NWScript_ItemPropertyEnhancementBonusVsRace },
	{ "ItemPropertyEnhancementBonusVsSAlign", NWScript_ItemPropertyEnhancementBonusVsSAlign },
	{ "ItemPropertyEnhancementPenalty", NWScript_ItemPropertyEnhancementPenalty },
	{ "ItemPropertyWeightReduction", NWScript_ItemPropertyWeightReduction },
	{ "ItemPropertyBonusFeat", NWScript_ItemPropertyBonusFeat },
	{ "ItemPropertyBonusLevelSpell", NWScript_ItemPropertyBonusLevelSpell },
	{ "ItemPropertyCastSpell", NWScript_ItemPropertyCastSpell },
	{ "ItemPropertyDamageBonus", NWScript_ItemPropertyDamageBonus },
	{ "ItemPropertyDamageBonusVsAlign", NWScript_ItemPropertyDamageBonusVsAlign },
	{ "ItemPropertyDamageBonusVsRace", NWScript_ItemPropertyDamageBonusVsRace },
	{ "ItemPropertyDamageBonusVsSAlign", NWScript_ItemPropertyDamageBonusVsSAlign },
	{ "ItemPropertyDamageImmunity", NWScript_ItemPropertyDamageImmunity },
	{ "ItemPropertyDamagePenalty", NWScript_ItemPropertyDamagePenalty },
	{ "ItemPropertyDamageReduction", NWScript_ItemPropertyDamageReduction },
	{ "ItemPropertyDamageResistance", NWScript_ItemPropertyDamageResistance },
	{ "ItemPropertyDamageVulnerability", NWScript_ItemPropertyDamageVulnerability },
	{ "ItemPropertyDarkvision", NWScript_ItemPropertyDarkvision },
	{ "ItemPropertyDecreaseAbility", NWScript_ItemPropertyDecreaseAbility },
	{ "ItemPropertyDecreaseAC", NWScript_ItemPropertyDecreaseAC },
	{ "ItemPropertyDecreaseSkill", NWScript_ItemPropertyDecreaseSkill },
	{ "ItemPropertyContainerReducedWeight", NWScript_ItemPropertyContainerReducedWeight },
	{ "ItemPropertyExtraMeleeDamageType", NWScript_ItemPropertyExtraMeleeDamageType },
	{ "ItemPropertyExtraRangeDamageType", NWScript_ItemPropertyExtraRangeDamageType },
	{ "ItemPropertyHaste", NWScript_ItemPropertyHaste },
	{ "ItemPropertyHolyAvenger", NWScript_ItemPropertyHolyAvenger },
	{ "ItemPropertyImmunityMisc", NWScript_ItemPropertyImmunityMisc },
	{ "ItemPropertyImprovedEvasion", NWScript_ItemPropertyImprovedEvasion },
	{ "ItemPropertyBonusSpellResistance", NWScript_ItemPropertyBonusSpellResistance },
	{ "ItemPropertyBonusSavingThrowVsX", NWScript_ItemPropertyBonusSavingThrowVsX },
	{ "ItemPropertyBonusSavingThrow", NWScript_ItemPropertyBonusSavingThrow },
	{ "ItemPropertyKeen", NWScript_ItemPropertyKeen },
	{ "ItemPropertyLight", NWScript_ItemPropertyLight },
	{ "ItemPropertyMaxRangeStrengthMod", NWScript_ItemPropertyMaxRangeStrengthMod },
	{ "ItemPropertyNoDamage", NWScript_ItemPropertyNoDamage },
	{ "ItemPropertyOnHitProps", NWScript_ItemPropertyOnHitProps },
	{ "ItemPropertyReducedSavingThrowVsX", NWScript_ItemPropertyReducedSavingThrowVsX },
	{ "ItemPropertyReducedSavingThrow", NWScript_ItemPropertyReducedSavingThrow },
	{ "ItemPropertyRegeneration", NWScript_ItemPropertyRegeneration },
	{ "ItemPropertySkillBonus", NWScript_ItemPropertySkillBonus },
	{ "ItemPropertySpellImmunitySpecific", NWScript_ItemPropertySpellImmunitySpecific },
	{ "ItemPropertySpellImmunitySchool", NWScript_ItemPropertySpellImmunitySchool },
	{ "ItemPropertyThievesTools", NWScript_ItemPropertyThievesTools },
	{ "ItemPropertyAttackBonus", NWScript_ItemPropertyAttackBonus },
	{ "ItemPropertyAttackBonusVsAlign", NWScript_ItemPropertyAttackBonusVsAlign },
	{ "ItemPropertyAttackBonusVsRace", NWScript_ItemPropertyAttackBonusVsRace },
	{ "ItemPropertyAttackBonusVsSAlign", NWScript_ItemPropertyAttackBonusVsSAlign },
	{ "ItemPropertyAttackPenalty", NWScript_ItemPropertyAttackPenalty },
	{ "ItemPropertyUnlimitedAmmo", NWScript_ItemPropertyUnlimitedAmmo },
	{ "ItemPropertyLimitUseByAlign", NWScript_ItemPropertyLimitUseByAlign },
	{ "ItemPropertyLimitUseByClass", NWScript_ItemPropertyLimitUseByClass },
	{ "ItemPropertyLimitUseByRace", NWScript_ItemPropertyLimitUseByRace },
	{ "ItemPropertyLimitUseBySAlign", NWScript_ItemPropertyLimitUseBySAlign },
	{ "BadBadReplaceMeThisDoesNothing", NWScript_BadBadReplaceMeThisDoesNothing },
	{ "ItemPropertyVampiricRegeneration", NWScript_ItemPropertyVampiricRegeneration },
	{ "ItemPropertyTrap", NWScript_ItemPropertyTrap },
	{ "ItemPropertyTrueSeeing", NWScript_ItemPropertyTrueSeeing },
	{ "ItemPropertyOnMonsterHitProperties", NWScript_ItemPropertyOnMonsterHitProperties },
	{ "ItemPropertyTurnResistance", NWScript_ItemPropertyTurnResistance },
	{ "ItemPropertyMassiveCritical", NWScript_ItemPropertyMassiveCritical },
	{ "ItemPropertyFreeAction", NWScript_ItemPropertyFreeAction },
	{ "ItemPropertyMonsterDamage", NWScript_ItemPropertyMonsterDamage },
	{ "ItemPropertyImmunityToSpellLevel", NWScript_ItemPropertyImmunityToSpellLevel },
	{ "ItemPropertySpecialWalk", NWScript_ItemPropertySpecialWalk },
	{ "ItemPropertyHealersKit", NWScript_ItemPropertyHealersKit },
	{ "ItemPropertyWeightIncrease", NWScript_ItemPropertyWeightIncrease },
	{ "GetIsSkillSuccessful", NWScript_GetIsSkillSuccessful },
	{ "EffectSpellFailure", NWScript_EffectSpellFailure },
	{ "SpeakStringByStrRef", NWScript_SpeakStringByStrRef },
	{ "SetCutsceneMode", NWScript_SetCutsceneMode },
	{ "GetLastPCToCancelCutscene", NWScript_GetLastPCToCancelCutscene },
	{ "GetDialogSoundLength", NWScript_GetDialogSoundLength },
	{ "FadeFromBlack", NWScript_FadeFromBlack },
	{ "FadeToBlack", NWScript_FadeToBlack },
	{ "StopFade", NWScript_StopFade },
	{ "BlackScreen", NWScript_BlackScreen },
	{ "GetBaseAttackBonus", NWScript_GetBaseAttackBonus },
	{ "SetImmortal", NWScript_SetImmortal },
	{ "OpenInventory", NWScript_OpenInventory },
	{ "StoreCameraFacing", NWScript_StoreCameraFacing },
	{ "RestoreCameraFacing", NWScript_RestoreCameraFacing },
	{ "LevelUpHenchman", NWScript_LevelUpHenchman },
	{ "SetDroppableFlag", NWScript_SetDroppableFlag },
	{ "GetWeight", NWScript_GetWeight },
	{ "GetModuleItemAcquiredBy", NWScript_GetModuleItemAcquiredBy },
	{ "GetImmortal", NWScript_GetImmortal },
	{ "DoWhirlwindAttack", NWScript_DoWhirlwindAttack },
	{ "Get2DAString", NWScript_Get2DAString },
	{ "EffectEthereal", NWScript_EffectEthereal },
	{ "GetAILevel", NWScript_GetAILevel },
	{ "SetAILevel", NWScript_SetAILevel },
	{ "GetIsPossessedFamiliar", NWScript_GetIsPossessedFamiliar },
	{ "UnpossessFamiliar", NWScript_UnpossessFamiliar },
	{ "GetIsAreaInterior", NWScript_GetIsAreaInterior },
	{ "SendMessageToPCByStrRef", NWScript_SendMessageToPCByStrRef },
	{ "IncrementRemainingFeatUses", NWScript_IncrementRemainingFeatUses },
	{ "ExportSingleCharacter", NWScript_ExportSingleCharacter },
	{ "PlaySoundByStrRef", NWScript_PlaySoundByStrRef },
	{ "SetSubRace", NWScript_SetSubRace },
	{ "SetDeity", NWScript_SetDeity },
	{ "GetIsDMPossessed", NWScript_GetIsDMPossessed },
	{ "GetWeather", NWScript_GetWeather },
	{ "GetIsAreaNatural", NWScript_GetIsAreaNatural },
	{ "GetIsAreaAboveGround", NWScript_GetIsAreaAboveGround },
	{ "GetPCItemLastEquipped", NWScript_GetPCItemLastEquipped },
	{ "GetPCItemLastEquippedBy", NWScript_GetPCItemLastEquippedBy },
	{ "GetPCItemLastUnequipped", NWScript_GetPCItemLastUnequipped },
	{ "GetPCItemLastUnequippedBy", NWScript_GetPCItemLastUnequippedBy },
	{ "CopyItemAndModify", NWScript_CopyItemAndModify },
	{ "GetItemAppearance", NWScript_GetItemAppearance },
	{ "ItemPropertyOnHitCastSpell", NWScript_ItemPropertyOnHitCastSpell },
	{ "GetItemPropertySubType", NWScript_GetItemPropertySubType },
	{ "GetActionMode", NWScript_GetActionMode },
	{ "SetActionMode", NWScript_SetActionMode },
	{ "GetArcaneSpellFailure", NWScript_GetArcaneSpellFailure },
	{ "ActionExamine", NWScript_ActionExamine },
	{ "ItemPropertyVisualEffect", NWScript_ItemPropertyVisualEffect },
	{ "SetLootable", NWScript_SetLootable },
	{ "GetLootable", NWScript_GetLootable },
	{ "GetCutsceneCameraMoveRate", NWScript_GetCutsceneCameraMoveRate },
	{ "SetCutsceneCameraMoveRate", NWScript_SetCutsceneCameraMoveRate },
	{ "GetItemCursedFlag", NWScript_GetItemCursedFlag },
	{ "SetItemCursedFlag", NWScript_SetItemCursedFlag },
	{ "SetMaxHenchmen", NWScript_SetMaxHenchmen },
	{ "GetMaxHenchmen", NWScript_GetMaxHenchmen },
	{ "GetAssociateType", NWScript_GetAssociateType },
	{ "GetSpellResistance", NWScript_GetSpellResistance },
	{ "DayToNight", NWScript_DayToNight },
	{ "NightToDay", NWScript_NightToDay },
	{ "LineOfSightObject", NWScript_LineOfSightObject },
	{ "LineOfSightVector", NWScript_LineOfSightVector },
	{ "GetLastSpellCastClass", NWScript_GetLastSpellCastClass },
	{ "SetBaseAttackBonus", NWScript_SetBaseAttackBonus },
	{ "RestoreBaseAttackBonus", NWScript_RestoreBaseAttackBonus },
	{ "EffectCutsceneGhost", NWScript_EffectCutsceneGhost },
	{ "ItemPropertyArcaneSpellFailure", NWScript_ItemPropertyArcaneSpellFailure },
	{ "GetStoreGold", NWScript_GetStoreGold },
	{ "SetStoreGold", NWScript_SetStoreGold },
	{ "GetStoreMaxBuyPrice", NWScript_GetStoreMaxBuyPrice },
	{ "SetStoreMaxBuyPrice", NWScript_SetStoreMaxBuyPrice },
	{ "GetStoreIdentifyCost", NWScript_GetStoreIdentifyCost },
	{ "SetStoreIdentifyCost", NWScript_SetStoreIdentifyCost },
	{ "SetCreatureAppearanceType", NWScript_SetCreatureAppearanceType },
	{ "GetCreatureStartingPackage", NWScript_GetCreatureStartingPackage },
	{ "EffectCutsceneImmobilize", NWScript_EffectCutsceneImmobilize },
	{ "GetIsInSubArea", NWScript_GetIsInSubArea },
	{ "GetItemPropertyCostTable", NWScript_GetItemPropertyCostTable },
	{ "GetItemPropertyCostTableValue", NWScript_GetItemPropertyCostTableValue },
	{ "GetItemPropertyParam1", NWScript_GetItemPropertyParam1 },
	{ "GetItemPropertyParam1Value", NWScript_GetItemPropertyParam1Value },
	{ "GetIsCreatureDisarmable", NWScript_GetIsCreatureDisarmable },
	{ "SetStolenFlag", NWScript_SetStolenFlag },
	{ "ForceRest", NWScript_ForceRest },
	{ "SetCameraHeight", NWScript_SetCameraHeight },
	{ "SetSkyBox", NWScript_SetSkyBox },
	{ "GetPhenoType", NWScript_GetPhenoType },
	{ "SetPhenoType", NWScript_SetPhenoType },
	{ "SetFogColor", NWScript_SetFogColor },
	{ "GetCutsceneMode", NWScript_GetCutsceneMode },
	{ "GetSkyBox", NWScript_GetSkyBox },
	{ "GetFogColor", NWScript_GetFogColor },
	{ "SetFogAmount", NWScript_SetFogAmount },
	{ "GetFogAmount", NWScript_GetFogAmount },
	{ "GetPickpocketableFlag", NWScript_GetPickpocketableFlag },
	{ "SetPickpocketableFlag", NWScript_SetPickpocketableFlag },
	{ "GetFootstepType", NWScript_GetFootstepType },
	{ "SetFootstepType", NWScript_SetFootstepType },
	{ "GetCreatureWingType", NWScript_GetCreatureWingType },
	{ "SetCreatureWingType", NWScript_SetCreatureWingType },
	{ "GetCreatureBodyPart", NWScript_GetCreatureBodyPart },
	{ "SetCreatureBodyPart", NWScript_SetCreatureBodyPart },
	{ "GetCreatureTailType", NWScript_GetCreatureTailType },
	{ "SetCreatureTailType", NWScript_SetCreatureTailType },
	{ "GetHardness", NWScript_GetHardness },
	{ "SetHardness", NWScript_SetHardness },
	{ "SetLockKeyRequired", NWScript_SetLockKeyRequired },
	{ "SetLockKeyTag", NWScript_SetLockKeyTag },
	{ "SetLockLockable", NWScript_SetLockLockable },
	{ "SetLockUnlockDC", NWScript_SetLockUnlockDC },
	{ "SetLockLockDC", NWScript_SetLockLockDC },
	{ "SetTrapDisarmable", NWScript_SetTrapDisarmable },
	{ "SetTrapDetectable", NWScript_SetTrapDetectable },
	{ "SetTrapOneShot", NWScript_SetTrapOneShot },
	{ "SetTrapKeyTag", NWScript_SetTrapKeyTag },
	{ "SetTrapDisarmDC", NWScript_SetTrapDisarmDC },
	{ "SetTrapDetectDC", NWScript_SetTrapDetectDC },
	{ "CreateTrapAtLocation", NWScript_CreateTrapAtLocation },
	{ "CreateTrapOnObject", NWScript_CreateTrapOnObject },
	{ "SetWillSavingThrow", NWScript_SetWillSavingThrow },
	{ "SetReflexSavingThrow", NWScript_SetReflexSavingThrow },
	{ "SetFortitudeSavingThrow", NWScript_SetFortitudeSavingThrow },
	{ "GetTilesetResRef", NWScript_GetTilesetResRef },
	{ "GetTrapRecoverable", NWScript_GetTrapRecoverable },
	{ "SetTrapRecoverable", NWScript_SetTrapRecoverable },
	{ "GetModuleXPScale", NWScript_GetModuleXPScale },
	{ "SetModuleXPScale", NWScript_SetModuleXPScale },
	{ "GetKeyRequiredFeedback", NWScript_GetKeyRequiredFeedback },
	{ "SetKeyRequiredFeedback", NWScript_SetKeyRequiredFeedback },
	{ "GetTrapActive", NWScript_GetTrapActive },
	{ "SetTrapActive", NWScript_SetTrapActive },
	{ "LockCameraPitch", NWScript_LockCameraPitch },
	{ "LockCameraDistance", NWScript_LockCameraDistance },
	{ "LockCameraDirection", NWScript_LockCameraDirection },
	{ "GetPlaceableLastClickedBy", NWScript_GetPlaceableLastClickedBy },
	{ "GetInfiniteFlag", NWScript_GetInfiniteFlag },
	{ "SetInfiniteFlag", NWScript_SetInfiniteFlag },
	{ "GetAreaSize", NWScript_GetAreaSize },
	{ "SetName", NWScript_SetName },
	{ "GetPortraitId", NWScript_GetPortraitId },
	{ "SetPortraitId", NWScript_SetPortraitId },
	{ "GetPortraitResRef", NWScript_GetPortraitResRef },
	{ "SetPortraitResRef", NWScript_SetPortraitResRef },
	{ "SetUseableFlag", NWScript_SetUseableFlag },
	{ "GetDescription", NWScript_GetDescription },
	{ "SetDescription", NWScript_SetDescription },
	{ "GetPCChatSpeaker", NWScript_GetPCChatSpeaker },
	{ "GetPCChatMessage", NWScript_GetPCChatMessage },
	{ "GetPCChatVolume", NWScript_GetPCChatVolume },
	{ "SetPCChatMessage", NWScript_SetPCChatMessage },
	{ "SetPCChatVolume", NWScript_SetPCChatVolume },
	{ "GetColor", NWScript_GetColor },
	{ "SetColor", NWScript_SetColor },
	{ "ItemPropertyMaterial", NWScript_ItemPropertyMaterial },
	{ "ItemPropertyQuality", NWScript_ItemPropertyQuality },
	{ "ItemPropertyAdditional", NWScript_ItemPropertyAdditional },
        { NULL, NULL }
};
// VECTOR

static int vector_get_x(lua_State *L)
{
	Vector *ptr = (Vector *)luaL_checkudata(L, 1, VECTOR);
	lua_pushnumber(L, ptr->X);
	return 1;
}

static int vector_get_y(lua_State *L)
{
	Vector *ptr = (Vector *)luaL_checkudata(L, 1, VECTOR);
	lua_pushnumber(L, ptr->Y);
	return 1;
}
static int vector_get_z(lua_State *L)
{
	Vector *ptr = (Vector *)luaL_checkudata(L, 1, VECTOR);
	lua_pushnumber(L, ptr->Z);
	return 1;
}

static int vector_to_string(lua_State *L)
{
	Vector *ptr = (Vector *)luaL_checkudata(L, 1, VECTOR);
	lua_pushfstring(L, "vector: %p", ptr);
	return 1;
}
static int vector_equal(lua_State *L)
{
	Vector *ptr = (Vector *)luaL_checkudata(L, 1, VECTOR);
	Vector *ptr2 = (Vector *)luaL_checkudata(L, 2, VECTOR);

	if ((ptr->X == ptr2->X) && (ptr->Y == ptr2->Y) && (ptr->Z == ptr2->Z)){
		lua_pushboolean(L, 1);
		return 1;
	}
	lua_pushboolean(L, 0);
	return 1;
}

static int
vector_set(lua_State *L) {
	Vector *vec = (Vector *) luaL_checkudata(L, 1, VECTOR);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double z = luaL_checknumber(L, 4);

	vec->X = (float) x;
	vec->Y = (float) y;
	vec->Z = (float) z;

	return 0;
}

static const luaL_Reg vectorlib [] = {
  {"new", NWScript_Vector},
  {"x", vector_get_x},
  {"y", vector_get_y},
  {"z", vector_get_z},
  {"set", vector_set},
  {"magnitude", NWScript_VectorMagnitude},
  {"normalize", NWScript_VectorNormalize},
  {"toangle", NWScript_VectorToAngle},
  {NULL, NULL}
};

static const luaL_Reg vectorlib_m [] = {
  {"__tostring", vector_to_string},
  {"__eq", vector_equal},
  {NULL, NULL}
};


// LOCATION
static int location_get_x(lua_State *L)
{
	CScriptLocation *ptr = lua_nwn_checkloc(L, 1);
	lua_pushnumber(L, ptr->X);
	return 1;
}
static int location_get_y(lua_State *L)
{
	CScriptLocation *ptr = lua_nwn_checkloc(L, 1);
	lua_pushnumber(L, ptr->Y);
	return 1;
}
static int location_get_z(lua_State *L)
{
	CScriptLocation *ptr = lua_nwn_checkloc(L, 1);
	lua_pushnumber(L, ptr->Z);
	return 1;
}
static int location_get_area_id(lua_State *L)
{
	CScriptLocation *ptr = lua_nwn_checkloc(L, 1);
	lua_pushinteger(L, ptr->AreaID);
	return 1;
}
static int location_get_facing(lua_State *L)
{
	CScriptLocation *ptr = lua_nwn_checkloc(L, 1);
	lua_pushnumber(L, ptr->OrientationX);
	return 1;
}

static int location_new(lua_State *L) {
	dword area = luaL_checkint(L, 1);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double z = luaL_checknumber(L, 4);
	double facing = luaL_checknumber(L, 5);

	Vector pos = { (float) x, (float) y, (float) z };

	StackPushFloat(facing);
	StackPushVector(pos);
	StackPushObject(area);
	VM_ExecuteCommand(215, 3);

	void *ptr;
	StackPopEngineStructure(ENGINE_STRUCTURE_LOCATION, &ptr);
	lua_pushlightuserdata(L, ptr);

	return 1;
}

static const luaL_Reg locationlib [] = {
  {"new", location_new},
  {"x", location_get_x},
  {"y", location_get_y},
  {"z", location_get_z},
  {"area", location_get_area_id},
  {"facing", location_get_facing},
  {NULL, NULL}
};

static int
native_getoid(lua_State *L) {
	char *ptr = nwn_checkptr(L, 1, "CGameObject");
	dword oid = *(dword *) (ptr + 4);
	lua_pushinteger(L, oid);

	return 1;
}

static const luaL_Reg nativelib[] = {
	{ "getoid", native_getoid },
	{ NULL, NULL }
};

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))
int
luaopen_nwn_vector(lua_State *L) {
	lua_createtable(L, 0, ARRAY_LEN(vectorlib) - 1);
	luaL_register(L, NULL, vectorlib);

	luaL_newmetatable(L, VECTOR);
	luaL_register(L, NULL, vectorlib_m);

	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3);   // dup methods table
	lua_rawset(L, -3);      // metatable.__index = methods

	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, -3);   // dup methods table
	lua_rawset(L, -3);      // hide metatable: mt.__metatable = methods
	lua_pop(L, 1);          // drop metatable
	return 1;               // return methods on the stack
}

int
luaopen_nwn_location(lua_State *L) {
	lua_createtable(L, 0, ARRAY_LEN(locationlib) - 1);
	luaL_register(L, NULL, locationlib);
	return 1;
}

int
luaopen_nwn_script(lua_State *L) {
	lua_createtable(L, 0, ARRAY_LEN(nwscriptlib) - 1);
	luaL_register(L, NULL, nwscriptlib);
	return 1;
}

int
luaopen_nwn_native(lua_State *L) {
	lua_createtable(L, 0, ARRAY_LEN(nativelib) - 1);
	luaL_register(L, NULL, nativelib);
	return 1;
}
