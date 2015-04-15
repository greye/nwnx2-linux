/*
 *  NWNeXalt - Empty File
 *  (c) 2007 Doug Swarin (zac@intertex.net)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  $Id$
 *  $HeadURL$
 *
 */

#ifndef _NX_NWN_STRUCT_CNWSAREAOFEFFECTOBJECT_
#define _NX_NWN_STRUCT_CNWSAREAOFEFFECTOBJECT_

enum {
    AOE_EVENT_HEARTBEAT = 0,
    AOE_EVENT_ENTER = 2,
    AOE_EVENT_EXIT = 3
};

struct CNWSAreaOfEffectObject_s {
    CNWSObject serverObject;
    int arrayIndex;
    int aoeId;
    int shape;
    int spellId;
    float radius;
    float width;
    float length;
    Vector *corners;
    nwn_objid_t creator;
    nwn_objid_t target;
    nwn_objid_t lastEntered;
    nwn_objid_t lastLeft;
    int saveDC;
    int spellLevel;
    CExoString eventScript[4];
    uint32_t lastHeartbeatDay;
    uint32_t lastHeartbeatTime;
    int msecDuration;
    int durationType;
};

#endif /* _NX_NWN_STRUCT_CNWSAREAOFEFFECTOBJECT_ */

/* vim: set sw=4: */
