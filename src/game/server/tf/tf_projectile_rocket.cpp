
//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// TF Rocket
//
//=============================================================================
#include "cbase.h"
#include "tf_projectile_rocket.h"

//=============================================================================
//
// TF Rocket functions (Server specific).
//
#define ROCKET_MODEL "models/weapons/w_models/w_rocket.mdl"

LINK_ENTITY_TO_CLASS( tf_projectile_rocket, CTFProjectile_Rocket );
PRECACHE_REGISTER( tf_projectile_rocket );

IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_Rocket, DT_TFProjectile_Rocket )

BEGIN_NETWORK_TABLE( CTFProjectile_Rocket, DT_TFProjectile_Rocket )
	SendPropInt( SENDINFO( m_bCritical ) ),
END_NETWORK_TABLE()

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Rocket *CTFProjectile_Rocket::Create ( CTFWeaponBase *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer )
{
	CTFProjectile_Rocket *pRocket = static_cast<CTFProjectile_Rocket*>( CTFBaseRocket::Create( pWeapon, "tf_projectile_rocket", vecOrigin, vecAngles, pOwner ) );

	if ( pRocket )
	{
		pRocket->SetScorer( pScorer );
	}

	return pRocket;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Rocket::Spawn()
{
	SetModel( ROCKET_MODEL );
	
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Rocket::Precache()
{
	PrecacheModel( ROCKET_MODEL );
	PrecacheParticleSystem( "critical_rocket_blue" );
	PrecacheParticleSystem( "critical_rocket_red" );
	PrecacheParticleSystem( "rockettrail" );
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Rocket::SetScorer( CBaseEntity *pScorer )
{
	m_Scorer = pScorer;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBasePlayer *CTFProjectile_Rocket::GetScorer( void )
{
	return dynamic_cast<CBasePlayer *>( m_Scorer.Get() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CTFProjectile_Rocket::GetDamageType() 
{ 
	int iDmgType = BaseClass::GetDamageType();
	if ( m_bCritical )
	{
		iDmgType |= DMG_CRITICAL;
	}

	return iDmgType;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CTFProjectile_Rocket::GetCustomDamageType() 
{ 
	if ( m_bCritical >= 2)
	{
		return TF_DMG_CUSTOM_CRIT_POWERUP;
	}
	else
	{
		return TF_DMG_CUSTOM_NONE;
	}
}



//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFCProjectile_IncendRocket *CTFCProjectile_IncendRocket::Create ( CTFWeaponBase *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer )
{
	CTFCProjectile_IncendRocket *pRocket = static_cast<CTFCProjectile_IncendRocket*>( CTFBaseRocket::Create( pWeapon, "tfc_projectile_incendrocket", vecOrigin, vecAngles, pOwner ) );

	if ( pRocket )
	{
		pRocket->SetScorer( pScorer );
	}

	return pRocket;
}

LINK_ENTITY_TO_CLASS( tfc_projectile_incendrocket, CTFCProjectile_IncendRocket );
PRECACHE_REGISTER( tfc_projectile_incendrocket );

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(tf_projectile_bouncyrocket, CTFProjectile_BouncyRocket);
PRECACHE_REGISTER(tf_projectile_bouncyrocket);

IMPLEMENT_NETWORKCLASS_ALIASED(TFProjectile_BouncyRocket, DT_TFProjectile_BouncyRocket)

BEGIN_NETWORK_TABLE(CTFProjectile_BouncyRocket, DT_TFProjectile_BouncyRocket)
END_NETWORK_TABLE()

#define BOUNCYROCKET_TIMER 2.5f
#define BOUNCYROCKET_SPEED 750.f
#define BOUNCYROCKET_MODEL "models/weapons/w_models/w_grenade_conc.mdl"

CTFProjectile_BouncyRocket *CTFProjectile_BouncyRocket::Create(CTFWeaponBase *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer)
{
	CTFProjectile_BouncyRocket *pRocket = static_cast<CTFProjectile_BouncyRocket*>(CTFBaseRocket::Create(pWeapon, "tf_projectile_bouncyrocket", vecOrigin, vecAngles, pOwner));
	if (!pRocket)
		return NULL;

	//set movement parameters
	pRocket->SetMoveType(MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM);
	pRocket->SetElasticity(0.9f);

	//adjust angle velocity slightly upward
	Vector vel = pRocket->GetAbsVelocity();
	VectorNormalize(vel);
	QAngle angle;
	VectorAngles(vel, angle);
	angle.x -= 10.f;
	if (angle.x < 270.f && angle.x > 180.f)
		angle.x = 270.f;
	AngleVectors(angle, &vel);
	pRocket->SetAbsVelocity(vel * BOUNCYROCKET_SPEED);

	pRocket->SetThink(&CTFProjectile_BouncyRocket::FlyThink);

	return pRocket;
}

void CTFProjectile_BouncyRocket::Precache(void)
{
	//PrecacheScriptSound("Weapon_HandGrenade.GrenadeBounce");
	CTFBaseRocket::Precache();
}

void CTFProjectile_BouncyRocket::FlyThink(void)
{
	if (gpGlobals->curtime > m_flCollideWithTeammatesTime && m_bCollideWithTeammates == false)
		m_bCollideWithTeammates = true;

	//expiration time
	if (gpGlobals->curtime > m_flCreationTime + BOUNCYROCKET_TIMER)
		Detonate();

	//Water behavior
	int waterLevel = GetWaterLevel();
	SetGravity(waterLevel > WL_Waist ? 0.5f : 1.5f);
	if (waterLevel > WL_Waist)
	{
		Vector Vel = GetAbsVelocity();
		if ( waterLevel != iOldWaterLevel )
			SetAbsVelocity( Vel / 2.f );
		else if ( Vector2D(Vel.x, Vel.y).Length() > 10.f )
			SetAbsVelocity( Vector( Vel.x * 0.8f, Vel.y * 0.8f, Vel.z ) );
	}
	iOldWaterLevel = waterLevel;

	SetNextThink(gpGlobals->curtime + 0.1f);
}

void CTFProjectile_BouncyRocket::BounceSound(void)
{
	//EmitSound("Weapon_HandGrenade.GrenadeBounce");
}

void CTFProjectile_BouncyRocket::RocketTouch(CBaseEntity *pOther)
{
	// Verify a correct "other."
	Assert(pOther);
	if ( pOther && pOther->IsSolidFlagSet(FSOLID_TRIGGER | FSOLID_VOLUME_CONTENTS) )
		return;

	// Handle hitting skybox (disappear).
	const trace_t *pTrace = &CBaseEntity::GetTouchTrace();
	if (pTrace->surface.flags & SURF_SKY)
	{
		UTIL_Remove(this);
		return;
	}

	//if impacting with geometry bounce losing a bit of speed
	if (pTrace->m_pEnt->IsWorld() && GetAbsVelocity().Length() > 200.f)
	{
		Vector vecAbsVelocity;
		PhysicsClipVelocity(GetAbsVelocity(), pTrace->plane.normal, vecAbsVelocity, 2.0f);
		vecAbsVelocity *= GetWaterLevel() == WL_Eyes ? 0.6f : GetElasticity();
		SetAbsVelocity(vecAbsVelocity);

		BounceSound();
		return;
	}

	trace_t trace;
	memcpy(&trace, pTrace, sizeof(trace_t));
	Explode(&trace, pOther);
}