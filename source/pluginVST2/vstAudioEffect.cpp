#include "vstAudioEffect.h"

#include "rtems.h"
#include "chameleon.h"

//-------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new MyVSTAudioEffect (audioMaster);
}

//-------------------------------------------------------------------------------------------------------
MyVSTAudioEffect::MyVSTAudioEffect (audioMasterCallback audioMaster)
: AudioEffectX		(audioMaster, 1, 1)	// 1 program, 1 parameter
{
	TRACE( "Initializing plugin");

	setNumInputs  (2);					// stereo in
	setNumOutputs (2);					// stereo out

	setUniqueID   ('nChE');				// identify
	canProcessReplacing ();				// supports replacing output

	isSynth();

	programsAreChunks();

	setSampleRate( getSampleRate() );

	TRACE( "Initialization completed" );
}

// _____________________________________________________________________________
// ~MyVSTAudioEffect
// 
MyVSTAudioEffect::~MyVSTAudioEffect()
{
}

//-----------------------------------------------------------------------------------------
void MyVSTAudioEffect::setParameter (VstInt32 index, float value)
{
	m_plugin.setParameter(index, value);
}

//-----------------------------------------------------------------------------------------
float MyVSTAudioEffect::getParameter (VstInt32 index)
{
	return m_plugin.getParameter(index);
}

//-----------------------------------------------------------------------------------------
void MyVSTAudioEffect::getParameterName (VstInt32 index, char* label)
{
	vst_strncpy (label, "Gain", kVstMaxParamStrLen);
}

//-----------------------------------------------------------------------------------------
void MyVSTAudioEffect::getParameterDisplay (VstInt32 index, char* text)
{
	vst_strncpy( text, "0", kVstMaxParamStrLen );
}

//-----------------------------------------------------------------------------------------
void MyVSTAudioEffect::getParameterLabel (VstInt32 index, char* label)
{
	vst_strncpy (label, "GainLbl", kVstMaxParamStrLen);
}

//------------------------------------------------------------------------
bool MyVSTAudioEffect::getEffectName (char* name)
{
	vst_strncpy (name, "Chameleon", kVstMaxEffectNameLen);
	return true;
}

//------------------------------------------------------------------------
bool MyVSTAudioEffect::getProductString (char* text)
{
	vst_strncpy (text, "Chameleon", kVstMaxProductStrLen);
	return true;
}

//------------------------------------------------------------------------
bool MyVSTAudioEffect::getVendorString (char* text)
{
	vst_strncpy (text, "Sound Art", kVstMaxVendorStrLen);
	return true;
}

//-----------------------------------------------------------------------------------------
VstInt32 MyVSTAudioEffect::getVendorVersion ()
{ 
	return 100; 
}

// _____________________________________________________________________________
// getPlugCategory
//
VstPlugCategory MyVSTAudioEffect::getPlugCategory()
{
	return kPlugCategSynth;
}

//-----------------------------------------------------------------------------------------
void MyVSTAudioEffect::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
	m_plugin.process(inputs, outputs, sampleFrames);
}

// _____________________________________________________________________________
// getChunk
//
VstInt32 MyVSTAudioEffect::getChunk( void** _data, bool _isPreset /*= false*/ )
{
	m_chunkData.clear();
	m_chunkData.push_back(0);

	*_data = &m_chunkData[0];
	
	return m_chunkData.size();
}

// _____________________________________________________________________________
// setChunk
//
VstInt32 MyVSTAudioEffect::setChunk( void* _data, VstInt32 _byteSize, bool _isPreset /*= false*/ )
{
	return AudioEffectX::setChunk(_data,_byteSize,_isPreset);
}

// _____________________________________________________________________________
// setSampleRate
//
void MyVSTAudioEffect::setSampleRate( float _sampleRate )
{
}

