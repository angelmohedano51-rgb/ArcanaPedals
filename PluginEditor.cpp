#include "PluginEditor.h"

using juce::Colour;

namespace
{
void coin (juce::Graphics& g, float cx, float cy, float r)
{
    g.setColour (Colour (0xfff2c230));
    g.fillEllipse (cx - r, cy - r, r * 2.0f, r * 2.0f);
    g.setColour (juce::Colours::black);
    g.drawEllipse (cx - r, cy - r, r * 2.0f, r * 2.0f, 1.2f);
    juce::Path p;
    p.addStar ({ cx, cy }, 5, r * 0.35f, r * 0.75f, 0.0f);
    g.strokePath (p, juce::PathStrokeType (1.0f));
}

void drawArt (juce::Graphics& g, int i, juce::Rectangle<float> r)
{
    auto X = [&] (float f) { return r.getX() + f * r.getWidth(); };
    auto Y = [&] (float f) { return r.getY() + f * r.getHeight(); };
    auto W = [&] (float f) { return f * r.getWidth(); };
    auto H = [&] (float f) { return f * r.getHeight(); };
    auto rect = [&] (juce::uint32 c, float x, float y, float w, float h)
    { g.setColour (Colour (c)); g.fillRect (X (x), Y (y), W (w), H (h)); };
    auto ell = [&] (juce::uint32 c, float cx, float cy, float w, float h)
    { g.setColour (Colour (c)); g.fillEllipse (X (cx - w * 0.5f), Y (cy - h * 0.5f), W (w), H (h)); };
    auto star = [&] (juce::uint32 c, float cx, float cy, float rad, int pts)
    {
        juce::Path p; p.addStar ({ X (cx), Y (cy) }, pts, W (rad * 0.35f), W (rad), 0.0f);
        g.setColour (Colour (c)); g.fillPath (p);
    };
    const juce::uint32 skin = 0xfff0c8a0, gold = 0xfff2c230, blue = 0xff3aa0c8;

    g.saveState();
    g.reduceClipRegion (r.toNearestInt());
    switch (i)
    {
        case 0: // Two of Cups
            rect (blue, 0, 0, 1, 1);
            ell (0xff58b04a, 0.5f, 0.82f, 1.2f, 0.2f);
            rect (0xfff0d048, 0, 0.9f, 1, 0.1f);
            ell (0xffb03030, 0.5f, 0.14f, 0.22f, 0.2f);
            ell (0xffb03030, 0.27f, 0.13f, 0.3f, 0.09f);
            ell (0xffb03030, 0.73f, 0.13f, 0.3f, 0.09f);
            rect (0xff5a7ab0, 0.14f, 0.36f, 0.07f, 0.52f);
            rect (0xfff0f0f0, 0.2f, 0.36f, 0.2f, 0.52f);
            ell (skin, 0.3f, 0.3f, 0.13f, 0.1f);
            rect (0xff222222, 0.62f, 0.36f, 0.2f, 0.34f);
            rect (0xffe8d040, 0.64f, 0.7f, 0.16f, 0.18f);
            ell (skin, 0.72f, 0.3f, 0.13f, 0.1f);
            ell (gold, 0.44f, 0.5f, 0.1f, 0.14f);
            ell (gold, 0.58f, 0.5f, 0.1f, 0.14f);
            break;
        case 1: // Two of Pentacles
            rect (blue, 0, 0, 1, 1);
            rect (0xff6a8aa0, 0, 0.82f, 1, 0.18f);
            g.setColour (Colour (0xff2e8a3a));
            g.drawEllipse (X (0.1f), Y (0.4f), W (0.42f), H (0.26f), 4.0f);
            g.drawEllipse (X (0.48f), Y (0.4f), W (0.42f), H (0.26f), 4.0f);
            rect (0xff8a3a2a, 0.4f, 0.42f, 0.2f, 0.3f);
            rect (0xffc02020, 0.42f, 0.72f, 0.16f, 0.12f);
            rect (0xffd02020, 0.42f, 0.1f, 0.16f, 0.24f);
            ell (skin, 0.5f, 0.37f, 0.14f, 0.1f);
            coin (g, X (0.14f), Y (0.52f), W (0.11f));
            coin (g, X (0.86f), Y (0.52f), W (0.11f));
            break;
        case 2: // Eight of Pentacles
            rect (0xffb8b8a8, 0, 0, 1, 1);
            rect (0xfff0d048, 0, 0.8f, 1, 0.2f);
            for (int k = 0; k < 6; ++k) coin (g, X (0.88f), Y (0.1f + (float) k * 0.14f), W (0.09f));
            rect (0xff9a4a30, 0.12f, 0.7f, 0.6f, 0.06f);
            rect (0xff3a5aa0, 0.3f, 0.45f, 0.22f, 0.25f);
            ell (skin, 0.4f, 0.38f, 0.14f, 0.1f);
            coin (g, X (0.28f), Y (0.85f), W (0.1f));
            break;
        case 3: // Page of Cups
            rect (0xffa0a09a, 0, 0, 1, 1);
            rect (0xff3a90b8, 0, 0.66f, 1, 0.08f);
            rect (0xffe8d040, 0, 0.8f, 1, 0.2f);
            rect (0xffe070b0, 0.4f, 0.55f, 0.2f, 0.28f);
            rect (0xff3a8aa8, 0.35f, 0.32f, 0.3f, 0.26f);
            ell (skin, 0.5f, 0.24f, 0.16f, 0.12f);
            rect (0xff3a8aa8, 0.38f, 0.1f, 0.24f, 0.1f);
            rect (gold, 0.16f, 0.36f, 0.12f, 0.1f);
            rect (gold, 0.2f, 0.46f, 0.04f, 0.08f);
            break;
        case 4: // Page of Swords
            rect (blue, 0, 0, 1, 1);
            ell (0xffe8e8e8, 0.2f, 0.5f, 0.3f, 0.2f);
            ell (0xffe8e8e8, 0.8f, 0.55f, 0.35f, 0.25f);
            ell (0xffe8e8e8, 0.75f, 0.3f, 0.25f, 0.14f);
            rect (0xff58a040, 0, 0.82f, 1, 0.18f);
            rect (0xffe8d040, 0.42f, 0.58f, 0.16f, 0.26f);
            rect (0xffd878b0, 0.36f, 0.34f, 0.28f, 0.26f);
            ell (skin, 0.5f, 0.26f, 0.14f, 0.1f);
            g.setColour (Colour (0xffc0c8d0));
            g.drawLine (X (0.7f), Y (0.02f), X (0.58f), Y (0.36f), 3.0f);
            break;
        case 5: // The Star
            rect (blue, 0, 0, 1, 1);
            star (0xfff2d230, 0.5f, 0.22f, 0.2f, 8);
            star (0xffffffff, 0.1f, 0.1f, 0.07f, 8);
            star (0xffffffff, 0.9f, 0.1f, 0.07f, 8);
            star (0xffffffff, 0.1f, 0.32f, 0.07f, 8);
            star (0xffffffff, 0.9f, 0.32f, 0.07f, 8);
            star (0xffffffff, 0.25f, 0.42f, 0.06f, 8);
            star (0xffffffff, 0.75f, 0.42f, 0.06f, 8);
            rect (0xff58a040, 0, 0.62f, 1, 0.38f);
            ell (0xff3a6a9a, 0.45f, 0.88f, 0.8f, 0.2f);
            ell (0xfff0c890, 0.52f, 0.76f, 0.14f, 0.2f);
            ell (skin, 0.5f, 0.64f, 0.1f, 0.08f);
            break;
        default: // Three of Swords
            rect (0xffa8a8a8, 0, 0, 1, 1);
            g.setColour (Colour (0xff5a5a5a));
            for (int k = 0; k < 12; ++k)
                g.drawLine (X ((float) k / 11.0f), Y (0.15f), X ((float) k / 11.0f - 0.03f), Y (0.9f), 1.0f);
            ell (0xffd02030, 0.4f, 0.42f, 0.26f, 0.2f);
            ell (0xffd02030, 0.6f, 0.42f, 0.26f, 0.2f);
            {
                juce::Path t;
                t.startNewSubPath (X (0.28f), Y (0.5f));
                t.lineTo (X (0.72f), Y (0.5f));
                t.lineTo (X (0.5f), Y (0.85f));
                t.closeSubPath();
                g.setColour (Colour (0xffd02030));
                g.fillPath (t);
            }
            g.setColour (Colour (0xff4a8aaa));
            g.drawLine (X (0.25f), Y (0.1f), X (0.45f), Y (0.62f), 4.0f);
            g.drawLine (X (0.5f), Y (0.06f), X (0.5f), Y (0.72f), 4.0f);
            g.drawLine (X (0.75f), Y (0.1f), X (0.55f), Y (0.62f), 4.0f);
            break;
    }
    g.restoreState();
    g.setColour (juce::Colours::black);
    g.drawRect (r, 2.0f);
}
} // namespace

