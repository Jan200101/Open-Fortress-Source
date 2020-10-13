//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_crowbar.h"
#include "in_buttons.h"

#ifdef CLIENT_DLL
	#include "c_tf_player.h"
#else
	#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon Crowbar tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFCrowbar, DT_TFWeaponCrowbar )

BEGIN_NETWORK_TABLE( CTFCrowbar, DT_TFWeaponCrowbar )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCrowbar )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_crowbar, CTFCrowbar );
//PRECACHE_WEAPON_REGISTER( tf_weapon_crowbar );

IMPLEMENT_NETWORKCLASS_ALIASED( TFUmbrella, DT_TFWeaponUmbrella )

BEGIN_NETWORK_TABLE( CTFUmbrella, DT_TFWeaponUmbrella )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFUmbrella )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_umbrella, CTFUmbrella );
//PRECACHE_WEAPON_REGISTER( tf_weapon_umbrella );

IMPLEMENT_NETWORKCLASS_ALIASED( TFCCrowbar, DT_TFCWeaponCrowbar )

BEGIN_NETWORK_TABLE( CTFCCrowbar, DT_TFCWeaponCrowbar )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCCrowbar )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tfc_weapon_crowbar, CTFCCrowbar );
//PRECACHE_WEAPON_REGISTER( tfc_weapon_crowbar );

IMPLEMENT_NETWORKCLASS_ALIASED( TFCUmbrella, DT_TFCWeaponUmbrella )

BEGIN_NETWORK_TABLE( CTFCUmbrella, DT_TFCWeaponUmbrella )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCUmbrella )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tfc_weapon_umbrella, CTFCUmbrella );
//PRECACHE_WEAPON_REGISTER( tfc_weapon_umbrella );

IMPLEMENT_NETWORKCLASS_ALIASED( TFPoisonShank, DT_TFWeaponPoisonShank)

BEGIN_NETWORK_TABLE(CTFPoisonShank, DT_TFWeaponPoisonShank)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CTFPoisonShank)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(tf_weapon_poisonshank, CTFPoisonShank);
//PRECACHE_WEAPON_REGISTER( tf_weapon_poisonshank );

IMPLEMENT_NETWORKCLASS_ALIASED(TFPipeWrench, DT_TFWeaponPipeWrench)

BEGIN_NETWORK_TABLE(CTFPipeWrench, DT_TFWeaponPipeWrench)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CTFPipeWrench)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(tf_weapon_pipe_wrench, CTFPipeWrench);
//PRECACHE_WEAPON_REGISTER( tf_weapon_pipe_wrench );

