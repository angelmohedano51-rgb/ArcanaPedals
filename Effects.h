#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
#include <cmath>
#include <vector>

namespace arcana
{
struct Params { float a = 0.5f, b = 0.5f, c = 0.5f; };

static constexpr float twoPi = 6.28318530718f;

// Linha de delay con lectura interpolada
struct Line
{
    std::vector<float> d;
    int w = 0;
    void init (int n) { d.assign ((size_t) n, 0.0f); w = 0; }
    void push (float x) { d[(size_t) w] = x; if (++w >= (int) d.size()) w = 0; }
    float read (float delaySamples) const
    {
        const float n = (float) d.size();
        float pos = (float) w - delaySamples;
        while (pos < 0.0f) pos += n;
        int i0 = (int) pos;
        const float fr = pos - (float) i0;
        int i1 = i0 + 1;
        if (i1 >= (int) d.size()) i1 = 0;
        return d[(size_t) i0] * (1.0f - fr) + d[(size_t) i1] * fr;
    }
};

class Effect
{
public:
    virtual ~Effect() = default;
    virtual void prepare (double sr) { sampleRate = sr; }
    // El buffer es estereo y se procesa en sitio. Devuelve solo la senal "wet".
    virtual void process (juce::AudioBuffer<float>& buf, const Params& p, double bpm, double ppq) = 0;
protected:
    double sampleRate = 44100.0;
};

//==============================================================================
// XVII THE STAR: reverb
class ReverbFx : public Effect
{
public:
    void prepare (double sr) override { Effect::prepare (sr); rv.setSampleRate (sr); rv.reset(); }
    void process (juce::AudioBuffer<float>& buf, const Params& p, double, double) override
    {
        juce::Reverb::Parameters rp;
        rp.roomSize = p.a; rp.damping = p.b; rp.width = p.c;
        rp.wetLevel = 0.33f; rp.dryLevel = 0.0f; rp.freezeMode = 0.0f;
        rv.setParameters (rp);
        rv.processStereo (buf.getWritePointer (0), buf.getWritePointer (1), buf.getNumSamples());
    }
private:
    juce::Reverb rv;
};

//==============================================================================
// II TWO OF CUPS: chorus
class ChorusFx : public Effect
{
public:
    void prepare (double sr) override
    {
        Effect::prepare (sr);
        for (auto& l : line) l.init ((int) (0.05 * sr) + 4);
        phase = 0.0;
    }
    void process (juce::AudioBuffer<float>& buf, const Params& p, double, double) override
    {
        const double rate = 0.1 + (double) p.a * p.a * 7.0;
        const float depth = p.b * 0.006f;
        const float spread = p.c * 0.5f;
        const double inc = rate / sampleRate;
        for (int i = 0; i < buf.getNumSamples(); ++i)
        {
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buf.getWritePointer (ch);
                const float lfo = std::sin (twoPi * ((float) phase + (float) ch * spread));
                const float dly = (0.015f + depth * lfo) * (float) sampleRate;
                line[ch].push (data[i]);
                data[i] = line[ch].read (dly);
            }
            phase += inc;
            if (phase >= 1.0) phase -= 1.0;
        }
    }
private:
    std::array<Line, 2> line;
    double phase = 0.0;
};

//==============================================================================
// II TWO OF PENTACLES: delay
class DelayFx : public Effect
{
public:
    void prepare (double sr) override
    {
        Effect::prepare (sr);
        for (auto& l : line) l.init ((int) (2.1 * sr));
        lp[0] = lp[1] = 0.0f;
        cur = 0.3f;
    }
    void process (juce::AudioBuffer<float>& buf, const Params& p, double, double) override
    {
        const float target = 0.02f + p.a * p.a * 0.98f;
        const float fb = p.b * 0.95f;
        const float coef = 0.05f + p.c * 0.95f;
        for (int i = 0; i < buf.getNumSamples(); ++i)
        {
            cur += (target - cur) * 0.0005f;
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buf.getWritePointer (ch);
                const float dl = line[ch].read (cur * (float) sampleRate);
                lp[ch] += coef * (dl - lp[ch]);
                line[ch].push (data[i] + lp[ch] * fb);
                data[i] = dl;
            }
        }
    }
