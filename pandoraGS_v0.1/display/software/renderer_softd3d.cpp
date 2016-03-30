/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   renderer_softd3d.cpp
    Description : software rendering engine (Direct3D WARP)
*******************************************************************************/
using namespace std;
#include "../../globals.h"
#if _SYSTEM == _WINDOWS
#include <windowsx.h>
#include "../../gpu_main.h"
#include "renderer_softd3d.h"
extern HWND  gpu_window;

// DX SDK June 2010
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

// global DX variables
IDXGISwapChain* softSwapChain;      // swap chain interface
ID3D11Device*   softDevice;         // device interface
ID3D11DeviceContext* softDeviceCtx; // device context
ID3D11RenderTargetView* softBackbuffer;
#define getDxComId(OBJECT) __uuidof(OBJECT)

// DX versions
#define SOFT_VERSIONS_NUMBER 2
D3D_FEATURE_LEVEL softVersions[SOFT_VERSIONS_NUMBER] = 
{
    //D3D_FEATURE_LEVEL_11_0, // not compatible with WARP on win7/vista
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0 
};
D3D_FEATURE_LEVEL selectedSoftVersion;



// SOFTWARE RENDERER CREATION
//______________________________________________________________________________

/* software renderer constructor - initialize values ------------------------ */
RendererSoftwareD3D::RendererSoftwareD3D() : Renderer()
{
    softSwapChain  = NULL;
    softBackbuffer = NULL;
    softDeviceCtx  = NULL;
    softDevice     = NULL;
}

/* software renderer destructor --------------------------------------------- */
RendererSoftwareD3D::~RendererSoftwareD3D()
{
    destroyRenderer(); // free common allocated memory
}

/* initialize dynamic allocations ------------------------------------------- */
long RendererSoftwareD3D::init()
{
    return initCommon(); // common memory allocation
}


// SOFTWARE PIPELINE CREATION
//______________________________________________________________________________
    
