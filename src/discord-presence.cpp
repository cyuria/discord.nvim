#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
  #define Sleep(ms) sleep(ms)
#endif

#include <cstring>
#include <ctime>

#include <fstream>
#include <thread>
#include <string>
#include <unordered_map>
#include <utility>

#include "discord_rpc.h"

using namespace std;

const pair<string, string> LANG_DEFAULTS = make_pair("", "");

class PresenceApp {
  
public:
  
  const char* APPLICATION_ID = "1076552220939665520";
  
  unordered_map<string, pair<string, string>> langmap;
  
  string projectname, filename, fileext;
  unsigned int currentfile, openfiles;
  long int StartTime;
  bool shouldUpdate;
  int initlangmapsize;
  
  std::thread updateThread;
  
  PresenceApp() {
    shouldUpdate = false;
    StartTime = time(0);
  }
  ~PresenceApp() {
    shouldUpdate = false;
    Discord_Shutdown();
  }
  
  void configLangMapping(const string fname) {
    ifstream f;
    f.open(fname);
    if (!f.is_open()) return;
    string line;
    initlangmapsize = 0;
    while (getline(f, line)) {
      const auto fext = line.find_last_of('=');
      const auto imgsep = line.substr(fext + 1).find_last_of('|');
      langmap[line.substr(0, fext)] = make_pair(line.substr(fext + 1, imgsep), line.substr(fext + imgsep + 2));
      initlangmapsize++;
    }
  }
  
  static string getExtension(string filename) {
    return filename.substr(filename.find_last_of('.') + 1);
  }
  
  void updateDiscordPresence() {
    const auto search = langmap.find(fileext);
    const auto language = (search != langmap.end() ? search->second : LANG_DEFAULTS);
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.details = projectname.c_str();
    discordPresence.state = ("Editing " + filename + " (" + language.first + ")").c_str();
    discordPresence.startTimestamp = StartTime;
    discordPresence.largeImageKey = "neovim";
    discordPresence.largeImageText = "The One True Text Editor";
    discordPresence.smallImageText = language.first.c_str();
    discordPresence.smallImageKey = language.second.c_str();
    discordPresence.partySize = (int) currentfile;
    discordPresence.partyMax = (int) openfiles;
    discordPresence.instance = 0;
    Discord_UpdatePresence(&discordPresence);
  }
  
  static void handleDiscordReady(const DiscordUser* connectedUser) {
    /*
    cout << "Discord: connected to user "
         << connectedUser->username << '#' << connectedUser->discriminator
         << " - "
         << connectedUser->userId
         << '\n';
    */
  }
  
  static void handleDiscordDisconnected(int errcode, const char* message) {
    /*
    cout << "Discord: disconnected ("
         << errcode << ": "
         << message << ")\n";
    */
  }
  
  static void handleDiscordError(int errcode, const char* message) {
    /*
    cout << "Discord: error ("
         << errcode << ": "
         << message << ")\n";
    */
  }
  
  void discordInit() {
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = handleDiscordReady;
    handlers.disconnected = handleDiscordDisconnected;
    handlers.errored = handleDiscordError;
    Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);
    shouldUpdate = true;
    updateThread = thread([this]() {
      while (this->shouldUpdate) {
        Sleep(500);
        this->update();
        this->updateDiscordPresence();
      }
    });
  }
  
  void discordShutDown() {
    shouldUpdate = false;
    updateThread.join();
    Discord_Shutdown();
  }
  
  void update() {
    #ifdef DISCORD_DISABLE_IO_THREAD
      Discord_UpdateConnection();
    #endif
    updateDiscordPresence();
    Discord_RunCallbacks();
  }
  
  void setProject(const string pname, const string fname) {
    filename = fname;
    fileext = getExtension(fname);
    if (projectname == pname) return;
    projectname = pname;
    StartTime = time(0);
  }
  
  void setFileName(const string fname) {
    filename = fname;
    fileext = getExtension(fname);
  }
  
  void updateFileNums(const unsigned int currfile, const unsigned int allfiles) {
    currentfile = currfile;
    openfiles = allfiles;
  }

  void setFileExt(const string ext) {
    fileext = ext;
  }
  
};

PresenceApp app;

extern "C" {
  
  void discordInit() {
    return app.discordInit();
  }
  
  void configMappings(const char* fname) {
    app.configLangMapping(fname);
  }
  
  void discordShutDown() {
    return app.discordShutDown();
  }
  
  void discordSetFolder(const char* pname, const char* fname) {
    return app.setProject(pname, fname);
  }
  
  void discordFileNums(const unsigned int currfile, const unsigned int allfiles) {
    return app.updateFileNums(currfile, allfiles);
  }
  
  void discordSetFile(const char* fname) {
    return app.setFileName(fname);
  }

  void discordSetExt(const char* ext) {
    return app.setFileExt(ext);
  }
  
}
