#include "PluginProcessor.h"
#include "PluginEditor.h"

SolarGlassesAudioProcessorEditor::SolarGlassesAudioProcessorEditor(SolarGlassesAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    
    bgImage = juce::ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);

    for (int note = 36; note <= 84; ++note)
    {
        auto* k = new GlassKey(audioProcessor, note, isBlackKey(note));
        if (isBlackKey(note)) blackKeys.add(k);
        else whiteKeys.add(k);
        addAndMakeVisible(k);
    }

    setSize(1024, 680);
    startTimerHz(60);
}

SolarGlassesAudioProcessorEditor::~SolarGlassesAudioProcessorEditor() { stopTimer(); }

void SolarGlassesAudioProcessorEditor::paint(juce::Graphics& g)
{
    if (bgImage.isValid())
        g.drawImageWithin(bgImage, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::fillDestination);
    else
        g.fillAll(juce::Colours::black);
}

void SolarGlassesAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().withTrimmedTop(490).reduced(15, 10);
    int numWhite = whiteKeys.size();
    float keyWidth = (float)area.getWidth() / numWhite;

    for (int i = 0; i < numWhite; ++i)
        whiteKeys[i]->setBounds(area.getX() + (int)(i * keyWidth), area.getY(), (int)keyWidth, area.getHeight());

    for (int i = 0; i < blackKeys.size(); ++i)
    {
        int note = blackKeys[i]->midiNote;
        for (int w = 0; w < whiteKeys.size(); ++w)
        {
            if (whiteKeys[w]->midiNote == note - 1)
            {
                int xPos = whiteKeys[w]->getRight() - (int)(keyWidth * 0.35f);
                blackKeys[i]->setBounds(xPos, area.getY(), (int)(keyWidth * 0.7f), (int)(area.getHeight() * 0.6f));
                blackKeys[i]->toFront(false);
                break;
            }
        }
    }
}

void SolarGlassesAudioProcessorEditor::timerCallback()
{
    auto updateArray = [this](auto& array) {
        for (auto* k : array) {
            if (audioProcessor.isNoteActive(k->midiNote)) k->alpha = 1.0f;
            else k->alpha *= 0.92f;
            if (k->alpha > 0.005f) k->repaint();
        }
        };
    updateArray(whiteKeys);
    updateArray(blackKeys);
}

bool SolarGlassesAudioProcessorEditor::isBlackKey(int midiNote)
{
    int n = midiNote % 12;
    return (n == 1 || n == 3 || n == 6 || n == 8 || n == 10);
}

//Jagoda Jazownik