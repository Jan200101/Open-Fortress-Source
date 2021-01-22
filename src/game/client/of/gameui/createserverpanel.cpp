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

CreateServerPanel::CreateServerPanel(Panel *parent, const char *panelName) : BaseClass(parent, panelName)
{
	m_pMapList = new ComboBox(this, "MapList", 12, false);

	LoadMapList();
	//m_szMapName[0] = 0;

}

void CreateServerPanel::PaintBackground()
{

	int xchange = 0;

	// move the background panel and other various elements
	vgui::Panel *pCreateServerPanelImage = FindChildByName("CreateServerPanelImage");
	if ( pCreateServerPanelImage )
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

	//vgui::Panel *pMapList = FindChildByName("MapList");

	// make background darker
	vgui::Panel *pDarkenPanel = FindChildByName("DarkenPanel");
	if ( pDarkenPanel )
	{
		int alpha;
		alpha = pDarkenPanel->GetAlpha();

		if (alpha < 200)
		{
			pDarkenPanel->SetAlpha(alpha + 2);
		}
	}
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