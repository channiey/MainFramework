#include "Collision_Manager.h"

#include "GameInstance.h"

#include "GameObject.h"
#include "Model.h"
#include "Mesh.h"

#include "Collider_Sphere.h"
#include "Collider_AABB.h"
#include "Collider_OBB.h"

IMPLEMENT_SINGLETON(CCollision_Manager)

CCollision_Manager::CCollision_Manager()
{

	
}

HRESULT CCollision_Manager::Initialize()
{
	return S_OK;
}

void CCollision_Manager::Check_Collision_Layer(const wstring& strLayerTag1, const wstring& strLayerTag2, const CCollider::TYPE& eType1, const CCollider::TYPE& eType2)
{
	/* 레이어 가져오기 */
	_uint iCurLevel = CGameInstance::GetInstance()->Get_CurLevelIndex();

	list<CGameObject*>* pLayer1 = CGameInstance::GetInstance()->Get_Layer(iCurLevel, strLayerTag1);
	list<CGameObject*>* pLayer2 = CGameInstance::GetInstance()->Get_Layer(iCurLevel, strLayerTag2);

	if (nullptr == pLayer1 || nullptr == pLayer2 || pLayer1->empty() || pLayer2->empty()) return;

	map<_ulonglong, _bool>::iterator	iter;

	/* Loop */
	for (auto& pObj1 : *pLayer1)
	{
		for (auto& pObj2 : *pLayer2)
		{
			/* 콜라이더 가져오기 */
			if (nullptr == pObj1 || nullptr == pObj2 || pObj1 == pObj2) continue;

			CCollider* pCollider1 = nullptr;
			CCollider* pCollider2 = nullptr;

			switch (eType1)
			{
			case Engine::CCollider::TYPE_AABB:
				pCollider1 = pObj1->Get_Collider_AABB();
				break;
			case Engine::CCollider::TYPE_OBB:
				pCollider1 = pObj1->Get_Collider_OBB();
				break;
			case Engine::CCollider::TYPE_SPHERE:
				pCollider1 = pObj1->Get_Collider_Sphere();
				break;
			default:
				break;
			}
			switch (eType2)
			{
			case Engine::CCollider::TYPE_AABB:
				pCollider2 = pObj2->Get_Collider_AABB();
				break;
			case Engine::CCollider::TYPE_OBB:
				pCollider2 = pObj2->Get_Collider_OBB();
				break;
			case Engine::CCollider::TYPE_SPHERE:
				pCollider2 = pObj2->Get_Collider_Sphere();
				break;
			default:
				break;
			}

			if (nullptr == pCollider1 || nullptr == pCollider2) continue;

			/* 충돌 정보 세팅 */
			Set_Info(iter, pCollider1, pCollider2);

			/* 충돌 여부 검사 */
			if (pCollider1->Check_Collision(pCollider2))
			{
				if (iter->second) // 이전에도 충돌
				{
					if (!pObj1->Is_Active() || !pObj2->Is_Active()) // 둘 중 하나 삭제 예정
					{
						pCollider1->OnCollision_Exit(pObj2);
						pCollider2->OnCollision_Exit(pObj1);
						iter->second = false;
					}
					else // 삭제 예정 없음
					{
						pCollider1->OnCollision_Stay(pObj2);
						pCollider2->OnCollision_Stay(pObj1);
					}
				}
				else // 이번에 처음 충돌 
				{
					if (pObj1->Is_Active() && pObj2->Is_Active())
					{
						pCollider1->OnCollision_Enter(pObj2);
						pCollider2->OnCollision_Enter(pObj1);
						iter->second = TRUE;
					}
				}
			}
			else
			{
				if (iter->second) // 이전에 충돌
				{
					pCollider1->OnCollision_Exit(pObj2);
					pCollider2->OnCollision_Exit(pObj1);
					iter->second = FALSE;
				}
			}
		}
	}
}



const _bool CCollision_Manager::Check_Collision_Ray(Ray& ray, CCollider* pCollider, OUT RAYHIT_DESC& hitDesc)
{
	if (nullptr == pCollider)
		return FALSE;

	return pCollider->Check_Collision(ray, hitDesc);
}

