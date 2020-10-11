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
#define CTFPipeWrench C_TFPipeWrench
#endif

enum PipeWrenchState_t
{
	// Firing states.
	AC_STATE_IDLE = 0,
	AC_STATE_CHARGE
};

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

class CTFPipeWrench : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS(CTFPipeWrench, CTFWeaponBaseMelee);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	CNetworkVar(float, m_flChargedDamage);

	CTFPipeWrench();
	virtual void		PrimaryAttack(void);
	virtual int			GetWeaponID(void) const			{ return TF_WEAPON_PIPE_WRENCH; }
	virtual float		GetMeleeDamage(CBaseEntity *pTarget, int &iCustomDamage);
	virtual void		ItemPostFrame(void);



private:

	CTFPipeWrench(const CTFPipeWrench &) {}

	CNetworkVar(PipeWrenchState_t, m_iWeaponState);
	CNetworkVar(bool, m_bReady);

	void WindUp(void);

};
#endif // TF_WEAPON_CROWBAR_H
