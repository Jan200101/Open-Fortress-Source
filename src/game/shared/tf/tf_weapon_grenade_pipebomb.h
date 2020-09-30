//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. ========//
//
// Purpose: TF Pipebomb Grenade.
//
//=============================================================================//
#ifndef TF_WEAPON_GRENADE_PIPEBOMB_H
#define TF_WEAPON_GRENADE_PIPEBOMB_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_grenadeproj.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFGrenadePipebombProjectile C_TFGrenadePipebombProjectile
#endif

//=============================================================================
//
// TF Pipebomb Grenade
//
class CTFGrenadePipebombProjectile : public CTFWeaponBaseGrenadeProj
{
public:

	DECLARE_CLASS( CTFGrenadePipebombProjectile, CTFWeaponBaseGrenadeProj );
	DECLARE_NETWORKCLASS();

	CTFGrenadePipebombProjectile();
	~CTFGrenadePipebombProjectile();

	// Unique identifier.
	virtual int			GetWeaponID( void ) const	{ return ( m_iType == TF_GL_MODE_REMOTE_DETONATE ) ? TF_WEAPON_GRENADE_PIPEBOMB : TF_WEAPON_GRENADE_DEMOMAN; }

	int GetType( void ){ return m_iType; } 
	virtual int			GetDamageType();

	void			SetChargeTime( float flChargeTime )				{ m_flChargeTime = flChargeTime; }

	CNetworkVar( bool, m_bTouched );
	CNetworkVar( int, m_iType ); // TF_GL_MODE_REGULAR or TF_GL_MODE_REMOTE_DETONATE
	float		m_flCreationTime;
	float		m_flChargeTime;
	bool		m_bPulsed;
	float		m_flFullDamage;

	virtual void	UpdateOnRemove( void );

	virtual bool	IsDeflectable( void ) { return true; }
#ifdef GAME_DLL
	virtual void	Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir );
#endif

#ifdef CLIENT_DLL

	virtual void    OnDataChanged( DataUpdateType_t updateType );
	virtual void	CreateLightEffects(void);
	virtual const char *GetTrailParticleName( void );
	virtual int     DrawModel( int flags );
	virtual void	Simulate( void );
	virtual void	SetDormant(bool bDormant);
#else

	DECLARE_DATADESC();

	// Creation.
	static CTFGrenadePipebombProjectile *Create( const Vector &position, const QAngle &angles, const Vector &velocity, 
		                                         const AngularImpulse &angVelocity, CBaseCombatCharacter *pOwner, const CTFWeaponInfo &weaponInfo, bool bRemoteDetonate, CTFWeaponBase *pWeapon );

	// Overrides.
	virtual void	Spawn();
	virtual void	Precache();
	
	virtual void	BounceSound( void );
	virtual void	Detonate();
	virtual void	Fizzle();

	void			SetPipebombMode( bool bRemoteDetonate );
	
	virtual void	PipebombTouch( CBaseEntity *pOther );
	virtual void	VPhysicsCollision( int index, gamevcollisionevent_t *pEvent );
	virtual bool	ExplodeOnImpact( void );

	virtual CBaseEntity		*GetEnemy(void)			{ return m_hEnemy; }

	virtual int		OnTakeDamage( const CTakeDamageInfo &info );
	
	virtual void	Explode( trace_t *pTrace, int bitsDamageType, int bitsCustomDamageType );


private:

	
	bool		m_bFizzle;

	CHandle<CBaseEntity>	m_hEnemy;

	float		m_flMinSleepTime;
#endif
};

#endif // TF_WEAPON_GRENADE_PIPEBOMB_H
