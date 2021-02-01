//=============================================================================
//
// Purpose: Fancy create server panel
//
//=============================================================================

#include "cbase.h"
#include "createserverpanel.h"
#include <vgui_controls/ComboBox.h>
#include "filesystem.h"

#include "tier0/dbg.h"

using namespace vgui;
using namespace BaseModUI;

#define RANDOM_MAP "#GameUI_RandomMap"

void CreateServerPanel::PaintBackground()
{
	int xchange = 0;

	// move the background panel and other various elements
	vgui::Panel *pCreateServerPanelImage = FindChildByName("CreateServerPanelImage");
	if (pCreateServerPanelImage)
	{
		int x, y;
		pCreateServerPanelImage->GetPos(x, y);
		//DevMsg("%i\n",x);

		xchange = floor((505 - x) * 0.1);

		if (x > 505 && (x + xchange >= 505))
		{
			pCreateServerPanelImage->SetPos(x + xchange, y);
		}
		else
		{
			pCreateServerPanelImage->SetPos(505, y);
		}

		//pCreateServerPanelImage->SetPos();
	}

	vgui::Panel *pCreateServerTopTitle = FindChildByName("CreateServerTopTitle");
	if (pCreateServerTopTitle)
	{
		int x, y;
		pCreateServerTopTitle->GetPos(x, y);
		pCreateServerTopTitle->SetPos(x + xchange, y);
	}

	vgui::Panel *pMapList = FindChildByName("MapList");
	if (pMapList)
	{
		int x, y;
		pMapList->GetPos(x, y);
		pMapList->SetPos(x + xchange, y);
	}

	vgui::Panel *pBotQuotaLabel = FindChildByName("BotQuotaLabel");
	if (pBotQuotaLabel)
	{
		int x, y;
		pBotQuotaLabel->GetPos(x, y);
		pBotQuotaLabel->SetPos(x + xchange, y);
	}

	vgui::Panel *pBotQuotaCombo = FindChildByName("BotQuotaCombo");
	if (pBotQuotaCombo)
	{
		int x, y;
		pBotQuotaCombo->GetPos(x, y);
		pBotQuotaCombo->SetPos(x + xchange, y);
	}

	vgui::Panel *pEnableBotsLabel = FindChildByName("EnableBotsLabel");
	if (pEnableBotsLabel)
	{
		int x, y;
		pEnableBotsLabel->GetPos(x, y);
		pEnableBotsLabel->SetPos(x + xchange, y);
	}

	vgui::Panel *pEnableBotsCheck = FindChildByName("EnableBotsCheck");
	if (pEnableBotsCheck)
	{
		int x, y;
		pEnableBotsCheck->GetPos(x, y);
		pEnableBotsCheck->SetPos(x + xchange, y);
	}

	vgui::Panel *pHostnameLabel = FindChildByName("HostnameLabel");
	if (pHostnameLabel)
	{
		int x, y;
		pHostnameLabel->GetPos(x, y);
		pHostnameLabel->SetPos(x + xchange, y);
	}

	vgui::Panel *pHostnameCombo = FindChildByName("HostnameCombo");
	if (pHostnameCombo)
	{
		int x, y;
		pHostnameCombo->GetPos(x, y);
		pHostnameCombo->SetPos(x + xchange, y);
	}

	vgui::Panel *pMaxPlayersLabel = FindChildByName("MaxPlayersLabel");
	if (pMaxPlayersLabel)
	{
		int x, y;
		pMaxPlayersLabel->GetPos(x, y);
		pMaxPlayersLabel->SetPos(x + xchange, y);
	}

	vgui::Panel *pMaxPlayersCombo = FindChildByName("MaxPlayersCombo");
	if (pMaxPlayersCombo)
	{
		int x, y;
		pMaxPlayersCombo->GetPos(x, y);
		pMaxPlayersCombo->SetPos(x + xchange, y);
	}

	vgui::Panel *pServerPasswordLabel = FindChildByName("ServerPasswordLabel");
	if (pServerPasswordLabel)
	{
		int x, y;
		pServerPasswordLabel->GetPos(x, y);
		pServerPasswordLabel->SetPos(x + xchange, y);
	}

	vgui::Panel *pServerPasswordCombo = FindChildByName("ServerPasswordCombo");
	if (pServerPasswordCombo)
	{
		int x, y;
		pServerPasswordCombo->GetPos(x, y);
		pServerPasswordCombo->SetPos(x + xchange, y);
	}

	vgui::Panel *pWinLimitLabel = FindChildByName("WinLimitLabel");
	if (pWinLimitLabel)
	{
		int x, y;
		pWinLimitLabel->GetPos(x, y);
		pWinLimitLabel->SetPos(x + xchange, y);
	}

	vgui::Panel *pWinLimitCombo = FindChildByName("WinLimitCombo");
	if (pWinLimitCombo)
	{
		int x, y;
		pWinLimitCombo->GetPos(x, y);
		pWinLimitCombo->SetPos(x + xchange, y);
	}

	vgui::Panel *pFragLimitLabel = FindChildByName("FragLimitLabel");
	if (pFragLimitLabel)
	{
		int x, y;
		pFragLimitLabel->GetPos(x, y);
		pFragLimitLabel->SetPos(x + xchange, y);
	}

	vgui::Panel *pFragLimitCombo = FindChildByName("FragLimitCombo");
	if (pFragLimitCombo)
	{
		int x, y;
		pFragLimitCombo->GetPos(x, y);
		pFragLimitCombo->SetPos(x + xchange, y);
	}

	vgui::Panel *pTimePerMapLabel = FindChildByName("TimePerMapLabel");
	if (pTimePerMapLabel)
	{
		int x, y;
		pTimePerMapLabel->GetPos(x, y);
		pTimePerMapLabel->SetPos(x + xchange, y);
	}

	vgui::Panel *pTimePerMapCombo = FindChildByName("TimePerMapCombo");
	if (pTimePerMapCombo)
	{
		int x, y;
		pTimePerMapCombo->GetPos(x, y);
		pTimePerMapCombo->SetPos(x + xchange, y);
	}

	vgui::Panel *pRoundLimitLabel = FindChildByName("RoundLimitLabel");
	if (pRoundLimitLabel)
	{
		int x, y;
		pRoundLimitLabel->GetPos(x, y);
		pRoundLimitLabel->SetPos(x + xchange, y);
	}

	vgui::Panel *pRountLimitCombo = FindChildByName("RoundLimitCombo");
	if (pRountLimitCombo)
	{
		int x, y;
		pRountLimitCombo->GetPos(x, y);
		pRountLimitCombo->SetPos(x + xchange, y);
	}

	vgui::Panel *pBtnExit = FindChildByName("BtnExit");
	if (pBtnExit)
	{
		int x, y;
		pBtnExit->GetPos(x, y);
		pBtnExit->SetPos(x + xchange, y);
	}

	// make background darker
	vgui::Panel *pDarkenPanel = FindChildByName("DarkenPanel");
	if (pDarkenPanel)
	{
		int alpha;
		alpha = pDarkenPanel->GetAlpha();

		if (alpha < 200)
		{
			pDarkenPanel->SetAlpha(alpha + 2);
		}
	}
}