const _bool CCollision_Manager::Check_Collision_CameraRay(CCollider* pCollider, const Matrix& matWorld, OUT RAYHIT_DESC& hitDesc)
{
	if (nullptr == pCollider) return FALSE;

	Ray ray = Create_CameraRay(matWorld);
	if (ray == Ray()) return FALSE;

	switch (pCollider->Get_Type())
	{
	case Engine::CCollider::TYPE_AABB:
	{
		CCollider_AABB* pAABBCollider = dynamic_cast<CCollider_AABB*>(pCollider);

		if (nullptr == pAABBCollider)
			return FALSE;

		if (pAABBCollider->Check_Collision(ray, hitDesc))
		{
			hitDesc.pGameObject = (void*)pAABBCollider->Get_Parent();
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
		break;
	case Engine::CCollider::TYPE_OBB:
	{
		CCollider_OBB* pOBBCollider = dynamic_cast<CCollider_OBB*>(pCollider);

		if (nullptr == pCollider)
			return FALSE;

		if (pOBBCollider->Check_Collision(ray, hitDesc))
		{
			hitDesc.pGameObject = (void*)pOBBCollider->Get_Parent();
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
		break;
	case Engine::CCollider::TYPE_SPHERE:
	{
		CCollider_Sphere* pSphereCollider = dynamic_cast<CCollider_Sphere*>(pCollider);

		if (nullptr == pSphereCollider)
			return FALSE;

		if (pSphereCollider->Check_Collision(ray, hitDesc))
		{
			hitDesc.pGameObject = (void*)pSphereCollider->Get_Parent();
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
		break;
	default:
		break;
	}


	return FALSE;
}

const _bool CCollision_Manager::Check_Collision_CameraRay(class CModel* pModel, const Matrix& matWorld, OUT RAYHIT_DESC& hitDesc, const _bool& bPreInterSphere)
{
	if (nullptr == pModel || !CGameInstance::GetInstance()->Is_Focus()) return FALSE;

	Ray ray = Create_CameraRay(matWorld);
	if (ray == Ray()) return FALSE;

	/* 메시 콜라이더 충돌 검사전, 스피어 콜라이더 충돌 검사 우선 진행 (최적화)*/
	if (bPreInterSphere)
	{
		if (!Check_Collision_CameraRay(pModel->Get_Parent()->Get_Collider_Sphere(), matWorld, hitDesc))
			return FALSE;
		else
			ZeroMemory(&hitDesc, sizeof(RAYHIT_DESC));
		cout << "스피어 콜라이더 통과\n";
	}


	/* 메시 콜라이더 충돌 검사 */
	vector<CMesh*>* pMeshes = pModel->Get_Meshes();
	if (nullptr == pMeshes) return FALSE;

	for (auto& pMesh : *pMeshes)
	{
		if (CModel::TYPE_ANIM == pModel->Get_Type())
		{
			VTXANIMMODEL* pVerticesAnim		= pMesh->Get_VerticesAnim();
			FACEINDICES32* pIndices			= pMesh->Get_Indices();
			if (nullptr == pVerticesAnim || nullptr == pIndices) continue;

			for (size_t i = 0; i < pMesh->Get_NumPrimitives() * 3; ++i)
			{
				if (ray.Intersects(pVerticesAnim[i].vPosition, pVerticesAnim[++i].vPosition, pVerticesAnim[++i].vPosition, hitDesc.fDistance))
				{
					int k = 0;
					cout << "메시 콜라이더 통과\n";
				}
			}
		}
		else
		{
			VTXMODEL* pVerticeStatic		= pMesh->Get_VerticesStatic();
			FACEINDICES32* pIndices			= pMesh->Get_Indices();
			
			if (nullptr == pVerticeStatic || nullptr == pIndices) continue;

			for (size_t i = 0; i < pMesh->Get_NumPrimitives() * 3; ++i)
			{
				if (ray.Intersects(pVerticeStatic[i].vPosition, pVerticeStatic[++i].vPosition, pVerticeStatic[++i].vPosition, hitDesc.fDistance))
				{
					int k = 0;
					cout << "메시 콜라이더 통과\n";

				}
			}
		}
	}
	
	RELEASE_INSTANCE(CGameInstance);

	return TRUE;
}

Ray CCollision_Manager::Create_CameraRay(const Matrix& matWorld)
{
	/* Unprojection */
	const Matrix matW = matWorld;
	const Matrix matV = CGameInstance::GetInstance()->Get_Transform(CPipeLine::STATE_VIEW);
	const Matrix matP = CGameInstance::GetInstance()->Get_Transform(CPipeLine::STATE_PROJ);
	const Viewport viewPort = CGameInstance::GetInstance()->Get_ViewPort();

	Vec2 vPickWindowPos;
	if (!CGameInstance::GetInstance()->Get_PickPos_Window(&vPickWindowPos))
		return Ray();

	Vec3 n = viewPort.Unproject(Vec3(vPickWindowPos.x, vPickWindowPos.y, 0.f), matP, matV, matW);
	Vec3 f = viewPort.Unproject(Vec3(vPickWindowPos.x, vPickWindowPos.y, 1.f), matP, matV, matW);

	/* Create Ray */
	Vec3 vOrigin = n;
	Vec3 vDir = f - n;
	vDir.Normalize();

	return Ray(vOrigin, vDir);
}

void CCollision_Manager::Set_Info(map<_ulonglong, _bool>::iterator& iter, CCollider* pCollider1, CCollider* pCollider2)
{
	COLLIDER_ID ID;

	ID.iLeft_id = pCollider1->Get_ID();
	ID.iRight_id = pCollider2->Get_ID();

	// 이전에 충돌 검사 여부 조사
	iter = m_mapColInfo.find(ID.ID);

	// 없다면 새로 추가
	if (m_mapColInfo.end() == iter)
	{
		m_mapColInfo.insert({ ID.ID, false });
		iter = m_mapColInfo.find(ID.ID);
	}
}

void CCollision_Manager::Free()
{
	__super::Free();
}
