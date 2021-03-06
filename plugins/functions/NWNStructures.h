#ifndef NWNXStructures_h_
#define NWNXStructures_h_

#include "typedefs.h"

struct CNWSAmbientSound
{
	dword Enabled;
	dword MusicDelay;
	dword MusicDay;
	dword MusicNight;
	dword field_10;
	dword MusicBattle;
	dword field_18;
	dword AmbientSndDay;
	dword AmbientSndNight;
	byte  AmbientSndDayVol;
	byte  AmbientSndNitVol;
	byte  field_26;
	byte  field_27;
	void *CNWSAmbientSoundClass;
};

struct AddressStruct
{
	word unk;		//0x0 
	word port;		//0x2
	dword ip;		//0x4
	dword unk2;		//0x8
	dword unk3;		//0xC
}; //total 0x10


struct CServerExoApp;
struct CServerExoAppInternal;
struct CServerExoAppConfig;
struct CExoString;
struct CExoLocString_st;
struct CResRef;
struct CExoLinkedList_st;
struct CExoArrayList;
struct CNWSObject;
struct CNWSCreature;

struct CServerExoApp
{
  /* 0x0/0 */ unsigned long field_0;
  /* 0x4/4 */ CServerExoAppInternal *Internal;
};
struct CServerExoAppInternal
{
  /* 0x0/0 */ unsigned long field_0[16384];
  /* 0x10000/65536 */ unsigned long UseNetworking;
  /* 0x10004/65540 */ unsigned long UseGameSpy;
  /* 0x10008/65544 */ unsigned long field_10008;
  /* 0x1000C/65548 */ CServerExoAppConfig *config;
  /* 0x10010/65552 */ unsigned short State;
  /* 0x10014/65556 */ char rsvd1[2];
  /* 0x10014/65556 */ unsigned long field_10014;
  /* 0x10018/65560 */ unsigned long CNWSMessage;
  /* 0x1001C/65564 */ unsigned long field_1001C;
  /* 0x10020/65568 */ unsigned long field_10020;
  /* 0x10024/65572 */ unsigned long NotTranslatedAddresses;
  /* 0x10028/65576 */ unsigned long field_10028;
  /* 0x1002C/65580 */ unsigned long field_1002C;
  /* 0x10030/65584 */ unsigned long ShutdownServer;
  /* 0x10034/65588 */ unsigned long field_10034;
  /* 0x10038/65592 */ unsigned long field_10038;
  /* 0x1003C/65596 */ unsigned long field_1003C;
  /* 0x10040/65600 */ unsigned long field_10040;
  /* 0x10044/65604 */ unsigned long field_10044;
  /* 0x10048/65608 */ unsigned long field_10048;
  /* 0x1004C/65612 */ unsigned long field_1004C;
  /* 0x10050/65616 */ unsigned long field_10050;
  /* 0x10054/65620 */ unsigned long field_10054;
  /* 0x10058/65624 */ unsigned long field_10058;
  /* 0x1005C/65628 */ unsigned long field_1005C;
  /* 0x10060/65632 */ unsigned long CNetLayer;
  /* 0x10064/65636 */ unsigned long CServerAIMaster;
  /* 0x10068/65640 */ unsigned long WorldTimer1;
  /* 0x1006C/65644 */ unsigned long WorldTimer2;
  /* 0x10070/65648 */ unsigned long WorldTimer3;
  /* 0x10074/65652 */ unsigned long CFactionManager;
  /* 0x10078/65656 */ unsigned long CConnectionLib;
  /* 0x1007C/65660 */ unsigned long field_1007C;
  /* 0x10080/65664 */ unsigned long CGameObjectArray;
  /* 0x10084/65668 */ unsigned long field_10084;
  /* 0x10088/65672 */ void *ClientsList;
  /* 0x1008C/65676 */ unsigned long field_1008C;
  /* 0x10090/65680 */ unsigned long field_10090;
  /* 0x10094/65684 */ unsigned long field_10094;
  /* 0x10098/65688 */ unsigned long field_10098;
  /* 0x1009C/65692 */ unsigned long field_1009C;
  /* 0x100A0/65696 */ unsigned long field_100A0;
  /* 0x100A4/65700 */ unsigned long field_100A4;
  /* 0x100A8/65704 */ unsigned long StartNewModule;
  /* 0x100AC/65708 */ unsigned long ModuleName;
  /* 0x100B0/65712 */ unsigned long field_100B0;
  /* 0x100B4/65716 */ unsigned long EndGame;
  /* 0x100B8/65720 */ unsigned long field_100B8;
  /* 0x100BC/65724 */ unsigned long field_100BC;
  /* 0x100C0/65728 */ unsigned long field_100C0;
  /* 0x100C4/65732 */ unsigned long field_100C4;
  /* 0x100C8/65736 */ unsigned long field_100C8;
  /* 0x100CC/65740 */ unsigned long field_100CC;
  /* 0x100D0/65744 */ unsigned long ExportCharacters;
  /* 0x100D4/65748 */ unsigned long CharacterToExport;
  /* 0x100D8/65752 */ unsigned long field_100D8;
  /* 0x100DC/65756 */ unsigned long field_100DC;
  /* 0x100E0/65760 */ unsigned long field_100E0;
  /* 0x100E4/65764 */ unsigned long field_100E4;
  /* 0x100E8/65768 */ unsigned long field_100E8;
  /* 0x100EC/65772 */ unsigned long field_100EC;
  /* 0x100F0/65776 */ unsigned long field_100F0;
  /* 0x100F4/65780 */ unsigned long field_100F4;
  /* 0x100F8/65784 */ unsigned long field_100F8;
  /* 0x100FC/65788 */ unsigned long field_100FC;
  /* 0x10100/65792 */ unsigned long field_10100;
  /* 0x10104/65796 */ unsigned long field_10104;
  /* 0x10108/65800 */ unsigned long field_10108;
  /* 0x1010C/65804 */ unsigned long field_1010C;
  /* 0x10110/65808 */ unsigned long field_10110;
  /* 0x10114/65812 */ unsigned long field_10114;
  /* 0x10118/65816 */ unsigned long field_10118;
  /* 0x1011C/65820 */ unsigned long field_1011C;
  /* 0x10120/65824 */ unsigned long field_10120;
  /* 0x10124/65828 */ unsigned long field_10124;
  /* 0x10128/65832 */ unsigned long field_10128;
  /* 0x1012C/65836 */ unsigned long field_1012C;
  /* 0x10130/65840 */ unsigned long field_10130;
  /* 0x10134/65844 */ unsigned long field_10134;
  /* 0x10138/65848 */ unsigned long field_10138;
  /* 0x1013C/65852 */ unsigned long field_1013C;
  /* 0x10140/65856 */ unsigned long field_10140;
  /* 0x10144/65860 */ unsigned long field_10144;
  /* 0x10148/65864 */ unsigned long field_10148;
  /* 0x1014C/65868 */ unsigned long field_1014C;
  /* 0x10150/65872 */ unsigned long field_10150;
  /* 0x10154/65876 */ unsigned long field_10154;
  /* 0x10158/65880 */ unsigned long LogDots;
  /* 0x1015C/65884 */ unsigned long field_1015C;
};
struct CServerExoAppConfig
{
  /* 0x0/0 */ unsigned long field_0;
  /* 0x4/4 */ unsigned long field_4;
  /* 0x8/8 */ unsigned long field_8;
  /* 0xC/12 */ unsigned long field_C;
  /* 0x10/16 */ unsigned long field_10;
  /* 0x14/20 */ unsigned long field_14;
  /* 0x18/24 */ unsigned long field_18;
  /* 0x1C/28 */ unsigned long field_1C;
  /* 0x20/32 */ unsigned long field_20;
  /* 0x24/36 */ unsigned long field_24;
  /* 0x28/40 */ unsigned long field_28;
  /* 0x2C/44 */ unsigned long field_2C;
  /* 0x30/48 */ unsigned long field_30;
  /* 0x34/52 */ unsigned long field_34;
  /* 0x38/56 */ unsigned long field_38;
  /* 0x3C/60 */ unsigned long field_3C;
  /* 0x40/64 */ unsigned long field_40;
  /* 0x44/68 */ unsigned long field_44;
  /* 0x48/72 */ unsigned long field_48;
  /* 0x4C/76 */ unsigned long field_4C;
  /* 0x50/80 */ unsigned long field_50;
  /* 0x54/84 */ unsigned long field_54;
  /* 0x58/88 */ unsigned long field_58;
  /* 0x5C/92 */ unsigned long field_5C;
  /* 0x60/96 */ unsigned long field_60;
  /* 0x64/100 */ unsigned long field_64;
  /* 0x68/104 */ unsigned long field_68;
  /* 0x6C/108 */ unsigned long field_6C;
  /* 0x70/112 */ unsigned long field_70;
  /* 0x74/116 */ unsigned long field_74;
  /* 0x78/120 */ unsigned long field_78;
  /* 0x7C/124 */ unsigned long field_7C;
  /* 0x80/128 */ unsigned long field_80;
  /* 0x84/132 */ unsigned long field_84;
  /* 0x88/136 */ unsigned long field_88;
  /* 0x8C/140 */ unsigned long field_8C;
  /* 0x90/144 */ unsigned long field_90;
  /* 0x94/148 */ unsigned long field_94;
  /* 0x98/152 */ unsigned long field_98;
  /* 0x9C/156 */ unsigned long field_9C;
  /* 0xA0/160 */ unsigned long field_A0;
  /* 0xA4/164 */ unsigned long field_A4;
  /* 0xA8/168 */ unsigned long field_A8;
  /* 0xAC/172 */ unsigned long field_AC;
  /* 0xB0/176 */ unsigned long field_B0;
  /* 0xB4/180 */ unsigned long field_B4;
  /* 0xB8/184 */ unsigned long AllowLocalChars;
  /* 0xBC/188 */ unsigned long field_BC;
  /* 0xC0/192 */ unsigned long field_C0;
  /* 0xC4/196 */ unsigned long field_C4;
  /* 0xC8/200 */ unsigned long field_C8;
  /* 0xCC/204 */ unsigned long field_CC;
  /* 0xD0/208 */ unsigned long field_D0;
  /* 0xD4/212 */ unsigned long field_D4;
  /* 0xD8/216 */ unsigned long field_D8;
  /* 0xDC/220 */ unsigned long field_DC;
  /* 0xE0/224 */ unsigned long field_E0;
  /* 0xE4/228 */ unsigned long field_E4;
  /* 0xE8/232 */ unsigned long PauseAndPlay;
  /* 0xEC/236 */ unsigned long OneParty;
  /* 0xF0/240 */ unsigned long EnforceLegalCharacters;
  /* 0xF4/244 */ unsigned long ItemLevelRestrictions;
  /* 0xF8/248 */ unsigned long CDBannedBehavior;
  /* 0xFC/252 */ unsigned long DisallowShouting;
  /* 0x100/256 */ unsigned long ShowDMJoinedMessage;
  /* 0x104/260 */ unsigned long BackupSavedChars;
  /* 0x108/264 */ unsigned long AutoFail1;
  /* 0x10C/268 */ unsigned long ValidateSpells;
  /* 0x110/272 */ unsigned long ExamineEffects;
  /* 0x114/276 */ unsigned long ExamineCR;
  /* 0x118/280 */ unsigned long MaxHP;
  /* 0x11C/284 */ unsigned long RestoreSpellUses;
  /* 0x120/288 */ unsigned long ResetEncounterSpawns;
  /* 0x124/292 */ unsigned long HideHPGained;
  /* 0x128/296 */ unsigned long field_128;
  /* 0x12C/300 */ unsigned long field_12C;
  /* 0x130/304 */ unsigned long field_130;
  /* 0x134/308 */ unsigned long field_134;
  /* 0x138/312 */ unsigned long field_138;
  /* 0x13C/316 */ unsigned long field_13C;
  /* 0x140/320 */ unsigned long field_140;
  /* 0x144/324 */ unsigned long field_144;
  /* 0x148/328 */ unsigned long field_148;
  /* 0x14C/332 */ unsigned long VaultCharsOnly;
  /* 0x150/336 */ unsigned long SaveCharsInSaveGame;
  /* 0x154/340 */ unsigned long SuppressBaseServervault;
  /* 0x158/344 */ unsigned long GameRegion;
  /* 0x15C/348 */ unsigned long field_15C;
  /* 0x160/352 */ unsigned long field_160;
  /* 0x164/356 */ unsigned long field_164;
};

