#include "MainFrame.h"
#include <wx/aboutdlg.h>

TinyProcessLib::Process *fas_process;
std::string program_name;
std::string cmd_std_str;

std::atomic<bool> fas_state;
std::mutex m_fas_output;
std::vector<std::string> fas_output;

RenderTimer::RenderTimer(wxTextCtrl *textout) : wxTimer() {
    RenderTimer::textout = textout;
}
 
void RenderTimer::Notify() {
    m_fas_output.lock();
    for (unsigned int i = 0; i < fas_output.size(); i += 1) {
        RenderTimer::textout->AppendText(fas_output[i]);
    }
    fas_output.clear();
    m_fas_output.unlock();
}
 
void RenderTimer::start()
{
    wxTimer::Start(1000);
}

wxBEGIN_EVENT_TABLE(PipeFrame, wxFrame)
    EVT_BUTTON(Exec_Btn_Close, PipeFrame::OnBtnClose)

    EVT_CLOSE(PipeFrame::OnClose)
wxEND_EVENT_TABLE()

PipeFrame::PipeFrame(wxFrame *parent,
                         const wxString& cmd,
                         const wxString& install_path)
           : wxFrame(parent, wxID_ANY, cmd, wxDefaultPosition, wxSize(500,400))
{
    wxIcon fs_icon = wxIcon(install_path + "icon.png", wxBITMAP_TYPE_PNG);

    SetIcon(fs_icon);
    
    SetMinSize(wxSize(500,400));
    
    wxPanel *panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(500,400));

    m_textOut = new wxTextCtrl(panel, wxID_ANY, wxEmptyString,
                              wxDefaultPosition, wxDefaultSize,
                              wxTE_MULTILINE | wxTE_RICH);
    m_textOut->SetEditable(false);

    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    sizerTop->Add(m_textOut, 1, wxGROW | wxALL | wxEXPAND, 5);

    wxSizer *sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    sizerBtns->Add(new wxButton(panel, Exec_Btn_Close, wxT("&Close")), 0, wxALL, 5);

    sizerTop->Add(sizerBtns, 0, wxCENTRE | wxALL, 5);
    
    panel->SetSizer(sizerTop);
    sizerTop->Fit(this);

    Show();
    
    timer = new RenderTimer(m_textOut);
    timer->start();
}

void PipeFrame::addOutput(const std::string &output)
{
    wxString out(output);
    
    m_textOut->AppendText(out);
}

void PipeFrame::DoClose() {
    timer->Stop();
    if (fas_process) {
        fas_process->write("q", 1);
        fas_process->close_stdin();
    }
    Destroy();
}

void PipeFrame::OnClose(wxCloseEvent& event)
{
    timer->Stop();
    if (fas_process) {
        fas_process->write("q", 1);
        fas_process->close_stdin();
    }
    event.Skip();
}

