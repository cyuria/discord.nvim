#define _CRT_SECURE_NO_WARNINGS /* thanks Microsoft */

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <thread>

#include "discord_rpc.h"
#include "langconfig.h"

using namespace std;

class PresenceApp {
  
public:
  
  const char* APPLICATION_ID = "1076552220939665520";
  
  string projectname, filename, fileext;
  unsigned int currentfile, openfiles;
  int64_t StartTime;
  bool shouldUpdate;
  
  std::thread updateThread;
  
  PresenceApp() {
    shouldUpdate = false;
    StartTime = time(0);
  }
  ~PresenceApp() {
    shouldUpdate = false;
    Discord_Shutdown();
  }
  
  static string getExtension(string filename) {
    return filename.substr(filename.find_last_of('.') + 1);
  }
  
  void updateDiscordPresence() {
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.state = ("Editing " + filename).c_str();
    discordPresence.details = projectname.c_str();
    discordPresence.startTimestamp = StartTime;
    discordPresence.largeImageKey = "neovim";
    discordPresence.largeImageText = "The One True Text Editor";
    auto search = langmap.find(fileext);
    auto language = (search != langmap.end() ? search->second : LANG_DEFAULTS);
    discordPresence.smallImageText = language.first.c_str();
    discordPresence.smallImageKey = language.second.c_str();
    discordPresence.partySize = currentfile;
    discordPresence.partyMax = openfiles;
    discordPresence.instance = 0;
    Discord_UpdatePresence(&discordPresence);
  }
  
  static void handleDiscordReady(const DiscordUser* connectedUser) {
    /*cout << "Discord: connected to user "
         << connectedUser->username << '#' << connectedUser->discriminator
         << " - "
         << connectedUser->userId
         << '\n';*/
  }
  
  static void handleDiscordDisconnected(int errcode, const char* message) {
    /*cout << "Discord: disconnected ("
         << errcode << ": "
         << message << ")\n";*/
  }
  
  static void handleDiscordError(int errcode, const char* message) {/*
    cout << "Discord: error ("
         << errcode << ": "
         << message << ")\n";*/
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
        Sleep(333);
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

static PresenceApp app;

extern "C" {

  void discordInit() {
    return app.discordInit();
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
