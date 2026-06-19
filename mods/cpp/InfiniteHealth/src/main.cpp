// InfiniteHealth — Subnautica 2 mod via UE4SS C++
// Restaure la santé à 100% toutes les RESTORE_INTERVAL_MS via SN2CheatManager::MyHealth.
//
// Pattern issu de LabrynthKing/Subnautica2RP (GameHandler.cpp) :
//   FindFirstOf("PlayerController")
//   → GetValuePtrByPropertyNameInChain<UObject*>("CheatManager")
//   → GetFunctionByNameInChain("MyHealth")
//   → ProcessEvent
// Référence : https://github.com/LabrynthKing/Subnautica2RP

#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UFunction.hpp>
#include <chrono>

using namespace RC;
using namespace RC::Unreal;

class InfiniteHealth : public CppUserModBase {

    static constexpr int RESTORE_INTERVAL_MS = 500;

    std::chrono::steady_clock::time_point m_last_restore{};

    // Retourne le CheatManager si le joueur est en jeu, nullptr sinon.
    // Même logique que cheats.lua : PlayerController.CheatManager en priorité,
    // puis FindFirstOf("SN2CheatManager") en fallback.
    UObject* find_cheat_manager()
    {
        UObject* pc = UObjectGlobals::FindFirstOf(STR("PlayerController"));
        if (!pc) return nullptr;

        // Exclure UWEFrontendPlayerController (menu principal) — pattern S2RP
        if (pc->GetName().find(STR("Frontend")) != std::wstring::npos)
            return nullptr;

        auto** cm_ptr = pc->GetValuePtrByPropertyNameInChain<UObject*>(STR("CheatManager"));
        if (cm_ptr && *cm_ptr) return *cm_ptr;

        // Fallback identique au cheats.lua de sub2_chaos
        UObject* cm = UObjectGlobals::FindFirstOf(STR("SN2CheatManager"));
        if (!cm)   cm = UObjectGlobals::FindFirstOf(STR("CheatManager"));
        return cm;
    }

    void restore_health()
    {
        UObject* cm = find_cheat_manager();
        if (!cm) return;

        // SN2CheatManager::MyHealth(float) — valeur 0..100 (pleine santé = 100)
        // Confirmé fonctionnel via sub2_chaos (Ctrl+PageDown → MyHealth(100))
        UFunction* fn = cm->GetFunctionByNameInChain(STR("MyHealth"));
        if (!fn) return;

        struct { float Value; } params{ 100.f };
        cm->ProcessEvent(fn, &params);
    }

public:
    InfiniteHealth() : CppUserModBase()
    {
        ModName        = STR("InfiniteHealth");
        ModVersion     = STR("1.0.0");
        ModDescription = STR("Restaure la santé à 100% toutes les 500ms.");
        ModAuthors     = STR("YourName");
    }

    ~InfiniteHealth() override = default;

    auto on_unreal_init() -> void override
    {
        Output::send<LogLevel::Verbose>(STR("[InfiniteHealth] Initialisé\n"));
    }

    auto on_update() -> void override
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_restore).count();
        if (elapsed < RESTORE_INTERVAL_MS) return;

        m_last_restore = now;
        restore_health();
    }
};

#define UE4SS_ENABLE_INLINE_MOD_LOADER
#include <macro_plugin_suffix.hpp>