MainFrame::MainFrame(wxWindow* parent, const wxString& path)
    : MainFrameBaseClass(parent)
{        
    install_path = path;

#ifdef __UNIX__
    const char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    
    std::string hd = std::string(homedir) + "/fragment";
    std::string db_path = std::string(homedir) + "/fragment/main.db";

    const int dir_err = mkdir(hd.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (dir_err == -1) {
        if (errno != EEXIST) {
            wxMessageBox(wxT("Failed to create '" + hd + "' directory."), wxT("sqlite3_open error."), wxICON_ERROR);
        }
    }
#endif

    wxIcon fs_icon;
    fs_icon = wxIcon(install_path + "icon.png", wxBITMAP_TYPE_PNG);

    SetIcon(fs_icon);
    
    UpdateDevices();
#ifdef __UNIX__
    sqlite3_open(db_path.c_str(), &db);
#else
    sqlite3_open(install_path + "main.db", &db);
#endif
	if (db == 0) {
		wxMessageBox(wxT("Failed to open settings database " + install_path + "'main.db'."), wxT("sqlite3_open error."), wxICON_ERROR);
    } else {
        const char *sql = "CREATE TABLE IF NOT EXISTS sessions("  \
            "id INTEGER PRIMARY KEY AUTOINCREMENT," \
            "name                  TEXT     NOT NULL," \
            "device                TEXT     NOT NULL," \
            "device_id             INT      NOT NULL," \
            "sample_rate           INT      NOT NULL," \
            "buffer_size           INT      NOT NULL," \
            "channels              INT      NOT NULL," \
            "alsa_rt               INT      NOT NULL," \
            "ip                    TEXT     NOT NULL," \
            "port                  INT      NOT NULL," \
            "rx                    INT      NOT NULL," \
            "compression           INT      NOT NULL," \
            "osc_out               INT      NOT NULL," \
            "osc_out_ip            TEXT     NOT NULL," \
            "osc_out_port          INT      NOT NULL," \
            "noise                 REAL     NOT NULL," \
            "smooth_factor         REAL     NOT NULL," \
            "granular_max_density  INT      NOT NULL," \
            "grains_folder         TEXT     NOT NULL," \
            "fps                   INT      NOT NULL," \
            "frames_queue          INT      NOT NULL," \
            "commands_queue        INT      NOT NULL);";
            
        char *error_msg;
            
        int rc = sqlite3_exec(db, sql, NULL, NULL, &error_msg);
        if (rc != SQLITE_OK) {
            wxMessageBox(wxT("Failed to execute 'create table sessions' query for database 'main.db' (see stdout)."), wxT("sqlite3_exec error."), wxICON_ERROR);
            
            std::cout << "SQLite query failed : "<< error_msg << std::endl;
            
            sqlite3_free(error_msg);
        }
        
        populateSessions();
    }
    
    default_settings = new Settings();
    current_settings = new Settings();
}

MainFrame::~MainFrame()
{
    Pa_Terminate();
    
    sqlite3_close(db);
    
    delete default_settings;
    delete current_settings;
}

void MainFrame::submitSettingsToDB(Settings *settings, std::string &session_name) {
    std::string sql("INSERT INTO sessions VALUES (null" \
        ",  '" + session_name +
        "', '" + settings->device +
        "', " + std::to_string(settings->device_id) +
        ", " + std::to_string(settings->sample_rate) +
        ", " + std::to_string(settings->buffer_size) +
        ", " + std::to_string(settings->channels) +
        ", " + std::to_string(settings->alsa_rt) +
        ", '" + settings->ip +
        "', " + std::to_string(settings->port) +
        ", " + std::to_string(settings->rx) +
        ", " + std::to_string(settings->compression) +
        ", " + std::to_string(settings->osc_out) +
        ", '" + settings->osc_out_ip +
        "', " + std::to_string(settings->osc_out_port) +
        ", " + std::to_string(settings->noise_amount) +
        ", " + std::to_string(settings->smooth_factor) +
        ", " + std::to_string(settings->granular_max_density) +
        ", '" + settings->grains_folder +
        "', " + std::to_string(settings->fps) +
        ", " + std::to_string(settings->frames_queue) +
        ", " + std::to_string(settings->commands_queue) +
        ");");
    
    char *error_msg;
    
    int rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        wxMessageBox(wxT("Failed to execute 'submitSettingsToDB' query for database 'main.db' (see stdout)."), wxT("sqlite3_exec error."), wxICON_ERROR);
        
        std::cout << "SQLite query failed : "<< error_msg << std::endl;
        
        sqlite3_free(error_msg);
    }
}

void MainFrame::updateDBSettings(Settings *settings, std::string &session_name) {
    std::string sql("UPDATE sessions SET " \
        "device='" + settings->device +
        "', device_id=" + std::to_string(settings->device_id) +
        ", sample_rate=" + std::to_string(settings->sample_rate) +
        ", buffer_size=" + std::to_string(settings->buffer_size) +
        ", channels=" + std::to_string(settings->channels) +
        ", alsa_rt=" + std::to_string(settings->alsa_rt) +
        ", ip='" + settings->ip +
        "', port=" + std::to_string(settings->port) +
        ", rx=" + std::to_string(settings->rx) +
        ", compression=" + std::to_string(settings->compression) +
        ", osc_out=" + std::to_string(settings->osc_out) +
        ", osc_out_ip='" + settings->osc_out_ip +
        "', osc_out_port=" + std::to_string(settings->port) +
        ", noise=" + std::to_string(settings->noise_amount) +
        ", smooth_factor=" + std::to_string(settings->smooth_factor) +
        ", granular_max_density=" + std::to_string(settings->granular_max_density) +
        ", grains_folder='" + settings->grains_folder +
        "', fps=" + std::to_string(settings->fps) +
        ", frames_queue=" + std::to_string(settings->frames_queue) +
        ", commands_queue=" + std::to_string(settings->commands_queue) +
        " WHERE name='" + session_name + "';");
    
    char *error_msg;
    
    int rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        wxMessageBox(wxT("Failed to execute 'updateDBSettings' query for database 'main.db' (see stdout)."), wxT("sqlite3_exec error."), wxICON_ERROR);
        
        std::cout << "SQLite query failed : "<< error_msg << std::endl;
        
        sqlite3_free(error_msg);
    }
}

