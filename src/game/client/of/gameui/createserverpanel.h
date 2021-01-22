//=============================================================================
//
// Purpose: Fancy create server panel
//
//=============================================================================

//#ifdef _WIN32
//#pragma once
//#endif

#include "basemodframe.h"

class CModelPanel;

namespace BaseModUI
{

	class CreateServerPanel : public CBaseModFrame
	{
	private:
		DECLARE_CLASS_SIMPLE(CreateServerPanel, CBaseModFrame);

	public:
		CreateServerPanel(Panel *parent, const char *panelName);

		virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
		virtual void PaintBackground();

		vgui::ComboBox *m_pMapList;

		void LoadMapList();
		void LoadMaps(const char *pszPathID);
		void SetMap(const char *name);

	};

}