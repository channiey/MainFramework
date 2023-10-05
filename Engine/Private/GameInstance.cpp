#include "..\Public\GameInstance.h"
#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Profiler_Manager.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pTimer_Manager(CTimer_Manager::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pThread_Manager(CThread_Manager::GetInstance())
	, m_pProfiler_Manager(CProfiler_Manager::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())

{
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pThread_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pProfiler_Manager);
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, HINSTANCE hInst, const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	/* 그래픽디바이스 초기화 처리. */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc, ppDevice, ppContext)))
		return E_FAIL;

	/* 사운드디바이스 초기화 처리. */

	/* 입력디바이스 초기화 처리. */
	if (FAILED(m_pInput_Device->Initialize(hInst, m_pGraphic_Device->Get_GraphicDesc().hWnd)))
		return E_FAIL;

	/* 오브젝트 매니저의 예약 처리. */
	if (FAILED(m_pObject_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;

	/* 컴포넌트 매니저의 예약 처리 */
	if (FAILED(m_pComponent_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;

	/* 파이프 라인의 초기화 처리 */
	if (FAILED(m_pPipeLine->Initialize()))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick(_float fTimeDelta)
{
	NULL_CHECK(m_pLevel_Manager);
	NULL_CHECK(m_pObject_Manager);
	NULL_CHECK(m_pInput_Device);
	NULL_CHECK(m_pPipeLine);

	m_pInput_Device->Tick();

	m_pObject_Manager->Tick(fTimeDelta);
	m_pLevel_Manager->Tick(fTimeDelta);

	/* 카메라는 게임오브젝트이다. 따라서 카메라의 월드행렬이 갱신된 후에, 뷰행렬과 투영행렬을 구하도록 한다. */
	m_pPipeLine->Tick();
}

void CGameInstance::LateTick(_float fTimeDelta)
{
	m_pObject_Manager->LateTick(fTimeDelta);
	m_pLevel_Manager->LateTick(fTimeDelta);
}

void CGameInstance::FinishTick()
{
	m_pObject_Manager->FinishTick();
	m_pProfiler_Manager->FinishTick();
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	m_pObject_Manager->Clear(iLevelIndex);
}

_float CGameInstance::Compute_TimeDelta(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);	
}

HRESULT CGameInstance::Add_Timer(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

ID3D11Device* CGameInstance::Get_Device()
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_Device();
}

GRAPHIC_DESC CGameInstance::Get_GraphicDesc()
{
	if (nullptr == m_pGraphic_Device)
		return GRAPHIC_DESC{};

	return m_pGraphic_Device->Get_GraphicDesc();
}

const Viewport CGameInstance::Get_ViewPort()
{
	if (nullptr == m_pGraphic_Device)
		return Viewport{};

	return m_pGraphic_Device->Get_ViewPort();
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

const _uint CGameInstance::Get_CurLevelIndex()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Get_CurLevelIndex();
}

HRESULT CGameInstance::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);	
}

class CGameObject* CGameInstance::Add_GameObject(const _uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Add_GameObject(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Delete_GameObject(const _uint iLevelIndex, CGameObject* pObj)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Delete_GameObject(iLevelIndex, pObj);
}
 
HRESULT CGameInstance::Reserve_Pool(const _uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, const _uint& iNumObj, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Reserve_Pool(iLevelIndex, strLayerTag, strPrototypeTag, iNumObj, pArg);
}

CGameObject* CGameInstance::Pop_Pool(const _uint iLevelIndex, const wstring& strPrototypeTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Pop_Pool(iLevelIndex, strPrototypeTag);
}

HRESULT CGameInstance::Return_Pool(const _uint iLevelIndex, CGameObject* pObj)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Return_Pool(iLevelIndex, pObj);
}

HRESULT CGameInstance::Add_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Layer(iLevelIndex, strLayerTag);
}

HRESULT CGameInstance::Delete_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Delete_Layer(iLevelIndex, strLayerTag);
}

map<const wstring, class CLayer*>* CGameInstance::Get_All_Layer(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_All_Layer(iLevelIndex);
}

map<const wstring, class CLayer*>* CGameInstance::Get_All_Layer_CurLevel()
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_All_Layer(Get_CurLevelIndex());
}

list<class CGameObject*>* CGameInstance::Get_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Layer(iLevelIndex, strLayerTag);
}

CLayer* CGameInstance::Get_LayerClass(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_LayerClass(iLevelIndex, strLayerTag);
}

