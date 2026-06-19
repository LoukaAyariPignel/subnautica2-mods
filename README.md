# Subnautica 2 — Mod Repository

Collection de mods pour **Subnautica 2** utilisant [UE4SS](https://github.com/UE4SS-RE/RE-UE4SS) (Unreal Engine Scripting System).  
Supporte les mods **C++ (DLL)** et **Lua (scripts)**.

---

## Structure

```
subnautica2/
├── mods/
│   ├── cpp/                  # Mods C++ compilés en DLL
│   │   ├── ExampleMod/       # Template de départ
│   │   └── InfiniteOxygen/   # Oxygène infini
│   └── lua/                  # Mods Lua interprétés par UE4SS
│       ├── ExampleLuaMod/    # Template de départ
│       └── SpeedBoost/       # Vitesse de nage multipliée
├── shared/
│   └── include/
│       └── SN2Helpers.hpp    # Utilitaires partagés (log, FindObject…)
├── vendor/
│   └── UE4SS/                # SDK UE4SS (non versionné — voir ci-dessous)
├── scripts/
│   ├── fetch_ue4ss.bat       # Télécharge le SDK UE4SS
│   ├── build.bat             # Compile tous les mods C++
│   └── install_mod.bat       # Installe un mod dans le jeu
├── CMakeLists.txt
└── .gitignore
```

---

## Prérequis

- **UE4SS v3.0+** installé dans le jeu (`Subnautica2/Mods/` ou via xinput1_4.dll)
- **Visual Studio 2022** (MSVC v143, C++20)
- **CMake 3.25+**
- **Git**

---

## Démarrage rapide

### 1. Cloner et préparer le SDK

```bat
git clone <url-de-ce-repo>
cd subnautica2
scripts\fetch_ue4ss.bat
```

### 2. Compiler les mods C++

```bat
scripts\build.bat
```

Les DLLs sont générées dans `out/<NomDuMod>/<NomDuMod>.dll`.

### 3. Installer un mod

```bat
scripts\install_mod.bat InfiniteOxygen "C:\...\Subnautica2"
scripts\install_mod.bat SpeedBoost     "C:\...\Subnautica2"
```

---

## Créer un nouveau mod

### Mod Lua (rapide)

1. Copier `mods/lua/ExampleLuaMod/` vers `mods/lua/MonMod/`
2. Éditer `mod.manifest` (nom, description)
3. Éditer `Scripts/main.lua`
4. Installer avec `install_mod.bat MonMod "<chemin_jeu>"`

### Mod C++ (avancé)

1. Copier `mods/cpp/ExampleMod/` vers `mods/cpp/MonMod/`
2. Ajouter `add_subdirectory(mods/cpp/MonMod)` dans `CMakeLists.txt`
3. Éditer `mod.manifest` et `src/main.cpp`
4. `scripts\build.bat` puis `install_mod.bat MonMod "<chemin_jeu>"`

---

## Mods disponibles

| Nom | Type | Description |
|-----|------|-------------|
| ExampleMod | C++ | Template de départ |
| InfiniteOxygen | C++ | Supprime la consommation d'O2 |
| ExampleLuaMod | Lua | Template de départ |
| SpeedBoost | Lua | Multiplie la vitesse de nage (x2.5) |

---

## Ressources

- [UE4SS Documentation](https://docs.ue4ss.com)
- [UE4SS Releases](https://github.com/UE4SS-RE/RE-UE4SS/releases)
- [Lua API UE4SS](https://docs.ue4ss.com/lua-api/)
