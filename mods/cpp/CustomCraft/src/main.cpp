#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>

#include "CraftHook.hpp"
#include "RecipeBuilder.hpp"
#include "Items.hpp"

using namespace RC;

class CustomCraft : public CppUserModBase {

    // Indique si on a déjà initialisé (on attend que le monde ClientLobby
    // soit chargé, comme SN2CustomCraft, pour que les data assets existent)
    bool m_initialized = false;

    // ── Définis tes recettes ici ─────────────────────────────────────────────
    // Utilise les constantes de Items.hpp (tous confirmés build 115506, UE 5.6).
    // Pour un item non listé, utilise directement le nom en string :
    //   .ingredient("MonItem", 1)  →  DA_MonItem_ItemType
    void registerRecipes()
    {
        // Exemple : 4 MetalSalvage + 1 Copper → 3 Titanium (3s)
        RecipeBuilder("ScrapToTitanium")
            .name("Scrap Smelting")
            .description("Smelt salvaged metal into usable titanium.")
            .ingredient(Items::MetalSalvage, 4)
            .ingredient(Items::Copper, 1)
            .output(Items::Titanium, 3)
            .craftingTime(3.0f)
            .build();

        // Exemple : 2 Gold + 1 Silver → 2 Lithium (5s)
        RecipeBuilder("GoldToLithium")
            .name("Transmutation")
            .description("Extract lithium from precious metals.")
            .ingredient(Items::Gold, 2)
            .ingredient(Items::Silver, 1)
            .output(Items::Lithium, 2)
            .craftingTime(5.0f)
            .build();

        // Ajoute tes propres recettes ci-dessous :
        //
        // RecipeBuilder("MonId")
        //     .name("Mon Item")
        //     .description("Description.")
        //     .ingredient(Items::Titanium, 2)
        //     .output(Items::Gold, 1)
        //     .craftingTime(4.0f)
        //     .build();
    }
    // ─────────────────────────────────────────────────────────────────────────

public:
    CustomCraft() : CppUserModBase()
    {
        ModName        = STR("CustomCraft");
        ModVersion     = STR("1.0.0");
        ModDescription = STR("Ajoute des recettes custom codées en C++.");
        ModAuthors     = STR("YourName");
    }

    ~CustomCraft() override
    {
        CraftHook::uninstall();
    }

    auto on_unreal_init() -> void override
    {
        Output::send<LogLevel::Verbose>(STR("[CustomCraft] on_unreal_init\n"));
    }

    // Attend que ClientLobby soit chargé (data assets présents dans la mémoire)
    // avant d'installer le hook et d'enregistrer les recettes.
    // Même stratégie que Mod.cpp de SN2CustomCraft.
    auto on_update() -> void override
    {
        if (m_initialized) return;

        const auto* world = RC::Unreal::UObjectGlobals::FindFirstOf(STR("World"));
        if (!world) return;

        // Le nom du monde doit contenir "ClientLobby" (monde principal du jeu)
        const auto name = world->GetName();
        if (name.find(STR("ClientLobby")) == std::wstring::npos) return;

        m_initialized = true;

        Output::send<LogLevel::Verbose>(STR("[CustomCraft] ClientLobby detected — installing hook\n"));
        CraftHook::install();
        registerRecipes();

        Output::send<LogLevel::Verbose>(
            STR("[CustomCraft] {} recipe(s) registered\n"),
            static_cast<int>(CraftHook::recipes.size()));
    }
};

#define UE4SS_ENABLE_INLINE_MOD_LOADER
#include <macro_plugin_suffix.hpp>
