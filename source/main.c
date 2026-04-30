#include <switch.h>
#include <stdio.h>
#include <string.h>

// ================= MENU =================
#define MENU_COUNT 4
const char* menuItems[MENU_COUNT] = {
    "Remap All Buttons",
    "Set Defaults For Joycon",
    "Set Defaults For Gamecube Adapter",
    "Exit"
};

// ================= HEADER =================
// Contient la configuration de base des sticks[cite: 1]
const char* header =
"[GCPad1]\n"
"Device = Switch/0/Joypad\n"
"Buttons/L = ``\n"
"Buttons/R = ``\n"
"Main Stick/Up = `Y0+`\n"
"Main Stick/Down = `Y0-`\n"
"Main Stick/Left = `X0-`\n"
"Main Stick/Right = `X0+`\n"
"Main Stick/Modifier = \n"
"Main Stick/Calibration = 100.00 141.42 100.00 141.42 100.00 141.42 100.00 141.42\n"
"C-Stick/Up = if(toggle(`Select`), `Y1-`, `Y1+`)\n"
"C-Stick/Down = if(toggle(`Select`), `Y1+`, `Y1-`)\n"
"C-Stick/Left = `X1-`\n"
"C-Stick/Right = `X1+`\n"
"C-Stick/Modifier = \n"
"C-Stick/Calibration = 100.00 141.42 100.00 141.42 100.00 141.42 100.00 141.42\n"
"Triggers/L-Analog = ``\n"
"Triggers/R-Analog = ``\n\n";

// ================= INPUT HELPERS =================
// Mis à jour : Détecte TOUS les boutons physiques de la Switch[cite: 1]
const char* getButtonName(u64 kDown)
{
    if (kDown & HidNpadButton_A) return "A";
    if (kDown & HidNpadButton_B) return "B";
    if (kDown & HidNpadButton_X) return "X";
    if (kDown & HidNpadButton_Y) return "Y";
    if (kDown & HidNpadButton_L) return "L";
    if (kDown & HidNpadButton_R) return "R";
    if (kDown & HidNpadButton_ZL) return "ZL";
    if (kDown & HidNpadButton_ZR) return "ZR";
    if (kDown & HidNpadButton_StickL) return "LS";
    if (kDown & HidNpadButton_StickR) return "RS";
    if (kDown & HidNpadButton_Plus) return "Start";
    if (kDown & HidNpadButton_Minus) return "Select";
    if (kDown & HidNpadButton_Up) return "Up";
    if (kDown & HidNpadButton_Down) return "Down";
    if (kDown & HidNpadButton_Left) return "Left";
    if (kDown & HidNpadButton_Right) return "Right";
    return NULL;
}

void waitForRelease(PadState* pad)
{
    while (appletMainLoop())
    {
        padUpdate(pad);
        if (padGetButtons(pad) == 0) return;
        consoleUpdate(NULL);
    }
}

const char* waitForButton(PadState* pad)
{
    waitForRelease(pad);
    while (appletMainLoop())
    {
        padUpdate(pad);
        u64 kDown = padGetButtonsDown(pad);
        const char* name = getButtonName(kDown);
        if (name) return name;
        consoleUpdate(NULL);
    }
    return "";
}

// ================= FILE WRITE =================
void writeConfig(const char* lines[], int count)
{
    FILE* f = fopen("sdmc:/tico/system/gc/User/Config/GCPadNew.ini", "w");
    if (f)
    {
        fprintf(f, "%s", header);
        for (int i = 0; i < count; i++)
        {
            fprintf(f, "%s\n", lines[i]);
        }
        fclose(f);
    }
}

// ================= REMAP =================
typedef struct {
    const char* gc_name;
    const char* value;
} Mapping;

void remapAllButtons()
{
    consoleClear();
    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);

    // Mis à jour : Ajout du D-Pad GameCube à configurer[cite: 1]
    Mapping map[] = {
        {"Buttons/A", ""},
        {"Buttons/B", ""},
        {"Buttons/X", ""},
        {"Buttons/Y", ""},
        {"Buttons/Z", ""},
        {"Buttons/Start", ""},
        {"Triggers/L", ""},
        {"Triggers/R", ""},
        {"D-Pad/Up", ""},
        {"D-Pad/Down", ""},
        {"D-Pad/Left", ""},
        {"D-Pad/Right", ""}
    };

    int count = sizeof(map)/sizeof(map[0]);

    for (int i = 0; i < count; i++)
    {
        consoleClear();
        printf("Configuring Step %d/%d\n", i + 1, count);
        printf("Press a Switch button for: %s\n", map[i].gc_name);
        map[i].value = waitForButton(&pad);
    }

    FILE* f = fopen("sdmc:/tico/system/gc/User/Config/GCPadNew.ini", "w");
    if (f)
    {
        fprintf(f, "%s", header);
        for (int i = 0; i < count; i++)
        {
            fprintf(f, "%s = `%s`\n", map[i].gc_name, map[i].value);
        }
        fclose(f);
    }

    consoleClear();
    printf("Saved! Press + to return\n");
    while (appletMainLoop())
    {
        padUpdate(&pad);
        if (padGetButtonsDown(&pad) & HidNpadButton_Plus) break;
        consoleUpdate(NULL);
    }
}

// ================= DEFAULTS =================
void setDefaultsJoycon()
{
    const char* lines[] = {
        "Buttons/A = `A`","Buttons/B = `B`","Buttons/X = `X`","Buttons/Y = `Y`","Buttons/Z = `R`","Buttons/Start = `Start`","Triggers/L = `ZL`","Triggers/R = `ZR`","D-Pad/Up = `Up`","D-Pad/Down = `Down`","D-Pad/Left = `Left`","D-Pad/Right = `Right`"
    };
    writeConfig(lines, 12);
    printf("Joycon defaults applied.\nPress + to return\n");
}

void setDefaultsGC()
{
    const char* lines[] = {
        "Buttons/A = `A`","Buttons/B = `B`","Buttons/X = `X`","Buttons/Y = `Y`","Buttons/Z = `R2`","Buttons/Start = `Start`","Triggers/L = `L`","Triggers/R = `R`","D-Pad/Up = `Up`","D-Pad/Down = `Down`","D-Pad/Left = `Left`","D-Pad/Right = `Right`"
    };
    writeConfig(lines, 12);
    printf("GC defaults applied.\nPress + to return\n");
}

// ================= MENU & MAIN =================
void drawMenu(int selected)
{
    consoleClear();
    printf("Tico GC Remapper (FULL BUTTONS)\n-----------------\n\n");
    for (int i = 0; i < MENU_COUNT; i++)
    {
        if (i == selected) printf("\x1b[7m> %s\x1b[0m\n", menuItems[i]);
        else printf("  %s\n", menuItems[i]);
    }
}

int main(int argc, char* argv[])
{
    consoleInit(NULL);
    fsInitialize();
    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);

    int selected = 0;
    while (appletMainLoop())
    {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        if (kDown & HidNpadButton_Up) { selected--; if (selected < 0) selected = MENU_COUNT - 1; }
        if (kDown & HidNpadButton_Down) { selected++; if (selected >= MENU_COUNT) selected = 0; }
        if (kDown & HidNpadButton_A)
        {
            waitForRelease(&pad);
            if (selected == 0) remapAllButtons();
            else if (selected == 1) setDefaultsJoycon();
            else if (selected == 2) setDefaultsGC();
            else if (selected == 3) break;
        }
        drawMenu(selected);
        consoleUpdate(NULL);
    }
    fsExit();
    consoleExit(NULL);
    return 0;
}
