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
	DECLARE_CLASS_SIMPLE(CreateServerPanel, CBaseModFrame);

	public:
		CreateServerPanel(Panel *parent, const char *panelName);
		~CreateServerPanel();

		virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
		virtual void PaintBackground();
		void OnCommand(const char *command);

		vgui::ComboBox *m_pMapList;

		void StartServer();
		void LoadMapList();
		void LoadMaps(const char *pszPathID);
		void SetMap(const char *name);
		const char *GetMapName();
		const char *GetHostName();
		const char *GetPassword();
		int GetMaxPlayers();
		int GetBotAmount();
		int GetWinLimit();
		int GetFragLimit();
		int GetTimePerMap();
		int GetRoundLimit();
		bool IsRandomMapSelected();

	private:
		bool beginClose;
		int m_iBackgroundXGotoPos;
		KeyValues *m_pSavedData;
	protected:
		virtual void OnClose();
	};

}