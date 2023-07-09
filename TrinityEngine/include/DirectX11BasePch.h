// System includes
#include <windows.h>

// Direct3D includes
#include <d3d11.h>       // D3D interface
#include <dxgi.h>        // DirectX driver interface
#include <d3dcompiler.h> // shader compiler
#include <DirectXMath.h> // include primitive math libraries
#include <DirectXColors.h>

// Link library dependencies
#pragma comment(lib, "winmm.lib")
#pragma comment( lib, "user32" )          // link against the win32 library
#pragma comment( lib, "d3d11.lib" )       // direct3D library
#pragma comment( lib, "dxgi.lib" )        // directx graphics interface
#pragma comment( lib, "d3dcompiler.lib" ) // shader compiler

// STL includes
#include <iostream>
#include <string>

// Main application includes
#include "EngineMath.h"

// Some assertion defines
#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

static int ToStringHash(std::string objectName)
{
	return std::hash<std::string>{}(objectName);
}


template< typename T >
inline void SafeRelease(T& ptr)
{
	// Call this to safely release a COM object and set the pointer to NULL
	if (ptr != NULL)
	{
		ptr->Release();
		ptr = NULL;
	}
}

template< typename T >
inline void SafeReleaseTemplate(T& ptr)
{
	// Call this to safely release a COM object and set the pointer to NULL
	if (ptr != NULL)
	{
		delete ptr;
		ptr = NULL;
	}
}