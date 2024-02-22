#include "main.h"
#include "home.h"

const wxStandardID HOME_PLAY = (wxStandardID)wxNewId();
const wxStandardID HOME_OPEN = (wxStandardID)wxNewId();
const wxStandardID HOME_HISTORY = (wxStandardID)wxNewId();

wxDEFINE_EVENT(HOME_DIALOG_ACTION, wxCommandEvent);

HomeDialog::HomeDialog(const wxString& titleText, const wxString& versionText, const wxSize& size) :
	wxDialog(nullptr, wxID_ANY, titleText, wxDefaultPosition, size)
{
	Centre();
	wxColour base_colour = wxColor(250, 250, 250);
	m_home_dialog_panel = new HomeDialogPanel(this,
		GetClientSize(),
		titleText,
		versionText,
		base_colour);
}

void HomeDialog::OnButtonClicked(const wxMouseEvent& event)
{
    auto* button = dynamic_cast<HomeDialogButton*>(event.GetEventObject());
    wxSize button_size = button->GetSize();
    wxPoint click_point = event.GetPosition();
    if(click_point.x > 0 && click_point.x < button_size.x && click_point.y > 0 && click_point.y < button_size.x) {
        if(button->GetAction() == HOME_HISTORY) {
            // show history
        } else {
            wxCommandEvent action_event(HOME_DIALOG_ACTION);
            if(button->GetAction() == HOME_PLAY) {

            }
            action_event.SetId(button->GetAction());
            ProcessWindowEvent(action_event);
        }
    }
}

HomeDialogPanel::HomeDialogPanel(HomeDialog* parent, const wxSize& size, const wxString& title_text, const wxString& version_text, const wxColour& base_colour) :
	wxPanel(parent),
    m_title_text(title_text),
    m_version_text(version_text),
    m_text_colour(base_colour.ChangeLightness(40)),
    m_background_colour(base_colour)
{
    wxSize button_size = wxSize(150, 35);
    wxColour button_base_colour = base_colour.ChangeLightness(90);

    int button_pos_center_x = size.x / 4 - button_size.x / 2;
    int button_pos_center_y = size.y / 2;

    wxPoint newMapButtonPoint(button_pos_center_x, button_pos_center_y);
    auto* new_map_button = new HomeDialogButton(this,
        wxDefaultPosition,
        button_size,
        button_base_colour,
        "Play");
    new_map_button->SetAction(HOME_PLAY);
    new_map_button->Bind(wxEVT_LEFT_UP, &HomeDialog::OnButtonClicked, parent);

    auto* open_map_button = new HomeDialogButton(this,
        wxDefaultPosition,
        button_size,
        button_base_colour,
        "Open");
    open_map_button->SetAction(HOME_OPEN);
    open_map_button->Bind(wxEVT_LEFT_UP, &HomeDialog::OnButtonClicked, parent);
    auto* preferences_button = new HomeDialogButton(this,
        wxDefaultPosition,
        button_size,
        button_base_colour,
        "History");
    preferences_button->SetAction(HOME_HISTORY);
    preferences_button->Bind(wxEVT_LEFT_UP, &HomeDialog::OnButtonClicked, parent);

    Bind(wxEVT_PAINT, &HomeDialogPanel::OnPaint, this);

    wxSizer* rootSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* buttons_sizer = new wxBoxSizer(wxVERTICAL);
    buttons_sizer->AddSpacer(size.y / 2);
    buttons_sizer->Add(new_map_button, 0, wxALIGN_CENTER | wxTOP, 10);
    buttons_sizer->Add(open_map_button, 0, wxALIGN_CENTER | wxTOP, 10);
    buttons_sizer->Add(preferences_button, 0, wxALIGN_CENTER | wxTOP, 10);

    wxSizer* vertical_sizer = new wxBoxSizer(wxVERTICAL);
    vertical_sizer->Add(buttons_sizer, 1, wxEXPAND);

    rootSizer->Add(vertical_sizer, 1, wxEXPAND);
    SetSizer(rootSizer);
}

void HomeDialogPanel::OnPaint(const wxPaintEvent& event)
{
}

void HomeDialogPanel::updateInputs()
{
}

HomeDialogButton::HomeDialogButton(wxWindow* parent, const wxPoint& pos, const wxSize& size, const wxColour& baseColour, const wxString& text) :
    wxPanel(parent, wxID_ANY, pos, size),
    m_action(wxID_CLOSE),
    m_text(text),
    m_text_colour(baseColour.ChangeLightness(40)),
    m_background(baseColour.ChangeLightness(96)),
    m_background_hover(baseColour.ChangeLightness(93)),
    m_is_hover(false)
{
    Bind(wxEVT_PAINT, &HomeDialogButton::OnPaint, this);
    Bind(wxEVT_ENTER_WINDOW, &HomeDialogButton::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &HomeDialogButton::OnMouseLeave, this);
}

void HomeDialogButton::OnPaint(const wxPaintEvent& event) {
    wxPaintDC dc(this);

    wxColour colour = m_is_hover ? m_background_hover : m_background;
    dc.SetBrush(wxBrush(colour));
    dc.SetPen(wxPen(colour, 1));
    dc.DrawRectangle(wxRect(wxPoint(0, 0), GetClientSize()));

    dc.SetFont(GetFont());
    dc.SetTextForeground(m_text_colour);
    wxSize text_size = dc.GetTextExtent(m_text);
    dc.DrawText(m_text, wxPoint(GetSize().x / 2 - text_size.x / 2, GetSize().y / 2 - text_size.y / 2));
}

void HomeDialogButton::OnMouseEnter(const wxMouseEvent& event) {
    m_is_hover = true;
    Refresh();
}

void HomeDialogButton::OnMouseLeave(const wxMouseEvent& event) {
    m_is_hover = false;
    Refresh();
}