//==============================================================================
PedalCard::PedalCard (juce::AudioProcessorValueTreeState& apvts, int idx, KnobLAF& laf)
    : index (idx)
{
    const auto& f = kFx[idx];
    const Colour accent (f.accent);
    const juce::String id (f.id);
    const float pi = juce::MathConstants<float>::pi;

    auto setup = [&] (juce::Slider& s)
    {
        s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        s.setRotaryParameters (pi * 1.25f, pi * 2.75f, true);
        s.setColour (juce::Slider::rotarySliderFillColourId, accent);
        s.setLookAndFeel (&laf);
        addAndMakeVisible (s);
    };
    const char* sfx[3] = { "_a", "_b", "_c" };
    for (int k = 0; k < 3; ++k)
    {
        setup (knob[k]);
        ka[k] = std::make_unique<SA> (apvts, id + sfx[k], knob[k]);
    }
    setup (mix);
    ma = std::make_unique<SA> (apvts, id + "_mix", mix);

    power.setColour (juce::ToggleButton::tickColourId, accent);
    power.setLookAndFeel (&laf);
    addAndMakeVisible (power);
    pa = std::make_unique<BA> (apvts, id + "_on", power);
}

PedalCard::~PedalCard()
{
    for (auto& k : knob) k.setLookAndFeel (nullptr);
    mix.setLookAndFeel (nullptr);
    power.setLookAndFeel (nullptr);
}

