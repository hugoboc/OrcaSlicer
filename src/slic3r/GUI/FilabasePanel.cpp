#include "FilabasePanel.hpp"

#include "GUI_App.hpp"
#include "Widgets/WebView.hpp"
#include "libslic3r/AppConfig.hpp"

#include <utility>

#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/webview.h>

namespace Slic3r { namespace GUI {

FilabasePanel::FilabasePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
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
    m_browser->Bind(EVT_WEBVIEW_RECREATED, [](wxCommandEvent&) {});
    GetSizer()->Add(m_browser, 1, wxEXPAND);
}

void FilabasePanel::load_url(const wxString& url)
{
    m_url = url;
    m_load_pending = false;
    WebView::LoadUrl(m_browser, m_url);
}

bool FilabasePanel::Show(bool show)
{
    const bool changed = wxPanel::Show(show);
    if (show && std::exchange(m_reset_on_show, false)) {
        // Follow the existing WebView2 recovery after a language rebuild.
        m_browser->Destroy();
        create_browser();
        m_load_pending = true;
        Layout();
    }
    if (show && std::exchange(m_load_pending, false))
        WebView::LoadUrl(m_browser, m_url);
    return changed;
}

void FilabasePanel::shutdown()
{
    m_browser->Stop();
}

}} // namespace Slic3r::GUI
