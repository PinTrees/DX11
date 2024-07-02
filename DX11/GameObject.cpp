#include "pch.h"
#include "GameObject.h"

atomic<uint64> GameObject::g_NextInstanceID = 0;

GameObject::GameObject()
	: m_InstanceID(g_NextInstanceID++),
	m_LayerIndex(0),
	m_pParentGameObject(nullptr)
{
}

GameObject::GameObject(const string& name)
	: m_Name(name),
	m_InstanceID(g_NextInstanceID++),
	m_LayerIndex(0),
	m_pParentGameObject(nullptr)
{
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


