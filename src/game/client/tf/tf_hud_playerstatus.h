//========= Copyright � 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_HUD_PLAYERSTATUS_H
#define TF_HUD_PLAYERSTATUS_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_controls.h"
#include "c_tf_player.h"

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CTFClassImage : public vgui::ImagePanel
{
public:
	DECLARE_CLASS_SIMPLE( CTFClassImage, vgui::ImagePanel );

	CTFClassImage( vgui::Panel *parent, const char *name ) : ImagePanel( parent, name )
	{
	}

	void SetClass( int iTeam, TFPlayerClassData_t *iClassData, int iCloakstate );
	void SetClassColorless( int iTeam, TFPlayerClassData_t *iClassData, int iCloakstate );
};

//-----------------------------------------------------------------------------
// Purpose:  Displays player class data
//-----------------------------------------------------------------------------
class CTFHudPlayerClass : public vgui::EditablePanel, public CGameEventListener
{
	DECLARE_CLASS_SIMPLE( CTFHudPlayerClass, EditablePanel );

public:

	CTFHudPlayerClass( Panel *parent, const char *name );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void Reset();

public: // IGameEventListener Interface
	virtual void FireGameEvent( IGameEvent * event );

protected:

	virtual void OnThink();

private:

	float				m_flNextThink;

	CTFClassImage		*m_pClassImage;
	CTFClassImage		*m_pClassImageColorless;
	CTFImagePanel		*m_pSpyImage; // used when spies are disguised
	CTFImagePanel		*m_pSpyOutlineImage;

	int					m_nTeam;
	int					m_nClass;
	int					m_nModifiers;
	int					m_nDisguiseTeam;
	int					m_nDisguiseClass;
	int					m_nCloakLevel;
	int					m_nTennisball;
};

//-----------------------------------------------------------------------------
// Purpose:  Clips the health image to the appropriate percentage
//-----------------------------------------------------------------------------
class CTFHealthPanel : public CTFImagePanel
{
public:
	DECLARE_CLASS_SIMPLE( CTFHealthPanel, CTFImagePanel );

	CTFHealthPanel( vgui::Panel *parent, const char *name );
	virtual void Paint();
	void SetHealth( float flHealth ){ m_flHealth = ( flHealth <= 1.0 ) ? flHealth : 1.0f; }

private:

	float	m_flHealth; // percentage from 0.0 -> 1.0
	int		m_iMaterialIndex;
	int		m_iDeadMaterialIndex;
};

//-----------------------------------------------------------------------------
// Purpose:  Clips the health image to the appropriate percentage
//-----------------------------------------------------------------------------
class CTFMegahealPanel : public CTFImagePanel
{
public:
	DECLARE_CLASS_SIMPLE( CTFMegahealPanel, CTFImagePanel );

	CTFMegahealPanel( vgui::Panel *parent, const char *name );
	virtual void Paint();
	void SetHealth( float flHealth ){ m_flHealth = ( flHealth <= 1.0 ) ? flHealth : 1.0f; }
private:

	float	m_flHealth; // percentage from 0.0 -> 1.0
	int		m_iMaterialIndex;
};

//-----------------------------------------------------------------------------
// Purpose:  Displays player health data
//-----------------------------------------------------------------------------
class CTFHudPlayerHealth : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CTFHudPlayerHealth, EditablePanel );

public:

	CTFHudPlayerHealth( Panel *parent, const char *name );

	virtual const char *GetResFilename( void ) { return "resource/UI/HudPlayerHealth.res"; }
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void Reset();

	void	SetHealth( int iNewHealth, int iMaxHealth, int iMaxBuffedHealth );
//	void	SetMegaHealth( int iNewHealth, int iMaxHealth, int iMaxBuffedHealth );
	void	HideHealthBonusImage( void );

protected:

	virtual void OnThink();

protected:
	float				m_flNextThink;

private:
	CTFHealthPanel		*m_pHealthImage;
//	CTFMegahealPanel	*m_pMegahealImage;
//	CTFImagePanel		*m_pMegahealImageBG;
//	CExLabel			*m_pMegahealLabel;
	CTFImagePanel		*m_pHealthBonusImage;
	CTFImagePanel		*m_pMegaHealthBonusImage;
	vgui::ImagePanel	*m_pHealthImageBG;

	int					m_nHealth;
	int					m_nMaxHealth;

	int					m_nMegaHealth;
	int					m_nMaxMegaHealth;
	
	int					m_nBonusHealthOrigX;
	int					m_nBonusHealthOrigY;
	int					m_nBonusHealthOrigW;
	int					m_nBonusHealthOrigH;

	CPanelAnimationVar( int, m_nHealthBonusPosAdj, "HealthBonusPosAdj", "25" );
	CPanelAnimationVar( float, m_flHealthDeathWarning, "HealthDeathWarning", "0.49" );
	CPanelAnimationVar( Color, m_clrHealthDeathWarningColor, "HealthDeathWarningColor", "HUDDeathWarning" );
};

//-----------------------------------------------------------------------------
// Purpose:  Parent panel for the player class/health displays
//-----------------------------------------------------------------------------
class CTFHudPlayerStatus : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CTFHudPlayerStatus, vgui::EditablePanel );

public:
	CTFHudPlayerStatus( const char *pElementName );
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void Reset();

private:

	CTFHudPlayerClass	*m_pHudPlayerClass;
	CTFHudPlayerHealth	*m_pHudPlayerHealth;
};

#endif	// TF_HUD_PLAYERSTATUS_H