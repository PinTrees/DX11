#pragma once

#define SINGLE_BODY(type) type* type::pInst = nullptr;
#define SINGLE_HEADER(type) private:						\
						static type* pInst;					\
					 public:								\
						static type* GetI()					\
						{									\
							if(pInst == nullptr)			\
								pInst = new type;			\
							return pInst;					\
						}									\
						static void Dispose()				\
						{									\
							if(pInst != nullptr)			\
								 delete pInst;				\
							pInst = nullptr;				\
						}									\
						private:							\
								type();						\
								~type();



#define DT TimeManager::GetI()->GetDT();

#define INPUT_CHECK(key , state) InputManager::GetI()->GetKeyState(key) == state
#define INPUT_KEY_HOLD(key) INPUT_CHECK(key,KEY_STATE::HOLD)
#define INPUT_KEY_DOWN(key) INPUT_CHECK(key, KEY_STATE::TAP)
#define INPUT_KEY_UP(key)	INPUT_CHECK(key,KEY_STATE::AWAY)
#define INPUT_KEY_NONE(key) INPUT_CHECK(key,KEY_STATE::NONE)

#define	MOUSE_POSITION InputManager::GetI()->GetMousePos()