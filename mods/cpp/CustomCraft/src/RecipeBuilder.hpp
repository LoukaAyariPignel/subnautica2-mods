#pragma once
#include <string>
#include <vector>
#include <format>

#include "CraftHook.hpp"
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObjectGlobals.hpp>

// SDK Dumper-7 — https://github.com/Evasion3356/Subnautica-2-Cheat
// Structs validés par ce SDK :
//
//   FCraftingRecipeRequirement (0x30 bytes)
//     0x0000  TSoftObjectPtr<UUWEItemType>  ItemType
//     0x0028  int32                         NumItems
//     0x002C  uint8[4]                      Pad_2C
//
//   FCraftingRecipeOutput (0x58 bytes)
//     0x0000  TSoftObjectPtr<UUWEItemType>  ItemType
//     0x0028  int32                         NumItems
//     0x002C  uint8[4]                      Pad_2C
//     0x0030  FVector                       CollisionCheckSize
//     0x0048  TArray<TSoftClassPtr<UClass>> ClassesToIgnoreForCollisionCheck
//
//   UUWECraftingRecipe fields (après UObject base à 0x48)
//     Name_0, Description, Thumbnail, Output, Requirements,
//     DefaultRecipeState, UpdatedUnlockingRequirements, CraftingTime, Category, OrderingIndex
#include "SDK/UWECrafting_classes.hpp"
#include "SDK/UWECrafting_structs.hpp"
#include "SDK/UWEAssetRegistry_classes.hpp"

using namespace SDK;
using namespace RC::Unreal;

inline std::wstring toW(const std::string& s) { return { s.begin(), s.end() }; }

// ─────────────────────────────────────────────────────────────────────────────
// RecipeBuilder
//
// Crée une recette custom en clonant DA_MetalSalvageRecipe comme base,
// puis surcharge ses propriétés.
//
// Usage dans main.cpp :
//
//   RecipeBuilder("TitaniumKnife")
//       .name("Titanium Knife")
//       .description("A blade of salvaged titanium.")
//       .ingredient("Titanium", 2)
//       .output("Knife", 1)
//       .craftingTime(3.0f)
//       .build();
// ─────────────────────────────────────────────────────────────────────────────
class RecipeBuilder {
public:
    explicit RecipeBuilder(std::string id) : id_(std::move(id)) {}

    RecipeBuilder& name(std::string n)         { name_ = std::move(n); return *this; }
    RecipeBuilder& description(std::string d)  { desc_ = std::move(d); return *this; }
    RecipeBuilder& category(std::string cat)   { cat_  = std::move(cat); return *this; }
    RecipeBuilder& craftingTime(float t)       { time_ = t; return *this; }
    RecipeBuilder& orderIndex(int32_t i)       { order_ = i; return *this; }

    RecipeBuilder& ingredient(const std::string& itemId, int32_t count = 1) {
        ingredients_.push_back({ itemId, count });
        return *this;
    }

    RecipeBuilder& output(const std::string& itemId, int32_t count = 1) {
        outputs_.push_back({ itemId, count });
        return *this;
    }

