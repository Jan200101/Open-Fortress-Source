//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======
//
// TF Nail
//
//=============================================================================
#include "cbase.h"
#include "tf_projectile_nail.h"

#ifdef CLIENT_DLL
	#include "c_tf_player.h"
#endif
#ifdef GAME_DLL
#include "tf_player.h"
#endif

//=============================================================================
//
// TF Syringe Projectile functions (Server specific).
//
#define SYRINGE_MODEL				"models/weapons/w_models/w_syringe_proj.mdl"
#define SYRINGE_DISPATCH_EFFECT		"ClientProjectile_Syringe"
#define SYRINGE_GRAVITY	0.3f

LINK_ENTITY_TO_CLASS(tf_projectile_syringe, CTFProjectile_Syringe);
PRECACHE_REGISTER(tf_projectile_syringe);

short g_sModelIndexSyringe;
void PrecacheSyringe(void *pUser)
{
	g_sModelIndexSyringe = modelinfo->GetModelIndex(SYRINGE_MODEL);
}

PRECACHE_REGISTER_FN(PrecacheSyringe);

CTFProjectile_Syringe::CTFProjectile_Syringe()
{
}

CTFProjectile_Syringe::~CTFProjectile_Syringe()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Syringe *CTFProjectile_Syringe::Create(const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer, int bCritical)
{
	return static_cast<CTFProjectile_Syringe*>(CTFBaseProjectile::Create("tf_projectile_syringe", vecOrigin, vecAngles, pOwner, CTFProjectile_Syringe::GetInitialVelocity(), g_sModelIndexSyringe, SYRINGE_DISPATCH_EFFECT, pScorer, bCritical));
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CTFProjectile_Syringe::GetProjectileModelName(void)
{
	return SYRINGE_MODEL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CTFProjectile_Syringe::GetGravity(void)
{
	return SYRINGE_GRAVITY;
}

#ifdef CLIENT_DLL

//-----------------------------------------------------------------------------
// Purpose: 
// Output : const char
//-----------------------------------------------------------------------------
const char *GetSyringeTrailParticleName(int iTeamNumber, bool bCritical)
{
	if (iTeamNumber == TF_TEAM_BLUE)
	{
		return (bCritical ? "nailtrails_medic_blue_crit" : "nailtrails_medic_blue");
	}
	else if (iTeamNumber == TF_TEAM_RED)
	{
		return (bCritical ? "nailtrails_medic_red_crit" : "nailtrails_medic_red");
	}
	else
	{
		return (bCritical ? "nailtrails_medic_dm_crit" : "nailtrails_medic_dm");
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ClientsideProjectileSyringeCallback(const CEffectData &data)
{
	// Get the syringe and add it to the client entity list, so we can attach a particle system to it.
	C_TFPlayer *pPlayer = dynamic_cast<C_TFPlayer*>(ClientEntityList().GetBaseEntityFromHandle(data.m_hEntity));
	if (pPlayer)
	{
		C_LocalTempEntity *pSyringe = ClientsideProjectileCallback(data, SYRINGE_GRAVITY);
		if (pSyringe)
		{
			if (pPlayer->GetTeamNumber() == TF_TEAM_RED) pSyringe->m_nSkin = 0;
			else if (pPlayer->GetTeamNumber() == TF_TEAM_BLUE) pSyringe->m_nSkin = 1;
			else pSyringe->m_nSkin = 2;
			bool bCritical = ((data.m_nDamageType & DMG_CRITICAL) != 0);


			pPlayer->m_Shared.UpdateParticleColor(pSyringe->AddParticleEffect(GetSyringeTrailParticleName(pPlayer->GetTeamNumber(), bCritical)));
			pSyringe->AddEffects(EF_NOSHADOW);
			pSyringe->flags |= FTENT_USEFASTCOLLISIONS;
		}
	}
}

DECLARE_CLIENT_EFFECT(SYRINGE_DISPATCH_EFFECT, ClientsideProjectileSyringeCallback);

#endif

//=============================================================================
//
// TF Nailgun Projectile functions (Server specific).
//
#define NAILGUN_NAIL_MODEL				"models/weapons/w_models/w_nail.mdl"
#define NAILGUN_NAIL_DISPATCH_EFFECT	"ClientProjectile_Nail"
#define NAILGUN_NAIL_GRAVITY	0.3f

LINK_ENTITY_TO_CLASS(tf_projectile_nail, CTFProjectile_Nail);
PRECACHE_REGISTER(tf_projectile_nail);

short g_sModelIndexNail;
void PrecacheNail(void *pUser)
{
	g_sModelIndexNail = modelinfo->GetModelIndex(NAILGUN_NAIL_MODEL);
}

PRECACHE_REGISTER_FN(PrecacheNail);

CTFProjectile_Nail::CTFProjectile_Nail()
{
}

CTFProjectile_Nail::~CTFProjectile_Nail()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Nail *CTFProjectile_Nail::Create(const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer, int bCritical)
{
	CTFProjectile_Nail *pNail = static_cast<CTFProjectile_Nail*>(CTFBaseProjectile::Create("tf_projectile_nail", vecOrigin, vecAngles, pOwner, CTFProjectile_Nail::GetInitialVelocity(), g_sModelIndexNail, NAILGUN_NAIL_DISPATCH_EFFECT, pScorer, bCritical));
//	pNail->SetExplosionDamage(iExplosionDamage);
//	pNail->SetExplosionRadius(flExplosionRadius);
	return pNail;
	//return static_cast<CTFProjectile_Nail*>(CTFBaseProjectile::Create("tf_projectile_nail", vecOrigin, vecAngles, pOwner, CTFProjectile_Nail::GetInitialVelocity(), g_sModelIndexNail, NAILGUN_NAIL_DISPATCH_EFFECT, pScorer, bCritical));
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CTFProjectile_Nail::GetProjectileModelName(void)
{
	return NAILGUN_NAIL_MODEL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CTFProjectile_Nail::GetGravity(void)
{
	return NAILGUN_NAIL_GRAVITY;
}
#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
// Output : const char
//-----------------------------------------------------------------------------
const char *GetNailTrailParticleName(int iTeamNumber, bool bCritical)
{
	if (iTeamNumber == TF_TEAM_BLUE)
	{
		return (bCritical ? "nailtrails_super_blue_crit" : "nailtrails_super_blue");
	}
	else if (iTeamNumber == TF_TEAM_RED)
	{
		return (bCritical ? "nailtrails_super_red_crit" : "nailtrails_super_red");
	}
	else
	{
		return (bCritical ? "nailtrails_super_dm_crit" : "nailtrails_super_dm");
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ClientsideProjectileNailCallback(const CEffectData &data)
{
	C_TFPlayer *pPlayer = dynamic_cast<C_TFPlayer*>(ClientEntityList().GetBaseEntityFromHandle(data.m_hEntity));
	if (pPlayer)
	{
		C_LocalTempEntity *pNail = ClientsideProjectileCallback(data, NAILGUN_NAIL_GRAVITY);
		if (pNail)
		{
			switch (pPlayer->GetTeamNumber())
			{
			case TF_TEAM_RED:
				pNail->m_nSkin = 0;
				break;
			case TF_TEAM_BLUE:
				pNail->m_nSkin = 1;
				break;
			case TF_TEAM_MERCENARY:
				pNail->m_nSkin = 2;
				break;
			}
			bool bCritical = ((data.m_nDamageType & DMG_CRITICAL) != 0);
			pPlayer->m_Shared.UpdateParticleColor(pNail->AddParticleEffect(GetNailTrailParticleName(pPlayer->GetTeamNumber(), bCritical)));
			pNail->AddEffects(EF_NOSHADOW);
			pNail->flags |= FTENT_USEFASTCOLLISIONS;
		}
	}
}

DECLARE_CLIENT_EFFECT(NAILGUN_NAIL_DISPATCH_EFFECT, ClientsideProjectileNailCallback);

#endif

//=============================================================================
//
// TF Tranq Projectile functions (Server specific).
//
//=============================================================================
#define TRANQ_MODEL				"models/weapons/w_models/w_classic_tranq_proj.mdl"
#define TRANQ_DISPATCH_EFFECT	"ClientProjectile_Tranq"
#define TRANQ_GRAVITY	0.01f

LINK_ENTITY_TO_CLASS(tf_projectile_tranq, CTFProjectile_Tranq);
PRECACHE_REGISTER(tf_projectile_tranq);

short g_sModelIndexTranq;
void PrecacheTranq(void *pUser)
{
	g_sModelIndexTranq = modelinfo->GetModelIndex(TRANQ_MODEL);
}

PRECACHE_REGISTER_FN(PrecacheTranq);


CTFProjectile_Tranq::CTFProjectile_Tranq()
{
}

CTFProjectile_Tranq::~CTFProjectile_Tranq()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Tranq *CTFProjectile_Tranq::Create(const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer, int bCritical)
{
	return static_cast<CTFProjectile_Tranq*>(CTFBaseProjectile::Create("tf_projectile_tranq", vecOrigin, vecAngles, pOwner, CTFProjectile_Tranq::GetInitialVelocity(), g_sModelIndexTranq, TRANQ_DISPATCH_EFFECT, pScorer, bCritical));
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CTFProjectile_Tranq::GetProjectileModelName(void)
{
	return TRANQ_MODEL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CTFProjectile_Tranq::GetGravity(void)
{
	return TRANQ_GRAVITY;
}
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
#ifdef GAME_DLL
void CTFProjectile_Tranq::ProjectileTouch(CBaseEntity *pOther)
{
	// Verify a correct "other."
	Assert(pOther);
	if (!pOther->IsSolid() || pOther->IsSolidFlagSet(FSOLID_VOLUME_CONTENTS))
		return;

	// Handle hitting skybox (disappear).
	const trace_t *pTrace = &CBaseEntity::GetTouchTrace();
	trace_t *pNewTrace = const_cast<trace_t*>(pTrace);

	if (pTrace->surface.flags & SURF_SKY)
	{
		UTIL_Remove(this);
		return;
	}

	// pass through ladders
	if (pTrace->surface.flags & CONTENTS_LADDER)
		return;

	if (pOther->IsWorld())
	{
		Msg("ass\n");
		SetAbsVelocity(vec3_origin);
		AddSolidFlags(FSOLID_NOT_SOLID);

		// Remove immediately. Clientside projectiles will stick in the wall for a bit.
		UTIL_Remove(this);
		return;
	}

	// determine the inflictor, which is the weapon which fired this projectile
	CBaseEntity *pInflictor = NULL;
	CTFPlayer *pTFOwner = ToTFPlayer( GetOwnerEntity() );

	if (pTFOwner)
		pInflictor = pTFOwner->Weapon_OwnsThisID(GetWeaponID());

	CTakeDamageInfo info;
	info.SetAttacker( pTFOwner );					// the player who operated the thing that emitted nails
	info.SetInflictor( pInflictor );				// the weapon that emitted this projectile
	info.SetDamage( GetDamage() );
	info.SetDamageForce( GetDamageForce() );
	info.SetDamagePosition( GetAbsOrigin() );
	info.SetDamageType( GetDamageType() );
	info.SetDamageCustom( GetCustomDamageType() );

	Vector dir;
	AngleVectors(GetAbsAngles(), &dir);

	pOther->DispatchTraceAttack(info, dir, pNewTrace);
	ApplyMultiDamage();

	CTFPlayer *pTFOther = ToTFPlayer(pOther);

	if ( pTFOther )
	{
		WEAPON_FILE_INFO_HANDLE	hWpnInfo = LookupWeaponInfoSlot( pInflictor->GetClassname() );
		Assert( hWpnInfo != GetInvalidWeaponInfoHandle() );
		CTFWeaponInfo *pWeaponInfo = dynamic_cast<CTFWeaponInfo *>( GetFileWeaponInfoFromHandle(hWpnInfo) );

		if (GetEnemyTeam(pTFOther) == pTFOwner->GetTeamNumber())
			pTFOther->m_Shared.Tranq( pTFOwner, pWeaponInfo->m_flEffectDuration, pWeaponInfo->m_flSpeedReduction, pWeaponInfo->m_flWeaponSpeedReduction);
	}

	UTIL_Remove(this);
}
#endif

#ifdef CLIENT_DLL

//-----------------------------------------------------------------------------
// Purpose: 
// Output : const char
//-----------------------------------------------------------------------------
const char *GetTranqTrailParticleName(int iTeamNumber, bool bCritical)
{
	if (iTeamNumber == TF_TEAM_BLUE)
	{
		return (bCritical ? "tranq_tracer_teamcolor_blue_crit" : "tranq_tracer_teamcolor_blue");
	}
	else if (iTeamNumber == TF_TEAM_RED)
	{
		return (bCritical ? "tranq_tracer_teamcolor_red_crit" : "tranq_tracer_teamcolor_red");
	}
	else
	{
		return (bCritical ? "tranq_tracer_teamcolor_dm_crit" : "tranq_tracer_teamcolor_dm");
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ClientsideProjectileTranqCallback(const CEffectData &data)
{
	C_TFPlayer *pPlayer = dynamic_cast<C_TFPlayer*>(ClientEntityList().GetBaseEntityFromHandle(data.m_hEntity));
	if (pPlayer)
	{
		C_LocalTempEntity *pNail = ClientsideProjectileCallback(data, TRANQ_GRAVITY);
		if (pNail)
		{
			switch (pPlayer->GetTeamNumber())
			{
			case TF_TEAM_RED:
				pNail->m_nSkin = 0;
				break;
			case TF_TEAM_BLUE:
				pNail->m_nSkin = 1;
				break;
			case TF_TEAM_MERCENARY:
				pNail->m_nSkin = 2;
				break;
			}
			bool bCritical = ((data.m_nDamageType & DMG_CRITICAL) != 0);
			pPlayer->m_Shared.UpdateParticleColor(pNail->AddParticleEffect(GetTranqTrailParticleName(pPlayer->GetTeamNumber(), bCritical)));
			pNail->AddEffects(EF_NOSHADOW);
			pNail->flags |= FTENT_USEFASTCOLLISIONS;
		}
	}
}

DECLARE_CLIENT_EFFECT(TRANQ_DISPATCH_EFFECT, ClientsideProjectileTranqCallback);

#endif
//=============================================================================
//
// TF FlakNail Projectile functions (Server specific).
//
//=============================================================================
#define FLAKNAIL_MODEL				"models/weapons/w_models/w_nail.mdl"
#define FLAKNAIL_DISPATCH_EFFECT	"ClientProjectile_FlakNail"
#define FLAKNAIL_GRAVITY	0.01f

LINK_ENTITY_TO_CLASS(tf_projectile_flaknail, CTFProjectile_FlakNail);
PRECACHE_REGISTER(tf_projectile_flaknail);

short g_sModelIndexFlakNail;
void PrecacheFlakNail(void *pUser)
{
	g_sModelIndexFlakNail = modelinfo->GetModelIndex(FLAKNAIL_MODEL);
}

PRECACHE_REGISTER_FN(PrecacheFlakNail);


CTFProjectile_FlakNail::CTFProjectile_FlakNail()
{
}

CTFProjectile_FlakNail::~CTFProjectile_FlakNail()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_FlakNail *CTFProjectile_FlakNail::Create(const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer, int bCritical)
{
	return static_cast<CTFProjectile_FlakNail*>(CTFBaseProjectile::Create("tf_projectile_flaknail", vecOrigin, vecAngles, pOwner, CTFProjectile_FlakNail::GetInitialVelocity(), g_sModelIndexFlakNail, FLAKNAIL_DISPATCH_EFFECT, pScorer, bCritical));
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CTFProjectile_FlakNail::GetProjectileModelName(void)
{
	return FLAKNAIL_MODEL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CTFProjectile_FlakNail::GetGravity(void)
{
	return FLAKNAIL_GRAVITY;
}
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
#ifdef GAME_DLL
void CTFProjectile_FlakNail::ProjectileTouch(CBaseEntity *pOther)
{
	// Verify a correct "other."
	Assert(pOther);
	if (!pOther->IsSolid() || pOther->IsSolidFlagSet(FSOLID_VOLUME_CONTENTS))
		return;

	// Handle hitting skybox (disappear).
	const trace_t *pTrace = &CBaseEntity::GetTouchTrace();
	trace_t *pNewTrace = const_cast<trace_t*>(pTrace);

	if (pTrace->surface.flags & SURF_SKY)
	{
		UTIL_Remove(this);
		return;
	}

	// pass through ladders
	if (pTrace->surface.flags & CONTENTS_LADDER)
		return;

	if (pOther->IsWorld())
	{
		Msg("worldtouch\n");
		Vector vecAbsVelocity = GetAbsVelocity();

		//do the bounce
		float backoff = DotProduct(vecAbsVelocity, pTrace->plane.normal) * 2.0f;
		Vector change = pTrace->plane.normal * backoff;
		vecAbsVelocity -= change;
		vecAbsVelocity *= 0.6;
		return;
	}

	// determine the inflictor, which is the weapon which fired this projectile
	CBaseEntity *pInflictor = NULL;
	CTFPlayer *pTFOwner = ToTFPlayer(GetOwnerEntity());

	if (pTFOwner)
		pInflictor = pTFOwner->Weapon_OwnsThisID(GetWeaponID());

	CTakeDamageInfo info;
	info.SetAttacker(pTFOwner);					// the player who operated the thing that emitted nails
	info.SetInflictor(pInflictor);				// the weapon that emitted this projectile
	info.SetDamage(GetDamage());
	info.SetDamageForce(GetDamageForce());
	info.SetDamagePosition(GetAbsOrigin());
	info.SetDamageType(GetDamageType());
	info.SetDamageCustom(GetCustomDamageType());

	Vector dir;
	AngleVectors(GetAbsAngles(), &dir);

	pOther->DispatchTraceAttack(info, dir, pNewTrace);
	ApplyMultiDamage();

	UTIL_Remove(this);
}
#endif

#ifdef CLIENT_DLL

//-----------------------------------------------------------------------------
// Purpose: 
// Output : const char
//-----------------------------------------------------------------------------
const char *GetFlakNailTrailParticleName(int iTeamNumber, bool bCritical)
{
	if (iTeamNumber == TF_TEAM_BLUE)
	{
		return (bCritical ? "nailtrails_super_blue_crit" : "nailtrails_super_blue");
	}
	else if (iTeamNumber == TF_TEAM_RED)
	{
		return (bCritical ? "nailtrails_super_red_crit" : "nailtrails_super_red");
	}
	else
	{
		return (bCritical ? "nailtrails_super_dm_crit" : "nailtrails_super_dm");
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ClientsideProjectileFlakNailCallback(const CEffectData &data)
{
	C_TFPlayer *pPlayer = dynamic_cast<C_TFPlayer*>(ClientEntityList().GetBaseEntityFromHandle(data.m_hEntity));
	if (pPlayer)
	{
		C_LocalTempEntity *pNail = ClientsideProjectileCallback(data, FLAKNAIL_GRAVITY);
		if (pNail)
		{
			switch (pPlayer->GetTeamNumber())
			{
			case TF_TEAM_RED:
				pNail->m_nSkin = 0;
				break;
			case TF_TEAM_BLUE:
				pNail->m_nSkin = 1;
				break;
			case TF_TEAM_MERCENARY:
				pNail->m_nSkin = 2;
				break;
			}
			bool bCritical = ((data.m_nDamageType & DMG_CRITICAL) != 0);
			pPlayer->m_Shared.UpdateParticleColor(pNail->AddParticleEffect(GetFlakNailTrailParticleName(pPlayer->GetTeamNumber(), bCritical)));
			pNail->AddEffects(EF_NOSHADOW);
			pNail->flags |= FTENT_USEFASTCOLLISIONS;
		}
	}
}

DECLARE_CLIENT_EFFECT(FLAKNAIL_DISPATCH_EFFECT, ClientsideProjectileFlakNailCallback);

#endif