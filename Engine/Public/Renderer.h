#pragma once

#include "Component.h"

/* 1. 화면에 그려져야하는 객체들을 그리는 순서대로 모아서 보관한다. */
/* 2. 보관하고 있는 객체들의 렌더콜(드로우콜)을 수행한다. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	/* 그리기 순서에 따른 분류 */
	/* 일단 RG_NONBLEND를 먼저 그린다. 그 후 RG_BLEND는 깊이 정렬 후 그린다. */
	enum RENDERGROUP { RG_PRIORITY, RG_NONLIGHT, RG_NONBLEND, RG_BLEND, RG_UI, RG_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	CRenderer(const CRenderer& rhs) = delete;
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw_RenderObjects();

#ifdef _DEBUG
public:
	HRESULT Add_DebugGroup(class CComponent* pDebugCom);
#endif // _DEBUG

private:
	list<class CGameObject*>			m_RenderObjects[RG_END];

#ifdef _DEBUG
private:
	list<class CComponent*>				m_DebugObject;

	class CShader*						m_pShader = nullptr;
	class CVIBuffer_Rect*				m_pVIBuffer = nullptr;
	_float4x4							m_ViewMatrix, m_ProjMatrix;

#endif // _DEBUG

private:
	HRESULT Render_Priority();
	HRESULT Render_NonLight();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();

#ifdef _DEBUG
	HRESULT Render_Debug();
#endif // _DEBUG

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END