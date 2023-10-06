#pragma once

#include "Base.h"
BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
	typedef map<const wstring, class CComponent*>				COMPONENTS;

protected:
	/* 원형을 생성할 때 */
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 (복사 생성자) */
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg);
	virtual void		Tick(_float fTimeDelta);
	virtual void		LateTick(_float fTimeDelta);
	virtual HRESULT		Render();

public:
	const _bool&		Is_Active() const { return m_bActive; }
	const _bool&		Is_Render() const { return m_bRender; }
	const _bool&		Is_Picked() const { return m_bPicked; }

public:
	const wstring&		Get_Name() const { return m_strName; }
	const wstring&		Get_LayerTag() const { return m_strLayer; }
	const _uint&		Get_PickID() const { return m_iPickID; }
	const _float&		Get_CamDistance() const { return m_fCamDistance; }

	class CTransform* const		Get_Transform();
	class CVIBuffer*			Get_VIBuffer();
	class CCollider_Sphere*		Get_Collider_Sphere();
	
	class CComponent* const			Get_Component(const wstring& strComponentTag) { return Find_Component(strComponentTag); };
	COMPONENTS&						Get_Components() { return m_Components; }
	class CMonoBehaviour* const		Get_MonoBehaviour(const _uint& iIndex);
	vector<class CMonoBehaviour*>&	Get_MonoBehaviours() { return m_MonoBehaviours; }

public:
	/* Active 이벤트 처리를 위한 가상함수화, 오버라이드시 __super::Active()먼저 호출 필요 */
	virtual void	Set_Active(const _bool& bActive) { m_bActive = bActive; }
	virtual void	Set_Render(const _bool& bRender) { m_bRender = bRender; }
	void			Set_Name(const wstring& strName) { m_strName = strName; }
	void			Set_LayerTag(const wstring& strLayer) { m_strLayer = strLayer; }
	void			Set_Picked(const _bool& bPicked) { m_bPicked = bPicked; }

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	/* 특정 컴포넌트의 검색을 용이하게 하기 위해서. */
	/* 검색을 고려하는 이유?! : 타 객체가 현재 객체의 정보(컴포넌트)를 참조하고자 하는 경우가 빈번하다. */
	COMPONENTS						m_Components;
	vector<class CMonoBehaviour*>	m_MonoBehaviours;

	/* UnActive 상태라면 라이프 사이클 함수를 타지 않는다. */
	_bool					m_bActive = { TRUE };
	_bool					m_bRender = { TRUE };	
	wstring					m_strLayer = { };
	wstring					m_strName = { };

	/* 피킹 처리를 위한 고유 아이디 */
	_uint					m_iPickID = { 0 };
	_float					m_fCamDistance = 0.f;
	_bool					m_bPicked = FALSE;

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, _Inout_ CComponent** ppOut, void* pArg = nullptr);
	class CComponent* Find_Component(const wstring& strComponentTag);
	HRESULT Compute_CamZ(_fvector vWorldPos);


private:
	HRESULT					Create_PickID() { return S_OK; }

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

	friend class CLayer;
};

END