/* software rendering pipeline preparation ---------------------------------- */
long RendererSoftwareD3D::setPipeline()
{
    Config* pConfig = ConfigManager::getInstance(); // get config singleton
    
    // create render window
    setPipelineCommon(false); // disable emulator menu and screensaver
    if (pConfig->isFullscreen()) // fullscreen mode
    {
        ShowWindow(gpu_window, SW_SHOWMAXIMIZED);
    }
    else // window mode
    {
        // get screen size
        int screenWidth  = GetSystemMetrics(SM_CXFULLSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
        if (screenWidth < DISPLAY_BASEWIDTH)   screenWidth  = 800;
        if (screenHeight < DISPLAY_BASEHEIGHT) screenHeight = 600;
        
        // show centered window
        ShowWindow(gpu_window, SW_SHOWNORMAL);
        MoveWindow(gpu_window, 
                   screenWidth/2 - pConfig->windowResX/2,
                   screenHeight/2 - pConfig->windowResY/2,
                   pConfig->windowResX + GetSystemMetrics(SM_CXFIXEDFRAME)+3,
                   pConfig->windowResY + GetSystemMetrics(SM_CYFIXEDFRAME)+3 
                                       + GetSystemMetrics(SM_CYCAPTION), TRUE);
        UpdateWindow(gpu_window);
    }
    
    // initialize black background
    return displayBaseBackground();
}

/* software rendering pipeline removal -------------------------------------- */
void RendererSoftwareD3D::unsetPipeline()
{
    // close graphic API
    unsetGraphicApi();
    
    // restore emulator menu and screensaver
    unsetPipelineCommon();
}

/* set software pixel format ------------------------------------------------ */
long RendererSoftwareD3D::setPixelFormat(HDC hDC)
{
    // get desktop color depth
    int bitsPerPixel = GetDeviceCaps(hDC, BITSPIXEL);
    if (bitsPerPixel <= 16 && bitsPerPixel > 1)
        ConfigManager::getInstance()->colorDepth = CFG_COLOR_16BIT;
    return 0;
}


/* initialize directdraw ---------------------------------------------------- */
long RendererSoftwareD3D::setGraphicApi()
{
    DXGI_SWAP_CHAIN_DESC scd;
    softSwapChain  = NULL;
    softBackbuffer = NULL;
    softDeviceCtx  = NULL;
    softDevice     = NULL;
    HRESULT result;

    // set swap chain information
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
    scd.BufferCount = 1;                                    // one back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // 32-bit colors
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // swap chain mode
    scd.OutputWindow = gpu_window;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    if (ConfigManager::getInstance()->isFullscreen())
        scd.Windowed = FALSE;
    else
        scd.Windowed = TRUE;

    // create device, context and swap chain
    result = D3D11CreateDeviceAndSwapChain(NULL,      // default GPU adapter
                                  D3D_DRIVER_TYPE_WARP, NULL, // WARP software mode
                                  D3D11_CREATE_DEVICE_BGRA_SUPPORT, // flags : enable Direct2D
                                  softVersions, SOFT_VERSIONS_NUMBER,
                                  D3D11_SDK_VERSION,
                                  &scd, &softSwapChain, &softDevice, 
                                  &selectedSoftVersion, &softDeviceCtx);
    if (FAILED(result))
    {
        unsetGpuStatus(gpuRawStatus, GPUSTATUS_IDLE);
        unsetGpuStatus(gpuRawStatus, GPUSTATUS_READYFORCOMMANDS);
        GPUclose();
        switch (result)
        {
            case DXGI_ERROR_UNSUPPORTED:
                MessageBox(NULL, L"Your system does not support DirectX 11.0", L"Error", MB_OK);
                break;
            case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE:
                MessageBox(NULL, L"DirectX is currently unavailable or busy", L"Error", MB_OK);
                break;
            default:
                MessageBox(NULL, L"DirectX unhandled initialization error", L"Error", MB_OK);
                break;
        }
        return -1;
    }
    
    // get back buffer address
    ID3D11Texture2D *pBackBuffer;
    result = softSwapChain->GetBuffer(0, getDxComId(ID3D11Texture2D), // 0 = buffer ID
                                      (LPVOID*)&pBackBuffer);
    if (FAILED(result))
    {
        unsetGpuStatus(gpuRawStatus, GPUSTATUS_IDLE);
        unsetGpuStatus(gpuRawStatus, GPUSTATUS_READYFORCOMMANDS);
        GPUclose();
        MessageBox(NULL, L"Invalid backbuffer address", L"Error", MB_OK);
        return -1;
    }
    // create render target
    result = softDevice->CreateRenderTargetView(pBackBuffer, NULL, &softBackbuffer);
    pBackBuffer->Release();
    if (FAILED(result))
    {
        unsetGpuStatus(gpuRawStatus, GPUSTATUS_IDLE);
        unsetGpuStatus(gpuRawStatus, GPUSTATUS_READYFORCOMMANDS);
        GPUclose();
        MessageBox(NULL, L"Render target creation failed", L"Error", MB_OK);
        return -1;
    }
    // set render target as back buffer
    softDeviceCtx->OMSetRenderTargets(1, &softBackbuffer, NULL); // 1 = number of buffers
    
    // set viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    if (ConfigManager::getInstance()->isFullscreen())
    {
        viewport.Width  = (float)ConfigManager::getInstance()->fullResX;
        viewport.Height = (float)ConfigManager::getInstance()->fullResY;
    }
    else
    {
        viewport.Width  = (float)ConfigManager::getInstance()->windowResX;
        viewport.Height = (float)ConfigManager::getInstance()->windowResY;
    }
    softDeviceCtx->RSSetViewports(1, &viewport); // 1 = number of viewports
    
    
    //! allouer éléments du menu (police, symboles, ...)
    //! préparer chainage d'effets/filtres/shaders
    //...
    
    _bApiState = true;
    return 0;
}

/* close directdraw --------------------------------------------------------- */
void RendererSoftwareD3D::unsetGraphicApi()
{
    // close pipeline
    if (_bApiState)
    {
        _bApiState = false;
        //...
        //! désallouer éléments du menu (police, symboles, ...)
        //...
    }
    else if (ConfigManager::getInstance()->isFullscreen())
        softSwapChain->SetFullscreenState(false, NULL);
    
    // release DX COM objects
    if (softSwapChain)  softSwapChain->Release();
    if (softBackbuffer) softBackbuffer->Release();
    if (softDeviceCtx)  softDeviceCtx->Release();
    if (softDevice)     softDevice->Release();
    softSwapChain  = NULL;
    softBackbuffer = NULL;
    softDeviceCtx  = NULL;
    softDevice     = NULL;
}


// SOFTWARE FRAME RENDERING
//______________________________________________________________________________

/**/
void RendererSoftwareD3D::render()
{
    // clear back buffer
    softDeviceCtx->ClearRenderTargetView(softBackbuffer, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f) );

    //...

    // switch back buffer and front buffer
    softSwapChain->Present(0, 0);
}

#endif
