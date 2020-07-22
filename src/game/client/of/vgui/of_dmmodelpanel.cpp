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
		for( int i = 0; i < m_iCosmetics.Count(); i++ )
		{
			KeyValues *pCosmetic = GetCosmetic( m_iCosmetics[i] );
			if( pCosmetic )
			{
				if( strcmp( pCosmetic->GetString("model"), "BLANK" ) && strcmp( pCosmetic->GetString("model"), "" ) )
					SetMergeMDL( pCosmetic->GetString("model"), NULL, 2 );

				KeyValues* pBodygroups = pCosmetic->FindKey("Bodygroups");
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
		for (int i = 0; i < m_iCosmetics.Count(); i++)
		{
			if (iCosmeticID == m_iCosmetics[i])
			{
				// Already has the cosmetic, don't add second time
				return;
			}
		}
		m_iCosmetics.AddToTail(iCosmeticID);
	}
	else
	{
		for (int i = 0; i < m_iCosmetics.Count(); i++)
		{
			if (iCosmeticID == m_iCosmetics[i])
			{
				m_iCosmetics.Remove(i);
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
	m_bUpdateCosmetics = true;
}

void DMModelPanel::SetLoadoutCosmetics()
{
	m_iCosmetics.RemoveAll();
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
					m_iCosmetics.AddToTail(pData->GetInt());
				}
			}
		}
	}
	m_bUpdateCosmetics = true;
}
