#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "Effects.h"

struct FxInfo
{
    const char* id;
    const char* name;
    const char* knobs[3];
    const char* roman;
    const char* title;
    juce::uint32 accent;
    float defMix;
};

// El orden es tambien el orden de la cadena de senal y de las cartas en pantalla
inline const FxInfo kFx[7] = {
    { "cho",  "CHORUS",    { "RATE", "DEPTH", "SPREAD" },    "II",   "TWO of CUPS",        0xff3aa0c8, 0.5f  },
    { "dly",  "DELAY",     { "TIME", "FEEDBACK", "TONE" },   "II",   "TWO of PENTACLES",   0xffe0b030, 0.4f  },
    { "grn",  "GRANULAR",  { "SIZE", "DENSITY", "PITCH" },   "VIII", "EIGHT of PENTACLES", 0xffd9a21b, 0.5f  },
    { "lofi", "LO-FI",     { "BITS", "RATE", "WOBBLE" },     "PAGE", "PAGE of CUPS",       0xffe06aa0, 0.5f  },
    { "nse",  "NOISE",     { "LEVEL", "COLOR", "CRACKLE" },  "PAGE", "PAGE of SWORDS",     0xff9ad0e8, 1.0f  },
    { "rev",  "REVERB",    { "SIZE", "DAMP", "WIDTH" },      "XVII", "THE STAR",           0xfff5d840, 0.35f },
    { "sc",   "SIDECHAIN", { "RATE", "DEPTH", "CURVE" },     "III",  "THREE of SWORDS",    0xffe04050, 1.0f  }
};

class ArcanaProcessor : public juce::AudioProcessor
{
public:
    ArcanaProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Arcana Pedals"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 4.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    std::unique_ptr<arcana::Effect> fx[7];
    struct Ptrs { std::atomic<float> *on, *a, *b, *c, *mix; } ptr[7];
    std::atomic<float> *masterMix = nullptr, *outGain = nullptr;
    juce::AudioBuffer<float> dry, masterDry;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArcanaProcessor)
};
