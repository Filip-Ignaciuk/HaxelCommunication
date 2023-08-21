#include "gui.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

// Credit to https://www.youtube.com/watch?v=Nrta_J_c9Cc, for allowing me to
// Start making ImGui applications

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);

// Handles all the events windows sends to your windows.
long __stdcall WindowProcess(HWND _window, UINT _message, WPARAM _wideParameter, LPARAM _longParameter)
{
	if(ImGui_ImplWin32_WndProcHandler(_window, _message, _wideParameter, _longParameter))
	{
		return true;
	}

	switch (_message)
	{
	case WM_SIZE:
	{
		if (gui::device && _wideParameter != SIZE_MINIMIZED)
		{
			gui::presentParamters.BackBufferWidth = LOWORD(_longParameter);
			gui::presentParamters.BackBufferHeight = LOWORD(_longParameter);
			gui::ResetDevice();
		}
	}return 0;

	case WM_SYSCOMMAND:
	{
		if ((_wideParameter & 0xfff0) == SC_KEYMENU) // This disables the ALT application menu
		{
			return 0;
		}
	}break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN:
	{
		gui::position = MAKEPOINTS(_longParameter);
	}return 0;
	case WM_MOUSEMOVE:
	{
		if (_wideParameter == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(_longParameter);
			auto rect = ::RECT{ };

			GetWindowRect(gui::window, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 &&
				gui::position.x <= gui::WIDTH &&
				gui::position.y >= 0 && gui::position.y <= 19)
			{
				SetWindowPos(gui::window, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
			}

		}
	}return 0;

	}
	

	return DefWindowProcW(_window, _message, _wideParameter, _longParameter); 
}

void gui::CreateHWindow(const char* windowName,const char* className) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = (WNDPROC)WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassExA(&windowClass);

	window = CreateWindowA(className, windowName, WS_POPUP, 100, 100, WIDTH, HEIGHT, 0, 0, windowClass.hInstance, 0);
	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}
void gui::DestroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);

}

bool gui::CreateDevice() noexcept
{
	// Initialise direct 9
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if(!d3d)
	{
		return false;
	}

	// Uses windows api to create space for our present parameters.
	ZeroMemory(&presentParamters, sizeof(presentParamters));

	presentParamters.Windowed = true;
	presentParamters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParamters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParamters.EnableAutoDepthStencil = TRUE;
	presentParamters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParamters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParamters, &device) < 0)
	{
		return false;
	}

	return true;
}
void gui::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParamters);

	if(result == D3DERR_INVALIDCALL)
	{
		IM_ASSERT(0);
	}

	ImGui_ImplDX9_CreateDeviceObjects();
}
void gui::DestroyDevice() noexcept
{
	if(device)
	{
		device->Release();
		device = nullptr;
	}
	if(d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}
}

void gui::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}
void gui::DestroyImGui() noexcept
{
	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX9_Shutdown();
	ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
	MSG message;
	while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
void gui::EndRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if(device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	if(result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ResetDevice();
	}

}
void gui::Render() noexcept
{
	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin("Nice	", &exit, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
	ImGui::Button("Nice");

	ImGui::End();
}