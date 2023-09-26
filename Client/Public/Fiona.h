#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CFiona final : public CGameObject
{
protected:
	/* 원형을 생성할 때 */
	CFiona(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CFiona(const CGameObject& rhs); /* 복사 생성자. */

	virtual ~CFiona() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CShader*				m_pShaderCom = { nullptr };	
	CRenderer*				m_pRendererCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CFiona* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END