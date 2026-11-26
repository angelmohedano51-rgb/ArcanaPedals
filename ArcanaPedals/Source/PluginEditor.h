#pragma once
#include "PluginProcessor.h"

// Aspecto de perillas y boton de encendido
class KnobLAF : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                           float startA, float endA, juce::Slider& s) override
    {
        auto b = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (3.0f);
        const float r = juce::jmin (b.getWidth(), b.getHeight()) * 0.5f;
        const auto c = b.getCentre();
        const auto accent = s.findColour (juce::Slider::rotarySliderFillColourId);
        const float ang = startA + pos * (endA - startA);

        juce::Path track;
        track.addCentredArc (c.x, c.y, r, r, 0.0f, startA, endA, true);
        g.setColour (accent.withAlpha (0.25f));
        g.strokePath (track, juce::PathStrokeType (2.0f));
        juce::Path val;
        val.addCentredArc (c.x, c.y, r, r, 0.0f, startA, ang, true);
        g.setColour (accent);
        g.strokePath (val, juce::PathStrokeType (2.5f));

        const float kr = r * 0.82f;
        g.setGradientFill (juce::ColourGradient (juce::Colour (0xff50535c), c.x - kr * 0.4f, c.y - kr * 0.6f,
                                                 juce::Colour (0xff0b0c0f), c.x + kr * 0.5f, c.y + kr * 0.7f, true));
        g.fillEllipse (c.x - kr, c.y - kr, kr * 2.0f, kr * 2.0f);

        juce::Path ptrPath;
        ptrPath.addRoundedRectangle (-1.5f, -kr * 0.9f, 3.0f, kr * 0.5f, 1.0f);
        g.setColour (juce::Colours::white);
        g.fillPath (ptrPath, juce::AffineTransform::rotation (ang).translated (c.x, c.y));
    }

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& b, bool, bool) override
    {
        const bool on = b.getToggleState();
        const auto accent = b.findColour (juce::ToggleButton::tickColourId);
        auto r = b.getLocalBounds().toFloat().reduced (1.0f);
        g.setColour (juce::Colour (0xff2a2c33));
        g.fillRoundedRectangle (r, 6.0f);
        g.setColour (on ? accent : juce::Colour (0xff555862));
        g.drawRoundedRectangle (r, 6.0f, 1.5f);
        const float led = 12.0f;
        g.setColour (on ? juce::Colour (0xff40e070) : juce::Colour (0xff1a3a22));
        g.fillEllipse (r.getX() + 10.0f, r.getCentreY() - led * 0.5f, led, led);
        g.setColour (juce::Colours::white.withAlpha (on ? 1.0f : 0.5f));
        g.setFont (juce::Font (12.0f, juce::Font::bold));
        g.drawText (on ? "ACTIVE" : "BYPASS", r.withTrimmedLeft (26.0f), juce::Justification::centred);
    }
};

// Una carta del tarot que es a la vez un pedal
class PedalCard : public juce::Component
{
public:
    PedalCard (juce::AudioProcessorValueTreeState& apvts, int idx, KnobLAF& laf);
    ~PedalCard() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using BA = juce::AudioProcessorValueTreeState::ButtonAttachment;
    int index;
    juce::Slider knob[3], mix;
    juce::ToggleButton power;
    std::unique_ptr<SA> ka[3], ma;
    std::unique_ptr<BA> pa;
};

class ArcanaEditor : public juce::AudioProcessorEditor
{
public:
    explicit ArcanaEditor (ArcanaProcessor&);
    ~ArcanaEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    ArcanaProcessor& proc;
    KnobLAF laf;
    std::unique_ptr<PedalCard> cards[7];
    juce::Slider masterMix, masterOut;
    std::unique_ptr<SA> mmA, moA;
};
