#include <windows.h>
#include <proxy.h>

#pragma region Proxy
struct winmm_dll {
	HMODULE dll;
	FARPROC oCloseDriver;
	FARPROC oDefDriverProc;
	FARPROC oDriverCallback;
	FARPROC oDrvGetModuleHandle;
	FARPROC oGetDriverModuleHandle;
	FARPROC oOpenDriver;
	FARPROC oPlaySound;
	FARPROC oPlaySoundA;
	FARPROC oPlaySoundW;
	FARPROC oSendDriverMessage;
	FARPROC oWOWAppExit;
	FARPROC oauxGetDevCapsA;
	FARPROC oauxGetDevCapsW;
	FARPROC oauxGetNumDevs;
	FARPROC oauxGetVolume;
	FARPROC oauxOutMessage;
	FARPROC oauxSetVolume;
	FARPROC ojoyConfigChanged;
	FARPROC ojoyGetDevCapsA;
	FARPROC ojoyGetDevCapsW;
	FARPROC ojoyGetNumDevs;
	FARPROC ojoyGetPos;
	FARPROC ojoyGetPosEx;
	FARPROC ojoyGetThreshold;
	FARPROC ojoyReleaseCapture;
	FARPROC ojoySetCapture;
	FARPROC ojoySetThreshold;
	FARPROC omciDriverNotify;
	FARPROC omciDriverYield;
	FARPROC omciExecute;
	FARPROC omciFreeCommandResource;
	FARPROC omciGetCreatorTask;
	FARPROC omciGetDeviceIDA;
	FARPROC omciGetDeviceIDFromElementIDA;
	FARPROC omciGetDeviceIDFromElementIDW;
	FARPROC omciGetDeviceIDW;
	FARPROC omciGetDriverData;
	FARPROC omciGetErrorStringA;
	FARPROC omciGetErrorStringW;
	FARPROC omciGetYieldProc;
	FARPROC omciLoadCommandResource;
	FARPROC omciSendCommandA;
	FARPROC omciSendCommandW;
	FARPROC omciSendStringA;
	FARPROC omciSendStringW;
	FARPROC omciSetDriverData;
	FARPROC omciSetYieldProc;
	FARPROC omidiConnect;
	FARPROC omidiDisconnect;
	FARPROC omidiInAddBuffer;
	FARPROC omidiInClose;
	FARPROC omidiInGetDevCapsA;
	FARPROC omidiInGetDevCapsW;
	FARPROC omidiInGetErrorTextA;
	FARPROC omidiInGetErrorTextW;
	FARPROC omidiInGetID;
	FARPROC omidiInGetNumDevs;
	FARPROC omidiInMessage;
	FARPROC omidiInOpen;
	FARPROC omidiInPrepareHeader;
	FARPROC omidiInReset;
	FARPROC omidiInStart;
	FARPROC omidiInStop;
	FARPROC omidiInUnprepareHeader;
	FARPROC omidiOutCacheDrumPatches;
	FARPROC omidiOutCachePatches;
	FARPROC omidiOutClose;
	FARPROC omidiOutGetDevCapsA;
	FARPROC omidiOutGetDevCapsW;
	FARPROC omidiOutGetErrorTextA;
	FARPROC omidiOutGetErrorTextW;
	FARPROC omidiOutGetID;
	FARPROC omidiOutGetNumDevs;
	FARPROC omidiOutGetVolume;
	FARPROC omidiOutLongMsg;
	FARPROC omidiOutMessage;
	FARPROC omidiOutOpen;
	FARPROC omidiOutPrepareHeader;
	FARPROC omidiOutReset;
	FARPROC omidiOutSetVolume;
	FARPROC omidiOutShortMsg;
	FARPROC omidiOutUnprepareHeader;
	FARPROC omidiStreamClose;
	FARPROC omidiStreamOpen;
	FARPROC omidiStreamOut;
	FARPROC omidiStreamPause;
	FARPROC omidiStreamPosition;
	FARPROC omidiStreamProperty;
	FARPROC omidiStreamRestart;
	FARPROC omidiStreamStop;
	FARPROC omixerClose;
	FARPROC omixerGetControlDetailsA;
	FARPROC omixerGetControlDetailsW;
	FARPROC omixerGetDevCapsA;
	FARPROC omixerGetDevCapsW;
	FARPROC omixerGetID;
	FARPROC omixerGetLineControlsA;
	FARPROC omixerGetLineControlsW;
	FARPROC omixerGetLineInfoA;
	FARPROC omixerGetLineInfoW;
	FARPROC omixerGetNumDevs;
	FARPROC omixerMessage;
	FARPROC omixerOpen;
	FARPROC omixerSetControlDetails;
	FARPROC ommDrvInstall;
	FARPROC ommGetCurrentTask;
	FARPROC ommTaskBlock;
	FARPROC ommTaskCreate;
	FARPROC ommTaskSignal;
	FARPROC ommTaskYield;
	FARPROC ommioAdvance;
	FARPROC ommioAscend;
	FARPROC ommioClose;
	FARPROC ommioCreateChunk;
	FARPROC ommioDescend;
	FARPROC ommioFlush;
	FARPROC ommioGetInfo;
	FARPROC ommioInstallIOProcA;
	FARPROC ommioInstallIOProcW;
	FARPROC ommioOpenA;
	FARPROC ommioOpenW;
	FARPROC ommioRead;
	FARPROC ommioRenameA;
	FARPROC ommioRenameW;
	FARPROC ommioSeek;
	FARPROC ommioSendMessage;
	FARPROC ommioSetBuffer;
	FARPROC ommioSetInfo;
	FARPROC ommioStringToFOURCCA;
	FARPROC ommioStringToFOURCCW;
	FARPROC ommioWrite;
	FARPROC ommsystemGetVersion;
	FARPROC osndPlaySoundA;
	FARPROC osndPlaySoundW;
	FARPROC otimeBeginPeriod;
	FARPROC otimeEndPeriod;
	FARPROC otimeGetDevCaps;
	FARPROC otimeGetSystemTime;
	FARPROC otimeGetTime;
	FARPROC otimeKillEvent;
	FARPROC otimeSetEvent;
	FARPROC owaveInAddBuffer;
	FARPROC owaveInClose;
	FARPROC owaveInGetDevCapsA;
	FARPROC owaveInGetDevCapsW;
	FARPROC owaveInGetErrorTextA;
	FARPROC owaveInGetErrorTextW;
	FARPROC owaveInGetID;
	FARPROC owaveInGetNumDevs;
	FARPROC owaveInGetPosition;
	FARPROC owaveInMessage;
	FARPROC owaveInOpen;
	FARPROC owaveInPrepareHeader;
	FARPROC owaveInReset;
	FARPROC owaveInStart;
	FARPROC owaveInStop;
	FARPROC owaveInUnprepareHeader;
	FARPROC owaveOutBreakLoop;
	FARPROC owaveOutClose;
	FARPROC owaveOutGetDevCapsA;
	FARPROC owaveOutGetDevCapsW;
	FARPROC owaveOutGetErrorTextA;
	FARPROC owaveOutGetErrorTextW;
	FARPROC owaveOutGetID;
	FARPROC owaveOutGetNumDevs;
	FARPROC owaveOutGetPitch;
	FARPROC owaveOutGetPlaybackRate;
	FARPROC owaveOutGetPosition;
	FARPROC owaveOutGetVolume;
	FARPROC owaveOutMessage;
	FARPROC owaveOutOpen;
	FARPROC owaveOutPause;
	FARPROC owaveOutPrepareHeader;
	FARPROC owaveOutReset;
	FARPROC owaveOutRestart;
	FARPROC owaveOutSetPitch;
	FARPROC owaveOutSetPlaybackRate;
	FARPROC owaveOutSetVolume;
	FARPROC owaveOutUnprepareHeader;
	FARPROC owaveOutWrite;
} winmm;

