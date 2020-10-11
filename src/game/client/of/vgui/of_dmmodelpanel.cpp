//========= Copyright � 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"
#include "of_dmmodelpanel.h"
#include "of_shared_schemas.h"
#include "gameui/basemodui.h"

using namespace BaseModUI;
using namespace vgui;

extern ConVar of_tennisball;
extern ConVar of_respawn_particle;
extern ConVar of_announcer_override;

DECLARE_BUILD_FACTORY(DMModelPanel);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
DMModelPanel::DMModelPanel(Panel *parent, const char *panelName) : BaseClass(parent, panelName) 
{
	iWeaponAnim = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void DMModelPanel::ApplySettings( KeyValues *inResourceData )
{
	BaseClass::ApplySettings(inResourceData);
	SetWeaponModel("models/weapons/w_models/w_supershotgun.mdl", 0);
}

//-----------------------------------------------------------------------------
// Purpose: Applies scheme settings
//-----------------------------------------------------------------------------
void DMModelPanel::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetLoadoutCosmetics();
	KeyValues *pWeapons = GetLoadout()->FindKey("Weapons");
	if( pWeapons )
	{
		KeyValues *pMercenary = pWeapons->FindKey("mercenary");
		if( pMercenary )
		{
			int i = 1;
			while( i < 3 )
			{
				KeyValues *pWeapon = GetWeaponFromSchema( pMercenary->GetString( VarArgs("%d", i) ) );
				if( pWeapon )
				{
					if( pWeapon->GetInt( "loadout_anim", -1 ) != -1 )
					{
						SetWeaponModel( pWeapon->FindKey( "WeaponData" )->GetString( "playermodel" ), pWeapon->GetInt( "loadout_anim" ) );
						break;
					}
				}
				i++;
			}
		}
	}

	SetPaintBackgroundEnabled(true);

	// Set the animation.
	Update();

	m_iCurrentParticle = of_respawn_particle.GetInt();
}

void DMModelPanel::PerformLayout()
{
	BaseClass::PerformLayout();

}

