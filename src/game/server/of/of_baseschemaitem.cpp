//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Base class for all animating characters and objects.
//
//=============================================================================//

#include "cbase.h"
#include "of_baseschemaitem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( CBaseSchemaEntity )
	DEFINE_FIELD( m_iszSchemaName, FIELD_STRING ),
END_DATADESC()

// SendTable stuff.
IMPLEMENT_SERVERCLASS_ST( CBaseSchemaEntity, DT_BaseSchemaEntity )
	SendPropString( SENDINFO( m_iszSchemaName ) ),
END_SEND_TABLE()


CBaseSchemaEntity::CBaseSchemaEntity()
{
	m_iszSchemaName.GetForModify()[0] = '\0';
}

CBaseSchemaEntity::~CBaseSchemaEntity()
{
}