private:
    std::array<Line, 2> line;
    float lp[2] { 0.0f, 0.0f };
    float cur = 0.3f;
};

//==============================================================================
// VIII EIGHT OF PENTACLES: granular
class GranularFx : public Effect
{
public:
    void prepare (double sr) override
    {
        Effect::prepare (sr);
        size = (int) (3.0 * sr) + 8;
        for (auto& b : buf) b.assign ((size_t) size, 0.0f);
        w = 0;
        for (auto& g : grains) g.on = false;
    }
    void process (juce::AudioBuffer<float>& io, const Params& p, double, double) override
    {
        const float grainLen = (0.01f + p.a * p.a * 0.29f) * (float) sampleRate;
        const float density = 1.0f + p.b * p.b * 39.0f;
        const float inc = std::pow (2.0f, (p.c - 0.5f) * 24.0f / 12.0f);
        const float norm = 1.0f / std::sqrt (std::max (1.0f, density * grainLen / (float) sampleRate));
        const float spawnProb = density / (float) sampleRate;

        for (int i = 0; i < io.getNumSamples(); ++i)
        {
            for (int ch = 0; ch < 2; ++ch)
                buf[(size_t) ch][(size_t) w] = io.getReadPointer (ch)[i];

            if (rnd.nextFloat() < spawnProb)
            {
                for (auto& g : grains)
                {
                    if (! g.on)
                    {
                        g.on = true;
                        g.age = 0;
                        g.len = std::max (32, (int) (grainLen * (0.7f + 0.6f * rnd.nextFloat())));
                        g.inc = inc;
                        const float offset = (float) g.len * std::max (inc, 1.0f) + rnd.nextFloat() * 0.5f * (float) sampleRate;
                        g.pos = (float) w - offset;
                        while (g.pos < 0.0f) g.pos += (float) size;
                        break;
                    }
                }
            }

            float out[2] { 0.0f, 0.0f };
            for (auto& g : grains)
            {
                if (! g.on) continue;
                const float ph = (float) g.age / (float) g.len;
                const float s = std::sin (3.14159265f * ph);
                const float win = s * s;
                const int i0 = (int) g.pos;
                const float fr = g.pos - (float) i0;
                const int i1 = (i0 + 1 >= size) ? 0 : i0 + 1;
                for (int ch = 0; ch < 2; ++ch)
                    out[ch] += (buf[(size_t) ch][(size_t) i0] * (1.0f - fr) + buf[(size_t) ch][(size_t) i1] * fr) * win;
                g.pos += g.inc;
                if (g.pos >= (float) size) g.pos -= (float) size;
                if (++g.age >= g.len) g.on = false;
            }
            for (int ch = 0; ch < 2; ++ch)
                io.getWritePointer (ch)[i] = out[ch] * norm;

            if (++w >= size) w = 0;
        }
    }
private:
    struct Grain { bool on = false; float pos = 0.0f, inc = 1.0f; int age = 0, len = 1; };
    std::array<Grain, 48> grains;
    std::vector<float> buf[2];
    int w = 0, size = 0;
    juce::Random rnd;
};

