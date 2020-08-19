//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef OF_BASESCHEMAITEM_H
#define OF_BASESCHEMAITEM_H
#ifdef _WIN32
#pragma once
#endif

#include "baseanimating.h"

class CBaseSchemaEntity : public CBaseAnimating
{
public:
	DECLARE_CLASS( CBaseSchemaEntity, CBaseAnimating );

	CBaseSchemaEntity();
	~CBaseSchemaEntity();

	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

public:

	const char *GetSchemaName()
	{
		return m_iszSchemaName.GetForModify()[0] != '\0' ? (char*)m_iszSchemaName.Get() : GetClassname();
	};
	
	void SetSchemaName( char *szSchemaName )
	{
		Q_strncpy( m_iszSchemaName.GetForModify(), szSchemaName, 128);
	};
	
	void SetSchemaName( const char *szSchemaName )
	{
		Q_strncpy( m_iszSchemaName.GetForModify(), szSchemaName, 128);
	};
	
	bool IsSchemaItem()
	{
		return m_iszSchemaName.GetForModify()[0] != '\0';
	};

public:

private:
	CNetworkString(m_iszSchemaName, 128);
};

#endif // OF_BASESCHEMAITEM_H
