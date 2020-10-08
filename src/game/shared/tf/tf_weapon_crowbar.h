//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_CROWBAR_H
#define TF_WEAPON_CROWBAR_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CTFCrowbar C_TFCrowbar
#define CTFUmbrella C_TFUmbrella
#define CTFCCrowbar C_TFCCrowbar
#define CTFCUmbrella C_TFCUmbrella
#define CTFPoisonShank C_TFPoisonShank
#define CTFLeadPipe C_TFLeadPipe
#endif

extern acttable_t m_acttableMeleeAllClass[];

//=============================================================================
//
// Crowbar class.
//
class CTFCrowbar : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFCrowbar, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFCrowbar();
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_CROWBAR; }
	virtual acttable_t *ActivityList( int &iActivityCount );

private:

	CTFCrowbar( const CTFCrowbar & ) {}
};

class CTFUmbrella : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFUmbrella, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFUmbrella();
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_UMBRELLA; }

private:

	CTFUmbrella( const CTFUmbrella & ) {}
};

class CTFCCrowbar : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS(CTFCCrowbar, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFCCrowbar();
	virtual int			GetWeaponID( void ) const			{ return TFC_WEAPON_CROWBAR; }

private:

	CTFCCrowbar( const CTFCCrowbar & ) {}
};

class CTFCUmbrella : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS(CTFCUmbrella, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFCUmbrella();
	virtual int			GetWeaponID( void ) const			{ return TFC_WEAPON_UMBRELLA; }

private:

	CTFCUmbrella( const CTFCUmbrella & ) {}
};

class CTFPoisonShank : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS(CTFPoisonShank, CTFWeaponBaseMelee);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFPoisonShank();
	virtual int			GetWeaponID(void) const			{ return TF_WEAPON_POISONSHANK; }
	virtual float		GetMeleeDamage(CBaseEntity *pTarget, int &iCustomDamage);

private:

	CTFPoisonShank(const CTFPoisonShank &) {}
};

class CTFLeadPipe : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS(CTFLeadPipe, CTFWeaponBaseMelee);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	CNetworkVar(float, m_flChargedDamage);

	CTFLeadPipe();
	virtual void		PrimaryAttack(void);
	virtual void		ItemPostFrame(void);
	virtual int			GetWeaponID(void) const			{ return TF_WEAPON_LEAD_PIPE; }
	virtual float		GetMeleeDamage(CBaseEntity *pTarget, int &iCustomDamage);
	//virtual void		LeadPipeThink(void);

	virtual bool		SendWeaponAnim(int iActivity);

	//virtual void		SendPlayerAnimEvent(CTFPlayer *pPlayer);

private:

	CTFLeadPipe(const CTFLeadPipe &) {}

	virtual bool Holster(CBaseCombatWeapon *pSwitchingTo);

	CNetworkVar(bool, m_bReady);

};
#endif // TF_WEAPON_CROWBAR_H
