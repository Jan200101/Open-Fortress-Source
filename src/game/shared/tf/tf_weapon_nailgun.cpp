//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_nailgun.h"

#ifdef CLIENT_DLL
	#include "c_tf_player.h"
#else
	#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon SMG tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFNailgun, DT_WeaponNailgun )

BEGIN_NETWORK_TABLE( CTFNailgun, DT_WeaponNailgun )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFNailgun )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_nailgun, CTFNailgun );
//PRECACHE_WEAPON_REGISTER( tf_weapon_nailgun);

// Server specific.
#ifdef GAME_DLL
BEGIN_DATADESC( CTFNailgun )
END_DATADESC()
#endif


IMPLEMENT_NETWORKCLASS_ALIASED( TFCNailgun, DT_TFCNailgun )

BEGIN_NETWORK_TABLE( CTFCNailgun, DT_TFCNailgun )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCNailgun )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tfc_weapon_nailgun, CTFCNailgun );
//PRECACHE_WEAPON_REGISTER( tfc_weapon_nailgun);

// Server specific.
#ifdef GAME_DLL
BEGIN_DATADESC( CTFCNailgun )
END_DATADESC()
#endif


IMPLEMENT_NETWORKCLASS_ALIASED( TFCNailgunSuper, DT_TFCNailgunSuper )

BEGIN_NETWORK_TABLE( CTFCNailgunSuper, DT_TFCNailgunSuper )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCNailgunSuper )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tfc_weapon_nailgun_super, CTFCNailgunSuper );
//PRECACHE_WEAPON_REGISTER( tfc_weapon_nailgun_super );

// Server specific.
#ifdef GAME_DLL
BEGIN_DATADESC( CTFCNailgunSuper )
END_DATADESC()
#endif


IMPLEMENT_NETWORKCLASS_ALIASED(TFFlakCannon, DT_TFFlakCannon)

BEGIN_NETWORK_TABLE(CTFFlakCannon, DT_TFFlakCannon)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CTFFlakCannon)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(tf_weapon_flakcannon, CTFFlakCannon);
//PRECACHE_WEAPON_REGISTER( tf_weapon_flakcannon );

// Server specific.
#ifdef GAME_DLL
BEGIN_DATADESC( CTFFlakCannon )
END_DATADESC()
#endif

//-----------------------------------------------------------------------------
// Purpose: Detonate active pipebombs
//-----------------------------------------------------------------------------
void CTFFlakCannon::SecondaryAttack(void)
{
	Msg("Second Attack\n");
	
	if ((m_iReserveAmmo - 5) >= 0)
	{
		m_iReserveAmmo = m_iReserveAmmo - 5;
		m_iWeaponMode = TF_WEAPON_SECONDARY_MODE;

		if (m_flNextSecondaryAttack > gpGlobals->curtime)
			return;

		CTFPlayer *pPlayer = ToTFPlayer(GetPlayerOwner());
		if (!pPlayer)
			return;

		FireFlakBall(pPlayer);
		pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);

		float flFiringInterval = GetFireRate();
		m_flNextSecondaryAttack = gpGlobals->curtime + flFiringInterval;
		m_flNextPrimaryAttack = gpGlobals->curtime + flFiringInterval;
	}
	else
	{
		Msg("No, you can't shot, idiot.\n");
	}
}

//=============================================================================
//
// Weapon Nailgun functions.
//