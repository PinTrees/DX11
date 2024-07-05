#include "pch.h"
#include "GameObject.h"
#include "Transform.h"

atomic<uint64> GameObject::g_NextInstanceID = 0;

GameObject::GameObject()
	: m_InstanceID(g_NextInstanceID++),
	m_LayerIndex(0),
	m_pParentGameObject(nullptr)
{
    AddComponent<Transform>();
}

GameObject::GameObject(const string& name)
	: m_Name(name),
	m_InstanceID(g_NextInstanceID++),
	m_LayerIndex(0),
	m_pParentGameObject(nullptr)
{
	AddComponent<Transform>();
}

GameObject::~GameObject()
{
}

void GameObject::SetParent(GameObject* parent)
{
	m_pParentGameObject = parent;
	parent->SetChild(this);
}

void GameObject::SetChild(GameObject* child)
{
	m_pChildGameObjects.push_back(child);
}

void GameObject::Awake()
{
}

void GameObject::Start()
{
}

void GameObject::Update()
{
}

void GameObject::LateUpdate()
{
}

void GameObject::FixedUpdate()
{
}

void GameObject::ApplyPendingComponents()
{
    for (const auto& component : m_ComponentsToAdd)
    {
        AddComponent(component);
    }
    m_ComponentsToAdd.clear();
}

void GameObject::OnInspectorGUI()
{
    for (const auto& component : m_Components)
    {
        component->OnInspectorGUI();
        ImGui::Separator();
    }

    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("add_component_popup");
    }

    if (ImGui::BeginPopup("add_component_popup"))
    {
        std::vector<std::string> componentTypes = ComponentFactory::Instance().GetComponentTypes();
        for (const auto& type : componentTypes) 
        {
            if (ImGui::MenuItem(type.c_str())) 
            {
                auto newComponent = ComponentFactory::Instance().CreateComponent(type);
                if (newComponent) 
                { 
                    m_ComponentsToAdd.push_back(newComponent);
                }
            }
        }
        ImGui::EndPopup(); 
    }
}

void to_json(json& j, const GameObject& obj)
{
    j = json{
        { "name", obj.m_Name },
        { "components", json::array() }
    };
    for (const shared_ptr<Component>& component : obj.m_Components)
    {
        j["components"].push_back(component->toJson());
    }
}

void from_json(const json& j, GameObject& obj)
{
    obj.m_Name = j.at("name").get<std::string>();
    for (const auto& compJson : j.at("components"))
    {
        if (compJson.is_null())
        {
            continue;
        }

        std::string type = compJson.at("type").get<std::string>();
        shared_ptr<Component> component = nullptr;
        if (type == "Transform")
        {
            Component* pComponent = component.get();
            pComponent = obj.GetComponent<Transform>();
            pComponent->fromJson(compJson); 
        }

        if (component)
        {
            component->fromJson(compJson);
            obj.m_Components.push_back(component);
        }
    }
}