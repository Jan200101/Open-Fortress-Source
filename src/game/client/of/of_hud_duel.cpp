
#include "cbase.h"
#include "tf_controls.h"
#include "of_hud_duel.h"
#include "tf_gamerules.h"
#include "c_tf_player.h"
#include "iclientmode.h"
#include "c_playerresource.h"
#include "vgui_controls/AnimationController.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

CTFDuelRank::CTFDuelRank( vgui::Panel *parent, const char *name ) : BaseClass( parent, name ) 
{
	iPlayerIndex = 0;
	iQueuePos = 0;
	szPlayerName[0] = '\0';
	
	m_pAvatar = new CAvatarImagePanel( this, "Avatar" );
}

void CTFDuelRank::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// load control settings...
	LoadControlSettings( "resource/ui/HudDuelRank.res" );
}

void CTFDuelRank::ApplySettings( KeyValues *inResourceData )
{
	BaseClass::ApplySettings( inResourceData );
}

void CTFDuelRank::SetPlayer( int iIndex, int iOffset )
{	
	Q_strncpy( szPlayerName, g_PR ? g_PR->GetPlayerName( iIndex ) : "", sizeof( szPlayerName ) );
	iQueuePos = OFDuelQueue()->GetDuelQueuePos(iIndex);
	
	iQueuePos = iQueuePos % 2 == 0 ? iQueuePos : iQueuePos - 1;
	
	iQueuePos /= 2;
	
	SetDialogVariable( "QueuePos", iQueuePos );
	SetDialogVariable( "PlayerName", szPlayerName );
	iPlayerIndex = iIndex;
	m_pAvatar->SetPlayer( UTIL_PlayerByIndex(iPlayerIndex) );
	m_pAvatar->SetShouldDrawFriendIcon( false );
}

int CTFDuelRank::GetPlayerPos()
{
	return OFDuelQueue()->GetDuelQueuePos(iPlayerIndex);
}

CTFDuelList::CTFDuelList( vgui::Panel *parent, const char *name ) : BaseClass( parent, name ) 
{
}

void CTFDuelList::AddPlayer( int iIndex, int iOffset )
{
	CTFDuelRank *pNewRank = new CTFDuelRank( this, "HudDuelRank" );
	pNewRank->SetPlayer( iIndex, iOffset );
	
	m_hRankList.AddToTail( pNewRank );
	m_hPlayerList.AddToTail( iIndex );
}

void CTFDuelList::RemovePlayer( int iIndex )
{
	// We asume the list is properly sorted before we do this because otherwise we get doo doo
	int iPos = OFDuelQueue()->GetDuelQueuePos( iIndex );
	
	m_hRankList[iPos]->DeletePanel();
	
	m_hRankList.Remove( iPos );
	m_hPlayerList.Remove( iPos );
}

void CTFDuelList::Purge( void )
{
	for( int i = 0; i < m_hRankList.Count(); i++ )
		m_hRankList[i]->DeletePanel();
	
	m_hRankList.Purge();
	m_hPlayerList.Purge();

	RecalcPositions();
}

void CTFDuelList::RecalcPositions()
{
	int iXPos = 0;
	int iYPos = 0;
	
	int iParentWidth, iParentHight;
	GetSize( iParentWidth, iParentHight );
	
	int iRankWidth = 0, iRankHight = 0;
	if( m_hRankList.Count() )
		m_hRankList[0]->GetSize( iRankWidth, iRankHight );
	
	for( int i = 0; i < m_hRankList.Count(); i++ )
	{
		if( iXPos + iRankWidth > iParentWidth )
		{
			iXPos = 0;
			iYPos += YRES(iRankHight) * 2 + YRES(m_iCollumnSpacing);
		}

		m_hRankList[i]->SetPos( iXPos, iYPos );
		
		iXPos += XRES(iRankWidth) * 2 + XRES(m_iRowSpacing);
		if( iXPos > iParentWidth )
		{
			iXPos = 0;
			iYPos += YRES(iRankHight) * 2 + YRES(m_iCollumnSpacing);
		}
	}
}

DECLARE_HUDELEMENT( CTFDuelHUD );

CTFDuelHUD::CTFDuelHUD( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudDuel" ) 
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	pListRight = new CTFDuelList( this, "DuelListRight" );
	pListLeft  = new CTFDuelList( this, "DuelListLeft" );
	
//	pLabelYou	 = new EditablePanel( this, "FarmZombie" );
//	pLabelUpNext = new CExLabel( this, "UpNextText" );

	ListenForGameEvent( "duel_refresh" );
	ListenForGameEvent( "duel_nextplayer" );
	ListenForGameEvent( "duel_enterqueue" );
	ListenForGameEvent( "teamplay_round_start" );
	
	m_bIsInQueue = false;
	m_bIsNext  = false;
	
}

void CTFDuelHUD::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// load control settings...
	LoadControlSettings( "resource/ui/HudDuel.res" );
	
	RefreshPlayerList();
}

bool CTFDuelHUD::ShouldDraw()
{
	if( !TFGameRules() )
		return false;
	
	if( !TFGameRules()->IsDuelGamemode() )
		return false;
	
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if( pPlayer && pPlayer->IsAlive() )
		return false;

	return CHudElement::ShouldDraw();
}

void CTFDuelHUD::RefreshPlayerList( void )
{
	pListRight->Purge();
	pListLeft->Purge();
	
	for( int i = 2; i < OFDuelQueue()->GetDuelerCount(); i += 2 )
	{
		pListLeft->AddPlayer(OFDuelQueue()->GetIndex(i), i);
	}

	for( int i = 3; i < OFDuelQueue()->GetDuelerCount(); i += 2 )
	{
		pListRight->AddPlayer(OFDuelQueue()->GetIndex(i), i);
	}

	pListRight->RecalcPositions();
	pListLeft->RecalcPositions();
	
	if( m_bIsNext )
	{
		m_bIsNext = false;

		NotifyNext();
	}
	
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if( m_bIsInQueue && pPlayer && !pPlayer->IsAlive() )
	{	
		m_bIsInQueue = false;

		if( TFGameRules() )
			TFGameRules()->BroadcastSound( TEAM_UNASSIGNED, "DuelJoinQueue" );
	}
}

void CTFDuelHUD::NotifyNext()
{
	g_pClientMode->GetViewportAnimationController()->StartAnimationSequence(this, "UpNext");
	
	if( TFGameRules() )
		TFGameRules()->BroadcastSound( TEAM_UNASSIGNED, "DuelNext" );

}

void CTFDuelHUD::FireGameEvent( IGameEvent *event )
{
	const char *eventname = event->GetName();
	
	if( FStrEq( eventname, "duel_refresh" ) )
		InvalidateLayout( false, true );
	else if( FStrEq( eventname, "duel_nextplayer" ) )
	{
		C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
		int iIndex = pPlayer ? pPlayer->entindex() : 0;
		
		DevMsg("Searching for Player with index %d, found %d\n", event->GetInt( "playerid" ), iIndex );
		
		if( iIndex == event->GetInt( "playerid" ) )
			m_bIsNext = true;
	}
	else if( FStrEq( eventname, "duel_enterqueue" ) )
	{
		C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
		int iIndex = pPlayer ? pPlayer->entindex() : 0;
		
		DevMsg("Searching for Player with index %d, found %d\n", event->GetInt( "playerid" ), iIndex );
		
		if( iIndex == event->GetInt( "playerid" ) )
		m_bIsInQueue = true;
	}
}