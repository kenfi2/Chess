#include "main.h"

#include <wx/snglinst.h>

#include "application.h"
#include "home.h"
#include "game.h"

MainFrame* g_root;

IMPLEMENT_APP(Application)

Application::~Application()
{
}

bool Application::OnInit()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	wxAppConsole::SetInstance(this);

	wxPNGHandler* handler = new wxPNGHandler;
	wxImage::AddHandler(handler);

	g_root = new MainFrame("Chess", wxDefaultPosition, wxSize(700, 500));
	SetTopWindow(g_root);

	g_root->Show();

	m_startup = true;
	return true;
}

void Application::OnEventLoopEnter(wxEventLoopBase* loop)
{
	if(!m_startup)
		return;
	m_startup = false;
}

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :
	wxFrame((wxFrame*)nullptr, -1, title, pos, size, wxDEFAULT_FRAME_STYLE)
{
	SetExtraStyle(wxWS_EX_PROCESS_IDLE);

	m_gameBoard = new GameBoard(this);

#if wxCHECK_VERSION(3, 1, 0) //3.1.0 or higher
	// Make sure ShowFullScreen() uses the full screen API on macOS
	EnableFullScreenView(true);
#endif
}

MainFrame::~MainFrame() = default;

void MainFrame::ShowHome()
{
	m_homeDialog = new HomeDialog("Chess", "Version 1", wxSize(800, 480));
	m_homeDialog->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnHomeDialogClosed, this);
	m_homeDialog->Bind(HOME_DIALOG_ACTION, &MainFrame::OnHomeDialogAction, this);
	m_homeDialog->Show();
}

void MainFrame::FinishHomeDialog() {
	if(m_homeDialog != nullptr) {
		m_homeDialog->Hide();
		g_root->Show();
		m_homeDialog->Destroy();
		m_homeDialog = nullptr;
	}
}

void MainFrame::OnHomeDialogClosed(wxCloseEvent& event)
{
	m_homeDialog->Destroy();
	g_root->Close();
}

void MainFrame::OnHomeDialogAction(wxCommandEvent& event)
{
	if(event.GetId() == HOME_PLAY) {
		// open tab
		NewGame();
	}
	else if(event.GetId() == HOME_OPEN) {
		// open tab
	}
}

void MainFrame::NewGame()
{
	FinishHomeDialog();
}