//==============================================================================
// PAGE OF CUPS: lo-fi (bitcrusher, reduccion de sample rate, wobble de cinta)
class LofiFx : public Effect
{
public:
    void prepare (double sr) override
    {
        Effect::prepare (sr);
        for (auto& l : wob) l.init ((int) (0.05 * sr) + 4);
        held[0] = held[1] = 0.0f; cnt = 0.0f; ph1 = ph2 = 0.0;
    }
    void process (juce::AudioBuffer<float>& buf, const Params& p, double, double) override
    {
        const float bits = 16.0f - p.a * 12.0f;
        const float levels = std::pow (2.0f, bits - 1.0f);
        const float hold = 1.0f / (1.0f - p.b * 0.95f);
        const float wobAmt = p.c * 0.003f;
        for (int i = 0; i < buf.getNumSamples(); ++i)
        {
            const float lfo = std::sin (twoPi * (float) ph1) + 0.3f * std::sin (twoPi * (float) ph2);
            ph1 += 0.6 / sampleRate; if (ph1 >= 1.0) ph1 -= 1.0;
            ph2 += 7.0 / sampleRate; if (ph2 >= 1.0) ph2 -= 1.0;
            const float dly = (0.004f + wobAmt * lfo) * (float) sampleRate;
            const bool sampleNow = (cnt >= hold);
            if (sampleNow) cnt -= hold;
            cnt += 1.0f;
            for (int ch = 0; ch < 2; ++ch)
            {
                auto* data = buf.getWritePointer (ch);
                wob[ch].push (data[i]);
                const float x = wob[ch].read (dly);
                if (sampleNow)
                    held[ch] = std::round (x * levels) / levels;
                data[i] = held[ch];
            }
        }
    }
private:
    std::array<Line, 2> wob;
    float held[2] { 0.0f, 0.0f };
    float cnt = 0.0f;
    double ph1 = 0.0, ph2 = 0.0;
};

//==============================================================================
// PAGE OF SWORDS: noise (blanco a marron, mas crackle de vinilo)
class NoiseFx : public Effect
{
public:
    void process (juce::AudioBuffer<float>& buf, const Params& p, double, double) override
    {
        const float level = p.a * p.a * 0.25f;
        const float k = 1.0f - 0.97f * p.b;
        const float comp = 1.0f / std::sqrt (k / (2.0f - k));
        const float crackProb = p.c * p.c * 0.0008f;
        for (int i = 0; i < buf.getNumSamples(); ++i)
        {
            for (int ch = 0; ch < 2; ++ch)
            {
                const float white = rnd.nextFloat() * 2.0f - 1.0f;
                lp[ch] += k * (white - lp[ch]);
                if (rnd.nextFloat() < crackProb)
                    crack[ch] = (rnd.nextFloat() * 2.0f - 1.0f) * 0.8f;
                crack[ch] *= 0.9f;
                auto* data = buf.getWritePointer (ch);
                data[i] += lp[ch] * comp * level + crack[ch] * 0.5f;
            }
        }
    }
private:
    juce::Random rnd;
    float lp[2] { 0.0f, 0.0f };
    float crack[2] { 0.0f, 0.0f };
};

//==============================================================================
// III THREE OF SWORDS: sidechain interno sincronizado al tempo del host
class SidechainFx : public Effect
{
public:
    void process (juce::AudioBuffer<float>& buf, const Params& p, double bpm, double ppq) override
    {
        static const double beatsPerCycle[5] { 4.0, 2.0, 1.0, 0.5, 0.25 };
        const int idx = juce::jlimit (0, 4, (int) (p.a * 4.999f));
        const double bpc = beatsPerCycle[idx];
        const float depth = p.b;
        const float curve = 1.0f + p.c * 5.0f;
        if (ppq >= 0.0)
            phase = std::fmod (ppq / bpc, 1.0);
        const double inc = (bpm / 60.0) / sampleRate / bpc;
        for (int i = 0; i < buf.getNumSamples(); ++i)
        {
            const float target = 1.0f - depth * std::pow (1.0f - (float) phase, curve);
            gain += (target - gain) * 0.02f;
            for (int ch = 0; ch < 2; ++ch)
                buf.getWritePointer (ch)[i] *= gain;
            phase += inc;
            if (phase >= 1.0) phase -= 1.0;
        }
    }
private:
    double phase = 0.0;
    float gain = 1.0f;
};
} // namespace arcana
