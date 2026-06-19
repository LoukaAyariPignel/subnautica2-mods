#include <Mod/CppUserModBase.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UFunction.hpp>
#include <SN2Helpers.hpp>

// Hook sur la fonction qui consomme l'oxygène et annule la consommation
static RC::Unreal::CallbackId g_oxygenHookId{};

class InfiniteOxygen : public RC::CppUserModBase {
public:
    InfiniteOxygen() : CppUserModBase() {
        ModName        = STR("InfiniteOxygen");
        ModVersion     = STR("1.0.0");
        ModDescription = STR("Oxygène infini — supprime le drain d'O2.");
        ModAuthors     = STR("YourName");
    }

    ~InfiniteOxygen() override = default;

    auto on_unreal_init() -> void override {
        // Remplacer "OxygenManager:ConsumeOxygen" par le vrai nom de fonction
        // une fois les headers du jeu disponibles.
        g_oxygenHookId = RC::Unreal::UObjectGlobals::RegisterHook(
            STR("/Script/Subnautica.OxygenManager:ConsumeOxygen"),
            [](RC::Unreal::UnrealScriptFunctionCallableContext& ctx, void*) {
                // Bloquer l'appel pour éviter la consommation d'O2
                ctx.TheStack.bIsCallingOutParams = false;
            },
            nullptr
        );

        SN2::Log(STR("[InfiniteOxygen] Hook O2 installé (id={})\n"), g_oxygenHookId);
    }

    auto on_program_start() -> void override {
        SN2::Log(STR("[InfiniteOxygen] Chargé\n"));
    }

    auto on_update() -> void override {}
};

#define UE4SS_ENABLE_INLINE_MOD_LOADER
#include <macro_plugin_suffix.hpp>