//=============================================================================
//
// Weapon Crowbar functions.
//

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFCrowbar::CTFCrowbar()
{
}
CTFUmbrella::CTFUmbrella()
{
}
CTFCCrowbar::CTFCCrowbar()
{
}
CTFCUmbrella::CTFCUmbrella()
{
}
CTFPoisonShank::CTFPoisonShank()
{
}
CTFPipeWrench::CTFPipeWrench()
{
}
acttable_t m_acttableMeleeAllClass[] =
{
	{ ACT_MP_STAND_IDLE,						ACT_MP_STAND_MELEE_ALLCLASS,			        false },		
	{ ACT_MP_CROUCH_IDLE,	                    ACT_MP_CROUCH_MELEE_ALLCLASS,	                false },
	{ ACT_MP_WALK,								ACT_MP_WALK_MELEE,								false },
	{ ACT_MP_RUN,		                    ACT_MP_RUN_MELEE_ALLCLASS,		                false },
	{ ACT_MP_AIRWALK,	                    ACT_MP_AIRWALK_MELEE_ALLCLASS,	                false },
	{ ACT_MP_CROUCHWALK,                  ACT_MP_CROUCHWALK_MELEE_ALLCLASS,               false },
	{ ACT_MP_JUMP,	                    ACT_MP_JUMP_MELEE_ALLCLASS,	                    false },
	{ ACT_MP_JUMP_START,                  ACT_MP_JUMP_START_MELEE_ALLCLASS,               false },
	{ ACT_MP_JUMP_FLOAT,                  ACT_MP_JUMP_FLOAT_MELEE_ALLCLASS,               false },
	{ ACT_MP_JUMP_LAND,                   ACT_MP_JUMP_LAND_MELEE_ALLCLASS,				false },
	{ ACT_MP_SWIM,	                    ACT_MP_SWIM_MELEE_ALLCLASS,					    false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,                ACT_MP_ATTACK_STAND_MELEE_ALLCLASS,			    false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,               ACT_MP_ATTACK_CROUCH_MELEE_ALLCLASS,		    false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE,	                ACT_MP_ATTACK_SWIM_MELEE_ALLCLASS,	            false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE,              ACT_MP_ATTACK_AIRWALK_MELEE_ALLCLASS,           false },

	{ ACT_MP_ATTACK_STAND_SECONDARYFIRE,	ACT_MP_ATTACK_STAND_MELEE_SECONDARY, false },
	{ ACT_MP_ATTACK_CROUCH_SECONDARYFIRE,	ACT_MP_ATTACK_CROUCH_MELEE_SECONDARY,false },
	{ ACT_MP_ATTACK_SWIM_SECONDARYFIRE,		ACT_MP_ATTACK_SWIM_MELEE,		false },
	{ ACT_MP_ATTACK_AIRWALK_SECONDARYFIRE,	ACT_MP_ATTACK_AIRWALK_MELEE,	false },

	{ ACT_MP_GESTURE_FLINCH,	ACT_MP_GESTURE_FLINCH_MELEE, false },

	{ ACT_MP_GRENADE1_DRAW,		ACT_MP_MELEE_GRENADE1_DRAW,	false },
	{ ACT_MP_GRENADE1_IDLE,		ACT_MP_MELEE_GRENADE1_IDLE,	false },
	{ ACT_MP_GRENADE1_ATTACK,	ACT_MP_MELEE_GRENADE1_ATTACK,	false },
	{ ACT_MP_GRENADE2_DRAW,		ACT_MP_MELEE_GRENADE2_DRAW,	false },
	{ ACT_MP_GRENADE2_IDLE,		ACT_MP_MELEE_GRENADE2_IDLE,	false },
	{ ACT_MP_GRENADE2_ATTACK,	ACT_MP_MELEE_GRENADE2_ATTACK,	false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH,	ACT_MP_GESTURE_VC_HANDMOUTH_MELEE,	false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT,	ACT_MP_GESTURE_VC_FINGERPOINT_MELEE,	false },
	{ ACT_MP_GESTURE_VC_FISTPUMP,	ACT_MP_GESTURE_VC_FISTPUMP_MELEE,	false },
	{ ACT_MP_GESTURE_VC_THUMBSUP,	ACT_MP_GESTURE_VC_THUMBSUP_MELEE,	false },
	{ ACT_MP_GESTURE_VC_NODYES,	ACT_MP_GESTURE_VC_NODYES_MELEE,	false },
	{ ACT_MP_GESTURE_VC_NODNO,	ACT_MP_GESTURE_VC_NODNO_MELEE,	false },
};

//Act table remapping for Merc
acttable_t *CTFCrowbar::ActivityList( int &iActivityCount )
{
	if ( GetTFPlayerOwner()->GetPlayerClass()->GetClassIndex() != TF_CLASS_CIVILIAN )
	{
		iActivityCount = ARRAYSIZE( m_acttableMeleeAllClass );
		return m_acttableMeleeAllClass;
	}
	return BaseClass::ActivityList( iActivityCount );
}

