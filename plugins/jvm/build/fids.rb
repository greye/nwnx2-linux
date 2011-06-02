FIDS = {
  'random' => 0,
  'printString' => 1,
  'printFloat' => 2,
  'floatToString' => 3,
  'printInteger' => 4,
  'printObject' => 5,
  'assignCommand' => 6,
  'delayCommand' => 7,
  'executeScript' => 8,
  'clearAllActions' => 9,
  'setFacing' => 10,
  'setCalendar' => 11,
  'setTime' => 12,
  'getCalendarYear' => 13,
  'getCalendarMonth' => 14,
  'getCalendarDay' => 15,
  'getTimeHour' => 16,
  'getTimeMinute' => 17,
  'getTimeSecond' => 18,
  'getTimeMillisecond' => 19,
  'actionRandomWalk' => 20,
  'actionMoveToLocation' => 21,
  'actionMoveToObject' => 22,
  'actionMoveAwayFromObject' => 23,
  'getArea' => 24,
  'getEnteringObject' => 25,
  'getExitingObject' => 26,
  'getPosition' => 27,
  'getFacing' => 28,
  'getItemPossessor' => 29,
  'getItemPossessedBy' => 30,
  'createItemOnObject' => 31,
  'actionEquipItem' => 32,
  'actionUnequipItem' => 33,
  'actionPickUpItem' => 34,
  'actionPutDownItem' => 35,
  'getLastAttacker' => 36,
  'actionAttack' => 37,
  'getNearestCreature' => 38,
  'actionSpeakString' => 39,
  'actionPlayAnimation' => 40,
  'getDistanceToObject' => 41,
  'getIsObjectValid' => 42,
  'actionOpenDoor' => 43,
  'actionCloseDoor' => 44,
  'setCameraFacing' => 45,
  'playSound' => 46,
  'getSpellTargetObject' => 47,
  'actionCastSpellAtObject' => 48,
  'getCurrentHitPoints' => 49,
  'getMaxHitPoints' => 50,
  'getLocalInt' => 51,
  'getLocalFloat' => 52,
  'getLocalString' => 53,
  'getLocalObject' => 54,
  'setLocalInt' => 55,
  'setLocalFloat' => 56,
  'setLocalString' => 57,
  'setLocalObject' => 58,
  'getStringLength' => 59,
  'getStringUpperCase' => 60,
  'getStringLowerCase' => 61,
  'getStringRight' => 62,
  'getStringLeft' => 63,
  'insertString' => 64,
  'getSubString' => 65,
  'findSubString' => 66,
  'fabs' => 67,
  'cos' => 68,
  'sin' => 69,
  'tan' => 70,
  'acos' => 71,
  'asin' => 72,
  'atan' => 73,
  'log' => 74,
  'pow' => 75,
  'sqrt' => 76,
  'abs' => 77,
  'effectHeal' => 78,
  'effectDamage' => 79,
  'effectAbilityIncrease' => 80,
  'effectDamageResistance' => 81,
  'effectResurrection' => 82,
  'effectSummonCreature' => 83,
  'getCasterLevel' => 84,
  'getFirstEffect' => 85,
  'getNextEffect' => 86,
  'removeEffect' => 87,
  'getIsEffectValid' => 88,
  'getEffectDurationType' => 89,
  'getEffectSubType' => 90,
  'getEffectCreator' => 91,
  'intToString' => 92,
  'getFirstObjectInArea' => 93,
  'getNextObjectInArea' => 94,
  'd2' => 95,
  'd3' => 96,
  'd4' => 97,
  'd6' => 98,
  'd8' => 99,
  'd10' => 100,
  'd12' => 101,
  'd20' => 102,
  'd100' => 103,
  'vectorMagnitude' => 104,
  'getMetaMagicFeat' => 105,
  'getObjectType' => 106,
  'getRacialType' => 107,
  'fortitudeSave' => 108,
  'reflexSave' => 109,
  'willSave' => 110,
  'getSpellSaveDC' => 111,
  'magicalEffect' => 112,
  'supernaturalEffect' => 113,
  'extraordinaryEffect' => 114,
  'effectACIncrease' => 115,
  'getAC' => 116,
  'effectSavingThrowIncrease' => 117,
  'effectAttackIncrease' => 118,
  'effectDamageReduction' => 119,
  'effectDamageIncrease' => 120,
  'roundsToSeconds' => 121,
  'hoursToSeconds' => 122,
  'turnsToSeconds' => 123,
  'getLawChaosValue' => 124,
  'getGoodEvilValue' => 125,
  'getAlignmentLawChaos' => 126,
  'getAlignmentGoodEvil' => 127,
  'getFirstObjectInShape' => 128,
  'getNextObjectInShape' => 129,
  'effectEntangle' => 130,
  'signalEvent' => 131,
  'eventUserDefined' => 132,
  'effectDeath' => 133,
  'effectKnockdown' => 134,
  'actionGiveItem' => 135,
  'actionTakeItem' => 136,
  'vectorNormalize' => 137,
  'effectCurse' => 138,
  'getAbilityScore' => 139,
  'getIsDead' => 140,
  'printVector' => 141,
  'vector' => 142,
  'setFacingPoint' => 143,
  'angleToVector' => 144,
  'vectorToAngle' => 145,
  'touchAttackMelee' => 146,
  'touchAttackRanged' => 147,
  'effectParalyze' => 148,
  'effectSpellImmunity' => 149,
  'effectDeaf' => 150,
  'getDistanceBetween' => 151,
  'setLocalLocation' => 152,
  'getLocalLocation' => 153,
  'effectSleep' => 154,
  'getItemInSlot' => 155,
  'effectCharmed' => 156,
  'effectConfused' => 157,
  'effectFrightened' => 158,
  'effectDominated' => 159,
  'effectDazed' => 160,
  'effectStunned' => 161,
  'setCommandable' => 162,
  'getCommandable' => 163,
  'effectRegenerate' => 164,
  'effectMovementSpeedIncrease' => 165,
  'getHitDice' => 166,
  'actionForceFollowObject' => 167,
  'getTag' => 168,
  'resistSpell' => 169,
  'getEffectType' => 170,
  'effectAreaOfEffect' => 171,
  'getFactionEqual' => 172,
  'changeFaction' => 173,
  'getIsListening' => 174,
  'setListening' => 175,
  'setListenPattern' => 176,
  'testStringAgainstPattern' => 177,
  'getMatchedSubstring' => 178,
  'getMatchedSubstringsCount' => 179,
  'effectVisualEffect' => 180,
  'getFactionWeakestMember' => 181,
  'getFactionStrongestMember' => 182,
  'getFactionMostDamagedMember' => 183,
  'getFactionLeastDamagedMember' => 184,
  'getFactionGold' => 185,
  'getFactionAverageReputation' => 186,
  'getFactionAverageGoodEvilAlignment' => 187,
  'getFactionAverageLawChaosAlignment' => 188,
  'getFactionAverageLevel' => 189,
  'getFactionAverageXP' => 190,
  'getFactionMostFrequentClass' => 191,
  'getFactionWorstAC' => 192,
  'getFactionBestAC' => 193,
  'actionSit' => 194,
  'getListenPatternNumber' => 195,
  'actionJumpToObject' => 196,
  'getWaypointByTag' => 197,
  'getTransitionTarget' => 198,
  'effectLinkEffects' => 199,
  'getObjectByTag' => 200,
  'adjustAlignment' => 201,
  'actionWait' => 202,
  'setAreaTransitionBMP' => 203,
  'actionStartConversation' => 204,
  'actionPauseConversation' => 205,
  'actionResumeConversation' => 206,
  'effectBeam' => 207,
  'getReputation' => 208,
  'adjustReputation' => 209,
  'getSittingCreature' => 210,
  'getGoingToBeAttackedBy' => 211,
  'effectSpellResistanceIncrease' => 212,
  'getLocation' => 213,
  'actionJumpToLocation' => 214,
  'location' => 215,
  'applyEffectAtLocation' => 216,
  'getIsPC' => 217,
  'feetToMeters' => 218,
  'yardsToMeters' => 219,
  'applyEffectToObject' => 220,
  'speakString' => 221,
  'getSpellTargetLocation' => 222,
  'getPositionFromLocation' => 223,
  'getAreaFromLocation' => 224,
  'getFacingFromLocation' => 225,
  'getNearestCreatureToLocation' => 226,
  'getNearestObject' => 227,
  'getNearestObjectToLocation' => 228,
  'getNearestObjectByTag' => 229,
  'intToFloat' => 230,
  'floatToInt' => 231,
  'stringToInt' => 232,
  'stringToFloat' => 233,
  'actionCastSpellAtLocation' => 234,
  'getIsEnemy' => 235,
  'getIsFriend' => 236,
  'getIsNeutral' => 237,
  'getPCSpeaker' => 238,
  'getStringByStrRef' => 239,
  'actionSpeakStringByStrRef' => 240,
  'destroyObject' => 241,
  'getModule' => 242,
  'createObject' => 243,
  'eventSpellCastAt' => 244,
  'getLastSpellCaster' => 245,
  'getLastSpell' => 246,
  'getUserDefinedEventNumber' => 247,
  'getSpellId' => 248,
  'randomName' => 249,
  'effectPoison' => 250,
  'effectDisease' => 251,
  'effectSilence' => 252,
  'getName' => 253,
  'getLastSpeaker' => 254,
  'beginConversation' => 255,
  'getLastPerceived' => 256,
  'getLastPerceptionHeard' => 257,
  'getLastPerceptionInaudible' => 258,
  'getLastPerceptionSeen' => 259,
  'getLastClosedBy' => 260,
  'getLastPerceptionVanished' => 261,
  'getFirstInPersistentObject' => 262,
  'getNextInPersistentObject' => 263,
  'getAreaOfEffectCreator' => 264,
  'deleteLocalInt' => 265,
  'deleteLocalFloat' => 266,
  'deleteLocalString' => 267,
  'deleteLocalObject' => 268,
  'deleteLocalLocation' => 269,
  'effectHaste' => 270,
  'effectSlow' => 271,
  'objectToString' => 272,
  'effectImmunity' => 273,
  'getIsImmune' => 274,
  'effectDamageImmunityIncrease' => 275,
  'getEncounterActive' => 276,
  'setEncounterActive' => 277,
  'getEncounterSpawnsMax' => 278,
  'setEncounterSpawnsMax' => 279,
  'getEncounterSpawnsCurrent' => 280,
  'setEncounterSpawnsCurrent' => 281,
  'getModuleItemAcquired' => 282,
  'getModuleItemAcquiredFrom' => 283,
  'setCustomToken' => 284,
  'getHasFeat' => 285,
  'getHasSkill' => 286,
  'actionUseFeat' => 287,
  'actionUseSkill' => 288,
  'getObjectSeen' => 289,
  'getObjectHeard' => 290,
  'getLastPlayerDied' => 291,
  'getModuleItemLost' => 292,
  'getModuleItemLostBy' => 293,
  'actionDoCommand' => 294,
  'eventConversation' => 295,
  'setEncounterDifficulty' => 296,
  'getEncounterDifficulty' => 297,
  'getDistanceBetweenLocations' => 298,
  'getReflexAdjustedDamage' => 299,
  'playAnimation' => 300,
  'talentSpell' => 301,
  'talentFeat' => 302,
  'talentSkill' => 303,
  'getHasSpellEffect' => 304,
  'getEffectSpellId' => 305,
  'getCreatureHasTalent' => 306,
  'getCreatureTalentRandom' => 307,
  'getCreatureTalentBest' => 308,
  'actionUseTalentOnObject' => 309,
  'actionUseTalentAtLocation' => 310,
  'getGoldPieceValue' => 311,
  'getIsPlayableRacialType' => 312,
  'jumpToLocation' => 313,
  'effectTemporaryHitpoints' => 314,
  'getSkillRank' => 315,
  'getAttackTarget' => 316,
  'getLastAttackType' => 317,
  'getLastAttackMode' => 318,
  'getMaster' => 319,
  'getIsInCombat' => 320,
  'getLastAssociateCommand' => 321,
  'giveGoldToCreature' => 322,
  'setIsDestroyable' => 323,
  'setLocked' => 324,
  'getLocked' => 325,
  'getClickingObject' => 326,
  'setAssociateListenPatterns' => 327,
  'getLastWeaponUsed' => 328,
  'actionInteractObject' => 329,
  'getLastUsedBy' => 330,
  'getAbilityModifier' => 331,
  'getIdentified' => 332,
  'setIdentified' => 333,
  'summonAnimalCompanion' => 334,
  'summonFamiliar' => 335,
  'getBlockingDoor' => 336,
  'getIsDoorActionPossible' => 337,
  'doDoorAction' => 338,
  'getFirstItemInInventory' => 339,
  'getNextItemInInventory' => 340,
  'getClassByPosition' => 341,
  'getLevelByPosition' => 342,
  'getLevelByClass' => 343,
  'getDamageDealtByType' => 344,
  'getTotalDamageDealt' => 345,
  'getLastDamager' => 346,
  'getLastDisarmed' => 347,
  'getLastDisturbed' => 348,
  'getLastLocked' => 349,
  'getLastUnlocked' => 350,
  'effectSkillIncrease' => 351,
  'getInventoryDisturbType' => 352,
  'getInventoryDisturbItem' => 353,
  'getHenchman' => 354,
  'versusAlignmentEffect' => 355,
  'versusRacialTypeEffect' => 356,
  'versusTrapEffect' => 357,
  'getGender' => 358,
  'getIsTalentValid' => 359,
  'actionMoveAwayFromLocation' => 360,
  'getAttemptedAttackTarget' => 361,
  'getTypeFromTalent' => 362,
  'getIdFromTalent' => 363,
  'getAssociate' => 364,
  'addHenchman' => 365,
  'removeHenchman' => 366,
  'addJournalQuestEntry' => 367,
  'removeJournalQuestEntry' => 368,
  'getPCPublicCDKey' => 369,
  'getPCIPAddress' => 370,
  'getPCPlayerName' => 371,
  'setPCLike' => 372,
  'setPCDislike' => 373,
  'sendMessageToPC' => 374,
  'getAttemptedSpellTarget' => 375,
  'getLastOpenedBy' => 376,
  'getHasSpell' => 377,
  'openStore' => 378,
  'effectTurned' => 379,
  'getFirstFactionMember' => 380,
  'getNextFactionMember' => 381,
  'actionForceMoveToLocation' => 382,
  'actionForceMoveToObject' => 383,
  'getJournalQuestExperience' => 384,
  'jumpToObject' => 385,
  'setMapPinEnabled' => 386,
  'effectHitPointChangeWhenDying' => 387,
  'popUpGUIPanel' => 388,
  'clearPersonalReputation' => 389,
  'setIsTemporaryFriend' => 390,
  'setIsTemporaryEnemy' => 391,
  'setIsTemporaryNeutral' => 392,
  'giveXPToCreature' => 393,
  'setXP' => 394,
  'getXP' => 395,
  'intToHexString' => 396,
  'getBaseItemType' => 397,
  'getItemHasItemProperty' => 398,
  'actionEquipMostDamagingMelee' => 399,
  'actionEquipMostDamagingRanged' => 400,
  'getItemACValue' => 401,
  'actionRest' => 402,
  'exploreAreaForPlayer' => 403,
  'actionEquipMostEffectiveArmor' => 404,
  'getIsDay' => 405,
  'getIsNight' => 406,
  'getIsDawn' => 407,
  'getIsDusk' => 408,
  'getIsEncounterCreature' => 409,
  'getLastPlayerDying' => 410,
  'getStartingLocation' => 411,
  'changeToStandardFaction' => 412,
  'soundObjectPlay' => 413,
  'soundObjectStop' => 414,
  'soundObjectSetVolume' => 415,
  'soundObjectSetPosition' => 416,
  'speakOneLinerConversation' => 417,
  'getGold' => 418,
  'getLastRespawnButtonPresser' => 419,
  'getIsDM' => 420,
  'playVoiceChat' => 421,
  'getIsWeaponEffective' => 422,
  'getLastSpellHarmful' => 423,
  'eventActivateItem' => 424,
  'musicBackgroundPlay' => 425,
  'musicBackgroundStop' => 426,
  'musicBackgroundSetDelay' => 427,
  'musicBackgroundChangeDay' => 428,
  'musicBackgroundChangeNight' => 429,
  'musicBattlePlay' => 430,
  'musicBattleStop' => 431,
  'musicBattleChange' => 432,
  'ambientSoundPlay' => 433,
  'ambientSoundStop' => 434,
  'ambientSoundChangeDay' => 435,
  'ambientSoundChangeNight' => 436,
  'getLastKiller' => 437,
  'getSpellCastItem' => 438,
  'getItemActivated' => 439,
  'getItemActivator' => 440,
  'getItemActivatedTargetLocation' => 441,
  'getItemActivatedTarget' => 442,
  'getIsOpen' => 443,
  'takeGoldFromCreature' => 444,
  'isInConversation' => 445,
  'effectAbilityDecrease' => 446,
  'effectAttackDecrease' => 447,
  'effectDamageDecrease' => 448,
  'effectDamageImmunityDecrease' => 449,
  'effectACDecrease' => 450,
  'effectMovementSpeedDecrease' => 451,
  'effectSavingThrowDecrease' => 452,
  'effectSkillDecrease' => 453,
  'effectSpellResistanceDecrease' => 454,
  'getPlotFlag' => 455,
  'setPlotFlag' => 456,
  'effectInvisibility' => 457,
  'effectConcealment' => 458,
  'effectDarkness' => 459,
  'effectDispelMagicAll' => 460,
  'effectUltravision' => 461,
  'effectNegativeLevel' => 462,
  'effectPolymorph' => 463,
  'effectSanctuary' => 464,
  'effectTrueSeeing' => 465,
  'effectSeeInvisible' => 466,
  'effectTimeStop' => 467,
  'effectBlindness' => 468,
  'getIsReactionTypeFriendly' => 469,
  'getIsReactionTypeNeutral' => 470,
  'getIsReactionTypeHostile' => 471,
  'effectSpellLevelAbsorption' => 472,
  'effectDispelMagicBest' => 473,
  'activatePortal' => 474,
  'getNumStackedItems' => 475,
  'surrenderToEnemies' => 476,
  'effectMissChance' => 477,
  'getTurnResistanceHD' => 478,
  'getCreatureSize' => 479,
  'effectDisappearAppear' => 480,
  'effectDisappear' => 481,
  'effectAppear' => 482,
  'actionUnlockObject' => 483,
  'actionLockObject' => 484,
  'effectModifyAttacks' => 485,
  'getLastTrapDetected' => 486,
  'effectDamageShield' => 487,
  'getNearestTrapToObject' => 488,
  'getDeity' => 489,
  'getSubRace' => 490,
  'getFortitudeSavingThrow' => 491,
  'getWillSavingThrow' => 492,
  'getReflexSavingThrow' => 493,
  'getChallengeRating' => 494,
  'getAge' => 495,
  'getMovementRate' => 496,
  'getFamiliarCreatureType' => 497,
  'getAnimalCompanionCreatureType' => 498,
  'getFamiliarName' => 499,
  'getAnimalCompanionName' => 500,
  'actionCastFakeSpellAtObject' => 501,
  'actionCastFakeSpellAtLocation' => 502,
  'removeSummonedAssociate' => 503,
  'setCameraMode' => 504,
  'getIsResting' => 505,
  'getLastPCRested' => 506,
  'setWeather' => 507,
  'getLastRestEventType' => 508,
  'startNewModule' => 509,
  'effectSwarm' => 510,
  'getWeaponRanged' => 511,
  'doSinglePlayerAutoSave' => 512,
  'getGameDifficulty' => 513,
  'setTileMainLightColor' => 514,
  'setTileSourceLightColor' => 515,
  'recomputeStaticLighting' => 516,
  'getTileMainLight1Color' => 517,
  'getTileMainLight2Color' => 518,
  'getTileSourceLight1Color' => 519,
  'getTileSourceLight2Color' => 520,
  'setPanelButtonFlash' => 521,
  'getCurrentAction' => 522,
  'setStandardFactionReputation' => 523,
  'getStandardFactionReputation' => 524,
  'floatingTextStrRefOnCreature' => 525,
  'floatingTextStringOnCreature' => 526,
  'getTrapDisarmable' => 527,
  'getTrapDetectable' => 528,
  'getTrapDetectedBy' => 529,
  'getTrapFlagged' => 530,
  'getTrapBaseType' => 531,
  'getTrapOneShot' => 532,
  'getTrapCreator' => 533,
  'getTrapKeyTag' => 534,
  'getTrapDisarmDC' => 535,
  'getTrapDetectDC' => 536,
  'getLockKeyRequired' => 537,
  'getLockKeyTag' => 538,
  'getLockLockable' => 539,
  'getLockUnlockDC' => 540,
  'getLockLockDC' => 541,
  'getPCLevellingUp' => 542,
  'getHasFeatEffect' => 543,
  'setPlaceableIllumination' => 544,
  'getPlaceableIllumination' => 545,
  'getIsPlaceableObjectActionPossible' => 546,
  'doPlaceableObjectAction' => 547,
  'getFirstPC' => 548,
  'getNextPC' => 549,
  'setTrapDetectedBy' => 550,
  'getIsTrapped' => 551,
  'effectTurnResistanceDecrease' => 552,
  'effectTurnResistanceIncrease' => 553,
  'popUpDeathGUIPanel' => 554,
  'setTrapDisabled' => 555,
  'getLastHostileActor' => 556,
  'exportAllCharacters' => 557,
  'musicBackgroundGetDayTrack' => 558,
  'musicBackgroundGetNightTrack' => 559,
  'writeTimestampedLogEntry' => 560,
  'getModuleName' => 561,
  'getFactionLeader' => 562,
  'sendMessageToAllDMs' => 563,
  'endGame' => 564,
  'bootPC' => 565,
  'actionCounterSpell' => 566,
  'ambientSoundSetDayVolume' => 567,
  'ambientSoundSetNightVolume' => 568,
  'musicBackgroundGetBattleTrack' => 569,
  'getHasInventory' => 570,
  'getStrRefSoundDuration' => 571,
  'addToParty' => 572,
  'removeFromParty' => 573,
  'getStealthMode' => 574,
  'getDetectMode' => 575,
  'getDefensiveCastingMode' => 576,
  'getAppearanceType' => 577,
  'spawnScriptDebugger' => 578,
  'getModuleItemAcquiredStackSize' => 579,
  'decrementRemainingFeatUses' => 580,
  'decrementRemainingSpellUses' => 581,
  'getResRef' => 582,
  'effectPetrify' => 583,
  'copyItem' => 584,
  'effectCutsceneParalyze' => 585,
  'getDroppableFlag' => 586,
  'getUseableFlag' => 587,
  'getStolenFlag' => 588,
  'setCampaignFloat' => 589,
  'setCampaignInt' => 590,
  'setCampaignVector' => 591,
  'setCampaignLocation' => 592,
  'setCampaignString' => 593,
  'destroyCampaignDatabase' => 594,
  'getCampaignFloat' => 595,
  'getCampaignInt' => 596,
  'getCampaignVector' => 597,
  'getCampaignLocation' => 598,
  'getCampaignString' => 599,
  'copyObject' => 600,
  'deleteCampaignVariable' => 601,
  'storeCampaignObject' => 602,
  'retrieveCampaignObject' => 603,
  'effectCutsceneDominated' => 604,
  'getItemStackSize' => 605,
  'setItemStackSize' => 606,
  'getItemCharges' => 607,
  'setItemCharges' => 608,
  'addItemProperty' => 609,
  'removeItemProperty' => 610,
  'getIsItemPropertyValid' => 611,
  'getFirstItemProperty' => 612,
  'getNextItemProperty' => 613,
  'getItemPropertyType' => 614,
  'getItemPropertyDurationType' => 615,
  'itemPropertyAbilityBonus' => 616,
  'itemPropertyACBonus' => 617,
  'itemPropertyACBonusVsAlign' => 618,
  'itemPropertyACBonusVsDmgType' => 619,
  'itemPropertyACBonusVsRace' => 620,
  'itemPropertyACBonusVsSAlign' => 621,
  'itemPropertyEnhancementBonus' => 622,
  'itemPropertyEnhancementBonusVsAlign' => 623,
  'itemPropertyEnhancementBonusVsRace' => 624,
  'itemPropertyEnhancementBonusVsSAlign' => 625,
  'itemPropertyEnhancementPenalty' => 626,
  'itemPropertyWeightReduction' => 627,
  'itemPropertyBonusFeat' => 628,
  'itemPropertyBonusLevelSpell' => 629,
  'itemPropertyCastSpell' => 630,
  'itemPropertyDamageBonus' => 631,
  'itemPropertyDamageBonusVsAlign' => 632,
  'itemPropertyDamageBonusVsRace' => 633,
  'itemPropertyDamageBonusVsSAlign' => 634,
  'itemPropertyDamageImmunity' => 635,
  'itemPropertyDamagePenalty' => 636,
  'itemPropertyDamageReduction' => 637,
  'itemPropertyDamageResistance' => 638,
  'itemPropertyDamageVulnerability' => 639,
  'itemPropertyDarkvision' => 640,
  'itemPropertyDecreaseAbility' => 641,
  'itemPropertyDecreaseAC' => 642,
  'itemPropertyDecreaseSkill' => 643,
  'itemPropertyContainerReducedWeight' => 644,
  'itemPropertyExtraMeleeDamageType' => 645,
  'itemPropertyExtraRangeDamageType' => 646,
  'itemPropertyHaste' => 647,
  'itemPropertyHolyAvenger' => 648,
  'itemPropertyImmunityMisc' => 649,
  'itemPropertyImprovedEvasion' => 650,
  'itemPropertyBonusSpellResistance' => 651,
  'itemPropertyBonusSavingThrowVsX' => 652,
  'itemPropertyBonusSavingThrow' => 653,
  'itemPropertyKeen' => 654,
  'itemPropertyLight' => 655,
  'itemPropertyMaxRangeStrengthMod' => 656,
  'itemPropertyNoDamage' => 657,
  'itemPropertyOnHitProps' => 658,
  'itemPropertyReducedSavingThrowVsX' => 659,
  'itemPropertyReducedSavingThrow' => 660,
  'itemPropertyRegeneration' => 661,
  'itemPropertySkillBonus' => 662,
  'itemPropertySpellImmunitySpecific' => 663,
  'itemPropertySpellImmunitySchool' => 664,
  'itemPropertyThievesTools' => 665,
  'itemPropertyAttackBonus' => 666,
  'itemPropertyAttackBonusVsAlign' => 667,
  'itemPropertyAttackBonusVsRace' => 668,
  'itemPropertyAttackBonusVsSAlign' => 669,
  'itemPropertyAttackPenalty' => 670,
  'itemPropertyUnlimitedAmmo' => 671,
  'itemPropertyLimitUseByAlign' => 672,
  'itemPropertyLimitUseByClass' => 673,
  'itemPropertyLimitUseByRace' => 674,
  'itemPropertyLimitUseBySAlign' => 675,
  'badBadReplaceMeThisDoesNothing' => 676,
  'itemPropertyVampiricRegeneration' => 677,
  'itemPropertyTrap' => 678,
  'itemPropertyTrueSeeing' => 679,
  'itemPropertyOnMonsterHitProperties' => 680,
  'itemPropertyTurnResistance' => 681,
  'itemPropertyMassiveCritical' => 682,
  'itemPropertyFreeAction' => 683,
  'itemPropertyMonsterDamage' => 684,
  'itemPropertyImmunityToSpellLevel' => 685,
  'itemPropertySpecialWalk' => 686,
  'itemPropertyHealersKit' => 687,
  'itemPropertyWeightIncrease' => 688,
  'getIsSkillSuccessful' => 689,
  'effectSpellFailure' => 690,
  'speakStringByStrRef' => 691,
  'setCutsceneMode' => 692,
  'getLastPCToCancelCutscene' => 693,
  'getDialogSoundLength' => 694,
  'fadeFromBlack' => 695,
  'fadeToBlack' => 696,
  'stopFade' => 697,
  'blackScreen' => 698,
  'getBaseAttackBonus' => 699,
  'setImmortal' => 700,
  'openInventory' => 701,
  'storeCameraFacing' => 702,
  'restoreCameraFacing' => 703,
  'levelUpHenchman' => 704,
  'setDroppableFlag' => 705,
  'getWeight' => 706,
  'getModuleItemAcquiredBy' => 707,
  'getImmortal' => 708,
  'doWhirlwindAttack' => 709,
  'get2DAString' => 710,
  'effectEthereal' => 711,
  'getAILevel' => 712,
  'setAILevel' => 713,
  'getIsPossessedFamiliar' => 714,
  'unpossessFamiliar' => 715,
  'getIsAreaInterior' => 716,
  'sendMessageToPCByStrRef' => 717,
  'incrementRemainingFeatUses' => 718,
  'exportSingleCharacter' => 719,
  'playSoundByStrRef' => 720,
  'setSubRace' => 721,
  'setDeity' => 722,
  'getIsDMPossessed' => 723,
  'getWeather' => 724,
  'getIsAreaNatural' => 725,
  'getIsAreaAboveGround' => 726,
  'getPCItemLastEquipped' => 727,
  'getPCItemLastEquippedBy' => 728,
  'getPCItemLastUnequipped' => 729,
  'getPCItemLastUnequippedBy' => 730,
  'copyItemAndModify' => 731,
  'getItemAppearance' => 732,
  'itemPropertyOnHitCastSpell' => 733,
  'getItemPropertySubType' => 734,
  'getActionMode' => 735,
  'setActionMode' => 736,
  'getArcaneSpellFailure' => 737,
  'actionExamine' => 738,
  'itemPropertyVisualEffect' => 739,
  'setLootable' => 740,
  'getLootable' => 741,
  'getCutsceneCameraMoveRate' => 742,
  'setCutsceneCameraMoveRate' => 743,
  'getItemCursedFlag' => 744,
  'setItemCursedFlag' => 745,
  'setMaxHenchmen' => 746,
  'getMaxHenchmen' => 747,
  'getAssociateType' => 748,
  'getSpellResistance' => 749,
  'dayToNight' => 750,
  'nightToDay' => 751,
  'lineOfSightObject' => 752,
  'lineOfSightVector' => 753,
  'getLastSpellCastClass' => 754,
  'setBaseAttackBonus' => 755,
  'restoreBaseAttackBonus' => 756,
  'effectCutsceneGhost' => 757,
  'itemPropertyArcaneSpellFailure' => 758,
  'getStoreGold' => 759,
  'setStoreGold' => 760,
  'getStoreMaxBuyPrice' => 761,
  'setStoreMaxBuyPrice' => 762,
  'getStoreIdentifyCost' => 763,
  'setStoreIdentifyCost' => 764,
  'setCreatureAppearanceType' => 765,
  'getCreatureStartingPackage' => 766,
  'effectCutsceneImmobilize' => 767,
  'getIsInSubArea' => 768,
  'getItemPropertyCostTable' => 769,
  'getItemPropertyCostTableValue' => 770,
  'getItemPropertyParam1' => 771,
  'getItemPropertyParam1Value' => 772,
  'getIsCreatureDisarmable' => 773,
  'setStolenFlag' => 774,
  'forceRest' => 775,
  'setCameraHeight' => 776,
  'setSkyBox' => 777,
  'getPhenoType' => 778,
  'setPhenoType' => 779,
  'setFogColor' => 780,
  'getCutsceneMode' => 781,
  'getSkyBox' => 782,
  'getFogColor' => 783,
  'setFogAmount' => 784,
  'getFogAmount' => 785,
  'getPickpocketableFlag' => 786,
  'setPickpocketableFlag' => 787,
  'getFootstepType' => 788,
  'setFootstepType' => 789,
  'getCreatureWingType' => 790,
  'setCreatureWingType' => 791,
  'getCreatureBodyPart' => 792,
  'setCreatureBodyPart' => 793,
  'getCreatureTailType' => 794,
  'setCreatureTailType' => 795,
  'getHardness' => 796,
  'setHardness' => 797,
  'setLockKeyRequired' => 798,
  'setLockKeyTag' => 799,
  'setLockLockable' => 800,
  'setLockUnlockDC' => 801,
  'setLockLockDC' => 802,
  'setTrapDisarmable' => 803,
  'setTrapDetectable' => 804,
  'setTrapOneShot' => 805,
  'setTrapKeyTag' => 806,
  'setTrapDisarmDC' => 807,
  'setTrapDetectDC' => 808,
  'createTrapAtLocation' => 809,
  'createTrapOnObject' => 810,
  'setWillSavingThrow' => 811,
  'setReflexSavingThrow' => 812,
  'setFortitudeSavingThrow' => 813,
  'getTilesetResRef' => 814,
  'getTrapRecoverable' => 815,
  'setTrapRecoverable' => 816,
  'getModuleXPScale' => 817,
  'setModuleXPScale' => 818,
  'getKeyRequiredFeedback' => 819,
  'setKeyRequiredFeedback' => 820,
  'getTrapActive' => 821,
  'setTrapActive' => 822,
  'lockCameraPitch' => 823,
  'lockCameraDistance' => 824,
  'lockCameraDirection' => 825,
  'getPlaceableLastClickedBy' => 826,
  'getInfiniteFlag' => 827,
  'setInfiniteFlag' => 828,
  'getAreaSize' => 829,
  'setName' => 830,
  'getPortraitId' => 831,
  'setPortraitId' => 832,
  'getPortraitResRef' => 833,
  'setPortraitResRef' => 834,
  'setUseableFlag' => 835,
  'getDescription' => 836,
  'setDescription' => 837,
  'getPCChatSpeaker' => 838,
  'getPCChatMessage' => 839,
  'getPCChatVolume' => 840,
  'setPCChatMessage' => 841,
  'setPCChatVolume' => 842,
  'getColor' => 843,
  'setColor' => 844,
  'itemPropertyMaterial' => 845,
  'itemPropertyQuality' => 846,
  'itemPropertyAdditional' => 847,
}.freeze