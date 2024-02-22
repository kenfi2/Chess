#ifndef APP_H
#define APP_H

class HomeDialog;
class GameBoard;

class Application : public wxApp {
public:
	~Application();
	virtual bool OnInit();
	virtual void OnEventLoopEnter(wxEventLoopBase* loop);

private:
	bool m_startup;
};

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title,
		const wxPoint& pos, const wxSize& size);
	~MainFrame();

	void ShowHome();
	void FinishHomeDialog();
	void OnHomeDialogClosed(wxCloseEvent& event);
	void OnHomeDialogAction(wxCommandEvent& event);

protected:
	void NewGame();

private:
	HomeDialog* m_homeDialog = nullptr;
	wxSizer* m_sizer = nullptr;
	GameBoard* m_gameBoard = nullptr;
};

#endif
