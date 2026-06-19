#pragma once
#include <vector>
#include <memory>
#include <cstdint>

// Dumper-7 generated SDK — required
#include "SDK/Subnautica2_classes.hpp"
#include "polyhook2/Detours/x64Detour.hpp"

// Manages the detour on USN2AssetRegistry::GetAllCraftingRecipes.
// When the game queries the recipe list, our hook appends the custom
// recipes stored in `recipes` before returning the array.
class CraftHook {
public:
    // All registered custom recipes — populate before calling install()
    static std::vector<SDK::UUWECraftingRecipe*> recipes;

    static void install();
    static void uninstall();

private:
    // Signature of the internal (non-UFunction) GetAllCraftingRecipes
    using GetRecipesFn = RC::Unreal::TArray<SDK::UUWECraftingRecipe*>(*)();

    static GetRecipesFn              s_original;
    static std::unique_ptr<PLH::x64Detour> s_hook;

    // Replacement function — calls original then appends our recipes
    static RC::Unreal::TArray<SDK::UUWECraftingRecipe*> hooked();

    // Scans from `addr` for the Nth 0xE8 CALL instruction and returns its target.
    // Used to skip the UFunction exec wrapper and reach the real C++ function.
    static uintptr_t scanCall(uintptr_t addr, int ordinal);
};
