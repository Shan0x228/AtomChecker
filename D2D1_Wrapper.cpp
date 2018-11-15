#include "D2D1_Wrapper.h"

// CTOR
D2D1_Wrapper::D2D1_Wrapper()
{
	this->hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &this->m_pD2D1Factory);
	if (!SUCCEEDED(this->hr))
		ExitProcess(EXIT_FAILURE);

	this->hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(m_pDWriteFactory), reinterpret_cast<IUnknown **>(&m_pDWriteFactory));
	if (!SUCCEEDED(this->hr))
		ExitProcess(EXIT_FAILURE);
}

// DTOR
D2D1_Wrapper::~D2D1_Wrapper()
{
	D2D1_Wrapper::FreeD2D1Obj(this->m_pDWriteFactory);
	D2D1_Wrapper::FreeD2D1Obj(this->m_pD2D1Factory);
}
