#pragma once
#include "Character.h"

BEGIN(Engine)
class CBehaviourTree;
class CRigidbody;
END

BEGIN(Client)

class CTriggerBattle;

class CEnemy abstract : public CCharacter
{

public:
	enum class ENEMY_TYPE {NORMAL, FLYING, BOSS, TYPEEND };

protected:
	CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy(const CEnemy& rhs);
	virtual ~CEnemy() = default;

public:
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg);
	virtual void		Tick(_double fTimeDelta);
	virtual void		LateTick(_double fTimeDelta);
	virtual HRESULT		Render();

public:
	virtual void		Set_State(const OBJ_STATE& eState) override;
	void				Set_BattleTrigger(CTriggerBattle* pBattleTrigger) { m_pBattleTrigger = pBattleTrigger; }

	const ENEMY_TYPE&	Get_EnemyType() const { return m_eEnemyType; }
	CTriggerBattle*		Get_BattleTrigger() const { return m_pBattleTrigger; }
	
	HRESULT				Return_To_Pool();

protected:
	HRESULT				Ready_Components();

protected:
	virtual void		OnCollision_Enter(CCollider* pCollider, const _int& iIndexAsChild = -1) override;
	virtual void		OnCollision_Stay(CCollider* pCollider, const _int& iIndexAsChild = -1) override;
	virtual void		OnCollision_Exit(CCollider* pCollider, const _int& iIndexAsChild = -1) override;

protected:
	ENEMY_TYPE			m_eEnemyType = ENEMY_TYPE::TYPEEND;
	CTriggerBattle*		m_pBattleTrigger = nullptr;

public:
	virtual CEnemy*		Clone(void* pArg) PURE;
	virtual void		Free() override;
};
END
