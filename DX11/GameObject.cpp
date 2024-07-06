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
    // 게임 오브젝트 이름 변경 인풋 필드
    if (ImGui::InputText("Name", &m_Name[0], m_Name.capacity() + 1))
    {
        // 이름 변경 시 필요한 추가 작업이 있으면 여기에 추가
    }

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
    j = json
    {
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
        else
        {
            // ComponentFactory를 사용하여 컴포넌트 생성
            component = ComponentFactory::Instance().CreateComponent(type);
            component->fromJson(compJson);
            obj.AddComponent(component);
        }
    }
}