void MainFrame::fillSettingsFromDBstmt(Settings *settings, sqlite3_stmt *stmt) {
    const char *device = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    int device_id = sqlite3_column_int(stmt, 3);
    int sample_rate = sqlite3_column_int(stmt, 4);
    int buffer_size = sqlite3_column_int(stmt, 5);
    int channels = sqlite3_column_int(stmt, 6);
    int alsa_rt = sqlite3_column_int(stmt, 7);
    const char *ip = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8));
    int port = sqlite3_column_int(stmt, 9);
    int rx = sqlite3_column_int(stmt, 10);
    int compression = sqlite3_column_int(stmt, 11);
    int osc_out = sqlite3_column_int(stmt, 12);
    const char *osc_out_ip = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 13));
    int osc_out_port = sqlite3_column_int(stmt, 14);
    double noise = sqlite3_column_double(stmt, 15);
    double smooth_factor = sqlite3_column_double(stmt, 16);
    int granular_max_density = sqlite3_column_int(stmt, 17);
    const char *grains_folder = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 18));
    int fps = sqlite3_column_int(stmt, 19);
    int frames_queue = sqlite3_column_int(stmt, 20);
    int commands_queue = sqlite3_column_int(stmt, 21);

    settings->device.clear();
    settings->device.append(device);
    
    settings->device_id = device_id;
    settings->sample_rate = sample_rate;
    settings->buffer_size = buffer_size;
    settings->channels = channels;
    settings->alsa_rt = alsa_rt;
    
    settings->ip.clear();
    settings->ip.append(ip);
    
    settings->port = port;
    settings->rx = rx;
    settings->compression = compression;
    
    settings->osc_out = osc_out;
    settings->osc_out_ip.clear();
    settings->osc_out_ip.append(osc_out_ip);
    settings->osc_out_port = osc_out_port;
    
    settings->smooth_factor = smooth_factor;
    settings->noise_amount = noise;
    
    settings->granular_max_density = granular_max_density;
    
    settings->grains_folder.clear();
    settings->grains_folder.append(grains_folder);
    
    settings->fps = fps;
    settings->frames_queue = frames_queue;
    settings->commands_queue = commands_queue;
}

void MainFrame::populateSessions() {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "select * from sessions", -1, &stmt, NULL);
    
    wxListBox* session_listbox = GetSessionsListBox();
    
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        
        if (strcmp((char *)name, "$default") == 0) {
            fillSettingsFromDBstmt(default_settings, stmt);
        } else {
            session_listbox->Append(name);
        }
    }
    
    sqlite3_finalize(stmt);
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Close();
}

void MainFrame::OnDocumentation(wxCommandEvent& event)
{
    wxLaunchDefaultBrowser("https://www.fsynth.com/documentation", wxBROWSER_NEW_WINDOW);
}

void MainFrame::OnDeviceSelected(wxCommandEvent& event)
{
    int device_id = event.GetSelection();
    
    // check if there is any output channels
    const PaDeviceInfo *device_info;
    device_info = Pa_GetDeviceInfo(device_id);
    
    if (device_info->maxOutputChannels <= 0) {
        wxMessageBox(wxT("The selected device has no output channels.\nPlease select a device with output channels."), wxT("Device has no output channels."), wxICON_WARNING);
    }
    
    SetDeviceCapability(device_id);
}

