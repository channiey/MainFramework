#include "..\Public\Collider_Sphere.h"

#include "Collider_AABB.h"
#include "Collider_OBB.h"

#include "GameObject.h"
#include "Transform.h"

CCollider_Sphere::CCollider_Sphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCollider(pDevice, pContext)
{
}

CCollider_Sphere::CCollider_Sphere(const CCollider_Sphere & rhs)
	: CCollider(rhs)
{
}

HRESULT CCollider_Sphere::Initialize_Prototype(CCollider::TYPE eColliderType)
{
	if (FAILED(__super::Initialize_Prototype(eColliderType)))
		return E_FAIL;

	m_eColliderType = CCollider::TYPE_SPHERE;

	return S_OK;
}

HRESULT CCollider_Sphere::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pOriginal_Sphere = new BoundingSphere(m_ColliderDesc.vCenter, m_ColliderDesc.vSize.x);

	m_pSphere = new BoundingSphere(*m_pOriginal_Sphere);

	return S_OK;
}

void CCollider_Sphere::Update(Matrix TransformMatrix)
{
	//m_pOriginal_Sphere->Transform(*m_pSphere, TransformMatrix);

	CTransform* pTransformCom = Get_Parent()->Get_Transform();

	m_pSphere->Center = m_ColliderDesc.vCenter + pTransformCom->Get_Position().ToVec3();

}

_bool CCollider_Sphere::Check_Collision(CCollider * pTargetCollider)
{
	CCollider::TYPE		eType = pTargetCollider->Get_Type();

	_bool bCollision = FALSE;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		bCollision = m_pSphere->Intersects(((CCollider_AABB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_OBB:
		bCollision = m_pSphere->Intersects(((CCollider_OBB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_SPHERE:
		bCollision = m_pSphere->Intersects(((CCollider_Sphere*)pTargetCollider)->Get_Collider());
		break;
	}

	return bCollision;
}

_bool CCollider_Sphere::Check_Collision(Ray& ray, OUT RAYHIT_DESC& pHitDesc)
{
	return m_pSphere->Intersects(ray.position, ray.direction, pHitDesc.fDistance);
}

HRESULT CCollider_Sphere::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pBatch->Begin();	

	DX::Draw(m_pBatch, *m_pSphere, XMLoadFloat4(&m_vColor));

	m_pBatch->End();

	return S_OK;
}

CCollider_Sphere * CCollider_Sphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CCollider::TYPE eColliderType)
{
	CCollider_Sphere* pInstance = new CCollider_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX("Failed To Created : CCollider_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCollider_Sphere::Clone(void * pArg)
{
	CCollider_Sphere* pInstance = new CCollider_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CCollider_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CCollider_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginal_Sphere);
	Safe_Delete(m_pSphere);
}
