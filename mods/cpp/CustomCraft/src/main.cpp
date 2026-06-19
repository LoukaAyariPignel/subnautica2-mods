#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>

#include "CraftHook.hpp"
#include "RecipeBuilder.hpp"

using namespace RC;

class CustomCraft : public CppUserModBase {

    // Indique si on a déjà initialisé (on attend que le monde ClientLobby
    // soit chargé, comme SN2CustomCraft, pour que les data assets existent)
    bool m_initialized = false;

    // ── Définis tes recettes ici ─────────────────────────────────────────────
    void registerRecipes()
    {
        // Exemple : 2 Titanium → 1 Knife, 3 secondes de craft
        RecipeBuilder("TitaniumKnife")
            .name("Titanium Knife")
            .description("A blade crafted from salvaged titanium.")
            .ingredient("Titanium", 2)
            .output("Knife", 1)
            .craftingTime(3.0f)
            .build();

        // Ajoute d'autres recettes en dessous, même syntaxe :
        //
        // RecipeBuilder("MaRecette")
        //     .name("Mon Item")
        //     .description("Description.")
        //     .ingredient("NomItem", quantite)
        //     .ingredient("AutreItem", quantite)
        //     .output("NomOutput", quantite)
        //     .craftingTime(5.0f)
        //     .category("NomCategorie")   // optionnel
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
