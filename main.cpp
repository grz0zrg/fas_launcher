#include <wx/app.h>
#include <wx/event.h>
#include "MainFrame.h"
#include <wx/image.h>

// Define the MainApp
class MainApp : public wxApp
{
public:
    MainApp() {}
    virtual ~MainApp() {}

    virtual bool OnInit() {
        // initialize portaudio
        PaError err = Pa_Initialize();
        if (err != paNoError) return false;

        // Add the common image handlers
        wxImage::AddHandler( new wxPNGHandler );
        wxImage::AddHandler( new wxJPEGHandler );

        MainFrame *mainFrame;
        #ifdef __unix__
            if (wxDir::Exists("/usr/local/share/fragment")) {
                mainFrame = new MainFrame(NULL, "/usr/local/share/fragment/");
            } else {
                mainFrame = new MainFrame(NULL, "");
            }
        #else
            mainFrame = new MainFrame(NULL, "");
        #endif
        
        SetTopWindow(mainFrame);
        
        return GetTopWindow()->Show();
    }
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)
