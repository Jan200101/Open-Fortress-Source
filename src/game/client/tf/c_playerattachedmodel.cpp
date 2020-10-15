//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: A clientside, visual only model that's attached to players
//
//=============================================================================

#include "cbase.h"
#include "c_playerattachedmodel.h"
#include "c_tf_player.h"

// Todo: Turn these all into parameters
#define PAM_ANIMATE_TIME		0.075
#define PAM_ROTATE_TIME			0.075

#define PAM_SCALE_SPEED			7
#define PAM_MAX_SCALE			3
#define PAM_SPIN_SPEED			360

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_PlayerAttachedModel *C_PlayerAttachedModel::Create( const char *pszModelName, C_BaseEntity *pParent, int iAttachment, Vector vecOffset, float flLifetime, int iFlags, int iEffects, bool SpyMask )
{
	C_PlayerAttachedModel *pFlash = new C_PlayerAttachedModel;
	if ( !pFlash )
		return NULL;

	if ( !pFlash->Initialize( pszModelName, pParent, iAttachment, vecOffset, flLifetime, iFlags, iEffects, SpyMask ) )
		return NULL;

	return pFlash;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C_PlayerAttachedModel::Initialize( const char *pszModelName, C_BaseEntity *pParent, int iAttachment, Vector vecOffset, float flLifetime, int iFlags, int iEffects ,bool SpyMask )
{
	m_bSpyMask = false;
	AddEffects( EF_NOSHADOW | iEffects );
	if ( InitializeAsClientEntity( pszModelName, RENDER_GROUP_OPAQUE_ENTITY ) == false )
	{
		Release();
		return false;
	}

	if ( SpyMask == true )
	{
		m_bSpyMask = true;
	}

	SetParent( pParent, iAttachment ); 
	SetLocalOrigin( vecOffset );
	SetLocalAngles( vec3_angle );

	AddSolidFlags( FSOLID_NOT_SOLID );

	SetLifetime( flLifetime );
	SetNextClientThink( CLIENT_THINK_ALWAYS );

	SetCycle( 0 );

	m_iFlags = iFlags;
	m_flScale = 0;

	if ( m_iFlags & PAM_ROTATE_RANDOMLY )
	{
		m_flRotateAt = gpGlobals->curtime + PAM_ANIMATE_TIME;
	}
	if ( m_iFlags & PAM_ANIMATE_RANDOMLY )
	{
		m_flAnimateAt = gpGlobals->curtime + PAM_ROTATE_TIME;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_PlayerAttachedModel::SetLifetime( float flLifetime )
{
	if ( flLifetime == PAM_PERMANENT )
	{
		m_flExpiresAt = PAM_PERMANENT;
	}
	else
	{
		// Expire when the lifetime is up
		m_flExpiresAt = gpGlobals->curtime + flLifetime;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_PlayerAttachedModel::ClientThink( void )
{
	if ( !GetMoveParent() || (m_flExpiresAt != PAM_PERMANENT && gpGlobals->curtime > m_flExpiresAt) )
	{
		Release();
		return;
	}

	if ( m_iFlags & PAM_ANIMATE_RANDOMLY && gpGlobals->curtime > m_flAnimateAt )
	{
		float flDelta = RandomFloat(0.2,0.4) * (RandomInt(0,1) == 1 ? 1 : -1);
		float flCycle = clamp( GetCycle() + flDelta, 0, 1 );
		SetCycle( flCycle );
		m_flAnimateAt = gpGlobals->curtime + PAM_ANIMATE_TIME;
	}

	if ( m_iFlags & PAM_ROTATE_RANDOMLY && gpGlobals->curtime > m_flRotateAt )
	{
		SetLocalAngles( QAngle(0,0,RandomFloat(0,360)) );
		m_flRotateAt = gpGlobals->curtime + PAM_ROTATE_TIME;
	}

	if ( m_iFlags & PAM_SPIN_Z )
	{
		float flAng = GetAbsAngles().y + (gpGlobals->frametime * PAM_SPIN_SPEED);
		SetLocalAngles( QAngle(0,flAng,0) );
	}

	if ( m_iFlags & PAM_SCALEUP )
	{
		m_flScale = min( m_flScale + (gpGlobals->frametime * PAM_SCALE_SPEED), PAM_MAX_SCALE );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_PlayerAttachedModel::ApplyBoneMatrixTransform( matrix3x4_t& transform )
{
	BaseClass::ApplyBoneMatrixTransform( transform );

	if ( !(m_iFlags & PAM_SCALEUP) )
		return;

	VectorScale( transform[0], m_flScale, transform[0] );
	VectorScale( transform[1], m_flScale, transform[1] );
	VectorScale( transform[2], m_flScale, transform[2] );
}	

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int C_PlayerAttachedModel::DrawModel( int flags )
{

	int ret = BaseClass::DrawModel( flags );
	
	C_TFPlayer *pLocalPlayer = dynamic_cast<C_TFPlayer*>(GetMoveParent());
	
	if ( pLocalPlayer && pLocalPlayer->m_Shared.InCondUber() )
	{
		
		// Force the invulnerable material
		modelrender->ForcedMaterialOverride( *pLocalPlayer->GetInvulnMaterialRef() );
		ret = this->DrawOverriddenViewmodel( flags );
		
		modelrender->ForcedMaterialOverride( NULL );
	}
	return ret;
}

int C_PlayerAttachedModel::DrawOverriddenViewmodel( int flags )
{
	return BaseClass::DrawModel( flags );
}

bool C_PlayerAttachedModel::ShouldDraw( void )
{
	C_TFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( m_bSpyMask )
	{
		if ( !pOwner )
			return false;

		if ( !pOwner->ShouldDrawThisPlayer() )
			return false;

		if ( pOwner->IsEnemyPlayer() )
			return false;

		return BaseClass::ShouldDraw();
	}
	else
	{
		if ( !pOwner )
			return false;
		
		if ( !pOwner->ShouldDrawThisPlayer() )
			return false;
		
		if ( pOwner->IsEffectActive(EF_NODRAW) )
			return false;
		
		return BaseClass::ShouldDraw();
	}
}

int C_PlayerAttachedModel::GetSkin( void )
{
	if ( m_bSpyMask )
	{
		C_TFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );

		if ( pOwner && pOwner->m_Shared.InCond( TF_COND_DISGUISED ) && !pOwner->IsEnemyPlayer() )
		{
			return ( pOwner->m_Shared.GetDisguiseClass() - 1 );
		}

		return m_nSkin;
	}
	else
	{
		return m_nSkin;
	}
}