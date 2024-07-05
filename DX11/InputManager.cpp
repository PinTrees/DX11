#include "pch.h"
#include "InputManager.h"

SINGLE_BODY(InputManager)

int g_arrVK[(int)KEY::LAST] =
{
	VK_LEFT,
	VK_RIGHT,
	VK_UP,
	VK_DOWN,

	'A',
	'B',
	'C',
	'D',
	'E',
	'F',
	'G',
	'H',
	'I',
	'J',
	'K',
	'L',
	'M',
	'N',
	'O',
	'P',
	'Q',
	'R',
	'S',
	'T',
	'U',
	'V',
	'W',
	'X',
	'Y',
	'Z',

	VK_MENU,	   	//ALT,
	VK_CONTROL,	   	//CTRL,
	VK_LSHIFT,	   	//LSHIFT,
	VK_SPACE,	   	//SPACE,
	VK_RETURN,	   	//ENTER
	VK_ESCAPE,   	//ESC,

	VK_LBUTTON,
	VK_RBUTTON,

	//
	//LAST
};

InputManager::InputManager()
{

}
InputManager::~InputManager()
{

}

void InputManager::Init()
{
	//�ʱ�ȭ �ݺ��� 
	for (int i = 0; i < (int)KEY::LAST; ++i)
	{
		m_vecKey.push_back(tKeyInfo{ KEY_STATE::NONE,false });
	}
}

void InputManager::Update()
{
	// ������ ��Ŀ�� �˾Ƴ���

	//HWND hMainWnd = CCore::GetI()->GetMainHwnd();
	HWND hWnd = GetFocus();

	// ������ ��Ŀ�� ���϶� Ű �̺�Ʈ ����
	if (nullptr != hWnd)
	{

		for (int i = 0; i < (int)KEY::LAST; ++i)
		{
			if (GetAsyncKeyState(g_arrVK[i]) & 0x8000)
			{
				if (m_vecKey[i].bPrevPush)
				{
					//�������� �����־�����
					m_vecKey[i].eState = KEY_STATE::HOLD;
				}
				else
				{
					m_vecKey[i].eState = KEY_STATE::TAP;
				}
				//���������� �����־����� true
				m_vecKey[i].bPrevPush = true;
			}
			else// Ű�� �ȴ����־�����
			{
				if (m_vecKey[i].bPrevPush)
				{
					// ������ �����־���.
					m_vecKey[i].eState = KEY_STATE::AWAY;
				}
				else
				{
					m_vecKey[i].eState = KEY_STATE::NONE;

				}
				m_vecKey[i].bPrevPush = false;
			}
		}
		//Mouse ��ġ ���
		POINT ptPos = {};
		GetCursorPos(&ptPos);
		ScreenToClient(Application::GetI()->GetMainHwnd(), &ptPos);

		mvCurMousePos = Vec2((float)ptPos.x, (float)ptPos.y);
	}
	// ������ ��Ŀ�� ���� ����
	else
	{
		for (int i = 0; i < (int)KEY::LAST; ++i)
		{
			m_vecKey[i].bPrevPush = false;

			if (KEY_STATE::TAP == m_vecKey[i].eState || KEY_STATE::HOLD == m_vecKey[i].eState)
			{
				m_vecKey[i].eState = KEY_STATE::AWAY;
			}
			else if (KEY_STATE::AWAY == m_vecKey[i].eState)
			{
				m_vecKey[i].eState = KEY_STATE::NONE;
			}
		}
	}
}

