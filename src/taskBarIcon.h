#pragma once
#include "pch.h"
class taskBarIcon : public wxTaskBarIcon
{

private:
    wxMenu* menu = nullptr;

protected:
    // creates menu to be displayed when user clicks on the icon
    virtual wxMenu* CreatePopupMenu();

public:
    void onCloseMenu(wxCommandEvent& evt);
    void onInjectMenu(wxCommandEvent& evt);
    void onOpenMenu(wxCommandEvent& evt);
    void onTaskBarDClick(wxTaskBarIconEvent& evt);
    wxDECLARE_EVENT_TABLE();
};

