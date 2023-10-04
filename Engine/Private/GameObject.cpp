#include "..\Public\GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_bActive(rhs.m_bActive)
	, m_bRender(rhs.m_bRender)
	, m_strLayer(rhs.m_strLayer)
	, m_strName(rhs.m_strName)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	return S_OK;
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::LateTick(_float fTimeDelta)
{
	Compute_CamZ(Get_Transform()->Get_State(CTransform::STATE_POSITION));
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

CTransform* const CGameObject::Get_Transform()
{
	return dynamic_cast<CTransform*>(Get_Component(TEXT("Com_Transform")));
}

CVIBuffer* CGameObject::Get_VIBuffer()
{
	return dynamic_cast<CVIBuffer*>(Get_Component(TEXT("Com_VIBuffer")));
}

CCollider_Sphere* CGameObject::Get_Collider_Sphere()
{
	return dynamic_cast<CCollider_Sphere*>(Get_Component(TEXT("Com_Collider_Sphere")));
}

CMonoBehaviour* const CGameObject::Get_MonoBehaviour(const _uint& iIndex)
{
	if(m_MonoBehaviours.size() > iIndex)
		return nullptr;
	
	return m_MonoBehaviours[iIndex];
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const wstring & strPrototypeTag, const wstring & strComponentTag, _Inout_ CComponent** ppOut, void * pArg)
{
	/* 추가하려는 컴포넌트가 이미 있는 경우 실패 리턴 */
	if (nullptr != Find_Component(strComponentTag))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	Safe_AddRef(pGameInstance);
	{
		CComponent*			pComponent = pGameInstance->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
		if (nullptr == pComponent)
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		/* 검색이 가능한 맵에 저장. */
		m_Components.emplace(strComponentTag, pComponent);

		/* 자식의 멤버변수에도 저장. */
		*ppOut = pComponent;

		Safe_AddRef(pComponent);
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CComponent * CGameObject::Find_Component(const wstring & strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;	
}

HRESULT CGameObject::Compute_CamZ(_fvector vWorldPos)
{
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	{
		_fvector		vCamPos = pPipeLine->Get_CamPosition();

		m_fCamDistance = XMVectorGetX(XMVector3Length(vWorldPos - vCamPos));
	}
	RELEASE_INSTANCE(CPipeLine);
	return S_OK;
}

void CGameObject::Free()
{
	__super::Free();

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
