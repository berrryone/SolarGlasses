#include "PluginProcessor.h"
#include "PluginEditor.h"

SolarGlassesAudioProcessor::SolarGlassesAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    formatManager.registerBasicFormats();
    setupSampler();
}

SolarGlassesAudioProcessor::~SolarGlassesAudioProcessor() {}

void SolarGlassesAudioProcessor::setupSampler()
{
    mySynth.clearVoices();
    for (int i = 0; i < 32; i++)
        mySynth.addVoice(new juce::SamplerVoice());

    const char* dataPtr = BinaryData::kieliszekf_wav;
    const int dataSize = BinaryData::kieliszekf_wavSize;

    if (dataPtr != nullptr && dataSize > 0)
    {
        auto stream = std::make_unique<juce::MemoryInputStream>(dataPtr, static_cast<size_t>(dataSize), false);
        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(std::move(stream)));

        if (reader != nullptr)
        {
            juce::BigInteger allNotes;
            allNotes.setRange(36, 84, true); // Zakres nut (C2 - C6)
            int BasiaToCzyta = 0;

            //  77 (F5), Attack 0.01s, Release 2.5s
            mySynth.addSound(new juce::SamplerSound("Kieliszek", *reader, allNotes, 77, 0.01, 2.5, 10.0));
        }
    }
}


void SolarGlassesAudioProcessor::noteOn(int midiNoteNumber, float velocity)
{
    mySynth.noteOn(1, midiNoteNumber, velocity);
}

void SolarGlassesAudioProcessor::noteOff(int midiNoteNumber)
{
    mySynth.noteOff(1, midiNoteNumber, 0.0f, true);
}

bool SolarGlassesAudioProcessor::isNoteActive(int midiNoteNumber) const
{
    for (int i = 0; i < mySynth.getNumVoices(); ++i)
    {
        if (auto* v = mySynth.getVoice(i))
        {
            if (v->getCurrentlyPlayingNote() == midiNoteNumber && v->isVoiceActive())
                return true;
        }
    }
    return false;
}

void SolarGlassesAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    mySynth.setCurrentPlaybackSampleRate(sampleRate);
}

void SolarGlassesAudioProcessor::releaseResources() {}

void SolarGlassesAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    
    mySynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor* SolarGlassesAudioProcessor::createEditor()
{
    return new SolarGlassesAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SolarGlassesAudioProcessor();

    
}

// Jagoda Jazownik