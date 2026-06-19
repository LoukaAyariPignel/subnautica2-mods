#pragma once
#include <string>
#include <vector>
#include <format>

#include "CraftHook.hpp"
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include "SDK/Subnautica2_classes.hpp"

// Converts a narrow UTF-8 string to a wide string for UE APIs.
inline std::wstring toW(const std::string& s) {
    return { s.begin(), s.end() };
}

// ─────────────────────────────────────────────────────────────────────────────
// RecipeBuilder
//
// Fluent builder that creates a UUWECraftingRecipe by cloning an existing
// base recipe (DA_MetalSalvageRecipe) and overwriting its fields.
// Call build() once done — it registers the recipe with CraftHook.
//
// Usage (in main.cpp):
//   RecipeBuilder("TitaniumKnife")
//       .name("Titanium Knife")
//       .description("A simple blade.")
//       .ingredient("Titanium", 2)
//       .output("Knife", 1)
//       .craftingTime(3.0f)
//       .build();
// ─────────────────────────────────────────────────────────────────────────────
class RecipeBuilder {
public:
    explicit RecipeBuilder(std::string id) : id_(std::move(id)) {}

    RecipeBuilder& name(std::string n)        { name_ = std::move(n); return *this; }
    RecipeBuilder& description(std::string d) { desc_ = std::move(d); return *this; }
    RecipeBuilder& category(std::string cat)  { cat_  = std::move(cat); return *this; }
    RecipeBuilder& craftingTime(float t)      { time_ = t; return *this; }

    RecipeBuilder& ingredient(const std::string& itemId, int32_t count = 1) {
        ingredients_.push_back({ itemId, count });
        return *this;
    }

    RecipeBuilder& output(const std::string& itemId, int32_t count = 1) {
        outputs_.push_back({ itemId, count });
        return *this;
    }

    SDK::UUWECraftingRecipe* build() {
        using namespace RC::Unreal;
        using namespace SDK;

        // ── 1. Find base recipe to clone ────────────────────────────────────
        auto* base = reinterpret_cast<UUWECraftingRecipe*>(
            UObjectGlobals::FindObject(L"UWECraftingRecipe", L"DA_MetalSalvageRecipe"));

        if (!base) {
            RC::Output::send<RC::LogLevel::Error>(
                STR("[CustomCraft] Base recipe DA_MetalSalvageRecipe not found — is the game loaded?\n"));
            return nullptr;
        }

        // ── 2. Clone it with a new asset name ───────────────────────────────
        // The name must follow the DA_<ID>Recipe convention.
        const std::string assetName = std::format("DA_{}Recipe", id_);

        using EF = EObjectFlags;
        constexpr EF flags =
            EF::RF_Public      | EF::RF_Standalone    | EF::RF_Transactional |
            EF::RF_WasLoaded   | EF::RF_LoadCompleted | EF::RF_MarkAsRootSet;

        auto* recipe = reinterpret_cast<UUWECraftingRecipe*>(
            UObjectGlobals::StaticConstructObject_Internal(
                base->GetClassPrivate(),
                base->GetOuterPrivate(),
                FName(toW(assetName).c_str()),
                flags,
                EInternalObjectFlags::None,
                base,   // template — copies all property values from base
                false,
                nullptr,
                false));

        if (!recipe) {
            RC::Output::send<RC::LogLevel::Error>(
                STR("[CustomCraft] Failed to construct recipe '{}'\n"), toW(id_));
            return nullptr;
        }

        // ── 3. Override fields ───────────────────────────────────────────────
        recipe->Name_0 = UKismetTextLibrary::Conv_StringToText(toW(name_).c_str());

        if (!desc_.empty())
            recipe->Description = UKismetTextLibrary::Conv_StringToText(toW(desc_).c_str());

        if (time_ > 0.0f)
            recipe->CraftingTime = time_;

        if (!cat_.empty()) {
            auto* cat = reinterpret_cast<UUWECraftingRecipeCategory*>(
                UObjectGlobals::FindObject(L"UWECraftingRecipeCategory",
                    (L"DA_" + toW(cat_)).c_str()));
            if (cat)
                recipe->Category = SDK::TSoftObjectPtr<UUWECraftingRecipeCategory>(
                    UKismetSystemLibrary::Conv_ObjectToSoftObjectReference(cat));
        }

        // ── 4. Set ingredients ───────────────────────────────────────────────
        auto* reqs = reinterpret_cast<TArray<FCraftingRecipeRequirement>*>(&recipe->Requirements);
        reqs->SetNum(0, EAllowShrinking::Yes);
        reqs->ResizeTo(static_cast<int32_t>(ingredients_.size()));

        for (const auto& [itemId, count] : ingredients_) {
            auto* item = UObjectGlobals::FindObject(
                L"UWEItemType", (L"DA_" + toW(itemId) + L"_ItemType").c_str());
            if (!item) {
                RC::Output::send<RC::LogLevel::Warning>(
                    STR("[CustomCraft] Ingredient '{}' not found — skipped\n"), toW(itemId));
                continue;
            }
            reqs->Add(FCraftingRecipeRequirement{
                .ItemType = UKismetSystemLibrary::Conv_ObjectToSoftObjectReference(item),
                .NumItems = count,
            });
        }

        // ── 5. Set outputs ───────────────────────────────────────────────────
        auto* outs = reinterpret_cast<TArray<FCraftingRecipeOutput>*>(&recipe->Output);
        outs->SetNum(0, EAllowShrinking::Yes);
        outs->ResizeTo(static_cast<int32_t>(outputs_.size()));

        for (const auto& [itemId, count] : outputs_) {
            auto* item = UObjectGlobals::FindObject(
                L"UWEItemType", (L"DA_" + toW(itemId) + L"_ItemType").c_str());
            if (!item) {
                RC::Output::send<RC::LogLevel::Warning>(
                    STR("[CustomCraft] Output '{}' not found — skipped\n"), toW(itemId));
                continue;
            }
            outs->Add(FCraftingRecipeOutput{
                .ItemType = UKismetSystemLibrary::Conv_ObjectToSoftObjectReference(item),
                .NumItems = count,
                .CollisionCheckSize = SDK::FVector(0.f, 0.f, 0.f),
            });
        }

        // ── 6. Register with the hook ────────────────────────────────────────
        CraftHook::recipes.push_back(recipe);

        RC::Output::send<RC::LogLevel::Verbose>(
            STR("[CustomCraft] Recipe '{}' registered ({} ingredient(s), {} output(s))\n"),
            toW(id_),
            static_cast<int>(ingredients_.size()),
            static_cast<int>(outputs_.size()));

        return recipe;
    }

private:
    struct Entry { std::string itemId; int32_t count; };

    std::string         id_, name_, desc_, cat_;
    float               time_ = 0.0f;
    std::vector<Entry>  ingredients_;
    std::vector<Entry>  outputs_;
};
