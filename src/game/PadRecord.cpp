// PadRecord.cpp: controller "shadowing" (0x802301E0-0x8023040C).
// Debug feature that records the frame time and all four pads to RecordedFrameDataNNN.log, or plays
// them back from PlaybackFrameData.log.
// Names are descriptive guesses; the code is matched against the retail DOL.

extern "C" {
void* fopen(const char* name, const char* mode);
int fclose(void* file);
unsigned long fread(void* ptr, unsigned long size, unsigned long count, void* file);
unsigned long fwrite(const void* ptr, unsigned long size, unsigned long count, void* file);
int sprintf(char* s, const char* format, ...);
}
#include <dolphin/pad.h>

void DebugPrintf(const char* format, ...);   // 0x8000781C
void ScreenPrintf(const char* format, ...);  // 0x801EA9F0
void PadShadow(int mode, PADStatus* pads);   // 0x80005800

enum { PADREC_OFF, PADREC_RECORD, PADREC_PLAYBACK };

struct PadFrame {
    float time;
    PADStatus pads[4];
};

char* gPadRecordName = "RecordedFrameData%03d.log";
int gPadRecordCount = -1;
char* gPadPlaybackName = "PlaybackFrameData.log";
char* gPadRecordMsg = "Recording to file: RecordedFrameData%03d.log\n";

static PadFrame sPadFrame;
static void* sPadFile;
static int sPadMode;
static int sPadFrameNum;
static int sPadUnused;

void PadRecordStop(void) {
    if (sPadMode == PADREC_RECORD || sPadMode == PADREC_PLAYBACK) {
        PadShadow(0, 0);
        fclose(sPadFile);
    }
}

void PadRecordFrame(float* time) {
    if (time) {
        if (sPadMode == PADREC_RECORD) {
            ScreenPrintf(gPadRecordMsg, gPadRecordCount);
            sPadFrame.time = *time;
            PADRead(sPadFrame.pads);
            fwrite(&sPadFrame, sizeof(PadFrame), 1, sPadFile);
        } else if (sPadMode == PADREC_PLAYBACK) {
            fread(&sPadFrame, sizeof(PadFrame), 1, sPadFile);
            *time = sPadFrame.time;
            ScreenPrintf("Playback Frame %d\n", sPadFrameNum);
            sPadFrameNum++;
        }
    }
}

void PadRecordStart(int mode) {
    char name[128];

    if (mode == PADREC_RECORD) {
        DebugPrintf("Controller Shadowing: RECORD\n");
        sprintf(name, gPadRecordName, ++gPadRecordCount);
        sPadFile = fopen(name, "wb");
        if (sPadFile) {
            sPadMode = PADREC_RECORD;
            sPadUnused = 0;
        } else {
            DebugPrintf("    Can't open log file for recording\n");
        }
    } else if (mode == PADREC_PLAYBACK) {
        DebugPrintf("Controller Shadowing: PLAYBACK\n");
        sPadFile = fopen(gPadPlaybackName, "rb");
        if (sPadFile) {
            sPadMode = PADREC_PLAYBACK;
            sPadUnused = 0;
        } else {
            DebugPrintf("    Can't open log file for playback\n");
        }
    }
    PadShadow(sPadMode, sPadFrame.pads);
    sPadFrameNum = 0;
}
