#include "pch.h"
#include "cApp.h"
#include "config.h"


cMain* ref; // I am to dumb for all this,
            // Don't do this but yeah...


wxIMPLEMENT_APP(cApp);

cApp::cApp() {

}

cApp::~cApp() {

}
bool cApp::OnInit() {

    char* shut_up_VS = _getcwd(working_dir, FILENAME_MAX);
    config cfg;
    if (cfg.loadConfig()) { //if error
        std::cout << "can't find file\n";
        cfg.saveConfig();
    }

	m_frame1 = new cMain();
	ref = m_frame1;
	m_frame1->Show();
	return true;
}

