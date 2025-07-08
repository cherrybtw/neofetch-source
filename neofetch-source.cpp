#include "tier1/convar.h"
#include "proto_version.h"
#include "tier0/platform.h"
#include "sys_dll.h"
#include "Color.h"
#include "host.h"
#include "ivideomode.h"

struct Colors {
    int r, g, b, a;
};
struct MemoryUsageInfo
{
    float totalMB;
    float availableMB;
    float usedMB;
    float usedPercent;
};
enum ColorIndex {
    BLACK = 0,
    DARK_RED,
    DARK_GREEN,
    DARK_YELLOW,
    DARK_BLUE,
    DARK_MAGENTA,
    DARK_CYAN,
    LIGHT_GRAY,
    DARK_GRAY,
    LIGHT_RED,
    LIGHT_GREEN,
    LIGHT_YELLOW,
    LIGHT_BLUE,
    LIGHT_MAGENTA,
    LIGHT_CYAN,
    WHITE,
    ORANGE,       
    INDIGO,       
    VIOLET,      
    PINK,        
    LIGHT_YELLOWISH, 
    TRANSPARENT
};

Color colors[] = {
    Color(0, 0, 0, 255),         // BLACK 0
    Color(128, 0, 0, 255),       // DARK_RED 1
    Color(0, 128, 0, 255),       // DARK_GREEN 2
    Color(128, 128, 0, 255),     // DARK_YELLOW 3
    Color(0, 0, 128, 255),       // DARK_BLUE 4
    Color(128, 0, 128, 255),     // DARK_MAGENTA 5
    Color(0, 128, 128, 255),     // DARK_CYAN 6
    Color(192, 192, 192, 255),   // LIGHT_GRAY 7
    Color(128, 128, 128, 255),   // DARK_GRAY 8
    Color(255, 0, 0, 255),       // LIGHT_RED 9
    Color(0, 255, 0, 255),       // LIGHT_GREEN 10
    Color(255, 255, 0, 255),     // LIGHT_YELLOW 11
    Color(0, 0, 255, 255),       // LIGHT_BLUE 12
    Color(255, 0, 255, 255),     // LIGHT_MAGENTA 13
    Color(0, 255, 255, 255),     // LIGHT_CYAN 14
    Color(255, 255, 255, 255),   // WHITE 15

    Color(255, 165, 0, 255),     // ORANGE 16
    Color(75, 0, 130, 255),      // INDIGO 17
    Color(238, 130, 238, 255),   // VIOLET 18
    Color(255, 192, 203, 255),   // PINK 19
    Color(255, 255, 224, 255),   // LIGHT_YELLOWISH 20
    Color(0, 0, 0, 0),           // TRANSPARENT 21
};
Color GetColorForChar(char c)
{
    switch (c)
    {
    case '@': return colors[TRANSPARENT];
    case '#': return colors[LIGHT_GRAY];
    case 'B': return colors[WHITE];
    case 'G': return colors[LIGHT_GRAY];
    case '5': return colors[LIGHT_GRAY];
    case 'Y': return colors[WHITE];
    case '?': return colors[LIGHT_CYAN];
    case '!': return colors[DARK_YELLOW];
    case '7': return colors[LIGHT_YELLOW];
    case 'J': return colors[LIGHT_MAGENTA];
    case 'P': return colors[LIGHT_GRAY];
    case '&': return colors[WHITE];
    case ' ': return colors[BLACK];
    default:  return colors[WHITE];
    }
}
const char* sourceLogo[] = {
    "@@@@@@&#BBGGGGGGGGGGGGGGGGGGGGGGGGG#&@@@",
    "@&BPY?77!!!!!!!!!!!!!!!!!!!!!!!!!!!7?5#@",
    "Y7!!!7?JY55555YJ?77!!!77777777777777!!7G",
    "GJJ5B&@@@@@@@@@@@&#PY7!!77777777777777!?",
    "@@@@@@@&&&&&&&@@@@@@&GJ!!777777777777!Y@",
    "@@@@#GP555555555PG#@@@@@B?!7777777777!J&",
    "@@&PYYY555PPPP5YYYY5#@@@@@5!77777777!?&@",
    "@@PY5555B@@@@@&P55555#@@@@@5!777777!7#@@",
    "@@PY5555PB&&@@@@&&&&&&@@@@@@J!7777!7B@@@",
    "@@&P5YY5YY555PPGB#&@@@@@@@@@G!7777!P@@@@",
    "@@@@#BGP555YYYYYYY55G&@@@@@@#7777!5@@@@@",
    "@@@@@@@@&&#BBP55555YP@@@@@@B!77!Y@@@@@@@",
    "@#PPPPPB@@@@@@@&555555&@@@@@Y!7!J@@@@@@@",
    "@@PYYYY5G#&&&&#G5555YG@@@@@B77!?&@@@@@@@",
    "@@@B55YYYY5555YYYY5PB@@@@@B7!!7#@@@@@@@@",
    "@@@@@#BGPPPPPPPPGB#@@@@@&57!!?#@@@@@@@@@",
    "@@@@@@@@@@@@@@@@@@@@@@&P?!!!Y&@@@@@@@@@@",
    "@@@@@@@@@@@@@@@@@@@@@@P!!7JB@@@@@@@@@@@@",
    "@@@@@@@@@@@@@@@@@@@@@@@BGB@@@@@@@@@@@@@@",
};
MemoryUsageInfo GetMemoryUsage()
{
    MemoryUsageInfo info = {};

    int64_t total = 0;
    int64_t available = 0;

    char meminfo[8196] = { 0 };
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f)
        return info;

    size_t size = fread(meminfo, 1, sizeof(meminfo) - 1, f);
    fclose(f);

    if (!size)
        return info;

    char *s_total = strstr(meminfo, "MemTotal:");
    char *s_available = strstr(meminfo, "MemAvailable:");

    if (s_total)     sscanf(s_total + 9, "%lld", &total);
    if (s_available) sscanf(s_available + 13, "%lld", &available);

    float totalMB = total / 1024.0f / 1024.0f;
    float availableMB = available / 1024.0f / 1024.0f;
    float usedMB = totalMB - availableMB;
    float usedPercent = (usedMB / totalMB) * 100.0f;

    info.totalMB = totalMB;
    info.availableMB = availableMB;
    info.usedMB = usedMB;
    info.usedPercent = usedPercent;

    return info;
}
size_t GetGameMemoryUsage()
{
    FILE* f = fopen("/proc/self/status", "r");
    if (!f)
        return 0;

    char line[256];
    size_t memory_kb = 0;

    while (fgets(line, sizeof(line), f))
    {
        if (strncmp(line, "VmRSS:", 6) == 0)
        {
            sscanf(line + 6, "%zu", &memory_kb);
            break;
        }
    }

    fclose(f);
    return memory_kb / 1024;
}
ConVar sourcefetch_color(
    "sourcefetch_color",
    "10",
    FCVAR_ARCHIVE | FCVAR_NOTIFY,
    "Choose color output for sourcefetch command\n"
    "List of supported colors:\n"
    " 1 = Black\n"
    " 2 = Red\n"
    " 3 = Green\n"
    " 4 = Yellow / Brown\n"
    " 5 = Blue\n"
    " 6 = Magenta\n"
    " 7 = Cyan\n"
    " 8 = Light Gray\n"
    " 9 = Dark Gray\n"
    "10 = Light Red\n"
    "11 = Light Green\n"
    "12 = Light Yellow\n"
    "13 = Light Blue\n"
    "14 = Light Magenta\n"
    "15 = Light Cyan\n"
    "16 = White\n"
    "17 = Orange\n"
    "18 = Indigo\n"
    "19 = Violet\n"
    "20 = Pink\n"
    "21 = Light Yellowish\n",
    true, 1, true, 21
);

