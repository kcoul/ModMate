/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ModMateAudioProcessor::ModMateAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

ModMateAudioProcessor::~ModMateAudioProcessor()
{
}

//==============================================================================
const String ModMateAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ModMateAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ModMateAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ModMateAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ModMateAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ModMateAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ModMateAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ModMateAudioProcessor::setCurrentProgram (int index)
{
}

const String ModMateAudioProcessor::getProgramName (int index)
{
    return {};
}

void ModMateAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ModMateAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ModMateAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ModMateAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

static MidiBuffer midiOut;

void ModMateAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    // the audio buffer in a midi effect will have zero channels!
    jassert(buffer.getNumChannels() == 0);

    midiOut.clear();

    MidiMessage msg;
    int samplePos;
    for (MidiBuffer::Iterator it(midiMessages); it.getNextEvent(msg, samplePos);)
    {
        if (msg.isPitchWheel())
        {
            float pitchBendUp = 0.0f;
            float pitchBendDown = 0.0f;

            int pwv = msg.getPitchWheelValue();
            if (pwv >= 8192)
            {
                float pitchBendUp = (pwv - 8192) / 8191.0f;
                DBG("pitchBendUp " + String(pitchBendUp));
            }
            else
            {
                float pitchBendDown = (8192 - pwv) / 8192.0f;
                DBG("pitchBendDown " + String(pitchBendDown));
            }
        }
        else if (msg.isControllerOfType(1))
        {
            //float modWheel = msg.getControllerValue() / 127.0f;

            int cval = msg.getControllerValue();
            midiOut.addEvent(MidiMessage::controllerEvent(msg.getChannel(), 1, cval), samplePos);
            midiOut.addEvent(MidiMessage::controllerEvent(msg.getChannel(), 2, cval), samplePos);
            midiOut.addEvent(MidiMessage::controllerEvent(msg.getChannel(), 4, cval), samplePos);
            midiOut.addEvent(MidiMessage::controllerEvent(msg.getChannel(), 67, cval), samplePos);
        }
        else
        {
            // all other messages are passed through
            midiOut.addEvent(msg, samplePos);
        }
    }

    int inCount = midiMessages.getNumEvents();
    midiMessages.swapWith(midiOut);
    int outCount = midiMessages.getNumEvents();

    if (inCount != outCount)
        DBG("in " + String(inCount) + " out " + String(outCount));
}

//==============================================================================
bool ModMateAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ModMateAudioProcessor::createEditor()
{
    return new ModMateAudioProcessorEditor (*this);
}

//==============================================================================
void ModMateAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    destData.setSize(1);
}

void ModMateAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ModMateAudioProcessor();
}