void MainFrame::OnLaunchFASClicked(wxCommandEvent& event)
{
    if (!fas_process_state) {
        Settings *settings = getCurrentSettings();
        
        if (install_path == "") {
            cmd_std_str = settings->toFAS("./fas");
            program_name = std::string("./fas");
        } else {
            cmd_std_str = settings->toFAS("/usr/local/bin/fas");
            program_name = std::string("/usr/local/bin/fas");
        }

        delete current_settings;
        
        current_settings = settings;

        wxListBox* session_listbox = GetSessionsListBox();
        int selected_session = session_listbox->GetSelection();
        
        wxCheckBox* fasAppLaunchCk = GetFasAppLaunchCheckBox();
        int fasAppLaunch = fasAppLaunchCk->GetValue();
        
        if (selected_session != wxNOT_FOUND && fasAppLaunch == 1) {
            wxLaunchDefaultBrowser("https://www.fsynth.com/app/" + session_listbox->GetString(selected_session) + "?fas=1", wxBROWSER_NEW_WINDOW);
        }

        PipeFrame *fas_frame = new PipeFrame(this, cmd_std_str, install_path);
        
        fas_output.clear();

        std::thread fasThread([&]() {
            fas_process = new TinyProcessLib::Process(cmd_std_str, "", [](const char *bytes, size_t n) {
                    m_fas_output.lock();
                    //std::cout << "Output from stdout: " << std::string(bytes, n);
                    fas_output.push_back(std::string(bytes, n));
                    m_fas_output.unlock();
                }, nullptr, true);
                
            fas_state = true;
                
            if (fas_process->get_exit_status()) {
                fas_state = false;
                
                m_fas_output.lock();
                fas_process = 0;
                m_fas_output.unlock();
            }
        });
        fasThread.detach();
    } else {
        wxMessageBox(wxT("The server is already started!"));
    }
}

void MainFrame::updateUISettings(Settings *settings) {
    UpdateDevices();
    
    wxChoice* device_choice = GetChoice52();
    wxChoice* sample_rate_choice = GetChoice56();
    wxSpinCtrl* buffer_size_spin = GetSpinCtrl64();
    wxSpinCtrl* channels_spin = GetChannelsChoice();
    wxCheckBox* alsa_rt_ck = GetCheckBox96();
    wxTextCtrl* ip_txt_ctrl = GetTextCtrl72();
    wxSpinCtrl* port_spin = GetSpinCtrl86();
    wxSpinButton* rx_spin = GetSpinButton100();
    wxCheckBox* compression_ck = GetCheckBox90();
    wxSpinCtrlDouble* noise_amount_spin = GetSpinButton108();
    wxTextCtrl* grains_folder_txt_ctrl = GetTextCtrl224();
    wxSpinButton* fps_spin = GetSpinButton1126();
    wxSpinButton* fqueue_spin = GetSpinButton1168();
    wxSpinButton* cqueue_spin = GetSpinButton12010();
    
    wxSpinCtrlDouble* smooth_factor_spin = GetSpinButtonSInter();
    wxSpinCtrl* granular_max_density_spin = GetSpinCtrlGrDensity();
    
    // OSC
    wxTextCtrl *osc_out_ip_txt_ctrl = GetOscIpTextCtrl();
    wxSpinCtrl *osc_out_port_spin = GetOscPortSpinCtrl();
    wxCheckBox *osc_out_ck = GetOscOutCheckBox();
    
    wxString wdevice_name(settings->device);
    int device_id = device_choice->FindString(wdevice_name);
    if (device_id == wxNOT_FOUND) {
        wxMessageBox(wxT("The device '") + wdevice_name + wxT("' cannot be found.\n The device was set to default."));
    } else {
        device_choice->SetSelection(device_id);
    }
    
    wxString wsmp_rate;
    wsmp_rate << settings->sample_rate;
    int smp_rate_id = sample_rate_choice->FindString(wsmp_rate);
    if (smp_rate_id == wxNOT_FOUND) {
        wxMessageBox(wxT("The sample rate '") + wsmp_rate + wxT("' cannot be found.\n The sample rate was set to 44100."));
    } else {
        sample_rate_choice->SetSelection(smp_rate_id);
    }
    
    buffer_size_spin->SetValue(settings->buffer_size);
    channels_spin->SetValue(settings->channels);
    alsa_rt_ck->SetValue(settings->alsa_rt);
    ip_txt_ctrl->ChangeValue(settings->ip);
    port_spin->SetValue(settings->port);
    rx_spin->SetValue(settings->rx);
    compression_ck->SetValue(settings->compression);
    smooth_factor_spin->SetValue(settings->smooth_factor);
    granular_max_density_spin->SetValue(settings->granular_max_density);
    noise_amount_spin->SetValue(settings->noise_amount);
    grains_folder_txt_ctrl->ChangeValue(settings->grains_folder);
    fps_spin->SetValue(settings->fps);
    fqueue_spin->SetValue(settings->frames_queue);
    cqueue_spin->SetValue(settings->commands_queue);
    
    // OSC
    osc_out_ip_txt_ctrl->ChangeValue(settings->osc_out_ip);
    osc_out_port_spin->SetValue(settings->osc_out_port);
    osc_out_ck->SetValue(settings->osc_out);
}

