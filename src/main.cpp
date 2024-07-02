#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/Lua/LuaInterface.h>
#include <GarrysMod/Lua/LuaGameCallback.h>

#include <memory>
#include <dbg.h>

#include <detouring/hook.hpp>
#include <detouring/classproxy.hpp>

#if defined PLATFORM_WINDOWS
#include <Windows.h>
#endif

#if defined ARCHITECTURE_X86
#include <Color.h>
#endif

class LuaGameCallbackProxy : public Detouring::ClassProxy<GarrysMod::Lua::ILuaGameCallback, LuaGameCallbackProxy> {
public:
    LuaGameCallbackProxy(GarrysMod::Lua::ILuaGameCallback* callback) {
        Initialize(callback);
        Hook(&GarrysMod::Lua::ILuaGameCallback::Msg, &LuaGameCallbackProxy::Msg);
        Hook(&GarrysMod::Lua::ILuaGameCallback::MsgColour, &LuaGameCallbackProxy::MsgColour);
        Hook(&GarrysMod::Lua::ILuaGameCallback::ErrorPrint, &LuaGameCallbackProxy::ErrorPrint);
        Hook(&GarrysMod::Lua::ILuaGameCallback::LuaError, &LuaGameCallbackProxy::LuaError);
    }

    ~LuaGameCallbackProxy() {
        UnHook(&GarrysMod::Lua::ILuaGameCallback::Msg);
        UnHook(&GarrysMod::Lua::ILuaGameCallback::MsgColour);
        UnHook(&GarrysMod::Lua::ILuaGameCallback::ErrorPrint);
        UnHook(&GarrysMod::Lua::ILuaGameCallback::LuaError);
    }

    virtual void Msg(const char* msg, bool useless) {
        ::Msg("\x1b[96m%s\x1b[0m", msg);

    }
    virtual void MsgColour(const char* msg, const Color& color) {
        ::Msg("\033[38;2;%d;%d;%dm%s", color.r(), color.b(), color.g(), msg);
    }

    virtual void ErrorPrint(const char* error, bool print) {
        ::Msg("\x1b[91m%s\x1b[0m\n", error);
    }

    virtual void LuaError(const GarrysMod::Lua::ILuaGameCallback::CLuaError* error) {
        ::Msg("\n\x1b[30m\x1b[101m[ERROR]\x1b[0m \x1b[91m%s\x1b[0m\n", error->message.c_str());

        size_t stackLen = error->stack.size();
        for (size_t i = 0; i < stackLen; i++) {
            const auto& entry = error->stack[i];
            int pos = i + 1;
            std::string_view func = entry.function;
            if (func.size() == 0) func = "unknown";

            ::Msg(" \x1b[90m%*d. \x1b[0m", pos, pos);
            ::Msg("\x1b[31m%s \x1b[90m- ", func.data());
            ::Msg("\x1b[93m%s\x1b[90m:\x1b[93m%d\x1b[0m\n", entry.source.c_str(), entry.line);
        }
        ::Msg("\n");
    }
};

std::unique_ptr<LuaGameCallbackProxy> g_LuaGameCallbackProxy;

std::vector<char> FormatString(const char* pMsg, std::va_list args) {
    std::va_list args_copy;
    va_copy(args_copy, args);

    std::vector<char> buf(1 + std::vsnprintf(nullptr, 0, pMsg, args));
    std::vsnprintf(buf.data(), buf.size(), pMsg, args_copy);
    va_end(args_copy);
    return buf;
}

#define FORMAT_STRING(fmt) std::va_list args; va_start(args, fmt); std::vector<char> buf = FormatString(fmt, args); va_end(args);

void ColorizedWarning(const tchar* pMsg, ...) {
    FORMAT_STRING(pMsg);
    Msg("\x1b[33m%s\x1b[0m", buf.data());
}
Detouring::Hook g_WarningHook;

void ColorizedDevWarning1(const tchar* pMsg, ...) {
    FORMAT_STRING(pMsg);
    DevMsg("\x1b[33m%s\x1b[0m", buf.data());
}
Detouring::Hook g_DevWarning1Hook;
void ColorizedDevWarning2(int level, const tchar* pMsg, ...) {
    FORMAT_STRING(pMsg);
    DevMsg(level, "\x1b[33m%s\x1b[0m", buf.data());
}
Detouring::Hook g_DevWarning2Hook;
void HookDevWarning(void (*DevWarning1)(PRINTF_FORMAT_STRING const tchar* pMsg, ...), void (*DevWarning2)(int level, PRINTF_FORMAT_STRING const tchar* pMsg, ...)) {
    g_DevWarning1Hook.Create(reinterpret_cast<void*>(DevWarning1), reinterpret_cast<void*>(&ColorizedDevWarning1));
    g_DevWarning1Hook.Enable();
    g_DevWarning2Hook.Create(reinterpret_cast<void*>(DevWarning2), reinterpret_cast<void*>(&ColorizedDevWarning2));
    g_DevWarning2Hook.Enable();
}

