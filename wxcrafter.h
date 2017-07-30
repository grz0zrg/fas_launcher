//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxcrafter.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _FS_FAS_LAUNCHER_WXCRAFTER_BASE_CLASSES_H
#define _FS_FAS_LAUNCHER_WXCRAFTER_BASE_CLASSES_H

#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/timer.h>
#include <wx/app.h> 
#include <wx/xrc/xh_bmp.h>
#include <wx/frame.h>
#include <wx/iconbndl.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/defs.h> 
#include <wx/msgdlg.h>
#include <wx/txtstrm.h>
#include <wx/statbox.h>
#include <wx/listbox.h>
#include <wx/collpane.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/arrstr.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include "wx/process.h"
#include <wx/spinbutt.h>
#include <wx/button.h>
#include <wx/menu.h>
#include <wx/textdlg.h>
#if wxVERSION_NUMBER >= 2900
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/treebook.h>
#endif

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#if wxVERSION_NUMBER >= 3100
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)
#else
#define WXC_FROM_DIP(x) x
#endif


class MainFrameBaseClass : public wxFrame
{
protected:
    wxPanel* mainPanel;
    wxListBox* sessionsListBox;
    wxCollapsiblePane* m_collPane194;
    wxStaticText* DeviceStaticText;
    wxChoice* m_choice52;
    wxStaticText* m_staticText62;
    wxChoice* m_choice56;
    wxStaticText* m_staticText66;
    wxSpinCtrl* m_spinCtrl64;
    wxStaticText* m_staticText58;
    wxSpinCtrl* m_spinCtrlChannels;
    wxStaticLine* m_staticLine94;
    wxCheckBox* m_checkBox96;
    wxCollapsiblePane* m_collPane176;
    wxStaticText* m_staticText70;
    wxTextCtrl* m_textCtrl72;
    wxStaticText* m_staticText74;
    wxSpinCtrl* m_spinCtrl86;
    wxStaticText* m_staticText98;
    wxSpinButton* m_spinButton100;
    wxStaticLine* m_staticLine92;
    wxCheckBox* m_checkBox90;
    wxCollapsiblePane* m_collPane188;
    wxStaticText* m_staticText106;
    wxSpinCtrlDouble* m_spinButton108;
    wxStaticText* m_staticText222;
    wxTextCtrl* m_textCtrl224;
    wxCollapsiblePane* m_collPane192;
    wxStaticText* m_staticText1105;
    wxSpinButton* m_spinButton1126;
    wxStaticText* m_staticText1147;
    wxSpinButton* m_spinButton1168;
    wxStaticText* m_staticText1189;
    wxSpinButton* m_spinButton12010;
    wxButton* m_button136;
    wxButton* m_button139;
    wxButton* m_button229;
    wxButton* m_button231;
    wxButton* m_button233;
    wxMenuBar* m_menuBar;
    wxMenu* m_name6;
    wxMenuItem* m_menuItem7;
    wxMenu* m_name8;
    wxMenuItem* m_menuItem9;
    wxMenuItem* m_menuItem184;
    wxMenuItem* m_menuItem182;

protected:
    virtual void OnExit(wxCommandEvent& event) { event.Skip(); }
    virtual void OnDocumentation(wxCommandEvent& event) { event.Skip(); }
    virtual void OnAbout(wxCommandEvent& event) { event.Skip(); }
    virtual void OnDeviceSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnAddSessionClicked(wxCommandEvent& event) { event.Skip(); }
    virtual void OnDelSessionClicked(wxCommandEvent& event) { event.Skip(); }
    virtual void OnLaunchFASClicked(wxCommandEvent& event) { event.Skip(); }
    virtual void OnSaveAsDefaultClicked(wxCommandEvent& event) { event.Skip(); }
    virtual void OnSessionSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnSessionDblClick(wxCommandEvent& event) { event.Skip(); }
    virtual void OnSettingsChanged(wxCommandEvent& event) { event.Skip(); }

public:
    wxListBox* GetSessionsListBox() { return sessionsListBox; }
    wxStaticText* GetDeviceStaticText() { return DeviceStaticText; }
    wxChoice* GetChoice52() { return m_choice52; }
    wxStaticText* GetStaticText62() { return m_staticText62; }
    wxChoice* GetChoice56() { return m_choice56; }
    wxStaticText* GetStaticText66() { return m_staticText66; }
    wxSpinCtrl* GetSpinCtrl64() { return m_spinCtrl64; }
    wxStaticText* GetStaticText58() { return m_staticText58; }
    wxSpinCtrl* GetChannelsChoice() { return m_spinCtrlChannels; }
    wxStaticLine* GetStaticLine94() { return m_staticLine94; }
    wxCheckBox* GetCheckBox96() { return m_checkBox96; }
    wxCollapsiblePane* GetCollPane194() { return m_collPane194; }
    wxStaticText* GetStaticText70() { return m_staticText70; }
    wxTextCtrl* GetTextCtrl72() { return m_textCtrl72; }
    wxStaticText* GetStaticText74() { return m_staticText74; }
    wxSpinCtrl* GetSpinCtrl86() { return m_spinCtrl86; }
    wxStaticText* GetStaticText98() { return m_staticText98; }
    wxSpinButton* GetSpinButton100() { return m_spinButton100; }
    wxStaticLine* GetStaticLine92() { return m_staticLine92; }
    wxCheckBox* GetCheckBox90() { return m_checkBox90; }
    wxCollapsiblePane* GetCollPane176() { return m_collPane176; }
    wxStaticText* GetStaticText106() { return m_staticText106; }
    wxSpinCtrlDouble* GetSpinButton108() { return m_spinButton108; }
    wxStaticText* GetStaticText222() { return m_staticText222; }
    wxTextCtrl* GetTextCtrl224() { return m_textCtrl224; }
    wxCollapsiblePane* GetCollPane188() { return m_collPane188; }
    wxStaticText* GetStaticText1105() { return m_staticText1105; }
    wxSpinButton* GetSpinButton1126() { return m_spinButton1126; }
    wxStaticText* GetStaticText1147() { return m_staticText1147; }
    wxSpinButton* GetSpinButton1168() { return m_spinButton1168; }
    wxStaticText* GetStaticText1189() { return m_staticText1189; }
    wxSpinButton* GetSpinButton12010() { return m_spinButton12010; }
    wxCollapsiblePane* GetCollPane192() { return m_collPane192; }
    wxButton* GetButton136() { return m_button136; }
    wxButton* GetButton229() { return m_button229; }
    wxButton* GetButton231() { return m_button231; }
    wxButton* GetButton233() { return m_button233; }
    wxPanel* GetMainPanel() { return mainPanel; }
    wxMenuBar* GetMenuBar() { return m_menuBar; }
    MainFrameBaseClass(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Fragment (launcher) - https://www.fsynth.com"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500,360), long style = wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxSYSTEM_MENU|wxCLOSE_BOX);
    virtual ~MainFrameBaseClass();
};

#endif
