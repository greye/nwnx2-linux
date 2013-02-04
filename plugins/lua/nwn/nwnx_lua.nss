string LuaEval(string sCode)
{
    SetLocalString(GetModule(), "NWNX!LUA!EVAL", sCode);
    return GetLocalString(GetModule(), "NWNX!LUA!EVAL");
}
void LuaEvalVoid(string sCode)
{
    SetLocalString(GetModule(), "NWNX!LUA!EVAL", sCode);
}
void LuaRunEvent(string sEvent, object oSelf=OBJECT_SELF)
{
    SetLocalString(GetModule(), "NWNX!LUA!EVENT", sEvent+":0x"+ObjectToString(oSelf));
}
void LuaRunToken(string sToken)
{
    SetLocalString(GetModule(), "NWNX!LUA!TOKEN", sToken);
}

void luaPushString(string s);
void luaPushInterger(int n);
void luaPushNumber(float f);
void luaPushObject(object oid);

void luaCall(string callee, int nargs = 0);
string luaCallRet(string callee, int nargs = 0, int nret = 0);

void luaEvaluate(string code);
string luaEvaluateRet(string code);
string luaStringOID(object oid);

void luaPushString(string s)
{
    SetLocalString(GetModule(), "NWNX!LUA!PUSHSTR", s);
}

void luaPushInteger(int n)
{
    SetLocalString(GetModule(), "NWNX!LUA!PUSHINT", IntToHexString(n));
}

void luaPushNumber(float f)
{
    SetLocalString(GetModule(), "NWNX!LUA!PUSHNUM", FloatToString(f));
}

void luaPushObject(object oid)
{
    SetLocalString(GetModule(), "NWNX!LUA!PUSHINT", ObjectToString(oid));
}

void luaCall(string callee, int nargs)
{
    string call = callee + "!" + IntToString(nargs) + "!0";
    SetLocalString(GetModule(), "NWNX!LUA!CALL", call);
}

string luaCallRet(string callee, int nargs, int nret)
{
    string call = callee + "!" + IntToString(nargs) + "!" + IntToString(nret);
    int i;
    for (i = 0; i < nret; ++i) {
        call += "                ";
    }
    SetLocalString(GetModule(), "NWNX!LUA!CALL", call);
    return GetLocalString(GetModule(), "NWNX!LUA!CALL");
}

void luaEvaluate(string code)
{
    SetLocalString(GetModule(), "NWNX!LUA!EVAL", code);
}

string luaEvaluateRet(string code)
{
    SetLocalString(GetModule(), "NWNX!LUA!EVAL", code);
    return GetLocalString(GetModule(), "NWNX!LUA!EVAL");
}

string luaStringOID(object oid)
{
    return "0x" + ObjectToString(oid);
}