void PedalCard::resized()
{
    for (int k = 0; k < 3; ++k) knob[k].setBounds (10 + k * 43, 258, 43, 42);
    mix.setBounds (55, 336, 40, 40);
    power.setBounds (25, 424, 100, 28);
}

void PedalCard::paint (juce::Graphics& g)
{
    const auto& f = kFx[index];
    const auto b = getLocalBounds().toFloat();
    g.setColour (Colour (0xfff2e6c8));
    g.fillRoundedRectangle (b, 10.0f);
    g.setColour (juce::Colours::black);
    g.drawRoundedRectangle (b.reduced (1.0f), 10.0f, 2.0f);

    g.setFont (juce::Font (12.0f, juce::Font::bold));
    g.drawText (f.roman, 0, 7, getWidth(), 18, juce::Justification::centred);

    drawArt (g, index, { 10.0f, 28.0f, 130.0f, 160.0f });

    g.setColour (Colour (0xffd8d0b8));
    g.fillRect (10.0f, 188.0f, 130.0f, 22.0f);
    g.setColour (juce::Colours::black);
    g.drawRect (10.0f, 188.0f, 130.0f, 22.0f, 1.5f);
    g.setFont (juce::Font (11.0f, juce::Font::bold));
    g.drawText (f.title, 10, 188, 130, 22, juce::Justification::centred);

    g.setColour (Colour (0xff17181c));
    g.fillRoundedRectangle (6.0f, 216.0f, 138.0f, 244.0f, 8.0f);
    g.setColour (Colour (f.accent));
    g.setFont (juce::Font (15.0f, juce::Font::bold));
    g.drawText (f.name, 6, 222, 138, 22, juce::Justification::centred);

    g.setColour (juce::Colours::white.withAlpha (0.75f));
    g.setFont (juce::Font (9.5f, juce::Font::bold));
    for (int k = 0; k < 3; ++k)
        g.drawText (f.knobs[k], 10 + k * 43, 302, 43, 12, juce::Justification::centred);
    g.drawText ("MIX", 55, 378, 40, 12, juce::Justification::centred);
}

