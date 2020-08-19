//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef C_OF_BASESCHEMAITEM_H
#define C_OF_BASESCHEMAITEM_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseanimating.h"

class C_BaseSchemaEntity : public C_BaseAnimating
{
public:
	DECLARE_CLASS( C_BaseSchemaEntity, C_BaseAnimating );

	C_BaseSchemaEntity();
	~C_BaseSchemaEntity();

	DECLARE_DATADESC();
	DECLARE_CLIENTCLASS();

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

#endif // C_OF_BASESCHEMAITEM_H
