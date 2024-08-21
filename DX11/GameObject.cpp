#include "pch.h"
#include "GameObject.h"
#include "Transform.h"

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
    // �ڽ� ��ü�� nullptr���� Ȯ��
    if (child == nullptr)
    {
        return;
    }

    // �ڽ� ��ü�� m_pChildGameObjects ���Ϳ��� ã��
    auto it = std::find(m_pChildGameObjects.begin(), m_pChildGameObjects.end(), child);

    // �ڽ� ��ü�� ���Ϳ� �����ϴ� ��쿡�� ����
    if (it != m_pChildGameObjects.end())
    {
        m_pChildGameObjects.erase(it);  // �ڽ� ��ü ����
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
    // ���� ������Ʈ �̸� ���� ��ǲ �ʵ�
    if (ImGui::InputText("Name", &m_Name[0], m_Name.capacity() + 1))
    {
        // �̸� ���� �� �ʿ��� �߰� �۾��� ������ ���⿡ �߰�
    }

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
        { "children", json::array() }  // �ڽ� ��ü���� ������ �迭 �߰�
    };

    // �� ������Ʈ�� JSON���� ��ȯ�Ͽ� components �迭�� �߰�
    for (const std::shared_ptr<Component>& component : obj.m_Components)
    {
        j["components"].push_back(component->toJson());
    }

    // �ڽ� GameObject���� ��������� JSON�� �߰�
    for (const GameObject* child : obj.m_pChildGameObjects) 
    {
        json childJson;
        to_json(childJson, *child);  // �ڽ� ��ü�� JSON���� ��ȯ
        j["children"].push_back(childJson);  // children �迭�� �߰�
    }
}

void from_json(const json& j, GameObject& obj)
{
    obj.m_Name = j.at("name").get<std::string>();

    // ������Ʈ ����
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
            // ComponentFactory�� ����Ͽ� ������Ʈ ����
            component = ComponentFactory::Instance().CreateComponent(type);
            component->fromJson(compJson);
            obj.AddComponent(component);
        }
    }

    // �ڽ� GameObject ����
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