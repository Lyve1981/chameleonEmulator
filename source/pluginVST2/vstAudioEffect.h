#pragma once

#include <vector>

#include "public.sdk/source/vst2.x/audioeffectx.h"

class k1ui;

class MyVSTAudioEffect : public AudioEffectX
{
public:
					MyVSTAudioEffect			(audioMasterCallback audioMaster);
	virtual			~MyVSTAudioEffect			();

	// Processing
	void 			processReplacing			(float**  inputs, float**  outputs, VstInt32 sampleFrames) override;
	void 			processDoubleReplacing		(double** inputs, double** outputs, VstInt32 sampleFrames) override {}

	VstInt32		processEvents				(VstEvents* events) override;

	void			setSampleRate				(float sampleRate) override;

	VstInt32		canDo						(char* text) override;

	// Midi
	VstInt32		getNumMidiInputChannels		() override { return 16; }				/// Returns number of MIDI input channels used [0, 16]
	VstInt32		getNumMidiOutputChannels	() override { return 16; }				/// Returns number of MIDI output channels used [0, 16]

	// Parameters
	void			setParameter				(VstInt32 index, float value) override;
	float			getParameter				(VstInt32 index) override;
	void 			getParameterLabel			(VstInt32 index, char* label) override;
	void 			getParameterDisplay			(VstInt32 index, char* text) override;
	void 			getParameterName			(VstInt32 index, char* label) override;

	// effect
	bool			getEffectName				(char* name) override;
	bool			getVendorString				(char* text) override;
	bool			getProductString			(char* text) override;
	VstInt32		getVendorVersion			() override;
	VstPlugCategory	getPlugCategory				() override;

	VstInt32 		getChunk 					(void** _data, bool _isPreset = false) override;
	VstInt32 		setChunk 					(void* _data, VstInt32 _byteSize, bool isPreset = false) override;
	VstInt32		beginLoadBank				(VstPatchChunkInfo* /*ptr*/) override { return 1; }
	VstInt32		beginLoadProgram			(VstPatchChunkInfo* /*ptr*/) override { return 1; }

private:
//	void			sendMidiEventsToHost		(const std::vector<k1lib::SMidiEvent>& _midiEvents);

	std::vector<unsigned char>					m_chunkData;
};
