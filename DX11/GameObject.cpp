#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "EditorGUI.h"

atomic<uint64> GameObject::g_NextInstanceID = 0;

GameObject::GameObject()
	: m_InstanceID(g_NextInstanceID++),
	m_LayerIndex(0),
	m_pParentGameObject(nullptr)
{
    m_pTransform = AddComponent<Transform>();
}

GameObject::GameObject(const string& name)
	: m_Name(name),
	m_InstanceID(g_NextInstanceID++),
	m_LayerIndex(0),
	m_pParentGameObject(nullptr)
{
    m_pTransform = AddComponent<Transform>();
}

GameObject::~GameObject()
{
}

void GameObject::SetParent(GameObject* parent)
{
    if (m_pParentGameObject == nullptr) 
    {
        SceneManager::GetI()->GetCurrentScene()->RemoveRootGameObjects(this); 
    }
    else 
    {
        m_pParentGameObject->RemoveChild(this); 
    }

    m_pParentGameObject = parent; 
    GetTransform()->SetParent(parent->GetComponent_SP<Transform>()); 
    parent->SetChild(this);

    GetTransform()->UpdateTransform();
}

void GameObject::SetChild(GameObject* child)
{
	m_pChildGameObjects.push_back(child);
    GetTransform()->AddChild(child->GetComponent_SP<Transform>());
}

void GameObject::RemoveChild(GameObject* child)
{
    if (child == nullptr)
    {
        return;
    }
    auto it = std::find(m_pChildGameObjects.begin(), m_pChildGameObjects.end(), child);
    if (it != m_pChildGameObjects.end())
    {
        m_pChildGameObjects.erase(it);  
    }
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
    ImGui::Dummy(ImVec2(0, 4));
    ImGui::Dummy(ImVec2(2, 0));
    ImGui::SameLine();
    EditorGUI::Checkbox();
    ImGui::SameLine();
    if (EditorGUI::InputField(m_Name))
    {
        // 이름 변경 시 필요한 추가 작업이 있으면 여기에 추가
    }
    ImGui::Dummy(ImVec2(0, 4));

    for (const auto& component : m_Components)
    {
        component->RenderInspectorGUI();
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
        { "components", json::array() },
        { "children", json::array() } 
    };

    for (const shared_ptr<Component>& component : obj.m_Components)
    {
        j["components"].push_back(component->toJson());
    }

    for (const GameObject* child : obj.m_pChildGameObjects)
    {
        json childJson;
        to_json(childJson, *child);  
        j["children"].push_back(childJson);  
    }
}

void from_json(const json& j, GameObject& obj)
{
    obj.m_Name = j.at("name").get<std::string>();

    // 컴포넌트 복원
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

    if (j.contains("children"))
    {
        for (const auto& childJson : j.at("children"))
        {
            GameObject* child = new GameObject;
            from_json(childJson, *child); 
            child->SetParentImmediate(&obj); 
            child->GetComponent<Transform>()->SetParent(obj.GetComponent_SP<Transform>()); 
            child->GetComponent<Transform>()->UpdateTransform(); 
            obj.SetChild(child); 
        }
    }
}