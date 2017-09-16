#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <vector>
#include <atomic>

#include "wxcrafter.h"
#include "portaudio.h"
#include "sqlite3.h"
#include "process.hpp"

enum {
    Exec_Btn_Close
};

class RenderTimer : public wxTimer
{
    wxTextCtrl *textout;
public:
    RenderTimer(wxTextCtrl *textout);
    void Notify();
    void start();
};

class PipeFrame : public wxFrame
{
public:
    PipeFrame(wxFrame *parent,
                const wxString& cmd);
    ~PipeFrame() {
        delete timer;
    }
                
    void addOutput(const std::string &output);

protected:
    void OnBtnClose(wxCommandEvent& WXUNUSED(event)) { DoClose(); }

    void OnClose(wxCloseEvent& event);

    void DoClose();

private:
    RenderTimer* timer;
    
    wxTextCtrl *m_textOut;

    wxDECLARE_EVENT_TABLE();
};

class Settings {
    public:
        std::string device = "default";
        int device_id = -1;
        int sample_rate = 44100;
        int buffer_size = 512;
        int channels = 2;
        int alsa_rt = 0;
        std::string ip = "127.0.0.1";
        int port = 3003;
        int rx = 4096;
        int compression = 0;
        std::string osc_out_ip = "127.0.0.1";
        int osc_out_port = 57120;
        int osc_out = 0;
        double noise_amount = 0.1;
        std::string grains_folder = "grains";
        int granular_max_density = 128;
        int fps = 60;
        int frames_queue = 7;
        int commands_queue = 16;
        double smooth_factor = 8;
        
        const std::string toFAS() {
            std::string parameters;
            
            parameters = " --device " + std::to_string(device_id);
            parameters += " --sample_rate " + std::to_string(sample_rate);
            parameters += " --frames " + std::to_string(buffer_size);
            parameters += " --output_channels " + std::to_string(channels);
            parameters += " --deflate " + std::to_string(compression);
            parameters += " --alsa_realtime_scheduling " + std::to_string(alsa_rt);
            parameters += " --iface " + ip;
            parameters += " --port " + std::to_string(port);
            parameters += " --rx_buffer_size " + std::to_string(rx);
            parameters += " --noise_amount " + std::to_string(noise_amount);
            parameters += " --smooth_factor " + std::to_string(smooth_factor);
            parameters += " --grains_folder " + grains_folder;
            parameters += " --granular_max_density " + std::to_string(granular_max_density);
            parameters += " --osc_out " + std::to_string(osc_out);
            parameters += " --osc_addr " + osc_out_ip;
            parameters += " --osc_port " + std::to_string(osc_out_port);
            parameters += " --fps " + std::to_string(fps);
            parameters += " --frames_queue_size " + std::to_string(frames_queue);
            parameters += " --commands_queue_size " + std::to_string(commands_queue);
            
            return parameters;
        }
};

class MainFrame : public MainFrameBaseClass
{
public:
    MainFrame(wxWindow* parent);
    virtual ~MainFrame();
    
    void SetDeviceCapability(int device);
    void UpdateDevices();
    void ScanDevices();
    
    Settings *getCurrentSettings();
    void updateUISettings(Settings *settings);
    void updateDBSettings(Settings *settings, std::string &session_name);
    void submitSettingsToDB(Settings *settings, std::string &session_name);
    void fillSettingsFromDBstmt(Settings *settings, sqlite3_stmt *stmt);
    void populateSessions();

    void OnExit(wxCommandEvent& event);
    void OnDocumentation(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnDeviceSelected(wxCommandEvent& event);
    void OnAddSessionClicked(wxCommandEvent& event);
    void OnDelSessionClicked(wxCommandEvent& event);
    void OnLaunchFASClicked(wxCommandEvent& event);
    void OnSaveAsDefaultClicked(wxCommandEvent& event);
    void OnSessionSelected(wxCommandEvent& event);
    void OnSessionDblClick(wxCommandEvent& event);
    void OnSettingsChanged(wxCommandEvent& event);
    
    //TinyProcessLib::Process *fas_process;
    int fas_process_state = 0;

    sqlite3 *db = 0;
    
    Settings *default_settings;
    Settings *current_settings;
    
    std::vector<const PaDeviceInfo *> devices_info;
    std::vector<double> sampling_rates = { 8000, 11025, 16000, 22050, 32000, 37800, 44056, 44100, 47250, 48000, 50000, 50400, 88200, 96000, 176400, 192000 };
};

#endif // MAINFRAME_H
