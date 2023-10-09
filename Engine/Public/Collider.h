#pragma once

#include "Component.h"
#include "DebugDraw.h"

BEGIN(Engine)

class ENGINE_DLL CCollider abstract : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };

	typedef struct tagColliderDesc
	{
		Vec3			vCenter = Vec3::Zero;
		Vec3			vSize = Vec3::One;
		Vec3			vRotation = Vec3(0.f, DEG2RAD(45.f), 0.f);

		tagColliderDesc() {};
		tagColliderDesc(const _float fMag)
			: vCenter(Vec3::Zero), vSize(Vec3::One* fMag), vRotation(Vec3(0.f, DEG2RAD(45.f), 0.f)) {};

	}COLLIDERDESC;

protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT		Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT		Initialize(void* pArg);
	virtual void		Update(Matrix TransformMatrix) = 0;

public:
	void				OnCollision_Enter(class CGameObject* pGameObject);
	void				OnCollision_Stay(class CGameObject* pGameObject);
	void				OnCollision_Exit(class CGameObject* pGameObject);

public:
	virtual _bool		Check_Collision(CCollider* pTargetCollider) = 0;
	virtual _bool		Check_Collision(Ray& ray, OUT RAYHIT_DESC& pHitDesc) = 0;

public:
	TYPE				Get_Type() const { return m_eColliderType; }
	const _uint&		Get_ID() const { return m_iID; }

public:
	const _bool			Is_Collision() const {return  (0 < m_iCollison) ? TRUE : FALSE; }

protected:
	_matrix				Remove_Rotation(_fmatrix Matrix);

protected:
	TYPE				m_eColliderType = TYPE_END;
	COLLIDERDESC		m_ColliderDesc;
	//_bool				m_bCollision = FALSE;

	_uint				m_iCollison = 0; /* 현재 충돌중인 콜라이더 갯수 */
	_uint				m_iID = 0;		 /* 콜라이더 고유 아이디 */
	static _uint		g_iNextID;
	
public:	
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void		Free() override;


	/* ========================================== DEBUG ========================================== */

#ifdef _DEBUG

public:
	virtual HRESULT		Render() override;

protected:
	PrimitiveBatch<VertexPositionColor>*		m_pBatch = nullptr;
	BasicEffect*								m_pEffect = nullptr;	
	ID3D11InputLayout*							m_pInputLayout = nullptr;
	_float4										m_vColor = _float4(0.f, 1.f, 0.f, 1.f);

#endif
};

END