Settings *MainFrame::getCurrentSettings() {
    Settings *current = new Settings();

    wxChoice* device_choice = GetChoice52();
    wxChoice* sample_rate_choice = GetChoice56();
    wxSpinCtrl* buffer_size_spin = GetSpinCtrl64();
    wxSpinCtrl* channels_spin = GetChannelsChoice();
    wxCheckBox* alsa_rt_ck = GetCheckBox96();
    wxTextCtrl* ip_txt_ctrl = GetTextCtrl72();
    wxSpinCtrl* port_spin = GetSpinCtrl86();
    wxSpinButton* rx_spin = GetSpinButton100();
    wxCheckBox* compression_ck = GetCheckBox90();
    wxSpinCtrlDouble* noise_amount_spin = GetSpinButton108();
    wxTextCtrl* grains_folder_txt_ctrl = GetTextCtrl224();
    wxSpinButton* fps_spin = GetSpinButton1126();
    wxSpinButton* fqueue_spin = GetSpinButton1168();
    wxSpinButton* cqueue_spin = GetSpinButton12010();
    
    wxSpinCtrlDouble* smooth_factor_spin = GetSpinButtonSInter();
    wxSpinCtrl* granular_max_density_spin = GetSpinCtrlGrDensity();
    
    // OSC
    wxTextCtrl *osc_out_ip_txt_ctrl = GetOscIpTextCtrl();
    wxSpinCtrl *osc_out_port_spin = GetOscPortSpinCtrl();
    wxCheckBox *osc_out_ck = GetOscOutCheckBox();
    
    int device_id = device_choice->GetSelection();
    if (device_id != wxNOT_FOUND) {
        std::string device_name = std::string(device_choice->GetString(device_id).mb_str());
        
        current->device.clear();
        current->device.append(device_name);
        
        current->device_id = device_id;
    }
    
    int smp_rate_id = sample_rate_choice->GetSelection();
    if (smp_rate_id != wxNOT_FOUND) {
        current->sample_rate = wxAtoi(sample_rate_choice->GetString(smp_rate_id));
    }
    
    current->buffer_size = buffer_size_spin->GetValue();
    current->channels = channels_spin->GetValue();
    current->alsa_rt = alsa_rt_ck->GetValue();
    
    std::string ip = std::string(ip_txt_ctrl->GetValue().mb_str());
    
    current->ip.clear();
    current->ip.append(ip);
    
    current->port = port_spin->GetValue();
    current->rx = rx_spin->GetValue();
    current->compression = compression_ck->GetValue();
    current->noise_amount = noise_amount_spin->GetValue();
    current->smooth_factor = smooth_factor_spin->GetValue();
    current->granular_max_density = granular_max_density_spin->GetValue();
    
    std::string grains_folder = std::string(grains_folder_txt_ctrl->GetValue().mb_str());
    
    current->grains_folder.clear();
    current->grains_folder.append(grains_folder);
    
    current->fps = fps_spin->GetValue();
    current->frames_queue = fqueue_spin->GetValue();
    current->commands_queue = cqueue_spin->GetValue();
    
    // OSC
    std::string osc_out_ip = std::string(osc_out_ip_txt_ctrl->GetValue().mb_str());
    
    current->osc_out_ip.clear();
    current->osc_out_ip.append(osc_out_ip);
    
    current->osc_out = osc_out_ck->GetValue();
    current->osc_out_port = osc_out_port_spin->GetValue();
    
    return current;
}

