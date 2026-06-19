#pragma once
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/AActor.hpp>
#include <DynamicOutput/DynamicOutput.hpp>

namespace SN2 {

// Shorthand for UE4SS output
template<typename... Args>
void Log(std::wformat_string<Args...> fmt, Args&&... args) {
    RC::Output::send<RC::LogLevel::Verbose>(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void Warn(std::wformat_string<Args...> fmt, Args&&... args) {
    RC::Output::send<RC::LogLevel::Warning>(fmt, std::forward<Args>(args)...);
}

// Find first UObject of a given class by path
template<typename T>
T* FindObject(std::wstring_view path) {
    return static_cast<T*>(
        RC::Unreal::UObjectGlobals::StaticFindObject(nullptr, nullptr, path.data())
    );
}

} // namespace SN2
