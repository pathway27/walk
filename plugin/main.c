#include <vitasdk.h>
#include <taihen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <inttypes.h>

#include "draw.h"
#include "walk_config.h"

#define NUMBER_OF_PAGES 1
// No matter how much this is, due to lack of getline, we always get more than wanted.
#define LINES_TO_DISPLAY 50
// #define LINES_TO_BUFFER LINES_TO_DISPLAY * NUMBER_OF_PAGES
#define MAX_CHARS_PER_LINE 79
#define BUFFER_SIZE MAX_CHARS_PER_LINE * LINES_TO_DISPLAY * NUMBER_OF_PAGES
#define WALK_PATH "ux0:/data/walk"

#define HOOKS_NUM 1
static SceUID g_hooks[HOOKS_NUM];
static tai_hook_ref_t g_hookrefs[HOOKS_NUM];

static unsigned int g_walk_visible = 0;
// static Walk[] walks[];
static unsigned int g_buttons_pressed = 0;
static SceUInt32 g_time_now = 0;

static int g_scroll_index = 0;
static char * g_walkthrough_read_buffer;
static char * g_walkthrough_lines[790]; // ??

static SceUID g_fd_walkthrough;
static SceUID g_fd_database;

static SceOff g_walkthrough_size = 0;

static char g_appTitleID[12] = {0};

static void seekPage(int new_scroll_index) {
    int page_number = (new_scroll_index + (LINES_TO_DISPLAY - 1)) / (LINES_TO_DISPLAY);
    if (page_number >= 1)
        page_number--;

    g_scroll_index = new_scroll_index;

    // dont seek new page in file if we dont need to
    // if (!(new_scroll_index == g_scroll_index) &&
    //     page_number == old_offset)
    //     return;

    g_walkthrough_read_buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);

    sceIoPread(g_fd_walkthrough, g_walkthrough_read_buffer, BUFFER_SIZE, page_number * BUFFER_SIZE);
    g_walkthrough_lines[0] = strtok(g_walkthrough_read_buffer, "\n");
    int i = 0;
    while (g_walkthrough_lines[i] != NULL) {
        i++;
        g_walkthrough_lines[i] = strtok(NULL, "\n");
    }

    free(g_walkthrough_read_buffer);
}

static void updatePosition() {
    g_time_now = sceKernelGetProcessTimeLow();
    char buf[10];
    sprintf(buf, "%i", g_scroll_index);;
    sceIoLseek(g_fd_database, 0, SCE_SEEK_SET);
    sceIoWrite(g_fd_database, buf, sizeof(g_scroll_index));
}

static void checkButtons() {
    SceCtrlData ctrl;
    sceCtrlPeekBufferPositive(0, &ctrl, 1);
    unsigned int buttons = ctrl.buttons & ~g_buttons_pressed;

    if (ctrl.buttons & SCE_CTRL_SELECT) {
        // Toggle menu
        if (buttons & SCE_CTRL_RTRIGGER) {
            g_walk_visible = !g_walk_visible;
        }
    } else if (g_walk_visible) {
        if (g_fd_walkthrough == NULL || g_fd_database == NULL)
            return;

        if (buttons & SCE_CTRL_UP) {
            int new_value = g_scroll_index - 1;

            if (new_value >= 0) {
                seekPage(new_value);
            }

            updatePosition();
        } else if (buttons & SCE_CTRL_DOWN) {
            int new_value = g_scroll_index + 1;
            if (new_value < 10000) { // TODO: END OF FILE
                seekPage(new_value);
            }

            updatePosition();
        } else if (buttons & SCE_CTRL_LEFT) {
            int new_value = g_scroll_index - 27;
            if (new_value <= 0)
                g_scroll_index = 0;
            else {
                seekPage(new_value);
            }

            updatePosition();
        } else if (buttons & SCE_CTRL_RIGHT) {
            int new_value = g_scroll_index + 27;
            if (new_value >= 10000) // TOOD: END OF FILE
                g_scroll_index = 100;
            else {
                seekPage(new_value);
            }

            updatePosition();
        }
    }
    g_buttons_pressed = ctrl.buttons;
}