int GCD(int a, int b)//for calculating greatest common divisor
{
    while (b != 0)
    {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

void PrintASCIIArt()
{
    for (int i = 0; i < ARRAYSIZE(sourceLogo); ++i)
    {
        const char* line = sourceLogo[i];
        for (int j = 0; line[j] != '\0'; ++j)
        {
            Color c = GetColorForChar(line[j]);
            ConColorMsg(c, "%c", line[j]);
        }
        Msg("\n");
    }
    Msg("\n");
}

CON_COMMAND(sourcefetch, "Display system information using neofetch")
{
    ConVarRef name("name");
    ConVarRef cl_language("cl_language");
    const CPUInformation &ci = *GetCPUInformation();
    MemoryUsageInfo memInfo = GetMemoryUsage();
    int w = videomode->GetModeWidth();
    int h = videomode->GetModeHeight();
    Color colorfetch = Color(colors[sourcefetch_color.GetInt() - 1]);

    PrintASCIIArt();

    char usermodinfo[512];
    Q_snprintf(usermodinfo, sizeof(usermodinfo), "%s@%s", name.GetString(), GetCurrentMod());
    ConColorMsg(colorfetch, "%s\n", usermodinfo);
    for (int i = 0; i < Q_strlen(usermodinfo); ++i)
    {
        Msg("--");
    }
    Msg("\n");
    ConColorMsg(colorfetch, "Source Engine: ");
        Msg("%i build %s, commit %s, build from %s %s\n", PROTOCOL_VERSION, GetSteamInfIDVersionInfo().szVersionString, GIT_COMMIT_HASH, __DATE__, __TIME__);
    ConColorMsg(colorfetch, "CPU: ");
        Msg("%s (%d/%d) @ %.2f GHz\n", ci.m_szProcessorID, ci.m_nPhysicalProcessors, ci.m_nLogicalProcessors, ci.m_Speed / 1e9);
    ConColorMsg(colorfetch, "Memory: ");
        Msg("%.2f/%.2f GBytes(%.2f%%)\n", memInfo.usedMB, memInfo.totalMB, memInfo.usedPercent);
    ConColorMsg(colorfetch, "Screen resolution: ");
        Msg("%ix%i (%ix%i), %s mode\n", w, h, w/GCD(w, h), h/GCD(w, h), 
            videomode->IsWindowedMode() ? "Windowed" : "Fullscreen");
    ConColorMsg(colorfetch, "Language: ");
        Msg("%s\n", cl_language.GetString());
    ConColorMsg(colorfetch, "Allocated by engine: ");
        Msg("%zu MB\n", GetGameMemoryUsage());

    for (int i = 0; i < 16; ++i)
    {
        ConColorMsg(colors[i], "[x] ");
        if ((i + 1) % 8 == 0)
            Msg("\n");
    }
}