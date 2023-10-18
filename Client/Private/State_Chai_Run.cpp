#include "..\Default\stdafx.h"
#include "State_Chai_Run.h"

CState_Chai_Run::CState_Chai_Run()
{
}

CState_Chai_Run::CState_Chai_Run(const CState_Chai_Run& rhs)
{
}

HRESULT CState_Chai_Run::Initialize(CStateMachine* pStateMachine, const wstring& strStateName, CGameObject* pOwner)
{
	if (FAILED(__super::Initialize(pStateMachine, strStateName, pOwner)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Chai_Run::Enter()
{
	m_pChai->Get_Model()->Set_Animation(ANIM_CH::RUN, TRUE, 0.1f); /* TODO:: 애니메이션 다 적용하고 CChai에 있는 enum으로 사용 */

	return S_OK;
}

const wstring& CState_Chai_Run::Tick(const _float& fTimeDelta)
{
	Move(fTimeDelta);

	return m_strName;
}

const wstring& CState_Chai_Run::LateTick()
{
	return Check_Transition();
}

void CState_Chai_Run::Exit()
{
}

const wstring& CState_Chai_Run::Check_Transition()
{
	/*if (m_pChai->Get_Model()->Is_Tween())
		return m_strName;*/

	if (m_pChai->m_tFightDesc.bDamaged)
	{
		return StateNames[STATE_IDLE];
	}

	/* Movement */
	
	if (Input::Shift())
	{
		if (!m_pChai->m_tMoveDesc.bDash) /* Dash */
		{
			return StateNames[STATE_DASH];
		}
	}
	else if (Input::Attack()) /* Attack */
	{
		return StateNames[STATE_ATTACK];
	}
	else if (Input::Move())
	{
		if (m_pChai->m_tMoveDesc.bGround) /* Run */
		{
			return StateNames[STATE_RUN];
		}
	}

	return StateNames[STATE_IDLE];
}

void CState_Chai_Run::Move(const _float& fTimeDelta)
{
	CTransform* pTranform = m_pChai->Get_Transform();

	Vec4 vDir, vRotDir, vLook;

	vLook = pTranform->Get_State(CTransform::STATE_LOOK).Normalized();

	if (Input::Up() && !Input::Left() && !Input::Right()) // Up
	{
		vDir = GAME_INSTNACE->Get_CurCamera_State(CTransform::STATE_LOOK).ZeroY().Normalized();
		
		if(3.f < acos(vDir.Dot((vLook))))
			vRotDir = Vec4::Lerp(vLook, vDir, 0.5f);
		else
			vRotDir = Vec4::Lerp(vLook, vDir, fTimeDelta * 20.f);

		pTranform->Set_Look(vRotDir);
		pTranform->Translate(vDir * m_pChai->m_tMoveDesc.fMaxForwardSpeed * fTimeDelta);
	}
	else if (Input::Down() && !Input::Left() && !Input::Right()) // Down
	{
		vDir = GAME_INSTNACE->Get_CurCamera_State(CTransform::STATE_LOOK).ZeroY().Normalized().Inverse();

		if (3.f < acos(vDir.Dot((vLook))))
			vRotDir = Vec4::Lerp(vLook, vDir, 0.5f);
		else
			vRotDir = Vec4::Lerp(vLook, vDir, fTimeDelta * 20.f);

		pTranform->Set_Look(vRotDir);
		pTranform->Translate(vDir * m_pChai->m_tMoveDesc.fMaxForwardSpeed * fTimeDelta);
	}
	else if (Input::Left() && !Input::Up() && !Input::Down()) // Left 
	{
		vDir = GAME_INSTNACE->Get_CurCamera_State(CTransform::STATE_RIGHT).ZeroY().Normalized().Inverse();

		if (3.f < acos(vDir.Dot((vLook))))
			vRotDir = Vec4::Lerp(vLook, vDir, 0.5f);
		else
			vRotDir = Vec4::Lerp(vLook, vDir, fTimeDelta * 20.f);

		pTranform->Set_Look(vRotDir);
		pTranform->Translate(vDir * m_pChai->m_tMoveDesc.fMaxForwardSpeed * fTimeDelta);
	}
	else if (Input::Right() && !Input::Up() && !Input::Down()) // Right 
	{
		vDir = GAME_INSTNACE->Get_CurCamera_State(CTransform::STATE_RIGHT).ZeroY().Normalized();

		if (3.f < acos(vDir.Dot((vLook))))
			vRotDir = Vec4::Lerp(vLook, vDir, 0.5f);
		else
			vRotDir = Vec4::Lerp(vLook, vDir, fTimeDelta * 20.f);

		pTranform->Set_Look(vRotDir);
		pTranform->Translate(vDir * m_pChai->m_tMoveDesc.fMaxForwardSpeed * fTimeDelta);
	}
	else if (Input::Up() && Input::Left() && !Input::Right()) // Up + Left
	{
		vDir = GAME_INSTNACE->Get_CurCamera_State(CTransform::STATE_LOOK).ZeroY()
			+ GAME_INSTNACE->Get_CurCamera_State(CTransform::STATE_RIGHT).ZeroY().Inverse();

		vDir.Normalize();
		vRotDir = Vec4::Lerp(vLook, vDir, fTimeDelta * 20.f);
		pTranform->Set_Look(vRotDir);
		pTranform->Translate(vDir * m_pChai->m_tMoveDesc.fMaxForwardSpeed * fTimeDelta);
	}
	else if (Input::Up() && !Input::Left() && Input::Right()) // Up + Right
	{
		vDir = GAME_INSTNACE->Get_CurCamera_State(CTransform::STATE_LOOK).ZeroY()
			+ GAME_INSTNACE->Get_CurCamera_State(CTransform::STATE_RIGHT).ZeroY();

		vDir.Normalize();

		vRotDir = Vec4::Lerp(vLook, vDir, fTimeDelta * 20.f);
		pTranform->Set_Look(vRotDir);
		pTranform->Translate(vDir * m_pChai->m_tMoveDesc.fMaxForwardSpeed * fTimeDelta);
	}
	else if (Input::Down() && Input::Left() && !Input::Right()) // Down + Left
	{
		vDir = GAME_INSTNACE->Get_CurCamera_State(CTransform::STATE_LOOK).ZeroY().Inverse()
			+ GAME_INSTNACE->Get_CurCamera_State(CTransform::STATE_RIGHT).ZeroY().Inverse();

		vDir.Normalize();
		vRotDir = Vec4::Lerp(vLook, vDir, fTimeDelta * 20.f);
		pTranform->Set_Look(vRotDir);
		pTranform->Translate(vDir * m_pChai->m_tMoveDesc.fMaxForwardSpeed * fTimeDelta);
	}
	else if (Input::Down() && !Input::Left() && Input::Right()) // Down + Right
	{
		vDir = GAME_INSTNACE->Get_CurCamera_State(CTransform::STATE_LOOK).ZeroY().Inverse()
			+ GAME_INSTNACE->Get_CurCamera_State(CTransform::STATE_RIGHT).ZeroY();

		vDir.Normalize();
		vRotDir = Vec4::Lerp(vLook, vDir, fTimeDelta * 20.f);
		pTranform->Set_Look(vRotDir);
		pTranform->Translate(vDir * m_pChai->m_tMoveDesc.fMaxForwardSpeed * fTimeDelta);
	}
}

CState_Chai_Run* CState_Chai_Run::Create(CStateMachine* pStateMachine, const wstring& strStateName, CGameObject* pOwner)
{
	CState_Chai_Run* pInstance = new CState_Chai_Run();

	if (FAILED(pInstance->Initialize(pStateMachine, strStateName, pOwner)))
	{
		MSG_BOX("Failed to Created : CState_Chai_Run");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CState* CState_Chai_Run::Clone(void* pArg)
{
	return nullptr;
}

void CState_Chai_Run::Free()
{
}
