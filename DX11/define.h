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



