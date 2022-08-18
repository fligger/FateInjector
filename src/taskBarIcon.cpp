#include "pch.h"
#include "taskBarIcon.h"
#include "cMain.h"
#include "cApp.h"

wxBEGIN_EVENT_TABLE(taskBarIcon, wxTaskBarIcon)
EVT_MENU(wxID_CLOSE, taskBarIcon::onCloseMenu)
EVT_MENU(301, taskBarIcon::onInjectMenu)
EVT_MENU(302, taskBarIcon::onOpenMenu)
EVT_TASKBAR_LEFT_DCLICK(taskBarIcon::onTaskBarDClick)
wxEND_EVENT_TABLE();

wxMenu *taskBarIcon::CreatePopupMenu()
{

    menu = new wxMenu();
    menu->Append(301, "Inject");
    menu->Append(302, "Open");
    menu->Append(wxID_CLOSE);
    return menu;
}

void taskBarIcon::onTaskBarDClick(wxTaskBarIconEvent &evt)
{

    ref->Show();

    evt.Skip();
}

void taskBarIcon::onInjectMenu(wxCommandEvent &evt)
{

    cMain::OnInjectButtonExecute(evt, ref);

    evt.Skip();
}

void taskBarIcon::onCloseMenu(wxCommandEvent &evt)
{
    ref->Close();
    evt.Skip();
}

void taskBarIcon::onOpenMenu(wxCommandEvent &evt)
{

    ref->Show();
    evt.Skip();
}