void MainFrame::OnAddSessionClicked(wxCommandEvent& event)
{
   wxTextEntryDialog* dialog = new wxTextEntryDialog(this, "Create a new session");
   dialog->ShowModal();
   
   wxListBox* session_listbox = GetSessionsListBox();
   
   wxString session_name(dialog->GetValue());
   
   session_listbox->Append(session_name);
   
   dialog->Destroy();
   
   std::string s_name = std::string(session_name.mb_str());
   
   submitSettingsToDB(default_settings, s_name);
}

void MainFrame::OnDelSessionClicked(wxCommandEvent& event)
{
    wxListBox* session_listbox = GetSessionsListBox();
    
    int selected_session = session_listbox->GetSelection();
    
    if (selected_session != wxNOT_FOUND) {
        wxString session_name(session_listbox->GetString(selected_session));
        
        std::string s_name = std::string(session_name.mb_str());
        
        std::string sql = "DELETE FROM sessions WHERE name='" + s_name + "';";
        
        char *error_msg;
            
        int rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &error_msg);
        if (rc != SQLITE_OK) {
            wxMessageBox(wxT("Failed to execute 'DELETE FROM sessions' query for database 'main.db' (see stdout)."), wxT("sqlite3_exec error."), wxICON_ERROR);
            
            std::cout << "SQLite query failed : "<< error_msg << std::endl;
            
            sqlite3_free(error_msg);
        } else {
            session_listbox->Delete(selected_session);
            
            if ((selected_session - 1) >= 0) {
                session_listbox->SetSelection(selected_session - 1);
            }
        }
    }
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxIcon fs_icon = wxIcon(install_path + "icon_credits.png", wxBITMAP_TYPE_PNG);

    SetIcon(fs_icon);
    
    wxUnusedVar(event);
    wxAboutDialogInfo info;
    info.SetIcon(fs_icon);
    info.SetName("FGL: Fragment Graphical Launcher");
    info.SetWebSite("https://www.fsynth.com");
    info.AddDeveloper("Julien Verneuil - contact@fsynth.com - https://github.com/grz0zrg");
    info.SetCopyright(_("Fragment © 2016 - 2017"));
    info.SetLicence(_("BSD 2-Clause License\n\
        \n\
        Copyright (c) 2017, Julien Verneuil\n\
        All rights reserved. \n\
        \n\
        Redistribution and use in source and binary forms, with or without\n\
        modification, are permitted provided that the following conditions are met:\n\
        \n\
        * Redistributions of source code must retain the above copyright notice, this\n\
          list of conditions and the following disclaimer.\n\
        \n\
        * Redistributions in binary form must reproduce the above copyright notice,\n\
          this list of conditions and the following disclaimer in the documentation\n\
          and/or other materials provided with the distribution.\n\
        \n\
        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS'\n\
        AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n\
        IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n\
        DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE\n\
        FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n\
        DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\n\
        SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n\
        CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,\n\
        OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n\
        OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."));
    info.SetDescription(_("The Collaborative Spectral Synthesizer launcher"));
    ::wxAboutBox(info);
}

void MainFrame::SetDeviceCapability(int device) {
    PaError err;
    
    const PaDeviceInfo *device_info;
    device_info = Pa_GetDeviceInfo(device);
    if (device_info == NULL) {
        return;
    }

    int device_max_output_channels = device_info->maxOutputChannels;

    if (device_max_output_channels < 2) {
        wxMessageBox(wxT("Can't use this audio device because it has less than 2 output channels."), wxT("Can't use the audio device."), wxICON_ERROR);
        return;
    }
    
    PaStreamParameters outputParameters;
    memset(&outputParameters, 0, sizeof(PaStreamParameters));
    
    outputParameters.device = device;
    outputParameters.channelCount = device_max_output_channels;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    
    wxChoice* sample_rate_choice = GetChoice56();
    sample_rate_choice->Clear();
    
    int def_sample_rate_choice_id = -1;
    unsigned int k = 0;
    for (unsigned int i = 0; i < sampling_rates.size(); i++) {
        double sample_rate = sampling_rates[i];
        
        err = Pa_IsFormatSupported(NULL, &outputParameters, sample_rate);
        if (err == paFormatIsSupported) {
            sample_rate_choice->Append(wxString::Format(wxT("%i"), (unsigned int)sample_rate));
            
            if (sample_rate == device_info->defaultSampleRate) {
                def_sample_rate_choice_id = k;
            }
            
            k++;
        }
    }
    
    if (def_sample_rate_choice_id) {
        sample_rate_choice->SetSelection(def_sample_rate_choice_id);
    }
    
    wxSpinCtrl *channels_choice = GetChannelsChoice();
    channels_choice->SetRange(2, device_max_output_channels);
}

