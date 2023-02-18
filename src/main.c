//#define _CRT_SECURE_NO_WARNINGS /* thanks Microsoft */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "discord_rpc.h"

static const char* APPLICATION_ID = "1076552220939665520";
static int FrustrationLevel = 0;
static int64_t StartTime;

static void updateDiscordPresence(const char* filename, const char* projectname) {
  DiscordRichPresence discordPresence;
  memset(&discordPresence, 0, sizeof(discordPresence));
  discordPresence.state = filename;
  discordPresence.details = projectname;
  discordPresence.startTimestamp = StartTime;
  discordPresence.endTimestamp = time(0) + 5 * 60;
  discordPresence.largeImageKey = "canary-large";
  discordPresence.smallImageKey = "ptb-small";
  discordPresence.partyId = "party1234";
  discordPresence.partySize = 1;
  discordPresence.partyMax = 6;
  discordPresence.matchSecret = "xyzzy";
  discordPresence.joinSecret = "join";
  discordPresence.spectateSecret = "look";
  discordPresence.instance = 0;
  Discord_UpdatePresence(&discordPresence);
}

static void handleDiscordReady(const DiscordUser* connectedUser) {
    printf("\nDiscord: connected to user %s#%s - %s\n",
           connectedUser->username,
           connectedUser->discriminator,
           connectedUser->userId);
}

static void handleDiscordDisconnected(int errcode, const char* message)
{
    printf("\nDiscord: disconnected (%d: %s)\n", errcode, message);
}

static void handleDiscordError(int errcode, const char* message)
{
    printf("\nDiscord: error (%d: %s)\n", errcode, message);
}

static void handleDiscordJoin(const char* secret)
{
    printf("\nDiscord: join (%s)\n", secret);
}

static void handleDiscordSpectate(const char* secret)
{
    printf("\nDiscord: spectate (%s)\n", secret);
}

static void handleDiscordJoinRequest(const DiscordUser* request) {
  (void) request;
}

static void discordInit() {
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = handleDiscordReady;
    handlers.disconnected = handleDiscordDisconnected;
    handlers.errored = handleDiscordError;
    handlers.joinGame = handleDiscordJoin;
    handlers.spectateGame = handleDiscordSpectate;
    handlers.joinRequest = handleDiscordJoinRequest;
    Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);
}

static void gameLoop() {
  StartTime = time(0);

  int connected = 1;
  char c;
  while (connected) {
    
    c = (char) getchar();

    if (c == 'q')
      connected = 0;

    printf(".");
    
    updateDiscordPresence("somefile.txt", "a real project");
    
    #ifdef DISCORD_DISABLE_IO_THREAD
      Discord_UpdateConnection();
    #endif
      Discord_RunCallbacks();
  }
}

int main(int argc, char* argv[])
{
  (void) argc; (void) argv;
  
  discordInit();

  gameLoop();

  Discord_Shutdown();
  return 0;
}

