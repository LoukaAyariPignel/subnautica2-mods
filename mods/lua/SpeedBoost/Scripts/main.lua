local MOD_NAME    = "SpeedBoost"
local MOD_VERSION = "1.0.0"
local SPEED_MULTIPLIER = 2.5  -- modifier ici

print(string.format("[%s] v%s — multiplicateur x%.1f", MOD_NAME, MOD_VERSION, SPEED_MULTIPLIER))

-- Hook sur la fonction qui définit la vitesse de nage.
-- Le nom exact doit être vérifié via UE4SS Object Dumper.
RegisterHook("/Script/Subnautica.SwimMovementComponent:GetSwimSpeed",
    function(self, ReturnValue)
        local base = ReturnValue:get()
        ReturnValue:set(base * SPEED_MULTIPLIER)
    end
)
