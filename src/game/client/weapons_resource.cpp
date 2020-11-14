//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Weapons Resource implementation
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "history_resource.h"
#include <vgui_controls/Controls.h>
#include <vgui/ISurface.h>
#include "c_baseplayer.h"
#include "hud.h"
#ifdef OF_CLIENT_DLL
#include "of_shared_schemas.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef OF_CLIENT_DLL
extern ConVar of_weapon_testing;
#endif

WeaponsResource gWR;

void FreeHudTextureList( CUtlDict< CHudTexture *, int >& list );

static CHudTexture *FindHudTextureInDict( CUtlDict< CHudTexture *, int >& list, const char *psz )
{
	int idx = list.Find( psz );
	if ( idx == list.InvalidIndex() )
		return NULL;

	return list[ idx ];
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : 
//-----------------------------------------------------------------------------
WeaponsResource::WeaponsResource( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : 
//-----------------------------------------------------------------------------
WeaponsResource::~WeaponsResource( void )
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void WeaponsResource::Init( void )
{
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void WeaponsResource::Reset( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: Load all the sprites needed for all registered weapons
//-----------------------------------------------------------------------------
void WeaponsResource::LoadAllWeaponSprites( void )
{
	C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	if ( !player )
		return;

	for (int i = 0; i < MAX_WEAPONS; i++)
	{
		if ( player->GetWeapon(i) )
		{
			LoadWeaponSprites( player->GetWeapon(i)->GetWeaponFileInfoHandle() );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void WeaponsResource::LoadWeaponSprites( WEAPON_FILE_INFO_HANDLE hWeaponFileInfo )
{
	// WeaponsResource is a friend of C_BaseCombatWeapon
	FileWeaponInfo_t *pWeaponInfo = GetFileWeaponInfoFromHandle( hWeaponFileInfo );

	if ( !pWeaponInfo )
		return;

	// Already parsed the hud elements?
	if ( pWeaponInfo->bLoadedHudElements )
		return;

	pWeaponInfo->bLoadedHudElements = true;

	pWeaponInfo->iconRed = NULL;
	pWeaponInfo->iconBlue = NULL;
	pWeaponInfo->iconCustom = NULL;
	pWeaponInfo->iconAmmo = NULL;
	pWeaponInfo->iconAmmo2 = NULL;
	pWeaponInfo->iconCrosshair = NULL;
	pWeaponInfo->iconAutoaim = NULL;
	pWeaponInfo->iconSmall = NULL;

#ifdef OF_CLIENT_DLL
	bool bUsingItemsGame = false;
		
	KeyValues *pKV;
	
	pKV = GetWeaponFromSchema( pWeaponInfo->szClassName );
	if( pKV )
	{
		if( of_weapon_testing.GetBool() && GetWeaponFromSchema( VarArgs( "%s_beta", pWeaponInfo->szClassName ) ) )
			pKV = GetWeaponFromSchema( VarArgs( "%s_beta", pWeaponInfo->szClassName ) );

		pKV = pKV->FindKey("WeaponData");
		if( pKV )
		{
			bUsingItemsGame = true;
		}
	}
#endif
	char sz[128];
	
#ifdef OF_CLIENT_DLL
	Q_snprintf(sz, sizeof( sz ), "scripts/weapons/%s", pWeaponInfo->szClassName);
#else
	Q_snprintf(sz, sizeof( sz ), "scripts/%s", pWeaponInfo->szClassName);
#endif

	CUtlDict< CHudTexture *, int > tempList;
#ifdef OF_CLIENT_DLL
	if ( !bUsingItemsGame )
#endif
		LoadHudTextures( tempList, sz, g_pGameRules->GetEncryptionKey() );
#ifdef OF_CLIENT_DLL
	else
		LoadHudTextures( tempList, pKV->FindKey( "TextureData" ) );
#endif

	if ( !tempList.Count() )
	{
		// no sprite description file for weapon, use default small blocks
		pWeaponInfo->iconRed =	gHUD.GetIcon( "selection" );
		pWeaponInfo->iconBlue = gHUD.GetIcon( "selection" );
		pWeaponInfo->iconCustom = gHUD.GetIcon( "selection" );
		pWeaponInfo->iconAmmo = gHUD.GetIcon( "bucket1" );
		return;
	}

	CHudTexture *p;
	p = FindHudTextureInDict( tempList, "crosshair" );
	if ( p )
	{
		pWeaponInfo->iconCrosshair = gHUD.AddUnsearchableHudIconToList( *p );
	}

	p = FindHudTextureInDict( tempList, "autoaim" );
	if ( p )
	{
		pWeaponInfo->iconAutoaim = gHUD.AddUnsearchableHudIconToList( *p );
	}

	p = FindHudTextureInDict( tempList, "zoom" );
	if ( p )
	{
		pWeaponInfo->iconZoomedCrosshair = gHUD.AddUnsearchableHudIconToList( *p );
	}
	else
	{
		pWeaponInfo->iconZoomedCrosshair = pWeaponInfo->iconCrosshair; //default to non-zoomed crosshair
	}

	p = FindHudTextureInDict( tempList, "zoom_autoaim" );
	if ( p )
	{
		pWeaponInfo->iconZoomedAutoaim = gHUD.AddUnsearchableHudIconToList( *p );
	}
	else
	{
		pWeaponInfo->iconZoomedAutoaim = pWeaponInfo->iconZoomedCrosshair;  //default to zoomed crosshair
	}

	CHudHistoryResource *pHudHR = GET_HUDELEMENT( CHudHistoryResource );	
	if( pHudHR )
	{
		p = FindHudTextureInDict( tempList, "weapon" );
		if ( p )
		{
			pWeaponInfo->iconRed = gHUD.AddUnsearchableHudIconToList( *p );
			if ( pWeaponInfo->iconRed )
			{
				pWeaponInfo->iconRed->Precache();
				pHudHR->SetHistoryGap( pWeaponInfo->iconRed->Height() );
			}
		}

		p = FindHudTextureInDict( tempList, "weapon_s" );
		if ( p )
		{
			pWeaponInfo->iconBlue = gHUD.AddUnsearchableHudIconToList( *p );
			if ( pWeaponInfo->iconBlue )
			{
				pWeaponInfo->iconBlue->Precache();
			}
		}

		p = FindHudTextureInDict(tempList, "weapon_t");
		if (p)
		{
			pWeaponInfo->iconCustom = gHUD.AddUnsearchableHudIconToList(*p);
			if (pWeaponInfo->iconCustom)
			{
				pWeaponInfo->iconCustom->Precache();
			}
		}

		p = FindHudTextureInDict( tempList, "weapon_small" );
		if ( p )
		{
			pWeaponInfo->iconSmall = gHUD.AddUnsearchableHudIconToList( *p );
			if ( pWeaponInfo->iconSmall )
			{
				pWeaponInfo->iconSmall->Precache();
			}
		}

		p = FindHudTextureInDict( tempList, "ammo" );
		if ( p )
		{
			pWeaponInfo->iconAmmo = gHUD.AddUnsearchableHudIconToList( *p );
			if ( pWeaponInfo->iconAmmo )
			{
				pWeaponInfo->iconAmmo->Precache();
				pHudHR->SetHistoryGap( pWeaponInfo->iconAmmo->Height() );
			}
		}

		p = FindHudTextureInDict( tempList, "ammo2" );
		if ( p )
		{
			pWeaponInfo->iconAmmo2 = gHUD.AddUnsearchableHudIconToList( *p );
			if ( pWeaponInfo->iconAmmo2 )
			{
				pWeaponInfo->iconAmmo2->Precache();
				pHudHR->SetHistoryGap( pWeaponInfo->iconAmmo2->Height() );
			}
		}
	}

	FreeHudTextureList( tempList );
}

//-----------------------------------------------------------------------------
// Purpose: Helper function to return a Ammo pointer from id
//-----------------------------------------------------------------------------
CHudTexture *WeaponsResource::GetAmmoIconFromWeapon( int iAmmoId )
{
	C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	if ( !player )
		return NULL;

	for ( int i = 0; i < MAX_WEAPONS; i++ )
	{
		C_BaseCombatWeapon *weapon = player->GetWeapon( i );
		if ( !weapon )
			continue;

		if ( weapon->GetPrimaryAmmoType() == iAmmoId )
		{
			return weapon->GetWpnData().iconAmmo;
		}
		else if ( weapon->GetSecondaryAmmoType() == iAmmoId )
		{
			return weapon->GetWpnData().iconAmmo2;
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Get a pointer to a weapon using this ammo
//-----------------------------------------------------------------------------
const FileWeaponInfo_t *WeaponsResource::GetWeaponFromAmmo( int iAmmoId )
{
	C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	if ( !player )
		return NULL;

	for ( int i = 0; i < MAX_WEAPONS; i++ )
	{
		C_BaseCombatWeapon *weapon = player->GetWeapon( i );
		if ( !weapon )
			continue;

		if ( weapon->GetPrimaryAmmoType() == iAmmoId )
		{
			return &weapon->GetWpnData();
		}
		else if ( weapon->GetSecondaryAmmoType() == iAmmoId )
		{
			return &weapon->GetWpnData();
		}
	}

	return NULL;
}

