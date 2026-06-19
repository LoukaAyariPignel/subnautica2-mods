local MOD_NAME    = "ExampleLuaMod"
local MOD_VERSION = "1.0.0"

print(string.format("[%s] v%s chargé", MOD_NAME, MOD_VERSION))

-- ─── Utilitaires ────────────────────────────────────────────────────────────

local function log(msg)
    print(string.format("[%s] %s", MOD_NAME, msg))
end

-- ─── Exemple : hook sur une fonction Blueprint ──────────────────────────────
-- Remplace le chemin par le vrai chemin de fonction une fois les assets dumpés.
--
-- RegisterHook("/Script/Subnautica.PlayerController:BeginPlay",
--     function(self)
--         log("BeginPlay joueur déclenché")
--     end
-- )

-- ─── Exemple : exécution différée ───────────────────────────────────────────
ExecuteWithDelay(2000, function()
    log("Mod initialisé (2s après le chargement)")
end)

-- ─── Exemple : boucle toutes les 5 secondes ─────────────────────────────────
local function onTick()
    -- log("Tick")
    ExecuteWithDelay(5000, onTick)
end
-- onTick()  -- décommenter pour activer
