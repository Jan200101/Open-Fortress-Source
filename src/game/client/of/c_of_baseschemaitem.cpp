//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Base class for all animating characters and objects.
//
//=============================================================================//

#include "cbase.h"
#include "c_of_baseschemaitem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( C_BaseSchemaEntity )
	DEFINE_FIELD( m_iszSchemaName, FIELD_STRING ),
END_DATADESC()

IMPLEMENT_CLIENTCLASS_DT(C_BaseSchemaEntity, DT_BaseSchemaEntity, CBaseSchemaEntity)
	RecvPropString( RECVINFO( m_iszSchemaName )),
END_RECV_TABLE()

C_BaseSchemaEntity::C_BaseSchemaEntity()
{
	m_iszSchemaName.GetForModify()[0] = '\0';
}

C_BaseSchemaEntity::~C_BaseSchemaEntity()
{
}