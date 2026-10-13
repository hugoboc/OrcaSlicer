#pragma once

#include "Lazy.hpp"

#include <wx/panel.h>
#include <wx/string.h>

class wxWebView;
class wxWebViewEvent;
class wxWindow;
class wxStaticText;

namespace Slic3r { namespace GUI {

class FilabasePanel : public wxPanel, public LazyInstance<FilabasePanel>
{
public:
    explicit FilabasePanel(wxWindow* parent);
    ~FilabasePanel() override;

    void load_url(const wxString& url);
    void shutdown();
    bool Show(bool show = true) override;

private:
    void create_browser();
    void on_error(wxWebViewEvent& event);
    void show_failure(bool show);
    void update_failure_theme();

    wxWebView*    m_browser{nullptr};
    wxPanel*      m_failure_panel{nullptr};
    wxStaticText* m_failure_message{nullptr};
    wxString      m_url;
    bool          m_load_pending{false};
    bool          m_reset_on_show{false};
    bool          m_navigation_started{false};
};

}} // namespace Slic3r::GUI