// _____________________________________________________________________________
// canDo
//
VstInt32 MyVSTAudioEffect::canDo( char* text )
{
	if( !_stricmp( text, "receiveVstMidiEvent" ) )		return 1;
	if( !_stricmp( text, "receiveVstMidiEvents" ) )		return 1;
	if( !_stricmp( text, "receiveVstEvents" ) )			return 1;
	if( !_stricmp( text, "receiveVstTimeInfo" ) )		return 1;
	if( !_stricmp (text, "2in2out") )					return 1;
	if( !_stricmp (text, "0in2out") )					return 1;
	if( !_stricmp (text, "0in8out"))					return 1;
	if( !_stricmp (text, "2in8out"))					return 1;
	if( !_stricmp (text, "sendVstMidiEvent") )			return 1;
	if( !_stricmp (text, "sendVstMidiEvents") )			return 1;
	if( !_stricmp (text, "sendVstEvents") )				return 1;	

	return AudioEffectX::canDo(text);
}
// _____________________________________________________________________________
// processEvents
//
VstInt32 MyVSTAudioEffect::processEvents( VstEvents* events )
{
	/*
	Handling Events
	..., the host will call the processEvents() just before calling either process() or processReplacing().
	The method processEvents() has a block of VstEvents as its parameter. Each of these events in the block
	has a delta-time, in samples, that refers to positions into the audio block about to be processed with
	process() or processReplacing()...
	*/
	for( VstInt32 i=0; i<events->numEvents; ++i )
	{
		const VstEvent* eve = events->events[i];

		if( eve->type == kVstMidiType )
		{
			const VstMidiEvent* ev = (const VstMidiEvent*)eve;
			/*
			SMidiEvent midiEvent;
			midiEvent.a = ev->midiData[0];
			midiEvent.b = ev->midiData[1];
			midiEvent.c = ev->midiData[2];
			midiEvent.offset = ev->deltaFrames;

			addMidiEvent(midiEvent);
			*/
		}
		else if( eve->type == kVstSysExType )
		{
			const VstMidiSysexEvent* ev = (const VstMidiSysexEvent*)eve;
			/*
			SMidiEvent midiEvent;
			midiEvent.sysex.resize(ev->dumpBytes);
			midiEvent.offset = ev->deltaFrames;

			memcpy(&midiEvent.sysex[0], ev->sysexDump, ev->dumpBytes);
			addMidiEvent(midiEvent);
			*/
		}
		else
		{
			TRACE("Unknown event type %d", eve->type);
		}
	}

	return 1; // want more
}
/*
void MyVSTAudioEffect::sendMidiEventsToHost(const std::vector<k1lib::SMidiEvent>& _midiEvents)
{
	// send midi back to VST host
	if (_midiEvents.empty())
		return;

	{
		std::vector<VstMidiEvent> events;
		events.reserve(_midiEvents.size());

		for (size_t i = 0; i < _midiEvents.size(); ++i)
		{
			const auto& ev = _midiEvents[i];

			if (!ev.sysex.empty())
				continue;

			VstMidiEvent dst{};

			dst.type = kVstMidiType;
			dst.byteSize = sizeof(dst);

			dst.deltaFrames = ev.offset;

			dst.midiData[0] = ev.a;
			dst.midiData[1] = ev.b;
			dst.midiData[2] = ev.c;
			dst.midiData[3] = 0;

			events.push_back(dst);
		}

		if (!events.empty())
		{
			// we worst API i've seen in my entire C/C++ career:
			auto vstEvents = static_cast<VstEvents*>(malloc(sizeof(VstEvents) + sizeof(VstMidiEvent*) * events.size()));
			memset(vstEvents, 0, sizeof(VstEvents));

			vstEvents->numEvents = static_cast<VstInt32>(events.size());

			for (size_t i = 0; i < events.size(); ++i)
				vstEvents->events[i] = reinterpret_cast<VstEvent*>(&events[i]);

			sendVstEventsToHost(vstEvents);

			free(vstEvents);
		}
	}
	{
		std::vector<VstMidiSysexEvent> events;
		events.reserve(_midiEvents.size());

		for (size_t i = 0; i < _midiEvents.size(); ++i)
		{
			const auto& ev = _midiEvents[i];

			if (ev.sysex.empty())
				continue;

			VstMidiSysexEvent dst{};

			dst.type = kVstSysExType;
			dst.byteSize = sizeof(dst);

			dst.deltaFrames = ev.offset;

			dst.sysexDump = (char*)&ev.sysex[0];
			dst.dumpBytes = ev.sysex.size();

			events.push_back(dst);
		}

		if (!events.empty())
		{
			// and another time, now for sysex
			auto vstEvents = static_cast<VstEvents*>(malloc(sizeof(VstEvents) + sizeof(VstMidiSysexEvent*) * events.size()));
			memset(vstEvents, 0, sizeof(VstEvents));

			vstEvents->numEvents = static_cast<VstInt32>(events.size());

			for (size_t i = 0; i < events.size(); ++i)
				vstEvents->events[i] = reinterpret_cast<VstEvent*>(&events[i]);

			sendVstEventsToHost(vstEvents);

			free(vstEvents);
		}
	}
}
*/