    UUWECraftingRecipe* build() {

        // ── 1. Base recipe : DA_MetalSalvageRecipe ───────────────────────────
        auto* base = reinterpret_cast<UUWECraftingRecipe*>(
            UObjectGlobals::FindObject(L"UWECraftingRecipe", L"DA_MetalSalvageRecipe"));

        if (!base) {
            RC::Output::send<RC::LogLevel::Error>(
                STR("[CustomCraft] DA_MetalSalvageRecipe not found — game not fully loaded yet?\n"));
            return nullptr;
        }

        // ── 2. Clone : StaticConstructObject_Internal ────────────────────────
        // Flags identiques à RegistryHelper::StaticConstructTemplate de SN2CustomCraft
        using EF = EObjectFlags;
        constexpr EF flags =
            EF::RF_Public | EF::RF_Standalone | EF::RF_Transactional |
            EF::RF_WasLoaded | EF::RF_LoadCompleted | EF::RF_MarkAsRootSet;

        const std::wstring assetName = toW(std::format("DA_{}Recipe", id_));

        auto* recipe = reinterpret_cast<UUWECraftingRecipe*>(
            UObjectGlobals::StaticConstructObject_Internal(
                base->GetClassPrivate(),
                base->GetOuterPrivate(),
                FName(assetName.c_str()),
                flags,
                EInternalObjectFlags::None,
                base,   // template — copie les valeurs depuis la base
                false, nullptr, false));

        if (!recipe) {
            RC::Output::send<RC::LogLevel::Error>(
                STR("[CustomCraft] StaticConstructObject_Internal returned null for '{}'\n"),
                assetName);
            return nullptr;
        }

        // ── 3. Nom et description ────────────────────────────────────────────
        recipe->Name_0 = FText(toW(name_).c_str());
        if (!desc_.empty())
            recipe->Description = FText(toW(desc_).c_str());

        // ── 4. Temps de craft ────────────────────────────────────────────────
        if (time_ > 0.0f)
            recipe->CraftingTime = time_;

        // ── 5. Index d'affichage ─────────────────────────────────────────────
        if (order_ >= 0)
            recipe->OrderingIndex = order_;

        // ── 6. Catégorie ─────────────────────────────────────────────────────
        if (!cat_.empty()) {
            auto catW = L"DA_" + toW(cat_);
            auto* cat = reinterpret_cast<UUWECraftingRecipeCategory*>(
                UObjectGlobals::FindObject(L"UWECraftingRecipeCategory", catW.c_str()));
            if (cat)
                recipe->Category = TSoftObjectPtr<UUWECraftingRecipeCategory>(
                    UKismetSystemLibrary::Conv_ObjectToSoftObjectReference(cat));
            else
                RC::Output::send<RC::LogLevel::Warning>(
                    STR("[CustomCraft] Category '{}' not found\n"), toW(cat_));
        }

        // ── 7. Ingrédients ───────────────────────────────────────────────────
        // FCraftingRecipeRequirement = 0x30 bytes (SDK validé)
        auto* reqs = reinterpret_cast<TArray<FCraftingRecipeRequirement>*>(&recipe->Requirements);
        reqs->SetNum(0, EAllowShrinking::Yes);
        reqs->ResizeTo(static_cast<int32_t>(ingredients_.size()));

        for (const auto& [itemId, count] : ingredients_) {
            const std::wstring itemW = L"DA_" + toW(itemId) + L"_ItemType";
            auto* item = UObjectGlobals::FindObject(L"UWEItemType", itemW.c_str());
            if (!item) {
                RC::Output::send<RC::LogLevel::Warning>(
                    STR("[CustomCraft] Ingredient item '{}' not found — skipped\n"), toW(itemId));
                continue;
            }
            FCraftingRecipeRequirement req{};
            req.ItemType = TSoftObjectPtr<UUWEItemType>(
                UKismetSystemLibrary::Conv_ObjectToSoftObjectReference(item));
            req.NumItems = count;
            reqs->Add(req);
        }

        // ── 8. Outputs ───────────────────────────────────────────────────────
        // FCraftingRecipeOutput = 0x58 bytes :
        //   TSoftObjectPtr ItemType (0x28), int32 NumItems (0x04), pad (0x04),
        //   FVector CollisionCheckSize (0x18), TArray ClassesToIgnore (0x10)
        auto* outs = reinterpret_cast<TArray<FCraftingRecipeOutput>*>(&recipe->Output);
        outs->SetNum(0, EAllowShrinking::Yes);
        outs->ResizeTo(static_cast<int32_t>(outputs_.size()));

        for (const auto& [itemId, count] : outputs_) {
            const std::wstring itemW = L"DA_" + toW(itemId) + L"_ItemType";
            auto* item = UObjectGlobals::FindObject(L"UWEItemType", itemW.c_str());
            if (!item) {
                RC::Output::send<RC::LogLevel::Warning>(
                    STR("[CustomCraft] Output item '{}' not found — skipped\n"), toW(itemId));
                continue;
            }
            FCraftingRecipeOutput out{};
            out.ItemType = TSoftObjectPtr<UUWEItemType>(
                UKismetSystemLibrary::Conv_ObjectToSoftObjectReference(item));
            out.NumItems = count;
            out.CollisionCheckSize = FVector(0.f, 0.f, 0.f);
            // ClassesToIgnoreForCollisionCheck laissé vide (TArray zero-init par {})
            outs->Add(out);
        }

        // ── 9. Enregistrement dans le hook ───────────────────────────────────
        CraftHook::recipes.push_back(recipe);

        RC::Output::send<RC::LogLevel::Verbose>(
            STR("[CustomCraft] '{}' built ({} req, {} out, {:.1f}s)\n"),
            assetName,
            static_cast<int>(ingredients_.size()),
            static_cast<int>(outputs_.size()),
            recipe->CraftingTime);

        return recipe;
    }

private:
    struct Entry { std::string itemId; int32_t count; };

    std::string        id_, name_, desc_, cat_;
    float              time_  = 0.0f;
    int32_t            order_ = -1;
    std::vector<Entry> ingredients_;
    std::vector<Entry> outputs_;
};
