#include <windows.h>
#include <iostream>
#include <cmath>

#pragma comment(lib, "winmm.lib")

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int SAMPLE_RATE = 44100;
const int FREQUENCY = 15000;
const int DURATION_SECONDS = 999;
const int AMPLITUDE = 32760;

struct WaveHeader {
    char riff[4];
    unsigned int overallSize;
    char wave[4];
    char fmtChunk[4];
    unsigned int fmtChunkSize;
    unsigned short audioFormat;
    unsigned short numChannels;
    unsigned int sampleRate;
    unsigned int byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    char dataChunk[4];
    unsigned int dataSize;
};

void generateTone() {
    int numSamples = SAMPLE_RATE * DURATION_SECONDS;
    int dataSize = numSamples * sizeof(short);

    WaveHeader header;
    header.riff[0] = 'R'; header.riff[1] = 'I'; header.riff[2] = 'F'; header.riff[3] = 'F';
    header.wave[0] = 'W'; header.wave[1] = 'A'; header.wave[2] = 'V'; header.wave[3] = 'E';
    header.fmtChunk[0] = 'f'; header.fmtChunk[1] = 'm'; header.fmtChunk[2] = 't'; header.fmtChunk[3] = ' ';
    header.dataChunk[0] = 'd'; header.dataChunk[1] = 'a'; header.dataChunk[2] = 't'; header.dataChunk[3] = 'a';

    header.fmtChunkSize = 16;
    header.audioFormat = 1;
    header.numChannels = 1;
    header.sampleRate = SAMPLE_RATE;
    header.bitsPerSample = 16;
    header.byteRate = SAMPLE_RATE * sizeof(short);
    header.blockAlign = sizeof(short);
    header.dataSize = dataSize;
    header.overallSize = dataSize + sizeof(WaveHeader) - 8;

    short* buffer = new short[numSamples];

    for (int i = 0; i < numSamples; i++) {
        double time = (double)i / SAMPLE_RATE;
        buffer[i] = (short)(AMPLITUDE * sin(2.0 * M_PI * FREQUENCY * time));
    }

    HWAVEOUT hWaveOut;
    WAVEFORMATEX wfx;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = SAMPLE_RATE;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        std::cerr << "Error opening audio device!" << std::endl;
        delete[] buffer;
        return;
    }

    WAVEHDR waveHdr;
    ZeroMemory(&waveHdr, sizeof(WAVEHDR));
    waveHdr.lpData = (LPSTR)buffer;
    waveHdr.dwBufferLength = dataSize;

    waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &waveHdr, sizeof(WAVEHDR));

    Sleep(DURATION_SECONDS * 1000);

    waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);

    delete[] buffer;
}

int main() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(hConsole, &info);
    WORD originalColor = info.wAttributes;

    SetConsoleTextAttribute(hConsole, 6);
    std::cout << "Weapon Status";

    SetConsoleTextAttribute(hConsole, originalColor);
    std::cout << ": ";

    SetConsoleTextAttribute(hConsole, 2);
    std::cout << "ACTIVE";

    SetConsoleTextAttribute(hConsole, originalColor);
    std::cout << std::endl;
    SetConsoleTextAttribute(hConsole, 4);
    std::cout << "Stops in: " << DURATION_SECONDS << " seconds for security reasons" << std::endl;

    Sleep(1000);

    generateTone();

    std::cout << "Tone playback complete." << std::endl;

    return 0;
}