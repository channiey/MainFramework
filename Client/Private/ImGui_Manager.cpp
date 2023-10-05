#ifdef _DEBUG
#include "../Default/stdafx.h"

/* ImGui */
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "../ImGui/imgui.h" 
#include "../ImGui/imgui_impl_win32.h" 
#include "../ImGui/imgui_impl_dx11.h" 

/* Manager */
#include "ImGui_Manager.h"

/* Window */
#include "ImGui_Window.h"
#include "ImGui_Window_Main_Controller.h"
#include "ImGui_Window_Main_Object.h"
#include "ImGui_Window_Main_Hierarachy.h"
#include "ImGui_Window_Main_Demo.h"

/* Other Class */
#include "GameInstance.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImGui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	if (FAILED(ImGui_SetUp(pDevice, pContext)))
	{
		Safe_Release(m_pDevice);
		Safe_Release(m_pContext);
		return E_FAIL;
	}

	m_pMainWindows.reserve(WINDOW_MAIN_END);

	/* Create Main_Window Profiler */
	CImGui_Window* pWindow = CImGui_Window_Main_Controller::Create();
	NULL_CHECK_RETURN(pWindow, E_FAIL);
	pWindow->Set_Active(TRUE);
	m_pMainWindows.push_back(pWindow);

	/* Create Main_Window Object */
	pWindow = CImGui_Window_Main_Object::Create();
	NULL_CHECK_RETURN(pWindow, E_FAIL);
	pWindow->Set_Active(TRUE);
	m_pMainWindows.push_back(pWindow);

	/* Create Main_Window Hierarachy */
	pWindow = CImGui_Window_Main_Hierarachy::Create();
	NULL_CHECK_RETURN(pWindow, E_FAIL);
	pWindow->Set_Active(TRUE);
	m_pMainWindows.push_back(pWindow);

	/* Create Main_Window Demo */
	pWindow = CImGui_Window_Main_Demo::Create();
	NULL_CHECK_RETURN(pWindow, E_FAIL);
	pWindow->Set_Active(FALSE);
	m_pMainWindows.push_back(pWindow);

	return S_OK;
}

HRESULT CImGui_Manager::Render()
{
	/* 메인 윈도우들을 렌더링 한다. */
	ImGui_Tick();
	{
		for (auto& iter : m_pMainWindows)
		{
			if (nullptr != iter && iter->Is_Active())
				iter->Show_Window();
		}
	}

	FAILED_CHECK_RETURN(ImGui_Render(), E_FAIL);

	m_bClickedWindow = ImGui::GetIO().WantCaptureMouse && ImGui::IsMouseDown(0) ? TRUE : FALSE;

	if (!m_bClickedWindow)
	{
		Pick_Object();
		Hold_Object();
	}
	else
		m_bHoldingObject = FALSE;

	return S_OK;
}

HRESULT CImGui_Manager::ImGui_SetUp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows

	ImGui::StyleColorsClassic();

	if (!ImGui_ImplWin32_Init(g_hWnd)) return E_FAIL;

	if (!ImGui_ImplDX11_Init(pDevice, pContext)) return E_FAIL;

	return S_OK;
}

void CImGui_Manager::ImGui_Tick()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();
}

HRESULT CImGui_Manager::ImGui_Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

void CImGui_Manager::Pick_Object()
{
	if (!m_pGameInstance->Key_Down(VK_LBUTTON))
		return;

 	CGameObject* pGameObect = m_pGameInstance->Get_Pick_Object();
	if (nullptr != pGameObect)
	{
		const _int iIndex = m_pGameInstance->Get_ObjectIndex(m_pGameInstance->Get_CurLevelIndex(), pGameObect->Get_LayerTag(), pGameObect->Get_Name());
		//if (0 < iIndex) 
		{
			//if (m_pCurObject != nullptr)
				//Safe_Release(m_pCurObject);

			m_pCurObject			= pGameObect;
			m_strIndex_CurObject	= pGameObect->Get_Name();
			m_iIndex_CurObject		= iIndex;

			//Safe_AddRef(m_pCurObject);
		}
	}
}

void CImGui_Manager::Hold_Object()
{
	if (!m_pGameInstance->Key_Pressing(VK_LBUTTON))
	{
		m_bHoldingObject = FALSE;
		return;
	}

	CGameObject* pGameObect = m_pGameInstance->Get_Pick_Object();

	if (nullptr != pGameObect && nullptr != m_pCurObject && m_pCurObject == pGameObect)
		m_bHoldingObject = TRUE;
	else
		m_bHoldingObject = FALSE;
}

void CImGui_Manager::Reset_Index_CurLevel()
{
	m_iIndex_CurLevelID = -1;
}

void CImGui_Manager::Reset_Index_CurLayer()
{
	m_iIndex_CurLayerID = -1;
	m_strIndex_CurLayer = L"";
}

void CImGui_Manager::Reset_Index_CurObject()
{
	m_iIndex_CurObject = -1;
	m_strIndex_CurObject = L"";
	m_pCurObject = nullptr;
}

void CImGui_Manager::Reset_Index_PrefabObject()
{
	m_strIndex_PrefabObject = L"";
	m_iIndex_PrefabObject = -1;
	m_pPrefabObj = nullptr;
}


void CImGui_Manager::Set_Active_Main_Window(const WINDOW_MAIN_TYPE& eType, const _bool& bActive)
{
	if (m_pMainWindows.size() + 1 < eType)
		return;

	CImGui_Window* pWindow = m_pMainWindows[eType];
	Safe_AddRef(pWindow);

	if (nullptr == pWindow)
	{
		Safe_Release(pWindow);
		return;
	}

	pWindow->Set_Active(bActive);
	Safe_Release(pWindow);
}

HRESULT CImGui_Manager::Clear_ReferenceData()
{
	Reset_Index_CurLevel();
	Reset_Index_CurLayer();
	Reset_Index_CurObject();
	Reset_Index_PrefabObject();	

	return S_OK;
}

void CImGui_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& pMainWIndow : m_pMainWindows)
		Safe_Release(pMainWIndow);

	m_pMainWindows.clear();

	Safe_Release(m_pGameInstance);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

#endif // _DEBUG