void CreateServerPanel::OnCommand(const char *command)
{
	if (!Q_strcmp(command, "Exit"))
	{
		engine->ClientCmd_Unrestricted("gameui_allowescape\n");
		Close();
	}
}

CreateServerPanel::CreateServerPanel(Panel *parent, const char *panelName) : BaseClass(parent, panelName)
{
	engine->ClientCmd_Unrestricted("gameui_preventescape\n");
	m_pMapList = new ComboBox(this, "MapList", 12, false);

	LoadMapList();
	//m_szMapName[0] = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CreateServerPanel::~CreateServerPanel()
{
	delete m_pMapList;
}

//-----------------------------------------------------------------------------
// Purpose: loads the list of available maps into the map list
//-----------------------------------------------------------------------------
void CreateServerPanel::LoadMaps(const char *pszPathID)
{
	FileFindHandle_t findHandle = NULL;

	const char *pszFilename = g_pFullFileSystem->FindFirst("maps/*.bsp", &findHandle);
	while (pszFilename)
	{
		char mapname[256];
		char *ext, *str;

		// FindFirst ignores the pszPathID, so check it here
		// TODO: this doesn't find maps in fallback dirs
		Q_snprintf(mapname, sizeof(mapname), "maps/%s", pszFilename);
		if (!g_pFullFileSystem->FileExists(mapname, pszPathID))
		{
			goto nextFile;
		}

		// remove the text 'maps/' and '.bsp' from the file name to get the map name

		str = Q_strstr(pszFilename, "maps");
		if (str)
		{
			Q_strncpy(mapname, str + 5, sizeof(mapname) - 1);	// maps + \\ = 5
		}
		else
		{
			Q_strncpy(mapname, pszFilename, sizeof(mapname) - 1);
		}
		ext = Q_strstr(mapname, ".bsp");
		if (ext)
		{
			*ext = 0;
		}

		//!! hack: strip out single player HL maps
		// this needs to be specified in a seperate file
		//if (!stricmp(ModInfo().GetGameName(), "Half-Life") && (mapname[0] == 'c' || mapname[0] == 't') && mapname[2] == 'a' && mapname[1] >= '0' && mapname[1] <= '5')
		//{
		//	goto nextFile;
		//}

		// strip out maps that shouldn't be displayed
		//if (hiddenMaps)
		//{
		//	if (hiddenMaps->GetInt(mapname, 0))
		//	{
		//		goto nextFile;
		//	}
		//}

		// add to the map list
		m_pMapList->AddItem(mapname, new KeyValues("data", "mapname", mapname));

		// get the next file
	nextFile:
		pszFilename = g_pFullFileSystem->FindNext(findHandle);
	}
	g_pFullFileSystem->FindClose(findHandle);
}

//-----------------------------------------------------------------------------
// Purpose: loads the list of available maps into the map list
//-----------------------------------------------------------------------------
void CreateServerPanel::LoadMapList()
{
	// clear the current list (if any)
	m_pMapList->DeleteAllItems();

	// add special "name" to represent loading a randomly selected map
	m_pMapList->AddItem(RANDOM_MAP, new KeyValues("data", "mapname", RANDOM_MAP));

	// iterate the filesystem getting the list of all the files
	// UNDONE: steam wants this done in a special way, need to support that
	const char *pathID = "MOD";
	//if (!stricmp(ModInfo().GetGameName(), "Half-Life"))
	//{
	//	pathID = NULL; // hl is the base dir
	//}

	// Load the GameDir maps
	LoadMaps(pathID);

	// If we're not the Valve directory and we're using a "fallback_dir" in gameinfo.txt then include those maps...
	// (pathID is NULL if we're "Half-Life")
	//const char *pszFallback = ModInfo().GetFallbackDir();
	//if (pathID && pszFallback[0])
	//{
	//	LoadMaps("GAME_FALLBACK");
	//}

	// set the first item to be selected
	m_pMapList->ActivateItem(0);
}

void CreateServerPanel::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	SetPaintBackgroundEnabled(true);
	LoadControlSettings("resource/ui/basemodui/createserverpanel.res");
}