FARPROC PA = 0;
int runASM();

void fCloseDriver() { PA = winmm.oCloseDriver; runASM(); }
void fDefDriverProc() { PA = winmm.oDefDriverProc; runASM(); }
void fDriverCallback() { PA = winmm.oDriverCallback; runASM(); }
void fDrvGetModuleHandle() { PA = winmm.oDrvGetModuleHandle; runASM(); }
void fGetDriverModuleHandle() { PA = winmm.oGetDriverModuleHandle; runASM(); }
void fOpenDriver() { PA = winmm.oOpenDriver; runASM(); }
void fPlaySound() { PA = winmm.oPlaySound; runASM(); }
void fPlaySoundA() { PA = winmm.oPlaySoundA; runASM(); }
void fPlaySoundW() { PA = winmm.oPlaySoundW; runASM(); }
void fSendDriverMessage() { PA = winmm.oSendDriverMessage; runASM(); }
void fWOWAppExit() { PA = winmm.oWOWAppExit; runASM(); }
void fauxGetDevCapsA() { PA = winmm.oauxGetDevCapsA; runASM(); }
void fauxGetDevCapsW() { PA = winmm.oauxGetDevCapsW; runASM(); }
void fauxGetNumDevs() { PA = winmm.oauxGetNumDevs; runASM(); }
void fauxGetVolume() { PA = winmm.oauxGetVolume; runASM(); }
void fauxOutMessage() { PA = winmm.oauxOutMessage; runASM(); }
void fauxSetVolume() { PA = winmm.oauxSetVolume; runASM(); }
void fjoyConfigChanged() { PA = winmm.ojoyConfigChanged; runASM(); }
void fjoyGetDevCapsA() { PA = winmm.ojoyGetDevCapsA; runASM(); }
void fjoyGetDevCapsW() { PA = winmm.ojoyGetDevCapsW; runASM(); }
void fjoyGetNumDevs() { PA = winmm.ojoyGetNumDevs; runASM(); }
void fjoyGetPos() { PA = winmm.ojoyGetPos; runASM(); }
void fjoyGetPosEx() { PA = winmm.ojoyGetPosEx; runASM(); }
void fjoyGetThreshold() { PA = winmm.ojoyGetThreshold; runASM(); }
void fjoyReleaseCapture() { PA = winmm.ojoyReleaseCapture; runASM(); }
void fjoySetCapture() { PA = winmm.ojoySetCapture; runASM(); }
void fjoySetThreshold() { PA = winmm.ojoySetThreshold; runASM(); }
void fmciDriverNotify() { PA = winmm.omciDriverNotify; runASM(); }
void fmciDriverYield() { PA = winmm.omciDriverYield; runASM(); }
void fmciExecute() { PA = winmm.omciExecute; runASM(); }
void fmciFreeCommandResource() { PA = winmm.omciFreeCommandResource; runASM(); }
void fmciGetCreatorTask() { PA = winmm.omciGetCreatorTask; runASM(); }
void fmciGetDeviceIDA() { PA = winmm.omciGetDeviceIDA; runASM(); }
void fmciGetDeviceIDFromElementIDA() { PA = winmm.omciGetDeviceIDFromElementIDA; runASM(); }
void fmciGetDeviceIDFromElementIDW() { PA = winmm.omciGetDeviceIDFromElementIDW; runASM(); }
void fmciGetDeviceIDW() { PA = winmm.omciGetDeviceIDW; runASM(); }
void fmciGetDriverData() { PA = winmm.omciGetDriverData; runASM(); }
void fmciGetErrorStringA() { PA = winmm.omciGetErrorStringA; runASM(); }
void fmciGetErrorStringW() { PA = winmm.omciGetErrorStringW; runASM(); }
void fmciGetYieldProc() { PA = winmm.omciGetYieldProc; runASM(); }
void fmciLoadCommandResource() { PA = winmm.omciLoadCommandResource; runASM(); }
void fmciSendCommandA() { PA = winmm.omciSendCommandA; runASM(); }
void fmciSendCommandW() { PA = winmm.omciSendCommandW; runASM(); }
void fmciSendStringA() { PA = winmm.omciSendStringA; runASM(); }
void fmciSendStringW() { PA = winmm.omciSendStringW; runASM(); }
void fmciSetDriverData() { PA = winmm.omciSetDriverData; runASM(); }
void fmciSetYieldProc() { PA = winmm.omciSetYieldProc; runASM(); }
void fmidiConnect() { PA = winmm.omidiConnect; runASM(); }
void fmidiDisconnect() { PA = winmm.omidiDisconnect; runASM(); }
void fmidiInAddBuffer() { PA = winmm.omidiInAddBuffer; runASM(); }
void fmidiInClose() { PA = winmm.omidiInClose; runASM(); }
void fmidiInGetDevCapsA() { PA = winmm.omidiInGetDevCapsA; runASM(); }
void fmidiInGetDevCapsW() { PA = winmm.omidiInGetDevCapsW; runASM(); }
void fmidiInGetErrorTextA() { PA = winmm.omidiInGetErrorTextA; runASM(); }
void fmidiInGetErrorTextW() { PA = winmm.omidiInGetErrorTextW; runASM(); }
void fmidiInGetID() { PA = winmm.omidiInGetID; runASM(); }
void fmidiInGetNumDevs() { PA = winmm.omidiInGetNumDevs; runASM(); }
void fmidiInMessage() { PA = winmm.omidiInMessage; runASM(); }
void fmidiInOpen() { PA = winmm.omidiInOpen; runASM(); }
void fmidiInPrepareHeader() { PA = winmm.omidiInPrepareHeader; runASM(); }
void fmidiInReset() { PA = winmm.omidiInReset; runASM(); }
void fmidiInStart() { PA = winmm.omidiInStart; runASM(); }
void fmidiInStop() { PA = winmm.omidiInStop; runASM(); }
void fmidiInUnprepareHeader() { PA = winmm.omidiInUnprepareHeader; runASM(); }
void fmidiOutCacheDrumPatches() { PA = winmm.omidiOutCacheDrumPatches; runASM(); }
void fmidiOutCachePatches() { PA = winmm.omidiOutCachePatches; runASM(); }
void fmidiOutClose() { PA = winmm.omidiOutClose; runASM(); }
void fmidiOutGetDevCapsA() { PA = winmm.omidiOutGetDevCapsA; runASM(); }
void fmidiOutGetDevCapsW() { PA = winmm.omidiOutGetDevCapsW; runASM(); }
void fmidiOutGetErrorTextA() { PA = winmm.omidiOutGetErrorTextA; runASM(); }
void fmidiOutGetErrorTextW() { PA = winmm.omidiOutGetErrorTextW; runASM(); }
void fmidiOutGetID() { PA = winmm.omidiOutGetID; runASM(); }
void fmidiOutGetNumDevs() { PA = winmm.omidiOutGetNumDevs; runASM(); }
void fmidiOutGetVolume() { PA = winmm.omidiOutGetVolume; runASM(); }
void fmidiOutLongMsg() { PA = winmm.omidiOutLongMsg; runASM(); }
void fmidiOutMessage() { PA = winmm.omidiOutMessage; runASM(); }
void fmidiOutOpen() { PA = winmm.omidiOutOpen; runASM(); }
void fmidiOutPrepareHeader() { PA = winmm.omidiOutPrepareHeader; runASM(); }
void fmidiOutReset() { PA = winmm.omidiOutReset; runASM(); }
void fmidiOutSetVolume() { PA = winmm.omidiOutSetVolume; runASM(); }
void fmidiOutShortMsg() { PA = winmm.omidiOutShortMsg; runASM(); }
void fmidiOutUnprepareHeader() { PA = winmm.omidiOutUnprepareHeader; runASM(); }
void fmidiStreamClose() { PA = winmm.omidiStreamClose; runASM(); }
void fmidiStreamOpen() { PA = winmm.omidiStreamOpen; runASM(); }
void fmidiStreamOut() { PA = winmm.omidiStreamOut; runASM(); }
void fmidiStreamPause() { PA = winmm.omidiStreamPause; runASM(); }
void fmidiStreamPosition() { PA = winmm.omidiStreamPosition; runASM(); }
void fmidiStreamProperty() { PA = winmm.omidiStreamProperty; runASM(); }
void fmidiStreamRestart() { PA = winmm.omidiStreamRestart; runASM(); }
void fmidiStreamStop() { PA = winmm.omidiStreamStop; runASM(); }
void fmixerClose() { PA = winmm.omixerClose; runASM(); }
void fmixerGetControlDetailsA() { PA = winmm.omixerGetControlDetailsA; runASM(); }
void fmixerGetControlDetailsW() { PA = winmm.omixerGetControlDetailsW; runASM(); }
void fmixerGetDevCapsA() { PA = winmm.omixerGetDevCapsA; runASM(); }
void fmixerGetDevCapsW() { PA = winmm.omixerGetDevCapsW; runASM(); }
void fmixerGetID() { PA = winmm.omixerGetID; runASM(); }
void fmixerGetLineControlsA() { PA = winmm.omixerGetLineControlsA; runASM(); }
void fmixerGetLineControlsW() { PA = winmm.omixerGetLineControlsW; runASM(); }
void fmixerGetLineInfoA() { PA = winmm.omixerGetLineInfoA; runASM(); }
void fmixerGetLineInfoW() { PA = winmm.omixerGetLineInfoW; runASM(); }
void fmixerGetNumDevs() { PA = winmm.omixerGetNumDevs; runASM(); }
void fmixerMessage() { PA = winmm.omixerMessage; runASM(); }
void fmixerOpen() { PA = winmm.omixerOpen; runASM(); }
void fmixerSetControlDetails() { PA = winmm.omixerSetControlDetails; runASM(); }
void fmmDrvInstall() { PA = winmm.ommDrvInstall; runASM(); }
void fmmGetCurrentTask() { PA = winmm.ommGetCurrentTask; runASM(); }
void fmmTaskBlock() { PA = winmm.ommTaskBlock; runASM(); }
void fmmTaskCreate() { PA = winmm.ommTaskCreate; runASM(); }
void fmmTaskSignal() { PA = winmm.ommTaskSignal; runASM(); }
void fmmTaskYield() { PA = winmm.ommTaskYield; runASM(); }
void fmmioAdvance() { PA = winmm.ommioAdvance; runASM(); }
void fmmioAscend() { PA = winmm.ommioAscend; runASM(); }
void fmmioClose() { PA = winmm.ommioClose; runASM(); }
void fmmioCreateChunk() { PA = winmm.ommioCreateChunk; runASM(); }
void fmmioDescend() { PA = winmm.ommioDescend; runASM(); }
void fmmioFlush() { PA = winmm.ommioFlush; runASM(); }
void fmmioGetInfo() { PA = winmm.ommioGetInfo; runASM(); }
void fmmioInstallIOProcA() { PA = winmm.ommioInstallIOProcA; runASM(); }
void fmmioInstallIOProcW() { PA = winmm.ommioInstallIOProcW; runASM(); }
void fmmioOpenA() { PA = winmm.ommioOpenA; runASM(); }
void fmmioOpenW() { PA = winmm.ommioOpenW; runASM(); }
void fmmioRead() { PA = winmm.ommioRead; runASM(); }
void fmmioRenameA() { PA = winmm.ommioRenameA; runASM(); }
void fmmioRenameW() { PA = winmm.ommioRenameW; runASM(); }
void fmmioSeek() { PA = winmm.ommioSeek; runASM(); }
void fmmioSendMessage() { PA = winmm.ommioSendMessage; runASM(); }
void fmmioSetBuffer() { PA = winmm.ommioSetBuffer; runASM(); }
void fmmioSetInfo() { PA = winmm.ommioSetInfo; runASM(); }
void fmmioStringToFOURCCA() { PA = winmm.ommioStringToFOURCCA; runASM(); }
void fmmioStringToFOURCCW() { PA = winmm.ommioStringToFOURCCW; runASM(); }
void fmmioWrite() { PA = winmm.ommioWrite; runASM(); }
void fmmsystemGetVersion() { PA = winmm.ommsystemGetVersion; runASM(); }
void fsndPlaySoundA() { PA = winmm.osndPlaySoundA; runASM(); }
void fsndPlaySoundW() { PA = winmm.osndPlaySoundW; runASM(); }
void ftimeBeginPeriod() { PA = winmm.otimeBeginPeriod; runASM(); }
void ftimeEndPeriod() { PA = winmm.otimeEndPeriod; runASM(); }
void ftimeGetDevCaps() { PA = winmm.otimeGetDevCaps; runASM(); }
void ftimeGetSystemTime() { PA = winmm.otimeGetSystemTime; runASM(); }
void ftimeGetTime() { PA = winmm.otimeGetTime; runASM(); }
void ftimeKillEvent() { PA = winmm.otimeKillEvent; runASM(); }
void ftimeSetEvent() { PA = winmm.otimeSetEvent; runASM(); }
void fwaveInAddBuffer() { PA = winmm.owaveInAddBuffer; runASM(); }
void fwaveInClose() { PA = winmm.owaveInClose; runASM(); }
void fwaveInGetDevCapsA() { PA = winmm.owaveInGetDevCapsA; runASM(); }
void fwaveInGetDevCapsW() { PA = winmm.owaveInGetDevCapsW; runASM(); }
void fwaveInGetErrorTextA() { PA = winmm.owaveInGetErrorTextA; runASM(); }
void fwaveInGetErrorTextW() { PA = winmm.owaveInGetErrorTextW; runASM(); }
void fwaveInGetID() { PA = winmm.owaveInGetID; runASM(); }
void fwaveInGetNumDevs() { PA = winmm.owaveInGetNumDevs; runASM(); }
void fwaveInGetPosition() { PA = winmm.owaveInGetPosition; runASM(); }
void fwaveInMessage() { PA = winmm.owaveInMessage; runASM(); }
void fwaveInOpen() { PA = winmm.owaveInOpen; runASM(); }
void fwaveInPrepareHeader() { PA = winmm.owaveInPrepareHeader; runASM(); }
void fwaveInReset() { PA = winmm.owaveInReset; runASM(); }
void fwaveInStart() { PA = winmm.owaveInStart; runASM(); }
void fwaveInStop() { PA = winmm.owaveInStop; runASM(); }
void fwaveInUnprepareHeader() { PA = winmm.owaveInUnprepareHeader; runASM(); }
void fwaveOutBreakLoop() { PA = winmm.owaveOutBreakLoop; runASM(); }
void fwaveOutClose() { PA = winmm.owaveOutClose; runASM(); }
void fwaveOutGetDevCapsA() { PA = winmm.owaveOutGetDevCapsA; runASM(); }
void fwaveOutGetDevCapsW() { PA = winmm.owaveOutGetDevCapsW; runASM(); }
void fwaveOutGetErrorTextA() { PA = winmm.owaveOutGetErrorTextA; runASM(); }
void fwaveOutGetErrorTextW() { PA = winmm.owaveOutGetErrorTextW; runASM(); }
void fwaveOutGetID() { PA = winmm.owaveOutGetID; runASM(); }
void fwaveOutGetNumDevs() { PA = winmm.owaveOutGetNumDevs; runASM(); }
void fwaveOutGetPitch() { PA = winmm.owaveOutGetPitch; runASM(); }
void fwaveOutGetPlaybackRate() { PA = winmm.owaveOutGetPlaybackRate; runASM(); }
void fwaveOutGetPosition() { PA = winmm.owaveOutGetPosition; runASM(); }
void fwaveOutGetVolume() { PA = winmm.owaveOutGetVolume; runASM(); }
void fwaveOutMessage() { PA = winmm.owaveOutMessage; runASM(); }
void fwaveOutOpen() { PA = winmm.owaveOutOpen; runASM(); }
void fwaveOutPause() { PA = winmm.owaveOutPause; runASM(); }
void fwaveOutPrepareHeader() { PA = winmm.owaveOutPrepareHeader; runASM(); }
void fwaveOutReset() { PA = winmm.owaveOutReset; runASM(); }
void fwaveOutRestart() { PA = winmm.owaveOutRestart; runASM(); }
void fwaveOutSetPitch() { PA = winmm.owaveOutSetPitch; runASM(); }
void fwaveOutSetPlaybackRate() { PA = winmm.owaveOutSetPlaybackRate; runASM(); }
void fwaveOutSetVolume() { PA = winmm.owaveOutSetVolume; runASM(); }
void fwaveOutUnprepareHeader() { PA = winmm.owaveOutUnprepareHeader; runASM(); }
void fwaveOutWrite() { PA = winmm.owaveOutWrite; runASM(); }

