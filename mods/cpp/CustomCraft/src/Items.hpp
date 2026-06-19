#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// Items.hpp — Noms d'items confirmés pour RecipeBuilder
//
// Chaque constante correspond à l'argument attendu par RecipeBuilder :
//   .ingredient(Items::Titanium, 2)
//   .output(Items::Gold, 1)
//
// Le RecipeBuilder transforme "Titanium" → FindObject("UWEItemType", "DA_Titanium_ItemType")
//
// Sources de vérification (build 115506, UE 5.6) :
//   [A] Evasion3356/Subnautica-2-Cheat — get_resource_name() + ESP code
//   [B] caesarakalaeii/sub2_chaos — events.json (Give cheat, actorPath confirmés)
//   [C] LimoDerEchte/Subnautica2Mods — SN2CustomCraft recipes.toml
//   [D] Subnautica2Modding/Subnautica2-Project — DataIndex.json
// ─────────────────────────────────────────────────────────────────────────────

namespace Items {

    // ── Ressources brutes [A][B][C][D] ───────────────────────────────────────
    // Obtenues dans l'environnement, sans craft
    inline constexpr const char* Titanium    = "Titanium";    // [A][C][D] DA_Titanium_ItemType
    inline constexpr const char* Copper      = "Copper";      // [A][C]    DA_Copper_ItemType
    inline constexpr const char* Quartz      = "Quartz";      // [A]       DA_Quartz_ItemType
    inline constexpr const char* Silver      = "Silver";      // [A]       DA_Silver_ItemType
    inline constexpr const char* Gold        = "Gold";        // [A][B]    DA_Gold_ItemType (/Game/Blueprints/Items/Resources/BP_Gold)
    inline constexpr const char* Lead        = "Lead";        // [A]       DA_Lead_ItemType
    inline constexpr const char* Sulfur      = "Sulfur";      // [A]       DA_Sulfur_ItemType
    inline constexpr const char* Salt        = "Salt";        // [A]       DA_Salt_ItemType
    inline constexpr const char* Lithium     = "Lithium";     // [A]       DA_Lithium_ItemType
    inline constexpr const char* MetalSalvage = "MetalSalvage"; // [C]     DA_MetalSalvage_ItemType

    // ── Outils & équipements [C] ─────────────────────────────────────────────
    // Présents dans recipes.toml comme recettes modifiées/supprimées
    inline constexpr const char* Flashlight  = "Flashlight";  // [C]       DA_Flashlight_ItemType
    inline constexpr const char* BuilderTool = "BuilderTool"; // [C]       DA_BuilderTool_ItemType

    // ── Items craftés / cuisinés [C] ─────────────────────────────────────────
    // Poissons cuisinés, matériaux transformés
    inline constexpr const char* CookedBluemoon  = "CookedBluemoon";  // [C] DA_CookedBluemoon_ItemType
    inline constexpr const char* CookedHalfmoon  = "CookedHalfmoon";  // [C] DA_CookedHalfmoon_ItemType
    inline constexpr const char* BiofuelBlock     = "BiofuelBlock";    // [C] DA_BiofuelBlock_ItemType
    inline constexpr const char* TableDining      = "TableDining";     // [C] DA_TableDining_ItemType

    // ── Items non confirmés — à vérifier avec FModel ─────────────────────────
    // Présents dans la série originale Subnautica, probablement présents dans SN2
    // inline constexpr const char* Glass       = "Glass";
    // inline constexpr const char* Silicone    = "Silicone";
    // inline constexpr const char* Wire        = "Wire";
    // inline constexpr const char* Battery     = "Battery";
    // inline constexpr const char* Fiber       = "Fiber";
    // inline constexpr const char* Lubricant   = "Lubricant";
    // inline constexpr const char* Plasteel    = "Plasteel";
    // inline constexpr const char* Diamond     = "Diamond";
    // inline constexpr const char* Nickel      = "Nickel";
    // inline constexpr const char* Magnetite   = "Magnetite";
    // inline constexpr const char* Kyanite     = "Kyanite";
    // inline constexpr const char* Ruby        = "Ruby";

} // namespace Items
