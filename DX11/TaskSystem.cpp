#include "pch.h"
#include "TaskSystem.h"

queue<function<void()>> TaskSystem::mainThreadTasks = {};

void TaskSystem::ExecuteMainThreadTasks()
{
    while (!mainThreadTasks.empty())
    {
        mainThreadTasks.front()();  // �۾� ����
        mainThreadTasks.pop();      // ť���� ����
    }
}
