#include "FilabasePanel.hpp"

#include "GUI_App.hpp"
#include "I18N.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/WebView.hpp"
#include "libslic3r/AppConfig.hpp"

#include <utility>

#include <wx/event.h>
#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/webview.h>

namespace Slic3r { namespace GUI {

FilabasePanel::FilabasePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_failure_panel = new wxPanel(this);
    auto* failure_sizer = new wxBoxSizer(wxVERTICAL);
    failure_sizer->AddStretchSpacer();
    m_failure_message = new wxStaticText(m_failure_panel, wxID_ANY,
        _L("Filabase is unavailable. Check its address and network connection, then try again."));
    m_failure_message->Wrap(FromDIP(400));
    failure_sizer->Add(m_failure_message, 0, wxALIGN_CENTER | wxALL, FromDIP(12));

    auto* actions = new wxBoxSizer(wxHORIZONTAL);
    auto* retry = new Button(m_failure_panel, _L("Retry"));
    retry->SetStyle(ButtonStyle::Confirm, ButtonType::Window);
    retry->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        load_url(wxString::FromUTF8(wxGetApp().app_config->get("filabase_url")));
    });
    actions->Add(retry, 0, wxRIGHT, FromDIP(12));
    auto* open_browser = new Button(m_failure_panel, _L("Open in Browser"));
    open_browser->SetStyle(ButtonStyle::Regular, ButtonType::Window);
    open_browser->Bind(wxEVT_BUTTON, [](wxCommandEvent&) {
        wxGetApp().open_browser_with_warning_dialog(
            wxString::FromUTF8(wxGetApp().app_config->get("filabase_url")));
    });
    actions->Add(open_browser);
    failure_sizer->Add(actions, 0, wxALIGN_CENTER | wxALL, FromDIP(12));
    failure_sizer->AddStretchSpacer();
    m_failure_panel->SetSizer(failure_sizer);
    GetSizer()->Add(m_failure_panel, 1, wxEXPAND);
    m_failure_panel->Hide();
    update_failure_theme();
    create_browser();
    m_reset_on_show = WebView::NeedsRecreateOnShow();
    m_url = wxString::FromUTF8(wxGetApp().app_config->get("filabase_url"));
    m_load_pending = true;
}

FilabasePanel::~FilabasePanel()
{
    shutdown();
    m_browser->Destroy();
}

void FilabasePanel::create_browser()
{
    m_browser = WebView::CreateWebView(this, wxEmptyString);
    // Theme changes must not reload a remote page and discard its navigation or forms.
    m_browser->Bind(EVT_WEBVIEW_RECREATED, [this](wxCommandEvent&) { update_failure_theme(); });
    m_browser->Bind(wxEVT_WEBVIEW_ERROR, &FilabasePanel::on_error, this);
    GetSizer()->Add(m_browser, 1, wxEXPAND);
}

void FilabasePanel::on_error(wxWebViewEvent& event)
{
    if (!m_navigation_started || event.GetInt() == wxWEBVIEW_NAV_ERR_USER_CANCELLED)
        return;
#ifdef __WXMSW__
    // Edge also reports API errors with no URL. These are not navigation failures.
    if (event.GetURL().empty())
        return;
#endif
    // The supported Edge, WKWebView and WebKit2 backends report main-document
    // navigation failures here, not resource failures. IsTargetMainFrame() is
    // only valid for NAVIGATING events, while ERROR stores the error code.
    show_failure(true);
}

void FilabasePanel::show_failure(bool show)
{
    m_failure_panel->Show(show);
    m_browser->Show(!show);
    Layout();
}

void FilabasePanel::update_failure_theme()
{
    m_failure_panel->SetBackgroundColour(wxGetApp().get_window_default_clr());
    m_failure_message->SetForegroundColour(wxGetApp().get_label_clr_default());
    m_failure_panel->Refresh();
}

void FilabasePanel::load_url(const wxString& url)
{
    m_url = url;
    m_load_pending = false;
    m_navigation_started = true;
    show_failure(false);
    WebView::LoadUrl(m_browser, m_url);
}

bool FilabasePanel::Show(bool show)
{
    const bool changed = wxPanel::Show(show);
    if (show && std::exchange(m_reset_on_show, false)) {
        // Follow the existing WebView2 recovery after a language rebuild.
        m_navigation_started = false;
        m_browser->Destroy();
        create_browser();
        m_load_pending = true;
        Layout();
    }
    if (show && std::exchange(m_load_pending, false))
        load_url(m_url);
    return changed;
}

void FilabasePanel::shutdown()
{
    m_navigation_started = false;
    m_browser->Stop();
}

}} // namespace Slic3r::GUI