static int sceDisplaySetFrameBuf_patched(const SceDisplayFrameBuf *pParam, int sync) {
    // Check for pressed buttons
    checkButtons();

    if (!g_walk_visible)
        goto SKIP;

    osdUpdateFrameBuf(pParam);

    // BG
    osdSetBgColor(0, 0, 0, 225);
    osdFastDrawRectangle(0, 0, pParam->width, pParam->height);

    // Watermark
    osdSetBgColor(0, 0, 0, 0);
    osdSetTextColor(255, 255, 255, 255);

    osdSetTextScale(1);
#ifdef DEBUG
    osdDrawStringF(0, 0, "TITLEID %s    Scroll %i    LastTime %" PRId64 "    Page Number: %i Size %" PRIu64, g_appTitleID, g_scroll_index, g_time_now, (g_scroll_index + (LINES_TO_DISPLAY - 2)) / (LINES_TO_DISPLAY - 1), g_walkthrough_size);
#else
    osdDrawStringF(0, 0, "Walk v%s", GIT_VERSION);
#endif
    osdDrawStringF(0, 10, "%s/%s/walk.txt", WALK_PATH, g_appTitleID);

    if (g_fd_walkthrough == NULL || g_fd_database == NULL) {
        if (g_fd_database == NULL) {
            osdDrawStringF(200, 200, "Failed to Open Database File at %s/%s/last_position.txt", WALK_PATH, g_appTitleID);
            osdDrawStringF(200, 210, "Error %i", g_fd_database);
        }

        if (g_fd_walkthrough == NULL) {
            osdDrawStringF(200, 300, "Failed to Open File at %s/%s/walk.txt", WALK_PATH, g_appTitleID);
            osdDrawStringF(200, 310, "Error %i", g_fd_walkthrough);
        }

        goto SKIP;
    }

    for (int i = 0; i <= LINES_TO_DISPLAY; i++) {
        int ringed_index;

        if (g_scroll_index <= (LINES_TO_DISPLAY + 1))
            ringed_index = i + (g_scroll_index % ((LINES_TO_DISPLAY * NUMBER_OF_PAGES) + 1));
        else
            ringed_index = i + ((g_scroll_index - 1) % (LINES_TO_DISPLAY * NUMBER_OF_PAGES));
        
#ifdef DEBUG
        osdDrawStringF(0, 30 + (i * 10), "i %i", i);
        osdDrawStringF(50, 30 + (i * 10), "ringed_index %i", ringed_index);
#endif

        if (g_walkthrough_lines[ringed_index] == NULL)
            break;

        osdDrawStringF(190, 30 + (i * 10), "%s", g_walkthrough_lines[ringed_index]);
    }

    sceDisplayWaitSetFrameBufMulti(4);
SKIP:
    return TAI_CONTINUE(int, g_hookrefs[0], pParam, sync);
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args) {
    g_hooks[0] = taiHookFunctionImport(
                    &g_hookrefs[0],
                    TAI_MAIN_MODULE,
                    TAI_ANY_LIBRARY,
                    0x7A410B64, // sceDisplaySetFrameBuf
                    sceDisplaySetFrameBuf_patched);

    // Load TITLEID
    sceAppMgrAppParamGetString(0, 12, g_appTitleID, 12);

    char path[50];

    // Make TITLEID Folder If it doesn't exist
    sprintf(path, "%s/%s", WALK_PATH, g_appTitleID);
    SceIoStat stat;
    memset(&stat, 0, sizeof(SceIoStat));
    if (sceIoGetstat(path, &stat) < 0) {
        sceIoMkdir(path, SCE_S_IFDIR);
    }

    // Read last recorded position
    sprintf(path, "%s/%s/last_position.txt", WALK_PATH, g_appTitleID);
    g_fd_database = sceIoOpen(path, SCE_O_RDWR | SCE_O_CREAT, 0777);

    char buf[50];
    sceIoRead(g_fd_database, buf, sizeof(int));
    g_scroll_index = atoi(buf);

    // Load the walkthrough
    sprintf(path, "%s/%s/walk.txt", WALK_PATH, g_appTitleID);
    g_fd_walkthrough = sceIoOpen(path, SCE_O_RDONLY, 0755);
    if (sceIoGetstat(path, &stat) == 0) {
        g_walkthrough_size = stat.st_size;
        seekPage(g_scroll_index);
    }

    return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {
    // Release all hooks
    for (int i = 0; i < HOOKS_NUM; i++) {
        if (g_hooks[i] >= 0)
            taiHookRelease(g_hooks[i], g_hookrefs[i]);
    }

    sceIoClose(g_fd_walkthrough);
    sceIoClose(g_fd_database);

    return SCE_KERNEL_STOP_SUCCESS;
}