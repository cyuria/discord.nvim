#include <fstream>
#include <utility>
#include <string>
#include <unordered_map>

#define umap unordered_map

using namespace std;

const pair<string, string> LANG_DEFAULTS = make_pair("The Sun", "typescript");

static umap<string, pair<string, string>> langmap;

static void configLangMapping() {
  ifstream f;
  f.open("../langmappings.txt");
  if (!f.is_open()) return;
  string line;
  while (getline(f, line)) {
    const auto fext = line.find_last_of('=');
    const auto imgsep = line.substr(fext + 1).find_last_of('|');
    langmap[line.substr(0, fext)] = make_pair(line.substr(fext + 1, imgsep), line.substr(fext + imgsep + 2));
  }
}

