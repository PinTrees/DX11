#pragma once
#include <functional>
#include "Component.h"

class ComponentFactory
{
    using CreateComponentFn = std::function<std::shared_ptr<Component>()>;

private:
    ComponentFactory() = default;
    std::unordered_map<std::string, CreateComponentFn> m_FactoryMap;
    std::vector<HMODULE> m_LoadedLibraries;

public:
    static ComponentFactory& Instance()
    {
        static ComponentFactory instance;
        return instance;
    }

    bool RegisterComponent(const std::string& type, CreateComponentFn fn)
    {
        m_FactoryMap[type] = fn;
        return true;
    }

    std::shared_ptr<Component> CreateComponent(const std::string& type)
    {
        auto it = m_FactoryMap.find(type);
        if (it != m_FactoryMap.end())
        {
            return it->second();
        }
        return nullptr;
    }

    std::vector<std::string> GetComponentTypes() const
    {
        std::vector<std::string> types;
        for (const auto& pair : m_FactoryMap)
        {
            types.push_back(pair.first);
        }
        return types;
    }
};

