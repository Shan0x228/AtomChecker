#pragma once

#include "Application.h"
#include <d2d1.h>
#include <dwrite.h>
#include "UiRenderer.h"

class D2D1_Wrapper
{
private:
	ID2D1Factory* m_pD2D1Factory{};
	IDWriteFactory* m_pDWriteFactory{};
	HRESULT hr{};

	friend class UiRenderer;

public:
	// CTOR
	D2D1_Wrapper();
	
	template<typename T>
	static void FreeD2D1Obj(T& obj)
	{
		if (obj != nullptr)
			obj->Release();
		obj = nullptr;
	}

	// DTOR
	~D2D1_Wrapper();
};