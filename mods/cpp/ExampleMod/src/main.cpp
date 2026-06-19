#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <SN2Helpers.hpp>

class ExampleMod : public RC::CppUserModBase {
public:
    ExampleMod() : CppUserModBase() {
        ModName        = STR("ExampleMod");
        ModVersion     = STR("1.0.0");
        ModDescription = STR("Template de mod C++ pour Subnautica 2.");
        ModAuthors     = STR("YourName");
    }

    ~ExampleMod() override = default;

    auto on_program_start() -> void override {
        SN2::Log(STR("[ExampleMod] Mod chargé avec succès !\n"));

        // Exemple : hook sur le constructeur d'un acteur du jeu
        // RC::Unreal::UObjectGlobals::RegisterHook(
        //     STR("/Script/Subnautica.PlayerController:BeginPlay"),
        //     [](RC::Unreal::UnrealScriptFunctionCallableContext& ctx, void*) {
        //         SN2::Log(STR("[ExampleMod] BeginPlay déclenché\n"));
        //     },
        //     nullptr
        // );
    }

    auto on_update() -> void override {
        // Appelé chaque frame — garder léger
    }

    auto on_unreal_init() -> void override {
        SN2::Log(STR("[ExampleMod] Unreal Engine initialisé\n"));
    }
};

#define UE4SS_ENABLE_INLINE_MOD_LOADER
#include <macro_plugin_suffix.hpp>