//-----------------------------------------------------------------------------
// Purpose: Do backstab damage
//-----------------------------------------------------------------------------
float CTFPoisonShank::GetMeleeDamage(CBaseEntity *pTarget, int &iCustomDamage)
{
	float flBaseDamage = BaseClass::GetMeleeDamage(pTarget, iCustomDamage);

	CTFPlayer *pPlayer = ToTFPlayer(GetPlayerOwner());

	#ifdef GAME_DLL
	CTFPlayer *pTFPlayer = ToTFPlayer(pTarget);
	#endif 

	if ( pTarget->IsPlayer() )
	{
		if (GetEnemyTeam(pTarget) == pPlayer->GetTeamNumber())
		{
		#ifdef GAME_DLL
			CTakeDamageInfo info;

			info.SetAttacker(GetOwnerEntity());		// the player who operated the thing that emitted nails
			info.SetInflictor(pPlayer);				// the weapon that emitted this projectile

			pTFPlayer->m_Shared.Poison(pPlayer, GetTFWpnData().m_flEffectDuration);

			iCustomDamage = TF_DMG_CUSTOM_POISON;
		#endif
		}
	}

	return flBaseDamage;

}
//-----------------------------------------------------------------------------
// Purpose: Do backstab damage
//-----------------------------------------------------------------------------
float CTFPipeWrench::GetMeleeDamage(CBaseEntity *pTarget, int &iCustomDamage)
{
	float flBaseDamage = BaseClass::GetMeleeDamage(pTarget, iCustomDamage);

	if (m_flChargedDamage > GetDamage())
	{
		flBaseDamage = m_flChargedDamage;
		m_flChargedDamage = GetDamage();
	}

	return flBaseDamage;
}
// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFPipeWrench::PrimaryAttack()
{
	CTFPlayer *pPlayer = ToTFPlayer(GetPlayerOwner());
	if (!pPlayer)
		return;

	if (!CanAttack())
	{
		WeaponIdle();
		return;
	}

	switch (m_iWeaponState)
	{
	default:
	case AC_STATE_IDLE:
		{
			m_flChargedDamage = GetDamage();
			if (m_flNextPrimaryAttack <= gpGlobals->curtime)
			{
				DevMsg("You start to charge up the Pipe Wrench\n");
				WindUp();
			}

			break;
		}
	case AC_STATE_CHARGE:
		{

			if (GetActivity() == ACT_BACKSTAB_VM_UP)
			{
				if (IsViewModelSequenceFinished())

				SendWeaponAnim(ACT_BACKSTAB_VM_IDLE);

				m_bReady = true;
			}

			if (m_bReady)
			{
				m_flChargedDamage = min((m_flChargedDamage + gpGlobals->frametime * GetDamage()), GetDamage() * 3);

				float m_flChargedDamageMath = (m_flChargedDamage + gpGlobals->frametime * GetDamage());
				DevMsg("m_flChargedDamage is %f\n", m_flChargedDamageMath);
			}

			break;
		}
	}
}
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFPipeWrench::WindUp(void)
{
	// Get the player owning the weapon.
	CTFPlayer *pPlayer = ToTFPlayer(GetPlayerOwner());
	if (!pPlayer)
		return;

	// Play wind-up animation and sound (SPECIAL1).
	SendWeaponAnim(ACT_BACKSTAB_VM_UP);

	// Set the appropriate firing state.
	m_iWeaponState = AC_STATE_CHARGE;

#ifdef GAME_DLL
	pPlayer->StopRandomExpressions();
#endif
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFPipeWrench::ItemPostFrame()
{
	CTFPlayer *pOwner = ToTFPlayer(GetOwner());
	if (!pOwner)
		return;

	if (pOwner->m_afButtonReleased & IN_ATTACK)
	{
		if (m_flNextPrimaryAttack <= gpGlobals->curtime)
		{ 
			DevMsg("You let go of the Pipe Wrench\n");

			Swing(pOwner);

			m_bReady = false;

			m_flNextPrimaryAttack = (	(gpGlobals->curtime + GetFireRate() ) + ( (m_flChargedDamage / GetDamage() ) / 2 )	);

			DevMsg("The Attack Delay is %f\n", m_flNextPrimaryAttack);

			m_iWeaponState = AC_STATE_IDLE;
		}
	}

	BaseClass::ItemPostFrame();
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFPipeWrench::CalcIsAttackCriticalHelper(void)
{
	CTFPlayer *pOwner = ToTFPlayer(GetOwner());

	if (!pOwner)
		return false;
	
	if (pOwner->m_Shared.InCond(TF_COND_BLASTJUMP))
	{
		return true;
	}
	else
	{
		return false;
	}
}
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFPipeWrench::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	m_flChargedDamage = GetDamage();

	m_bReady = false;

	return BaseClass::Holster(pSwitchingTo);
}