void DMModelPanel::PaintBackground()
{
	BaseClass::PaintBackground();
	
	if( m_iCurrentParticle != of_respawn_particle.GetInt() )
	{
		m_iCurrentParticle = of_respawn_particle.GetInt();

		KeyValues *pParticle = GetRespawnParticle( m_iCurrentParticle );
		if( pParticle )
		{
			m_flParticleZOffset = pParticle->GetFloat( "particle_z_offset", 0.0f );
		}
		
		char pEffectName[32];
		pEffectName[0] = '\0';
		if ( m_iCurrentParticle < 10 )
			Q_snprintf( pEffectName, sizeof( pEffectName ), "dm_respawn_0%d", m_iCurrentParticle );
		else
			Q_snprintf( pEffectName, sizeof( pEffectName ), "dm_respawn_%d", m_iCurrentParticle );
		if ( pEffectName[0] != '\0' )
			SetParticleName(pEffectName);
		
	}

	if( of_tennisball.GetBool() != m_bTennisball )
	{
		m_bTennisball = of_tennisball.GetBool();
		m_BMPResData.m_nSkin = m_bTennisball ? 6 : 4;
		Update();
		// Since cosmetics can now have different tenisball skins, we need to update them
		// *sigh*, team classic
		m_bUpdateCosmetics = true;
	}

	if( m_bUpdateCosmetics )
	{
		if( !GetModelPtr()->IsValid() ) 
			return;

		ClearMergeMDLs();
		for( int i = 0; i < GetNumBodyGroups(); i++ )
		{
			SetBodygroup(i, 0);
		}
		// Set the animation.
		SetMergeMDL( szWeaponModel, NULL, 2 );
		for( int i = 0; i < m_flCosmetics.Count(); i++ )
		{
			KeyValues *pCosmetic = GetCosmetic( m_flCosmetics[i] );
			if( pCosmetic )
			{
				float flCosmetic = m_flCosmetics[i];
				flCosmetic += 0.001f;
				flCosmetic = flCosmetic - (int)m_flCosmetics[i];
				flCosmetic *= 100.0f;
				int iStyle = (int)flCosmetic;
				KeyValues *pInfo = new KeyValues("CosmeticInfo");
				pCosmetic->CopySubkeys(pInfo);

				KeyValues *pStyles = pCosmetic->FindKey("styles");
				if( pStyles )
				{
					KeyValues *pStyle = pStyles->FindKey(VarArgs("%i", iStyle));
					if( pStyle )
					{
						pStyle->CopySubkeys(pInfo);
						pInfo->RecursiveMergeKeyValues(pCosmetic);
					}
				}
				int iVisibleTeam = 1;
				int iTeamCount = 1;

				if( pInfo->GetBool( "team_skins", true ) )
				{
					iTeamCount = 3;
					iVisibleTeam = 2;
				}
				
				if( pInfo->GetBool( "uses_brightskins" ) )
				{
					iTeamCount++;
					if( of_tennisball.GetBool() )
						iVisibleTeam = iTeamCount - 1;
				}
				
				int nSkin = iVisibleTeam < 0 ? 0 : iVisibleTeam;
				
				nSkin += iTeamCount * pInfo->GetInt( "skin_offset" );

				if( strcmp( pInfo->GetString("model"), "BLANK" ) && strcmp( pInfo->GetString("model"), "" ) )
					SetMergeMDL( pInfo->GetString("model"), NULL, nSkin );

				KeyValues* pBodygroups = pInfo->FindKey("Bodygroups");
				if( pBodygroups )
				{
					for ( KeyValues *sub = pBodygroups->GetFirstValue(); sub; sub = sub->GetNextValue() )
					{
						int m_Bodygroup = FindBodygroupByName( sub->GetName() );
						if ( m_Bodygroup >= 0 )
						{
							SetBodygroup(m_Bodygroup, sub->GetInt());
						}
					}
				}
				pInfo->deleteThis();
			}
		}
		Update();
		SetModelAnim( iWeaponAnim );
		
		m_bUpdateCosmetics = false;
	}
}

void DMModelPanel::SetCosmetic(int iCosmeticID, bool bSelected)
{
	if (bSelected)
	{
		for (int i = 0; i < m_flCosmetics.Count(); i++)
		{
			if (iCosmeticID == m_flCosmetics[i])
			{
				// Already has the cosmetic, don't add second time
				return;
			}
		}
		m_flCosmetics.AddToTail(iCosmeticID);
	}
	else
	{
		for (int i = 0; i < m_flCosmetics.Count(); i++)
		{
			if (iCosmeticID == m_flCosmetics[i])
			{
				m_flCosmetics.Remove(i);
				break;
			}
		}
	}
	m_bUpdateCosmetics = true;
}

void DMModelPanel::SetWeaponModel( const char *szWeapon, int iAnim )
{
	Q_strncpy(szWeaponModel, szWeapon, sizeof(szWeaponModel));
	SetModelAnim( iAnim );
	iWeaponAnim = iAnim;
	m_iAnimationIndex = iAnim;
	m_bUpdateCosmetics = true;
}

void DMModelPanel::SetLoadoutCosmetics()
{
	m_flCosmetics.RemoveAll();
	if (GetLoadout())
	{
		KeyValues *kvCosmetics = GetLoadout()->FindKey("Cosmetics");
		if (kvCosmetics)
		{
			KeyValues *kvMerc = kvCosmetics->FindKey("mercenary");
			if (kvMerc)
			{
				for (KeyValues *pData = kvMerc->GetFirstSubKey(); pData != NULL; pData = pData->GetNextKey())
				{
					m_flCosmetics.AddToTail(pData->GetFloat());
				}
			}
		}
	}
	m_bUpdateCosmetics = true;
}
