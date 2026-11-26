#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

AudioProcessorValueTreeState::ParameterLayout ArcanaProcessor::createLayout()
{
    AudioProcessorValueTreeState::ParameterLayout layout;
    for (auto& f : kFx)
    {
        const String id (f.id), name (f.name);
        layout.add (std::make_unique<AudioParameterBool> (ParameterID { id + "_on", 1 }, name + " On", false));
        const char* suffix[3] = { "_a", "_b", "_c" };
        for (int k = 0; k < 3; ++k)
            layout.add (std::make_unique<AudioParameterFloat> (ParameterID { id + suffix[k], 1 },
                                                               name + " " + f.knobs[k],
                                                               NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.5f));
        layout.add (std::make_unique<AudioParameterFloat> (ParameterID { id + "_mix", 1 }, name + " Mix",
                                                           NormalisableRange<float> (0.0f, 1.0f, 0.001f), f.defMix));
    }
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID { "master_mix", 1 }, "Master Mix",
                                                       NormalisableRange<float> (0.0f, 1.0f, 0.001f), 1.0f));
    layout.add (std::make_unique<AudioParameterFloat> (ParameterID { "master_out", 1 }, "Output (dB)",
                                                       NormalisableRange<float> (-24.0f, 6.0f, 0.1f), 0.0f));
    return layout;
}

ArcanaProcessor::ArcanaProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", AudioChannelSet::stereo(), true)
                          .withOutput ("Output", AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createLayout())
{
    fx[0] = std::make_unique<arcana::ChorusFx>();
    fx[1] = std::make_unique<arcana::DelayFx>();
    fx[2] = std::make_unique<arcana::GranularFx>();
    fx[3] = std::make_unique<arcana::LofiFx>();
    fx[4] = std::make_unique<arcana::NoiseFx>();
    fx[5] = std::make_unique<arcana::ReverbFx>();
    fx[6] = std::make_unique<arcana::SidechainFx>();

    for (int i = 0; i < 7; ++i)
    {
        const String id (kFx[i].id);
        ptr[i].on  = apvts.getRawParameterValue (id + "_on");
        ptr[i].a   = apvts.getRawParameterValue (id + "_a");
        ptr[i].b   = apvts.getRawParameterValue (id + "_b");
        ptr[i].c   = apvts.getRawParameterValue (id + "_c");
        ptr[i].mix = apvts.getRawParameterValue (id + "_mix");
    }
    masterMix = apvts.getRawParameterValue ("master_mix");
    outGain   = apvts.getRawParameterValue ("master_out");
}

void ArcanaProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (auto& e : fx) e->prepare (sampleRate);
    dry.setSize (2, samplesPerBlock);
    masterDry.setSize (2, samplesPerBlock);
}

bool ArcanaProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    return l.getMainInputChannelSet() == AudioChannelSet::stereo()
        && l.getMainOutputChannelSet() == AudioChannelSet::stereo();
}

void ArcanaProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;
    const int n = buffer.getNumSamples();

    double bpm = 120.0, ppq = -1.0;
    if (auto* ph = getPlayHead())
        if (auto pos = ph->getPosition())
        {
            if (auto b = pos->getBpm()) bpm = *b;
            if (pos->getIsPlaying())
                if (auto q = pos->getPpqPosition()) ppq = *q;
        }

    masterDry.makeCopyOf (buffer, true);

    for (int i = 0; i < 7; ++i)
    {
        if (ptr[i].on->load() < 0.5f) continue;
        dry.makeCopyOf (buffer, true);
        arcana::Params p { ptr[i].a->load(), ptr[i].b->load(), ptr[i].c->load() };
        fx[i]->process (buffer, p, bpm, ppq);
        const float mix = ptr[i].mix->load();
        for (int ch = 0; ch < 2; ++ch)
        {
            buffer.applyGain (ch, 0, n, mix);
            buffer.addFrom (ch, 0, dry, ch, 0, n, 1.0f - mix);
        }
    }

    const float mm = masterMix->load();
    const float g = Decibels::decibelsToGain (outGain->load());
    for (int ch = 0; ch < 2; ++ch)
    {
        buffer.applyGain (ch, 0, n, mm);
        buffer.addFrom (ch, 0, masterDry, ch, 0, n, 1.0f - mm);
        buffer.applyGain (ch, 0, n, g);
    }
}

AudioProcessorEditor* ArcanaProcessor::createEditor() { return new ArcanaEditor (*this); }

void ArcanaProcessor::getStateInformation (MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, dest);
}

void ArcanaProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (ValueTree::fromXml (*xml));
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new ArcanaProcessor(); }
