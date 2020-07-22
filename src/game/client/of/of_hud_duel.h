#ifndef OF_HUD_DUEL_H
#define OF_HUD_DUEL_H
#ifdef _WIN32
#pragma once
#endif

#include "utlvector.h"
#include "hudelement.h"
#include "vgui_avatarimage.h"
#include <vgui_controls/EditablePanel.h>

class C_TFPlayer;

class CTFDuelRank : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CTFDuelRank, vgui::EditablePanel );
	
	CTFDuelRank( vgui::Panel *parent, const char *name );
	
	void 	SetPlayer( int iIndex, int iOffset = 0 );
	int 	GetPlayerPos();
	
	virtual void ApplySettings( KeyValues *inResourceData );
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
private:
	int iPlayerIndex;
	
	int iQueuePos;
	char szPlayerName[32];
	
	CAvatarImagePanel	*m_pAvatar;
};

class CTFDuelList : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CTFDuelList, vgui::EditablePanel );
	
	CTFDuelList( vgui::Panel *parent, const char *name );
	
	void AddPlayer( int iIndex, int iOffset = 0 );
	void RemovePlayer( int iIndex );
	void Purge();
	void RecalcPositions();
	
	void ClearList();
private:
	CUtlVector<CTFDuelRank *> m_hRankList;
	CUtlVector<int> m_hPlayerList;
	
	CPanelAnimationVar( int, m_iRowSpacing, "RowSpacing", "10" );
	CPanelAnimationVar( int, m_iCollumnSpacing, "CollumnSpacing", "0" );
};

class CTFDuelHUD : public vgui::EditablePanel, public CHudElement
{
	DECLARE_CLASS_SIMPLE( CTFDuelHUD, vgui::EditablePanel );

	CTFDuelHUD( const char *pElementName );

	virtual bool ShouldDraw();
	void 	RefreshPlayerList( void );
	void 	NotifyNext();
	virtual void FireGameEvent( IGameEvent *event );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
private:
	CTFDuelList *pListRight;
	CTFDuelList *pListLeft;
	
	bool		m_bIsInQueue;
	bool		m_bIsNext;
	
//	vgui::EditablePanel *pLabelYou;
//	CExLabel			*pLabelUpNext;
};
#endif