#ifndef HOME_H
#define HOME_H

extern const wxStandardID HOME_PLAY;
extern const wxStandardID HOME_OPEN;
extern const wxStandardID HOME_HISTORY;

wxDECLARE_EVENT(HOME_DIALOG_ACTION, wxCommandEvent);

class HomeDialogPanel;

class HomeDialog : public wxDialog {
public:
    HomeDialog(const wxString& titleText,
        const wxString& versionText,
        const wxSize& size);
    void OnButtonClicked(const wxMouseEvent& event);
private:
    HomeDialogPanel* m_home_dialog_panel;
};

class HomeDialogPanel : public wxPanel
{
public:
    HomeDialogPanel(HomeDialog* parent,
        const wxSize& size,
        const wxString& title_text,
        const wxString& version_text,
        const wxColour& base_colour);
    void OnPaint(const wxPaintEvent& event);
    void updateInputs();
private:
    wxString m_title_text;
    wxString m_version_text;
    wxColour m_text_colour;
    wxColour m_background_colour;
};

class HomeDialogButton : public wxPanel
{
public:
    HomeDialogButton(wxWindow* parent, const wxPoint& pos, const wxSize& size, const wxColour& base_colour, const wxString& text);
    void OnPaint(const wxPaintEvent& event);
    void OnMouseEnter(const wxMouseEvent& event);
    void OnMouseLeave(const wxMouseEvent& event);
    wxStandardID GetAction() { return m_action; };
    void SetAction(wxStandardID action) { m_action = action; };
private:
    wxStandardID m_action;
    wxString m_text;
    wxColour m_text_colour;
    wxColour m_background;
    wxColour m_background_hover;
    bool m_is_hover;
};

#endif