struct CExoString
{
  /* 0x0/0 */ char *Text;
  /* 0x4/4 */ unsigned long Length;
};
struct CResRef
{
  /* 0x0/0 */ char ResRef[16];
};
struct CExoArrayList
{
  /* 0x0/0 */ void *Array;
  /* 0x4/4 */ unsigned long Count;
};
struct CExoLinkedList_st
{
  /* 0x0/0 */ void *Header;
  /* 0x4/4 */ unsigned long Count;
};
struct CExoLocString_st
{
  /* 0x0/0 */ CExoLinkedList_st List;
};
struct CNWSObject
{
  /* 0x0/0 */ unsigned long field_0;
  /* 0x4/4 */ unsigned long ObjectID;
  /* 0x8/8 */ char ObjectType;
  /* 0x9/9 */ char field_9;
  /* 0xA/10 */ char field_A;
  /* 0xB/11 */ char field_B;
  /* 0xC/12 */ void *Methods;
  /* 0x10/16 */ CExoLocString_st LocString;
  /* 0x18/24 */ unsigned short field_18;
  /* 0x1A/26 */ unsigned short field_1A;
  /* 0x1C/28 */ void *vtbl2;
  /* 0x24/36 */ char rsvd1[4];
  /* 0x24/36 */ CExoString ResRef;
  /* 0x2C/44 */ CResRef field_2C;
  /* 0x40/64 */ char rsvd2[4];
  /* 0x40/64 */ unsigned long Dialog;
  /* 0x58/88 */ char rsvd3[20];
  /* 0x58/88 */ unsigned long ConversationWith;
  /* 0x5C/92 */ unsigned long InConversation;
  /* 0x60/96 */ unsigned long AILevel;
  /* 0x70/112 */ char rsvd4[12];
  /* 0x70/112 */ unsigned long field_70;
  /* 0x78/120 */ char rsvd5[4];
  /* 0x78/120 */ unsigned long AreaID;
  /* 0x7C/124 */ unsigned long X;
  /* 0x80/128 */ unsigned long Y;
  /* 0x84/132 */ unsigned long Z;
  /* 0x8C/140 */ char rsvd6[4];
  /* 0x8C/140 */ unsigned long field_8C;
  /* 0xB8/184 */ char rsvd7[40];
  /* 0xB8/184 */ unsigned long field_B8;
  /* 0xBC/188 */ unsigned short field_BC;
  /* 0xC4/196 */ char rsvd8[6];
  /* 0xC4/196 */ unsigned long CanModifyActionQueue;
  /* 0xC8/200 */ unsigned long IsDestroyable;
  /* 0xCC/204 */ unsigned long IsRaiseable;
  /* 0xD0/208 */ unsigned long DeadSelectable;
  /* 0xD4/212 */ unsigned long Invulnerable;
  /* 0xEC/236 */ char rsvd9[20];
  /* 0xEC/236 */ void *Effects;
  /* 0xF8/248 */ char rsvd10[8];
  /* 0xF8/248 */ unsigned long PerceptionList;
  /* 0x1C0/448 */ char rsvd11[196];
  /* 0x1C0/448 */ unsigned long field_1C0;
};
struct CNWSCreature
{
  /* 0x0/0 */ CNWSObject Object;
  /* 0x1C4/452 */ unsigned long field_1C4;
  /* 0x1F8/504 */ char rsvd1[48];
  /* 0x1F8/504 */ CExoString HeartbeatScript;
  /* 0x200/512 */ CExoString PerceptionScript;
  /* 0x208/520 */ CExoString SpellCastAtScript;
  /* 0x210/528 */ CExoString AttackedScript;
  /* 0x218/536 */ CExoString DamagedScript;
  /* 0x220/544 */ CExoString DisturbedScript;
  /* 0x228/552 */ CExoString EndCombatScript;
  /* 0x230/560 */ CExoString ConversationScript;
  /* 0x238/568 */ CExoString SpawnScript;
  /* 0x240/576 */ CExoString RestedScript;
  /* 0x248/584 */ CExoString DeathScript;
  /* 0x250/592 */ CExoString UserDefScript;
  /* 0x258/600 */ CExoString BlockedScript;
  /* 0x260/608 */ CExoString HeartbeatScript_;
  /* 0x268/616 */ CExoString PerceptionScript_;
  /* 0x270/624 */ CExoString SpellCastAtScript_;
  /* 0x278/632 */ CExoString AttackedScript_;
  /* 0x280/640 */ CExoString DamagedScript_;
  /* 0x288/648 */ CExoString EndCombatScript_;
  /* 0x290/656 */ CExoString ConversationScript_;
  /* 0x298/664 */ CExoString SpawnScript_;
  /* 0x2A0/672 */ CExoString RestedScript_;
  /* 0x2A8/680 */ CExoString DeathScript_;
  /* 0x2B0/688 */ CExoString UserDefScript_;
  /* 0x2B8/696 */ CExoString BlockedScript_;
  /* 0x2CC/716 */ char rsvd2[12];
  /* 0x2CC/716 */ unsigned long Lootable;
  /* 0x2D0/720 */ unsigned long DecayTime;
  /* 0x2D4/724 */ unsigned long BodyBagID;
  /* 0x304/772 */ char rsvd3[44];
  /* 0x304/772 */ unsigned long BlockedBy;
  /* 0x308/776 */ unsigned long PositionStruct;
  /* 0x488/1160 */ char rsvd4[380];
  /* 0x488/1160 */ unsigned long LawfulChaotic;
  /* 0x48C/1164 */ unsigned long field_48C;
  /* 0x490/1168 */ void *AreaMiniMaps;
  /* 0x494/1172 */ CExoArrayList AreaList;
  /* 0x49C/1180 */ unsigned long field_49C;
  /* 0x4A0/1184 */ unsigned long AreaCount;
  /* 0x4A4/1188 */ unsigned long field_4A4;
  /* 0x4B0/1200 */ char rsvd5[8];
  /* 0x4B0/1200 */ unsigned long field_4B0;
  /* 0x4C0/1216 */ char rsvd6[12];
  /* 0x4C0/1216 */ unsigned long InCombat;
  /* 0x4D4/1236 */ char rsvd7[16];
  /* 0x4D4/1236 */ unsigned long Disarmable;
  /* 0x4D8/1240 */ unsigned long CreatureSize;
  /* 0x4E4/1252 */ char rsvd8[8];
  /* 0x4E4/1252 */ unsigned long AttackTarget;
  /* 0x4E8/1256 */ unsigned long AttemptedAttackTarget;
  /* 0x4F8/1272 */ char rsvd9[12];
  /* 0x4F8/1272 */ unsigned long Attacker;
  /* 0x4FC/1276 */ unsigned long SpellTarget;
  /* 0x9E1/2529 */ char rsvd10[1249];
  /* 0x9E1/2529 */ char field_9E1;
  /* 0x9E2/2530 */ char field_9E2;
  /* 0xA18/2584 */ char rsvd11[53];
  /* 0xA18/2584 */ unsigned long PrimaryRange;
  /* 0xA20/2592 */ char rsvd12[4];
  /* 0xA20/2592 */ unsigned long SecondaryRange;
  /* 0xA84/2692 */ char rsvd13[96];
  /* 0xA84/2692 */ unsigned long field_A84;
  /* 0xA88/2696 */ unsigned long Spotted;
  /* 0xA8C/2700 */ unsigned long field_A8C[15];
  /* 0xAC8/2760 */ unsigned long field_AC8;
  /* 0xACC/2764 */ void *CombatRound;
  /* 0xAD0/2768 */ unsigned long field_AD0;
  /* 0xAD4/2772 */ void *Barter;
  /* 0xAD8/2776 */ unsigned long Gold;
  /* 0xADC/2780 */ unsigned long IsPC;
  /* 0xAE0/2784 */ unsigned long SoundSetFile;
  /* 0xAE4/2788 */ unsigned long FootstepType;
  /* 0xAE8/2792 */ unsigned long BodyBag;
  /* 0xAEC/2796 */ unsigned long field_AEC;
  /* 0xAF8/2808 */ char rsvd14[8];
  /* 0xAF8/2808 */ unsigned long IsImmortal;
  /* 0xAFC/2812 */ unsigned long field_AFC[4];
  /* 0xB0C/2828 */ unsigned short field_B0C;
  /* 0xB0E/2830 */ char AIState;
  /* 0xB10/2832 */ char rsvd15;
  /* 0xB10/2832 */ unsigned long field_B10;
  /* 0xB34/2868 */ char rsvd16[32];
  /* 0xB34/2868 */ unsigned long field_B34;
  /* 0xB5C/2908 */ char rsvd17[36];
  /* 0xB5C/2908 */ void *ReputationInformation;
  /* 0xB70/2928 */ char rsvd18[16];
  /* 0xB70/2928 */ void *Inventory;
  /* 0xB74/2932 */ void *ItemRepository;
  /* 0xB78/2936 */ unsigned short field_B78;
  /* 0xB7A/2938 */ unsigned short field_B7A;
  /* 0xB7C/2940 */ unsigned long field_B7C;
  /* 0xB80/2944 */ unsigned long field_B80[42];
  /* 0xC28/3112 */ unsigned long CNWSCreatureAppearanceInfo[16];
  /* 0xC68/3176 */ void *CNWSCreatureStats;
  /* 0xC6C/3180 */ unsigned long field_C6C;
  /* 0xC70/3184 */ unsigned long field_C70;
};


#endif
