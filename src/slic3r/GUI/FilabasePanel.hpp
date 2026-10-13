#pragma once

#include "Lazy.hpp"

#include <wx/panel.h>
#include <wx/string.h>

class wxWebView;
class wxWindow;

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

    wxWebView* m_browser{nullptr};
    wxString   m_url;
    bool       m_load_pending{false};
    bool       m_reset_on_show{false};
};

}} // namespace Slic3r::GUI
