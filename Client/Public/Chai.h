#pragma once

#include "Character.h"

BEGIN(Engine)
class CStateMachine;
END

BEGIN(Client)

enum STATE_CH { IDLE, RUN, DASH, JUMP, DOUBLEJUMP, FALL, LAND, ATTACK_1, ATTACK_2, ATTACK_3, DAMAGED, DEAD, NONE, STATE_END };

static const wstring StateNames_CH[STATE_CH::STATE_END] = { L"IDLE", L"RUN", L"DASH", L"JUMP", L"DOUBLEJUMP", L"FALL", L"LAND", L"ATTACK_1", L"ATTACK_2", L"ATTACK_3", L"DAMAGED", L"DEAD", L"NONE" };

class CChai final : public CCharacter
{
	enum CHILD_TYPE	{ CH_WEAPON_RIGHT, CH_END };

protected:
	CChai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChai(const CCharacter& rhs);
	virtual ~CChai() = default;

public:
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg);
	virtual void		Tick(_float fTimeDelta);
	virtual void		LateTick(_float fTimeDelta);
	virtual HRESULT		Render();

private:
	HRESULT				Ready_Components();
	HRESULT				Ready_Chilren();

private:
	HRESULT				Bind_ShaderResources();

private:
	virtual void		OnCollision_Enter(CGameObject* pGameObject) override;
	virtual void		OnCollision_Stay(CGameObject* pGameObject) override;
	virtual void		OnCollision_Exit(CGameObject* pGameObject) override;

private: 
	CStateMachine*		m_pStateMachineCom = { nullptr };

public:
	static CChai* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCharacter* Clone(void* pArg) override; 
	virtual void Free() override;

private:
	friend class CState_Chai_Base;
	friend class CState_Chai_Idle;
	friend class CState_Chai_Run;
	friend class CState_Chai_Attack_1;
};

END