void MainFrame::UpdateDevices() {
    unsigned int i;
    int def = -1;

    wxChoice* devices = GetChoice52();
    
    devices->Clear();
    
    ScanDevices();
    
    const PaDeviceInfo *device_info;
    for (i = 0; i < devices_info.size(); i += 1) {
        device_info = devices_info[i];
        
        if (strcmp(device_info->name, "default") == 0 || strcmp(device_info->name, "Primary Sound Driver") == 0) {
            def = i;
        }
        
        devices->Append(device_info->name);
    }
    
    if (def >= 0) {
        devices->SetSelection(def);
        
        SetDeviceCapability(def);
    }
}

void MainFrame::ScanDevices() {
    int i;
    
    PaStreamParameters outputParameters;
    memset(&outputParameters, 0, sizeof(PaStreamParameters));
    
    int num_devices;
    num_devices = Pa_GetDeviceCount();
    if (num_devices < 0) {
        std::cerr << "Error: Pa_CountDevices returned " << num_devices << std::endl;
    }
    
    devices_info.clear();

    const PaDeviceInfo *device_info;
    for (i = 0; i < num_devices; i += 1) {
        device_info = Pa_GetDeviceInfo(i);
        
        if (device_info != NULL) {
            devices_info.push_back(device_info);
        }
/*
        printf("\nPortAudio device %i - %s\n==========\n", i, device_info->name);
        printf("  max input channels : %i\n", device_info->maxInputChannels);
        printf("  max output channels : %i\n", device_info->maxOutputChannels);
        printf("  default low input latency : %f\n", device_info->defaultLowInputLatency);
        printf("  default low output latency : %f\n", device_info->defaultLowOutputLatency);
        printf("  default high input latency : %f\n", device_info->defaultHighInputLatency);
        printf("  default high output latency : %f\n", device_info->defaultHighOutputLatency);
        printf("  default sample rate : %f\n", device_info->defaultSampleRate);
*/
    }

    //printf("\n");
}

void MainFrame::OnSaveAsDefaultClicked(wxCommandEvent& event) {
    delete default_settings;
    
    default_settings = getCurrentSettings();
    
    std::string default_name = "$default";
    
    updateDBSettings(default_settings, default_name);
}

void MainFrame::OnSessionSelected(wxCommandEvent& event) {
    wxListBox* session_listbox = GetSessionsListBox();
    
    int selected_session = session_listbox->GetSelection();
    
    if (selected_session != wxNOT_FOUND) {
        wxString session_name(session_listbox->GetString(selected_session));
        
        std::string sql = "SELECT * FROM sessions WHERE name='";
        sql += std::string(session_name.mb_str());
        sql += "'";
        
        sqlite3_stmt *stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
        
        while (sqlite3_step(stmt) != SQLITE_DONE) {
            fillSettingsFromDBstmt(current_settings, stmt);
            
            updateUISettings(current_settings);
        }
        
        sqlite3_finalize(stmt);
    }
}

void MainFrame::OnSessionDblClick(wxCommandEvent& event) {
    OnLaunchFASClicked(event);
}

void MainFrame::OnSettingsChanged(wxCommandEvent& event) {
    wxListBox* session_listbox = GetSessionsListBox();
    
    int selected_session = session_listbox->GetSelection();
    
    if (selected_session != wxNOT_FOUND) {
        delete current_settings;
        
        current_settings = getCurrentSettings();
        
        wxString session_name(session_listbox->GetString(selected_session));
        
        std::string s_name = std::string(session_name.mb_str());
        
        updateDBSettings(current_settings, s_name);
    }
}