CGameObject* CGameInstance::Get_Player()
{
	return nullptr;
}

CGameObject* CGameInstance::Get_GameObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_GameObject(iLevelIndex, strLayerTag, strPrototypeTag);
}

const _int CGameInstance::Get_ObjectIndex(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strName)
{
	if (nullptr == m_pObject_Manager)
		return -1;

	return m_pObject_Manager->Get_ObjectIndex(iLevelIndex, strLayerTag, strName);
}

map<const wstring, class CGameObject*>* CGameInstance::Get_Prototypes()
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Prototypes();
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);	
}

//HRESULT CGameInstance::Set_MultiThreading(const _uint& iNumThread)
//{
//	if (nullptr == m_pThread_Manager)
//		return E_FAIL;
//
//	return m_pThread_Manager->Set_MultiThreading(iNumThread);
//}
//
//void CGameInstance::Finish_MultiThreading()
//{
//	if (nullptr == m_pThread_Manager)
//		return;
//
//	m_pThread_Manager->Finish_MultiThreading();
//}

const PROFILER_DESC CGameInstance::Get_ProfillingData() const
{
	if (nullptr == m_pProfiler_Manager)
	{
		PROFILER_DESC desc;
		ZeroMemory(&desc, sizeof(PROFILER_DESC));

		return desc;
	}
	return m_pProfiler_Manager->Get_Status();
}

_char CGameInstance::Get_DIKState(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(eKeyID);
}

_char CGameInstance::Get_DIMKeyState(CInput_Device::MOUSEKEYSTATE eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseState(eMouseKeyID);
}

_long CGameInstance::Get_DIMMoveState(CInput_Device::MOUSEMOVESTATE eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMouseMoveID);
}

const _bool CGameInstance::Key_Up(const _int& _iKey)
{
	if (nullptr == m_pInput_Device)
		return FALSE;

	return m_pInput_Device->Key_Up(_iKey);
}

const _bool CGameInstance::Key_Down(const _int& _iKey)
{
	if (nullptr == m_pInput_Device)
		return FALSE;

	return m_pInput_Device->Key_Down(_iKey);
}

const _bool CGameInstance::Key_Pressing(const _int& _iKey)
{
	if (nullptr == m_pInput_Device)
		return FALSE;

	return m_pInput_Device->Key_Pressing(_iKey);
}

const _bool CGameInstance::Get_PickPos_Terrain(class CVIBuffer_Terrain* pBuffer, Matrix matWorld, _Inout_ Vec3& vPickPos)
{
	if (nullptr == m_pInput_Device)
		return FALSE;

	return m_pInput_Device->Get_PickPos_Terrain(pBuffer, matWorld, vPickPos);
}

CGameObject* CGameInstance::Get_Pick_Object()
{
	if (nullptr == m_pInput_Device)
		return nullptr;

	return m_pInput_Device->Get_Pick_Object();
}

HRESULT CGameInstance::Bind_TransformToShader(CShader* pShader, const char* pConstantName, CPipeLine::TRANSFORM_STATE eState)
{
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	return m_pPipeLine->Bind_TransformToShader(pShader, pConstantName, eState);
}

Matrix CGameInstance::Get_Transform(const CPipeLine::TRANSFORM_STATE& eState) const
{
	if (nullptr == m_pPipeLine)
		return Matrix();

	return m_pPipeLine->Get_Transform(eState);
}

Matrix CGameInstance::Get_Transform_Inverse(const CPipeLine::TRANSFORM_STATE& eState) const
{
	if (nullptr == m_pPipeLine)
		return Matrix();

	return m_pPipeLine->Get_Transform_Inverse(eState);
}

Vec4 CGameInstance::Get_CamPosition() const
{
	if (nullptr == m_pPipeLine)
		return Vec4();

	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();
	CLevel_Manager::GetInstance()->DestroyInstance();
	CObject_Manager::GetInstance()->DestroyInstance();
	CComponent_Manager::GetInstance()->DestroyInstance();
	CTimer_Manager::GetInstance()->DestroyInstance();		
	CInput_Device::GetInstance()->DestroyInstance();
	CGraphic_Device::GetInstance()->DestroyInstance();
	CThread_Manager::GetInstance()->DestroyInstance();
	CPipeLine::GetInstance()->DestroyInstance();
	CProfiler_Manager::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pThread_Manager);
	Safe_Release(m_pProfiler_Manager);
}