//==============================================================================
ArcanaEditor::ArcanaEditor (ArcanaProcessor& p)
    : juce::AudioProcessorEditor (&p), proc (p)
{
    for (int i = 0; i < 7; ++i)
    {
        cards[i] = std::make_unique<PedalCard> (p.apvts, i, laf);
        addAndMakeVisible (*cards[i]);
    }
    const float pi = juce::MathConstants<float>::pi;
    for (auto* s : { &masterMix, &masterOut })
    {
        s->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        s->setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        s->setRotaryParameters (pi * 1.25f, pi * 2.75f, true);
        s->setColour (juce::Slider::rotarySliderFillColourId, Colour (0xfff2d230));
        s->setLookAndFeel (&laf);
        addAndMakeVisible (*s);
    }
    mmA = std::make_unique<SA> (p.apvts, "master_mix", masterMix);
    moA = std::make_unique<SA> (p.apvts, "master_out", masterOut);
    setSize (1146, 590);
}

ArcanaEditor::~ArcanaEditor()
{
    masterMix.setLookAndFeel (nullptr);
    masterOut.setLookAndFeel (nullptr);
}

void ArcanaEditor::resized()
{
    for (int i = 0; i < 7; ++i)
        cards[i]->setBounds (12 + i * 162, 112, 150, 470);
    masterMix.setBounds (getWidth() - 190, 22, 56, 56);
    masterOut.setBounds (getWidth() - 100, 22, 56, 56);
}

void ArcanaEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient (Colour (0xff1a1b22), 0, 0, Colour (0xff0b0b10), 0, (float) getHeight(), false));
    g.fillAll();

    // Mini carta de El Loco como emblema
    juce::Rectangle<float> fool (18.0f, 10.0f, 56.0f, 88.0f);
    g.setColour (Colour (0xfff2e6c8));
    g.fillRoundedRectangle (fool, 6.0f);
    g.setColour (Colour (0xfff5e03a));
    g.fillRect (fool.reduced (4.0f));
    g.setColour (juce::Colours::white);
    g.fillEllipse (fool.getRight() - 28.0f, fool.getY() + 6.0f, 20.0f, 20.0f);
    g.setColour (Colour (0xff2a7a4a));
    g.fillRect (fool.getCentreX() - 8.0f, fool.getY() + 36.0f, 16.0f, 30.0f);
    g.setColour (Colour (0xfff0c8a0));
    g.fillEllipse (fool.getCentreX() - 6.0f, fool.getY() + 26.0f, 12.0f, 12.0f);
    g.setColour (juce::Colours::black);
    g.drawRoundedRectangle (fool.reduced (1.0f), 6.0f, 2.0f);

    g.setColour (Colour (0xfff2d230));
    g.setFont (juce::Font (30.0f, juce::Font::bold));
    g.drawText ("ARCANA PEDALS", 90, 18, 500, 40, juce::Justification::centredLeft);
    g.setColour (juce::Colours::white.withAlpha (0.55f));
    g.setFont (juce::Font (13.0f, juce::Font::plain));
    g.drawText ("tarot multi-effect  v0.1", 92, 58, 400, 20, juce::Justification::centredLeft);

    g.setColour (juce::Colours::white.withAlpha (0.75f));
    g.setFont (juce::Font (10.0f, juce::Font::bold));
    g.drawText ("MIX", masterMix.getX(), masterMix.getBottom() + 2, 56, 12, juce::Justification::centred);
    g.drawText ("OUT", masterOut.getX(), masterOut.getBottom() + 2, 56, 12, juce::Justification::centred);
}