void ColorizedConColorMsg1(const Color& color, const tchar* pMsg, ...) {
    FORMAT_STRING(pMsg);
    ConMsg("\033[38;2;%d;%d;%dm%s\x1b[0m", color.r(), color.b(), color.g(), buf.data());
}
Detouring::Hook g_ConColorMsg1;

void HookConColorMsg1(void (*ConColorMsg)(const Color& color, const tchar* pMsg, ...)) {
    g_ConColorMsg1.Create(reinterpret_cast<void*>(ConColorMsg), reinterpret_cast<void*>(&ColorizedConColorMsg1));
    g_ConColorMsg1.Enable();

}

#if defined ARCHITECTURE_X86
void ColorizedConColorMsg2(int level, const Color& color, const tchar* pMsg, ...) {
    FORMAT_STRING(pMsg);
    ConMsg(level, "\033[38;2;%d;%d;%dm%s\x1b[0m", color.r(), color.b(), color.g(), buf.data());
}
Detouring::Hook g_ConColorMsg2;

void HookConColorMsg2(void (*ConColorMsg)(int level, const Color& color, const tchar* pMsg, ...)) {
    g_ConColorMsg2.Create(reinterpret_cast<void*>(ConColorMsg), reinterpret_cast<void*>(&ColorizedConColorMsg2));
    g_ConColorMsg2.Enable();
}

#endif

void CreditMessage() {
    std::string_view message = "colorized B)";
    static const Color msg[] = {
        Color(255, 87, 137),
        Color(255, 95, 191),
        Color(234, 119, 244),
        Color(185, 156, 255),
        Color(134, 182, 255),
        Color(74, 198, 255),
        Color(0, 211, 255),
        Color(0, 220, 243),
        Color(0, 230, 215),
        Color(0, 243, 172),
        Color(0, 245, 162),
        Color(57, 252, 97),
        Color(166, 249, 31),
    };

    Msg("terminal was ");
    for (size_t i = 0; i < message.size(); i++) {
        ConColorMsg(msg[i % std::size(msg)], "%c", message[i]);
    }
    Msg("\x1b[0m\n");
}


void Initialize(GarrysMod::Lua::CLuaInterface* LUA) {
    if (!LUA->IsServer()) return;

    // Make sure if we are running dedicated server
    LUA->GetField(GarrysMod::Lua::INDEX_GLOBAL, "game");
    LUA->GetField(-1, "IsDedicated");
    if (LUA->PCall(0, 1, 0) != 0) {
        LUA->Pop(1);
        LUA->PushBool(false);
    }
    if (LUA->GetBool(-1) == false) {
        Msg("gmsv_colorize only works on dedicated servers :p\n");
        return;
    }
    LUA->Pop(2);

    g_LuaGameCallbackProxy = std::make_unique<LuaGameCallbackProxy>(LUA->GetLuaGameCallback());

    g_WarningHook.Create(reinterpret_cast<void*>(&Warning), reinterpret_cast<void*>(&ColorizedWarning));
    g_WarningHook.Enable();

    HookDevWarning(&DevWarning, &DevWarning);

    HookConColorMsg1(&ConColorMsg);

#if defined ARCHITECTURE_X86
    HookConColorMsg2(&ConColorMsg);
#endif

#if defined PLATFORM_WINDOWS
    // Enable ANSI escape codes
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hConsole, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hConsole, dwMode);
#endif

    CreditMessage();
}

void Deinitialize(GarrysMod::Lua::CLuaInterface* LUA) {
    if (!LUA->IsServer()) return;

    g_WarningHook.Disable();
    g_WarningHook.Destroy();
    g_DevWarning1Hook.Disable();
    g_DevWarning1Hook.Destroy();
    g_DevWarning2Hook.Disable();
    g_DevWarning2Hook.Destroy();
    g_LuaGameCallbackProxy.reset();

#if defined ARCHITECTURE_X86
    g_ConColorMsg2.Disable();
    g_ConColorMsg2.Destroy();
#endif
}

GMOD_MODULE_OPEN() {
    Initialize(reinterpret_cast<GarrysMod::Lua::CLuaInterface*>(LUA));
    return 0;
}

GMOD_MODULE_CLOSE() {
    Deinitialize(reinterpret_cast<GarrysMod::Lua::CLuaInterface*>(LUA));
    return 0;
}
