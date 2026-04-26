#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

struct GlassKey : public juce::Component
{
    GlassKey(SolarGlassesAudioProcessor& p, int note, bool isBlack)
        : processor(p), midiNote(note), isBlackKey(isBlack) {
    }

    float alpha = 0.0f;
    SolarGlassesAudioProcessor& processor;
    int midiNote;
    bool isBlackKey;

    void paint(juce::Graphics& g) override
    {
        auto area = getLocalBounds().reduced(1).toFloat();
        juce::Colour base = isBlackKey ? juce::Colour(0xFF0F1419) : juce::Colour(0xFFDDEEFF);
        g.setColour(base.withAlpha(0.7f));
        g.fillRoundedRectangle(area, 2.0f);

        if (alpha > 0.01f)
        {
            g.setColour(juce::Colours::cyan.withAlpha(alpha * 0.6f));
            g.fillRoundedRectangle(area, 2.0f);
        }
        g.setColour(juce::Colours::white.withAlpha(0.1f));
        g.drawRoundedRectangle(area, 2.0f, 0.5f);
    }

    void mouseDown(const juce::MouseEvent&) override { processor.noteOn(midiNote, 0.8f); }
    void mouseUp(const juce::MouseEvent&) override { processor.noteOff(midiNote); }
};

class SolarGlassesAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    SolarGlassesAudioProcessorEditor(SolarGlassesAudioProcessor&);
    ~SolarGlassesAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    SolarGlassesAudioProcessor& audioProcessor;
    juce::OwnedArray<GlassKey> whiteKeys;
    juce::OwnedArray<GlassKey> blackKeys;
    juce::Image bgImage;

    bool isBlackKey(int midiNote);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SolarGlassesAudioProcessorEditor)
};

//Jagoda Jazownik
