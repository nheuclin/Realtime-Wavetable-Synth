#include "SoundModelPoly.h"
#include "SoundModelMono.h"
#include <cstring>
#include <vector>
#include <algorithm>
//#include <iostream>

using namespace SYNTHPI;
using namespace audio;

SoundModelPoly::SoundModelPoly(int poly, const int samplerate, double output_gain) {
	for (int i = 0; i < poly; i++) {
		soundModelList.push_back(new SoundModelMono(samplerate));
	}
	VoiceNo=poly;
}

void SoundModelPoly::setNoteOn(int midinote) { //add note priority here
	int active=0;
	for(unsigned int i = 0; i < soundModelList.size(); i++)
		if (soundModelList[i]->isPlaying()){
			active++;
		}

	if (active==VoiceNo){ //if all voices are playing reset the oldest voice to the new note
		soundModelList[lastSoundModel[0]]->setNoteOn(midinote);
		midiNoteList[lastSoundModel[0]]=midinote;
	}

	for(unsigned int i = 0; i < soundModelList.size(); i++)		
		if(!(soundModelList[i]->isPlaying())) {
			soundModelList[i]->setNoteOn(midinote);
			midiNoteList[i]=midinote;
			lastSoundModel.push_back(i);

			return;
		}
}

void SoundModelPoly::setNoteOff(int midinote) {

	for(unsigned int i = 0; i < soundModelList.size(); i++)
		if (midiNoteList[i]==midinote){
			soundModelList[i]->setNoteOff(midinote);
			position = std::find(lastSoundModel.begin(), lastSoundModel.end(), i); //does that work to get the position 
			lastSoundModel.erase(position);
		}

}

bool SoundModelPoly::isPlaying() {

	for(std::vector<SoundModel*>::iterator sndModIterator =
	        soundModelList.begin();
	    sndModIterator != soundModelList.end();
	    sndModIterator++) {
		if((*sndModIterator)->isPlaying()) return true;
	}

	return false;
}

std::vector<sample_t> getSamples(int nSamples){

    std::vector<sample_t> temp(nSamples);
    // Clear object buffer and set the size
    buffer.clear();
    buffer.resize(nSamples);

	for(std::vector<SoundModel*>::iterator  sndModIterator = soundModelList.begin();
	   										sndModIterator != soundModelList.end();
	    									sndModIterator++) {

		temp=(*sndModIterator)->getSamples(nSamples);
		for (i = 0; i < nSamples; i++) {
			buffer[i] += temp[i]*master_vol;
		}
	}

	return buffer;
	
	// Dezipper the audio output by changing the output gain
	// progressively along the outbut buffer length
	//double gain_step { (double)(target_gain-gain)/bufferSize };
	//for(i = 0; i < bufferSize; i++) {
	//	samples[i] = accumulator[i] * gain;
	//	gain += gain_step;
	//}
	//gain = target_gain;
}