void setupFunctions(HMODULE dll) {
	winmm.dll = dll;
	winmm.oCloseDriver = GetProcAddress(winmm.dll, "CloseDriver");
	winmm.oDefDriverProc = GetProcAddress(winmm.dll, "DefDriverProc");
	winmm.oDriverCallback = GetProcAddress(winmm.dll, "DriverCallback");
	winmm.oDrvGetModuleHandle = GetProcAddress(winmm.dll, "DrvGetModuleHandle");
	winmm.oGetDriverModuleHandle = GetProcAddress(winmm.dll, "GetDriverModuleHandle");
	winmm.oOpenDriver = GetProcAddress(winmm.dll, "OpenDriver");
	winmm.oPlaySound = GetProcAddress(winmm.dll, "PlaySound");
	winmm.oPlaySoundA = GetProcAddress(winmm.dll, "PlaySoundA");
	winmm.oPlaySoundW = GetProcAddress(winmm.dll, "PlaySoundW");
	winmm.oSendDriverMessage = GetProcAddress(winmm.dll, "SendDriverMessage");
	winmm.oWOWAppExit = GetProcAddress(winmm.dll, "WOWAppExit");
	winmm.oauxGetDevCapsA = GetProcAddress(winmm.dll, "auxGetDevCapsA");
	winmm.oauxGetDevCapsW = GetProcAddress(winmm.dll, "auxGetDevCapsW");
	winmm.oauxGetNumDevs = GetProcAddress(winmm.dll, "auxGetNumDevs");
	winmm.oauxGetVolume = GetProcAddress(winmm.dll, "auxGetVolume");
	winmm.oauxOutMessage = GetProcAddress(winmm.dll, "auxOutMessage");
	winmm.oauxSetVolume = GetProcAddress(winmm.dll, "auxSetVolume");
	winmm.ojoyConfigChanged = GetProcAddress(winmm.dll, "joyConfigChanged");
	winmm.ojoyGetDevCapsA = GetProcAddress(winmm.dll, "joyGetDevCapsA");
	winmm.ojoyGetDevCapsW = GetProcAddress(winmm.dll, "joyGetDevCapsW");
	winmm.ojoyGetNumDevs = GetProcAddress(winmm.dll, "joyGetNumDevs");
	winmm.ojoyGetPos = GetProcAddress(winmm.dll, "joyGetPos");
	winmm.ojoyGetPosEx = GetProcAddress(winmm.dll, "joyGetPosEx");
	winmm.ojoyGetThreshold = GetProcAddress(winmm.dll, "joyGetThreshold");
	winmm.ojoyReleaseCapture = GetProcAddress(winmm.dll, "joyReleaseCapture");
	winmm.ojoySetCapture = GetProcAddress(winmm.dll, "joySetCapture");
	winmm.ojoySetThreshold = GetProcAddress(winmm.dll, "joySetThreshold");
	winmm.omciDriverNotify = GetProcAddress(winmm.dll, "mciDriverNotify");
	winmm.omciDriverYield = GetProcAddress(winmm.dll, "mciDriverYield");
	winmm.omciExecute = GetProcAddress(winmm.dll, "mciExecute");
	winmm.omciFreeCommandResource = GetProcAddress(winmm.dll, "mciFreeCommandResource");
	winmm.omciGetCreatorTask = GetProcAddress(winmm.dll, "mciGetCreatorTask");
	winmm.omciGetDeviceIDA = GetProcAddress(winmm.dll, "mciGetDeviceIDA");
	winmm.omciGetDeviceIDFromElementIDA = GetProcAddress(winmm.dll, "mciGetDeviceIDFromElementIDA");
	winmm.omciGetDeviceIDFromElementIDW = GetProcAddress(winmm.dll, "mciGetDeviceIDFromElementIDW");
	winmm.omciGetDeviceIDW = GetProcAddress(winmm.dll, "mciGetDeviceIDW");
	winmm.omciGetDriverData = GetProcAddress(winmm.dll, "mciGetDriverData");
	winmm.omciGetErrorStringA = GetProcAddress(winmm.dll, "mciGetErrorStringA");
	winmm.omciGetErrorStringW = GetProcAddress(winmm.dll, "mciGetErrorStringW");
	winmm.omciGetYieldProc = GetProcAddress(winmm.dll, "mciGetYieldProc");
	winmm.omciLoadCommandResource = GetProcAddress(winmm.dll, "mciLoadCommandResource");
	winmm.omciSendCommandA = GetProcAddress(winmm.dll, "mciSendCommandA");
	winmm.omciSendCommandW = GetProcAddress(winmm.dll, "mciSendCommandW");
	winmm.omciSendStringA = GetProcAddress(winmm.dll, "mciSendStringA");
	winmm.omciSendStringW = GetProcAddress(winmm.dll, "mciSendStringW");
	winmm.omciSetDriverData = GetProcAddress(winmm.dll, "mciSetDriverData");
	winmm.omciSetYieldProc = GetProcAddress(winmm.dll, "mciSetYieldProc");
	winmm.omidiConnect = GetProcAddress(winmm.dll, "midiConnect");
	winmm.omidiDisconnect = GetProcAddress(winmm.dll, "midiDisconnect");
	winmm.omidiInAddBuffer = GetProcAddress(winmm.dll, "midiInAddBuffer");
	winmm.omidiInClose = GetProcAddress(winmm.dll, "midiInClose");
	winmm.omidiInGetDevCapsA = GetProcAddress(winmm.dll, "midiInGetDevCapsA");
	winmm.omidiInGetDevCapsW = GetProcAddress(winmm.dll, "midiInGetDevCapsW");
	winmm.omidiInGetErrorTextA = GetProcAddress(winmm.dll, "midiInGetErrorTextA");
	winmm.omidiInGetErrorTextW = GetProcAddress(winmm.dll, "midiInGetErrorTextW");
	winmm.omidiInGetID = GetProcAddress(winmm.dll, "midiInGetID");
	winmm.omidiInGetNumDevs = GetProcAddress(winmm.dll, "midiInGetNumDevs");
	winmm.omidiInMessage = GetProcAddress(winmm.dll, "midiInMessage");
	winmm.omidiInOpen = GetProcAddress(winmm.dll, "midiInOpen");
	winmm.omidiInPrepareHeader = GetProcAddress(winmm.dll, "midiInPrepareHeader");
	winmm.omidiInReset = GetProcAddress(winmm.dll, "midiInReset");
	winmm.omidiInStart = GetProcAddress(winmm.dll, "midiInStart");
	winmm.omidiInStop = GetProcAddress(winmm.dll, "midiInStop");
	winmm.omidiInUnprepareHeader = GetProcAddress(winmm.dll, "midiInUnprepareHeader");
	winmm.omidiOutCacheDrumPatches = GetProcAddress(winmm.dll, "midiOutCacheDrumPatches");
	winmm.omidiOutCachePatches = GetProcAddress(winmm.dll, "midiOutCachePatches");
	winmm.omidiOutClose = GetProcAddress(winmm.dll, "midiOutClose");
	winmm.omidiOutGetDevCapsA = GetProcAddress(winmm.dll, "midiOutGetDevCapsA");
	winmm.omidiOutGetDevCapsW = GetProcAddress(winmm.dll, "midiOutGetDevCapsW");
	winmm.omidiOutGetErrorTextA = GetProcAddress(winmm.dll, "midiOutGetErrorTextA");
	winmm.omidiOutGetErrorTextW = GetProcAddress(winmm.dll, "midiOutGetErrorTextW");
	winmm.omidiOutGetID = GetProcAddress(winmm.dll, "midiOutGetID");
	winmm.omidiOutGetNumDevs = GetProcAddress(winmm.dll, "midiOutGetNumDevs");
	winmm.omidiOutGetVolume = GetProcAddress(winmm.dll, "midiOutGetVolume");
	winmm.omidiOutLongMsg = GetProcAddress(winmm.dll, "midiOutLongMsg");
	winmm.omidiOutMessage = GetProcAddress(winmm.dll, "midiOutMessage");
	winmm.omidiOutOpen = GetProcAddress(winmm.dll, "midiOutOpen");
	winmm.omidiOutPrepareHeader = GetProcAddress(winmm.dll, "midiOutPrepareHeader");
	winmm.omidiOutReset = GetProcAddress(winmm.dll, "midiOutReset");
	winmm.omidiOutSetVolume = GetProcAddress(winmm.dll, "midiOutSetVolume");
	winmm.omidiOutShortMsg = GetProcAddress(winmm.dll, "midiOutShortMsg");
	winmm.omidiOutUnprepareHeader = GetProcAddress(winmm.dll, "midiOutUnprepareHeader");
	winmm.omidiStreamClose = GetProcAddress(winmm.dll, "midiStreamClose");
	winmm.omidiStreamOpen = GetProcAddress(winmm.dll, "midiStreamOpen");
	winmm.omidiStreamOut = GetProcAddress(winmm.dll, "midiStreamOut");
	winmm.omidiStreamPause = GetProcAddress(winmm.dll, "midiStreamPause");
	winmm.omidiStreamPosition = GetProcAddress(winmm.dll, "midiStreamPosition");
	winmm.omidiStreamProperty = GetProcAddress(winmm.dll, "midiStreamProperty");
	winmm.omidiStreamRestart = GetProcAddress(winmm.dll, "midiStreamRestart");
	winmm.omidiStreamStop = GetProcAddress(winmm.dll, "midiStreamStop");
	winmm.omixerClose = GetProcAddress(winmm.dll, "mixerClose");
	winmm.omixerGetControlDetailsA = GetProcAddress(winmm.dll, "mixerGetControlDetailsA");
	winmm.omixerGetControlDetailsW = GetProcAddress(winmm.dll, "mixerGetControlDetailsW");
	winmm.omixerGetDevCapsA = GetProcAddress(winmm.dll, "mixerGetDevCapsA");
	winmm.omixerGetDevCapsW = GetProcAddress(winmm.dll, "mixerGetDevCapsW");
	winmm.omixerGetID = GetProcAddress(winmm.dll, "mixerGetID");
	winmm.omixerGetLineControlsA = GetProcAddress(winmm.dll, "mixerGetLineControlsA");
	winmm.omixerGetLineControlsW = GetProcAddress(winmm.dll, "mixerGetLineControlsW");
	winmm.omixerGetLineInfoA = GetProcAddress(winmm.dll, "mixerGetLineInfoA");
	winmm.omixerGetLineInfoW = GetProcAddress(winmm.dll, "mixerGetLineInfoW");
	winmm.omixerGetNumDevs = GetProcAddress(winmm.dll, "mixerGetNumDevs");
	winmm.omixerMessage = GetProcAddress(winmm.dll, "mixerMessage");
	winmm.omixerOpen = GetProcAddress(winmm.dll, "mixerOpen");
	winmm.omixerSetControlDetails = GetProcAddress(winmm.dll, "mixerSetControlDetails");
	winmm.ommDrvInstall = GetProcAddress(winmm.dll, "mmDrvInstall");
	winmm.ommGetCurrentTask = GetProcAddress(winmm.dll, "mmGetCurrentTask");
	winmm.ommTaskBlock = GetProcAddress(winmm.dll, "mmTaskBlock");
	winmm.ommTaskCreate = GetProcAddress(winmm.dll, "mmTaskCreate");
	winmm.ommTaskSignal = GetProcAddress(winmm.dll, "mmTaskSignal");
	winmm.ommTaskYield = GetProcAddress(winmm.dll, "mmTaskYield");
	winmm.ommioAdvance = GetProcAddress(winmm.dll, "mmioAdvance");
	winmm.ommioAscend = GetProcAddress(winmm.dll, "mmioAscend");
	winmm.ommioClose = GetProcAddress(winmm.dll, "mmioClose");
	winmm.ommioCreateChunk = GetProcAddress(winmm.dll, "mmioCreateChunk");
	winmm.ommioDescend = GetProcAddress(winmm.dll, "mmioDescend");
	winmm.ommioFlush = GetProcAddress(winmm.dll, "mmioFlush");
	winmm.ommioGetInfo = GetProcAddress(winmm.dll, "mmioGetInfo");
	winmm.ommioInstallIOProcA = GetProcAddress(winmm.dll, "mmioInstallIOProcA");
	winmm.ommioInstallIOProcW = GetProcAddress(winmm.dll, "mmioInstallIOProcW");
	winmm.ommioOpenA = GetProcAddress(winmm.dll, "mmioOpenA");
	winmm.ommioOpenW = GetProcAddress(winmm.dll, "mmioOpenW");
	winmm.ommioRead = GetProcAddress(winmm.dll, "mmioRead");
	winmm.ommioRenameA = GetProcAddress(winmm.dll, "mmioRenameA");
	winmm.ommioRenameW = GetProcAddress(winmm.dll, "mmioRenameW");
	winmm.ommioSeek = GetProcAddress(winmm.dll, "mmioSeek");
	winmm.ommioSendMessage = GetProcAddress(winmm.dll, "mmioSendMessage");
	winmm.ommioSetBuffer = GetProcAddress(winmm.dll, "mmioSetBuffer");
	winmm.ommioSetInfo = GetProcAddress(winmm.dll, "mmioSetInfo");
	winmm.ommioStringToFOURCCA = GetProcAddress(winmm.dll, "mmioStringToFOURCCA");
	winmm.ommioStringToFOURCCW = GetProcAddress(winmm.dll, "mmioStringToFOURCCW");
	winmm.ommioWrite = GetProcAddress(winmm.dll, "mmioWrite");
	winmm.ommsystemGetVersion = GetProcAddress(winmm.dll, "mmsystemGetVersion");
	winmm.osndPlaySoundA = GetProcAddress(winmm.dll, "sndPlaySoundA");
	winmm.osndPlaySoundW = GetProcAddress(winmm.dll, "sndPlaySoundW");
	winmm.otimeBeginPeriod = GetProcAddress(winmm.dll, "timeBeginPeriod");
	winmm.otimeEndPeriod = GetProcAddress(winmm.dll, "timeEndPeriod");
	winmm.otimeGetDevCaps = GetProcAddress(winmm.dll, "timeGetDevCaps");
	winmm.otimeGetSystemTime = GetProcAddress(winmm.dll, "timeGetSystemTime");
	winmm.otimeGetTime = GetProcAddress(winmm.dll, "timeGetTime");
	winmm.otimeKillEvent = GetProcAddress(winmm.dll, "timeKillEvent");
	winmm.otimeSetEvent = GetProcAddress(winmm.dll, "timeSetEvent");
	winmm.owaveInAddBuffer = GetProcAddress(winmm.dll, "waveInAddBuffer");
	winmm.owaveInClose = GetProcAddress(winmm.dll, "waveInClose");
	winmm.owaveInGetDevCapsA = GetProcAddress(winmm.dll, "waveInGetDevCapsA");
	winmm.owaveInGetDevCapsW = GetProcAddress(winmm.dll, "waveInGetDevCapsW");
	winmm.owaveInGetErrorTextA = GetProcAddress(winmm.dll, "waveInGetErrorTextA");
	winmm.owaveInGetErrorTextW = GetProcAddress(winmm.dll, "waveInGetErrorTextW");
	winmm.owaveInGetID = GetProcAddress(winmm.dll, "waveInGetID");
	winmm.owaveInGetNumDevs = GetProcAddress(winmm.dll, "waveInGetNumDevs");
	winmm.owaveInGetPosition = GetProcAddress(winmm.dll, "waveInGetPosition");
	winmm.owaveInMessage = GetProcAddress(winmm.dll, "waveInMessage");
	winmm.owaveInOpen = GetProcAddress(winmm.dll, "waveInOpen");
	winmm.owaveInPrepareHeader = GetProcAddress(winmm.dll, "waveInPrepareHeader");
	winmm.owaveInReset = GetProcAddress(winmm.dll, "waveInReset");
	winmm.owaveInStart = GetProcAddress(winmm.dll, "waveInStart");
	winmm.owaveInStop = GetProcAddress(winmm.dll, "waveInStop");
	winmm.owaveInUnprepareHeader = GetProcAddress(winmm.dll, "waveInUnprepareHeader");
	winmm.owaveOutBreakLoop = GetProcAddress(winmm.dll, "waveOutBreakLoop");
	winmm.owaveOutClose = GetProcAddress(winmm.dll, "waveOutClose");
	winmm.owaveOutGetDevCapsA = GetProcAddress(winmm.dll, "waveOutGetDevCapsA");
	winmm.owaveOutGetDevCapsW = GetProcAddress(winmm.dll, "waveOutGetDevCapsW");
	winmm.owaveOutGetErrorTextA = GetProcAddress(winmm.dll, "waveOutGetErrorTextA");
	winmm.owaveOutGetErrorTextW = GetProcAddress(winmm.dll, "waveOutGetErrorTextW");
	winmm.owaveOutGetID = GetProcAddress(winmm.dll, "waveOutGetID");
	winmm.owaveOutGetNumDevs = GetProcAddress(winmm.dll, "waveOutGetNumDevs");
	winmm.owaveOutGetPitch = GetProcAddress(winmm.dll, "waveOutGetPitch");
	winmm.owaveOutGetPlaybackRate = GetProcAddress(winmm.dll, "waveOutGetPlaybackRate");
	winmm.owaveOutGetPosition = GetProcAddress(winmm.dll, "waveOutGetPosition");
	winmm.owaveOutGetVolume = GetProcAddress(winmm.dll, "waveOutGetVolume");
	winmm.owaveOutMessage = GetProcAddress(winmm.dll, "waveOutMessage");
	winmm.owaveOutOpen = GetProcAddress(winmm.dll, "waveOutOpen");
	winmm.owaveOutPause = GetProcAddress(winmm.dll, "waveOutPause");
	winmm.owaveOutPrepareHeader = GetProcAddress(winmm.dll, "waveOutPrepareHeader");
	winmm.owaveOutReset = GetProcAddress(winmm.dll, "waveOutReset");
	winmm.owaveOutRestart = GetProcAddress(winmm.dll, "waveOutRestart");
	winmm.owaveOutSetPitch = GetProcAddress(winmm.dll, "waveOutSetPitch");
	winmm.owaveOutSetPlaybackRate = GetProcAddress(winmm.dll, "waveOutSetPlaybackRate");
	winmm.owaveOutSetVolume = GetProcAddress(winmm.dll, "waveOutSetVolume");
	winmm.owaveOutUnprepareHeader = GetProcAddress(winmm.dll, "waveOutUnprepareHeader");
	winmm.owaveOutWrite = GetProcAddress(winmm.dll, "waveOutWrite");
}

void cleanupProxy(void) {
	if (winmm.dll != NULL) {
		FreeLibrary(winmm.dll);
		winmm.dll = NULL;
	}
}
#pragma endregion