-- InfiniteHealth — Subnautica 2 mod via UE4SS Lua
-- Restaure la santé à 100% toutes les INTERVAL_MS millisecondes.
--
-- Pattern identique à sub2_chaos/cheats.lua :
--   FindFirstOf("PlayerController") → pc.CheatManager → MyHealth(100)
-- Référence : https://github.com/caesarakalaeii/sub2_chaos

local INTERVAL_MS = 500

-- Retrouve le SN2CheatManager via PlayerController ou FindFirstOf en fallback.
-- Retourne nil si on est dans le menu principal ou si aucun CheatManager n'existe.
local function find_cheat_manager()
    local pc = FindFirstOf("PlayerController")
    if pc then
        -- Exclure UWEFrontendPlayerController (menu principal)
        local name_ok, full_name = pcall(function() return pc:GetFullName() end)
        if name_ok and full_name and full_name:find("Frontend") then
            return nil
        end

        local ok, cm = pcall(function() return pc.CheatManager end)
        if ok and cm then
            local valid_ok, valid = pcall(function() return cm:IsValid() end)
            if valid_ok and valid then return cm end
        end
    end

    -- Fallback direct (même ordre que cheats.lua)
    return FindFirstOf("SN2CheatManager") or FindFirstOf("CheatManager")
end

local function restore_health()
    local cm = find_cheat_manager()
    if not cm then return end

    -- SN2CheatManager::MyHealth(float) — valeur entre 0 et 100
    -- Confirmé fonctionnel dans sub2_chaos (Ctrl+PageDown)
    pcall(function()
        local fn = cm["MyHealth"]
        if fn then fn(cm, 100) end
    end)
end

local function health_loop()
    restore_health()
    ExecuteWithDelay(INTERVAL_MS, health_loop)
end

-- Attendre que le jeu soit initialisé avant de démarrer la boucle
ExecuteWithDelay(5000, function()
    print("[InfiniteHealth] Boucle de restauration démarrée (intervalle : " .. INTERVAL_MS .. "ms)")
    health_loop()
end)

print("[InfiniteHealth] v1.0.0 chargé")
