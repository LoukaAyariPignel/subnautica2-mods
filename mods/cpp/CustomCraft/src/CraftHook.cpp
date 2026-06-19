#include "CraftHook.hpp"

#include <DynamicOutput/DynamicOutput.hpp>
#include "SDK/Subnautica2_classes.hpp"

using namespace RC;
using namespace RC::Unreal;
using namespace SDK;

// ─── Statics ────────────────────────────────────────────────────────────────

std::vector<UUWECraftingRecipe*>      CraftHook::recipes;
CraftHook::GetRecipesFn               CraftHook::s_original = nullptr;
std::unique_ptr<PLH::x64Detour>       CraftHook::s_hook;

// ─── Hook implementation ─────────────────────────────────────────────────────

// Called instead of the original GetAllCraftingRecipes.
// We get the real list, resize it, then append our custom recipes.
TArray<UUWECraftingRecipe*> CraftHook::hooked()
{
    auto entries = s_original();
    entries.ResizeTo(entries.Num() + static_cast<int32_t>(recipes.size()));
    for (auto* r : recipes)
        entries.Add(r);
    return entries;
}

// Walks the bytes at `addr` counting CALL (0xE8) instructions.
// When the Nth one is found, resolves and returns its target address.
// SN2CustomCraft confirmed ordinal=1 for GetAllCraftingRecipes.
uintptr_t CraftHook::scanCall(uintptr_t addr, int ordinal)
{
    while (true) {
        if (*reinterpret_cast<uint8_t*>(addr) == 0xE8) {
            if (--ordinal == 0) {
                const int32_t rel = *reinterpret_cast<int32_t*>(addr + 1);
                return addr + rel + 5;
            }
        }
        ++addr;
    }
}

// ─── Public API ──────────────────────────────────────────────────────────────

void CraftHook::install()
{
    // Locate the UFunction through the asset registry class
    const auto* uclass = USN2AssetRegistry::StaticClass();
    if (!uclass) {
        Output::send<LogLevel::Error>(STR("[CustomCraft] USN2AssetRegistry class not found\n"));
        return;
    }

    const auto* fn = uclass->GetFunction(STR("SN2AssetRegistry"), STR("GetAllCraftingRecipes"));
    if (!fn || !fn->ExecFunction) {
        Output::send<LogLevel::Error>(STR("[CustomCraft] GetAllCraftingRecipes UFunction not found\n"));
        return;
    }

    // ExecFunction is the UFunction wrapper; the real C++ implementation
    // is the 1st CALL inside it (confirmed by SN2CustomCraft analysis).
    const auto wrapperPtr  = reinterpret_cast<uintptr_t>(*fn->ExecFunction);
    const auto internalPtr = scanCall(wrapperPtr, 1);

    Output::send<LogLevel::Verbose>(
        STR("[CustomCraft] Hooking GetAllCraftingRecipes at 0x{:X}\n"), internalPtr);

    s_hook = std::make_unique<PLH::x64Detour>(
        internalPtr,
        reinterpret_cast<uint64_t>(&CraftHook::hooked),
        reinterpret_cast<uint64_t*>(&s_original));

    if (s_hook->hook())
        Output::send<LogLevel::Verbose>(STR("[CustomCraft] Hook installed\n"));
    else
        Output::send<LogLevel::Error>(STR("[CustomCraft] Hook FAILED\n"));
}

void CraftHook::uninstall()
{
    if (s_hook)
        s_hook->unHook();
}
