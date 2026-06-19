#include "CraftHook.hpp"

#include <DynamicOutput/DynamicOutput.hpp>
#include "SDK/UWECrafting_classes.hpp"

using namespace RC;
using namespace RC::Unreal;
using namespace SDK;

// ─── Statics ────────────────────────────────────────────────────────────────

std::vector<UUWECraftingRecipe*>   CraftHook::recipes;
CraftHook::GetRecipesFn            CraftHook::s_original = nullptr;
std::unique_ptr<PLH::x64Detour>    CraftHook::s_hook;

// ─── Hook ────────────────────────────────────────────────────────────────────

TArray<UUWECraftingRecipe*> CraftHook::hooked()
{
    auto entries = s_original();
    entries.ResizeTo(entries.Num() + static_cast<int32_t>(recipes.size()));
    for (auto* r : recipes)
        entries.Add(r);
    return entries;
}

// Parcourt les opcodes à partir de `addr` et retourne la cible du Nième
// CALL relatif (opcode 0xE8). Utilisé pour passer le wrapper UFunction
// exec et atteindre l'implémentation C++ native.
// Ordinal 1 confirmé pour GetAllCraftingRecipes par SN2CustomCraft.
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

// ─── install() ───────────────────────────────────────────────────────────────

void CraftHook::install()
{
    // Trouver une instance de SN2AssetRegistry pour accéder à sa UClass
    auto* reg = UObjectGlobals::FindFirstOf(STR("SN2AssetRegistry"));
    if (!reg) {
        Output::send<LogLevel::Error>(STR("[CustomCraft] SN2AssetRegistry instance not found\n"));
        return;
    }

    const auto* uclass = reg->GetClassPrivate();
    if (!uclass) {
        Output::send<LogLevel::Error>(STR("[CustomCraft] SN2AssetRegistry UClass is null\n"));
        return;
    }

    // Trouver la UFunction GetAllCraftingRecipes
    const auto* fn = uclass->GetFunctionByNameInChain(STR("GetAllCraftingRecipes"));
    if (!fn || !fn->ExecFunction) {
        Output::send<LogLevel::Error>(
            STR("[CustomCraft] GetAllCraftingRecipes UFunction not found\n"));
        return;
    }

    // Passer le wrapper exec pour atteindre la vraie implémentation
    const auto wrapperPtr  = reinterpret_cast<uintptr_t>(*fn->ExecFunction);
    const auto internalPtr = scanCall(wrapperPtr, 1);

    Output::send<LogLevel::Verbose>(
        STR("[CustomCraft] GetAllCraftingRecipes internal @ 0x{:X}\n"), internalPtr);

    s_hook = std::make_unique<PLH::x64Detour>(
        internalPtr,
        reinterpret_cast<uint64_t>(&CraftHook::hooked),
        reinterpret_cast<uint64_t*>(&s_original));

    if (s_hook->hook())
        Output::send<LogLevel::Verbose>(STR("[CustomCraft] Hook OK\n"));
    else
        Output::send<LogLevel::Error>(STR("[CustomCraft] Hook FAILED\n"));
}

void CraftHook::uninstall()
{
    if (s_hook) s_hook->unHook();
}
