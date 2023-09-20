#include <iostream>
#include <libremidi/reader.hpp>
#include <istream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <libremidi/message.hpp>
#include <cmath>
#include <set>


extern "C" {

// 0 start, 1 on, 2 off, 3 shift, 4 velocity, 5 end
struct music_sequence_event{
  int event_type;
  int event_value;
};

struct tempo_signature_block {
  int tempo;
  int time_signature_1;
  int time_signature_2;
  int tick_start;
  int tick_end;
  int length_measures;
};

struct active_note {
  int notenum;
  int start_tick;
};

struct note{
  libremidi::track_event* start;
  libremidi::track_event* end;
};

struct sequence_array {
  int**** sequences;
  int dim1;
  int dim2;
  int dim3;
  int dim4;
  int successful;
};


struct sequence_array_singletrack {
  int*** sequences;
  int dim1;
  int dim2;
  int dim3;
  int successful;
};

void test_library(){
  std::cout << "hehe" << std::endl;
}

bool compare_tick_pointers(libremidi::track_event* i1, libremidi::track_event* i2)
{
  return (i1->tick < i2->tick);
}

bool compare_tick(libremidi::track_event i1, libremidi::track_event i2)
{
    return (i1.tick < i2.tick);
}

void sort_events(libremidi::reader& r){
  for (auto& track : r.tracks)
  {
    std::sort(track.begin(), track.end(), compare_tick);
  }
}

bool are_all_elements_neg_one(std::vector<int>& nes_track_nums){
  
  bool are_all_neg_one = true;

  for(int i = 0 ; i < nes_track_nums.size(); i++){
    if(nes_track_nums[i] != -1){
      are_all_neg_one = false;
      return are_all_neg_one;
    }
  }

  return are_all_neg_one;

}

void get_program_numbers(libremidi::reader& r, std::vector<int>& program_nums){
  int tracknum = 0;
  for (auto& track : r.tracks)
  {
    for (libremidi::track_event& event : track)
    {  
      if(!event.m.is_meta_event() && event.m.get_message_type() == libremidi::message_type::PROGRAM_CHANGE){
        program_nums[tracknum] = (int)event.m.bytes[1] + 1;
        break;
      }
    }
    tracknum++;
  }

}

void find_usable_programs(std::vector<int>& program_nums, std::vector<int>& nes_track_nums, int track_num, std::vector<int>& usable_programs){
  for(int k = 0; k < usable_programs.size(); k++){
    std::vector<int>::iterator it = std::find(program_nums.begin(), program_nums.end(), usable_programs[k]);
    if(it != program_nums.end()){
      int track_index = it - program_nums.begin();
      nes_track_nums[track_num] = track_index;
      program_nums[track_index] = -1;
      return;
    }
  }
}

void get_track_mappings(std::vector<int>& program_nums, std::vector<int>& nes_track_nums){
  std::vector<int> gm_mono_leads = {
  /*strings*/ 41, 42, 43, 
  /*brass*/ 57, 58, 59, 60, 61, 63, 64,
  /*reed*/ 65, 66, 67, 68, 69, 70, 71, 72,
  /*pipe*/ 73, 74, 75, 76, 77, 78, 79, 80,
  /*synth*/ 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96
  };

  std::vector<int> gm_bass = {33, 34, 35, 36, 37, 38, 39, 40, 44};

  std::vector<int> percussion = {
  /*percussion*/ 113, 114, 115, 116, 117, 118, 119, 120,
  /*noise*/ 212, 122, 123, 124, 125, 126, 127, 128
  };

  //std::cout << "monoleadsnum-" << std::size(gm_mono_leads) << "bassnum-" << std::size(gm_bass) << "percnum-" << std::size(percussion) << std::endl;

  int nes_tracks_num = 4;

  std::vector<std::string> nes_tracks = { "P1", "P2", "TR", "NO" };

  for(int i = 0; i < nes_tracks_num; i++){

    if(nes_tracks[i] == "P1"){
      find_usable_programs(program_nums, nes_track_nums, i, gm_mono_leads);

      if(nes_track_nums[i] == -1){
        find_usable_programs(program_nums, nes_track_nums, i, gm_bass);
      }

    }
    else if((nes_tracks[i] == "P2")){
      find_usable_programs(program_nums, nes_track_nums, i, gm_mono_leads);

      if(nes_track_nums[i] == -1){
        find_usable_programs(program_nums, nes_track_nums, i, gm_bass);
      }

    }
    else if((nes_tracks[i] == "TR")){
      find_usable_programs(program_nums, nes_track_nums, i, gm_bass);

      if(nes_track_nums[i] == -1){
        find_usable_programs(program_nums, nes_track_nums, i, gm_mono_leads);
      }

    }
    else if((nes_tracks[i] == "NO")){
      find_usable_programs(program_nums, nes_track_nums, i, percussion);
    }

  }

}

void print_out_sequence(libremidi::reader& r){

  int tracknum = 0;
  for (const auto& track : r.tracks)
  {
    
    std::cout << "\nTrack " << tracknum << "\n\n";
    
    for (const libremidi::track_event& event : track)
    {
      
      if(event.m.is_meta_event())
      {

        std::cout << "Event at " << event.tick << " : ";
        std::string meta_event_type_str = "";
        libremidi::meta_event_type eventType = event.m.get_meta_event_type();
        switch (eventType) {
            case libremidi::meta_event_type::SEQUENCE_NUMBER: meta_event_type_str = "SEQUENCE_NUMBER"; break;  
            case libremidi::meta_event_type::TEXT: meta_event_type_str = "TEXT"; break;
            case libremidi::meta_event_type::COPYRIGHT: meta_event_type_str = "COPYRIGHT"; break;
            case libremidi::meta_event_type::TRACK_NAME: meta_event_type_str = "TRACK_NAME"; break;
            case libremidi::meta_event_type::INSTRUMENT: meta_event_type_str = "INSTRUMENT"; break;
            case libremidi::meta_event_type::LYRIC: meta_event_type_str = "LYRIC"; break;
            case libremidi::meta_event_type::MARKER: meta_event_type_str = "MARKER"; break;
            case libremidi::meta_event_type::CUE: meta_event_type_str = "CUE"; break;
            case libremidi::meta_event_type::PATCH_NAME: meta_event_type_str = "PATCH_NAME"; break;
            case libremidi::meta_event_type::DEVICE_NAME: meta_event_type_str = "DEVICE_NAME"; break;
            case libremidi::meta_event_type::CHANNEL_PREFIX: meta_event_type_str = "CHANNEL_PREFIX"; break;
            case libremidi::meta_event_type::MIDI_PORT: meta_event_type_str = "MIDI_PORT"; break;
            case libremidi::meta_event_type::END_OF_TRACK: meta_event_type_str = "END_OF_TRACK"; break;
            case libremidi::meta_event_type::TEMPO_CHANGE: meta_event_type_str = "TEMPO_CHANGE"; break;
            case libremidi::meta_event_type::SMPTE_OFFSET: meta_event_type_str = "SMPTE_OFFSET"; break;
            case libremidi::meta_event_type::TIME_SIGNATURE: meta_event_type_str = "TIME_SIGNATURE"; break;
            case libremidi::meta_event_type::KEY_SIGNATURE: meta_event_type_str = "KEY_SIGNATURE"; break;
            case libremidi::meta_event_type::PROPRIETARY: meta_event_type_str = "PROPRIETARY"; break;
            case libremidi::meta_event_type::UNKNOWN: meta_event_type_str = "UNKNOWN"; break;    
            default: meta_event_type_str = "UNKNOWN"; break;
        }
        
        std::cout << "metaEvent - message_type: " << meta_event_type_str  << " data: ";  
        for(int i = 0; i < event.m.size(); i++){
          std::cout <<(int)event.m.bytes[i] << ' ';
        }
        std::cout << '\n';
      }
      else
      {
        
        std::string note_event_type_str = "";
        switch(event.m.get_message_type())
        {
          case libremidi::message_type::NOTE_ON:
            std::cout << "Event at " << event.tick << " : ";
            note_event_type_str = "NOTE_ON";
            std::cout << "Note ON: "
                      << "channel "  << event.m.get_channel() << ' '
                      << "note "     << (int)event.m.bytes[1] << ' '
                      << "velocity " << (int)event.m.bytes[2] << ' ' << std::endl;
            break;
          case libremidi::message_type::NOTE_OFF:
            std::cout << "Event at " << event.tick << " : ";
            note_event_type_str = "NOTE_OFF";
            std::cout << "Note OFF: "
                      << "channel "  << event.m.get_channel() << ' '
                      << "note "     << (int)event.m.bytes[1] << ' '
                      << "velocity " << (int)event.m.bytes[2] << ' ' << std::endl;
            break;
          case libremidi::message_type::PROGRAM_CHANGE:
            std::cout << "Event at " << event.tick << " : ";
            std::cout << "Program change: ";
            std::cout << "channel "  << event.m.get_channel() << " ";
            for(int i = 0; i < event.m.bytes.size(); i++){
              std::cout << (int)event.m.bytes[i] << " ";
            }
            std::cout << std::endl;
          break;
          default:
          break;
        }  
      }
    }

    tracknum++;
    
  }

}

void quantize_notes(std::vector<std::vector<note>>& all_notes, int current_ticks_per_beat, int quantized_ticks_per_beat){

  int smallest_timestep = current_ticks_per_beat/quantized_ticks_per_beat;

  for(int i = 0; i < all_notes.size(); i++){
    for(int j = 0; j < all_notes[i].size(); j++){

      if(all_notes[i][j].end->tick - all_notes[i][j].start->tick <= smallest_timestep){

        if(all_notes[i][j].start->tick%smallest_timestep < (smallest_timestep/2)){
          all_notes[i][j].start->tick = all_notes[i][j].start->tick - all_notes[i][j].start->tick%smallest_timestep;
          all_notes[i][j].end->tick = all_notes[i][j].start->tick + smallest_timestep;
        }
        else{
          all_notes[i][j].start->tick = all_notes[i][j].start->tick + (smallest_timestep - all_notes[i][j].start->tick%smallest_timestep);
          all_notes[i][j].end->tick = all_notes[i][j].start->tick + smallest_timestep;
        }
      
      }
      else{
        if(all_notes[i][j].start->tick%smallest_timestep < (smallest_timestep/2)){
          all_notes[i][j].start->tick = all_notes[i][j].start->tick - all_notes[i][j].start->tick%smallest_timestep;
        }
        else{
          all_notes[i][j].start->tick = all_notes[i][j].start->tick + (smallest_timestep - all_notes[i][j].start->tick%smallest_timestep);
        }

        if(all_notes[i][j].end->tick%smallest_timestep < (smallest_timestep/2)){
          all_notes[i][j].end->tick = all_notes[i][j].end->tick - all_notes[i][j].end->tick%smallest_timestep;
        }
        else{
          all_notes[i][j].end->tick = all_notes[i][j].end->tick + (smallest_timestep - all_notes[i][j].end->tick%smallest_timestep);
        }

      }


    }
  }

  for(int i = 0; i < all_notes.size(); i++){
    for(int j = 0; j < all_notes[i].size(); j++){

      all_notes[i][j].start->tick /= smallest_timestep;
      all_notes[i][j].end->tick /= smallest_timestep;

    }
  }
  
}

void extract_notes(libremidi::reader& r, std::vector<std::vector<note>>& all_notes) {

  //  22050/x = 96
  // 22050 = 96x
  // x = 22050/96

  int tracknum = 0;
  for (auto& track : r.tracks)
  {

    std::vector<libremidi::track_event*> active_notes;
    for (libremidi::track_event& event : track)
    {

      if(!event.m.is_meta_event()){
        if(event.m.get_message_type() == libremidi::message_type::NOTE_OFF || (event.m.get_message_type() == libremidi::message_type::NOTE_ON && (int)event.m.bytes[2] == 0)) {
          int active_note_index = 0;
          bool found = false;

          for(auto & active_note : active_notes){
            if ((int)active_note->m.bytes[1] == (int)event.m.bytes[1]) {
              found = true;
              break;
            }
            active_note_index++;
          }
          if(found){
            all_notes[tracknum].push_back(note{active_notes[active_note_index], &event});
            active_notes.erase(active_notes.begin() + active_note_index);
          }

        }
        else if(event.m.get_message_type() == libremidi::message_type::NOTE_ON && (int)event.m.bytes[2] > 0){
          active_notes.push_back(&event);
        }

  
      }

  
    }
    tracknum++;
  }

}

void extract_sequence_singletrack(std::vector<std::vector<note>>& all_notes, std::vector<std::vector<int>>& sequence, int ticks_per_sixteenth, int sequence_length, int block_start, int block_end){
  
  std::vector<std::vector<libremidi::track_event*>> block_notes_events(all_notes.size(), std::vector<libremidi::track_event*>());  
  for(int i = 0; i < all_notes.size(); i++){

    for(int j = 0; j < all_notes[i].size(); j++){

    //   std::cout << all_notes[i][j].start->tick << "-" << all_notes[i][j].end->tick << " ";
      if(all_notes[i][j].start->tick >= block_start && all_notes[i][j].start->tick < block_end){

        block_notes_events[i].push_back(all_notes[i][j].start);
        block_notes_events[i].push_back(all_notes[i][j].end);
      }
    }

    std::sort(block_notes_events[i].begin(), block_notes_events[i].end(), compare_tick_pointers);

  }
//   std::cout << "sequence_length" << sequence_length << std::endl;

  
  for (int i = 0; i < block_notes_events.size(); i++) {

    // std::cout << "Okej: " << block_notes_events.size() << "x" << block_notes_events[i].size() << std::endl;
    int note_playing = -1;
    int track_event_index = 0;
    int track_size = block_notes_events[i].size();
    std::vector<active_note> active_notes;
    // std::cout << "\nTrack " << i << " ,size " << track_size << "\n\n";


    for(int j = 0; j < sequence_length; j++) {

      int start_tick = block_start + ticks_per_sixteenth*j;
      int end_tick = block_start + ticks_per_sixteenth*(j+1);

      // std::cout << "start-tick-" << start_tick << " end-tick-" << end_tick << std::endl;

      while((track_event_index < track_size) && block_notes_events[i][track_event_index]->tick < end_tick){
        
        if(!block_notes_events[i][track_event_index]->m.is_meta_event()){
          if(block_notes_events[i][track_event_index]->m.get_message_type() == libremidi::message_type::NOTE_OFF || 
          (block_notes_events[i][track_event_index]->m.get_message_type() == libremidi::message_type::NOTE_ON && (int) block_notes_events[i][track_event_index]->m.bytes[2] == 0)) {
            
            int active_note_index = 0;
            for(active_note& n : active_notes){
              if (n.notenum == (int)block_notes_events[i][track_event_index]->m.bytes[1]) {
                active_notes.erase(active_notes.begin() + active_note_index);
                break;
              }
              active_note_index++;
            }
          }

          else if(block_notes_events[i][track_event_index]->m.get_message_type() == libremidi::message_type::NOTE_ON){
            active_notes.push_back(active_note{(int) block_notes_events[i][track_event_index]->m.bytes[1], block_notes_events[i][track_event_index]->tick});

          }


        }
        track_event_index++;   
      }  

      // std::cout << "heh" << active_notes.size() << std::endl;
      if(active_notes.size() > 0) {

        bool found = false;
        bool is_new = false;
        for(active_note& n : active_notes){
          if(n.notenum == note_playing) { 
            if(n.start_tick >= start_tick){
              is_new = true;
            }
            found = true;
            break;
          }
        }
        if(found && !is_new) {
          sequence[i][j] = -2;
        
        }
        else{

          int highest_new = -1;
          for(active_note& n : active_notes){
            if(n.notenum > highest_new && n.start_tick >= start_tick) {
              highest_new = n.notenum;
            }
          }

          if(highest_new > -1){
            sequence[i][j] = highest_new;
            note_playing = highest_new;
          }
          else{
            sequence[i][j] = -1;
            note_playing =-1;
          }

        }
      
      }
      else{
        sequence[i][j] = -1;
        note_playing =-1;

      }
      //  std::cout << "\n\n";   
    }

  }
}

void extract_sequence(std::vector<std::vector<note>>& all_notes,std::vector<std::vector<music_sequence_event>>& sequence, int block_start, int block_end, std::vector<int>& midi_programs){

  std::vector<std::vector<note>> block_notes(all_notes.size(), std::vector<note>());

  // std::cout << "block-ticks " << block_start << "-" << block_end << std::endl;

  for(int i = 0; i < all_notes.size(); i++){

    int notes_num = 0;
    std::set<int> unique_time_ticks;

    for(int j = 0; j < all_notes[i].size(); j++){

      // std::cout << all_notes[i][j].start->tick << "-" << all_notes[i][j].end->tick << " ";
      if(all_notes[i][j].start->tick >= block_start && all_notes[i][j].start->tick < block_end){


        if(all_notes[i][j].end->tick > block_end){
          all_notes[i][j].end->tick = block_end;
        }

        unique_time_ticks.insert(all_notes[i][j].start->tick);
        unique_time_ticks.insert(all_notes[i][j].end->tick);

        if(all_notes[i][j].start->tick > 0){
          unique_time_ticks.insert(all_notes[i][j].start->tick);
        }
        if(all_notes[i][j].end->tick > 0){
          unique_time_ticks.insert(all_notes[i][j].end->tick);
        }

        block_notes[i].push_back(all_notes[i][j]);
        notes_num++;
      }
    }
    unique_time_ticks.insert(block_end);
    // std::cout << std::endl;

    int events_num = 3 + 2 * notes_num + unique_time_ticks.size();

    //std::cout << "eventsnum " << events_num << std::endl;

    if(events_num > 64) {
      int events_to_erase = events_num - 64;
      int notes_to_erase = events_to_erase / 2 + (events_to_erase % 2);

      if(notes_to_erase > block_notes[i].size()){
        notes_to_erase = block_notes[i].size();
      }

      for (int k = 0; k < notes_to_erase; k++){
        block_notes[i].pop_back();
      }

    }

  }

  for (int i = 0; i < block_notes.size(); i++){
    

    std::vector<libremidi::track_event*> block_notes_events;

    for(int j = 0; j < block_notes[i].size(); j++){

      block_notes_events.push_back(block_notes[i][j].start);
      block_notes_events.push_back(block_notes[i][j].end);
    }   

    std::sort(block_notes_events.begin(), block_notes_events.end(), compare_tick_pointers);

    int previous_tick = block_start;
    int current_tick = block_start;
    int ticks_elapsed = 0;

    if(block_notes_events.size() == 0){
      sequence[i].push_back(music_sequence_event{5, 0});
    }
    else{

      sequence[i].push_back(music_sequence_event{0, midi_programs[i]});
      sequence[i].push_back(music_sequence_event{4, 1});

      for (auto& block_note : block_notes_events){

        // std::cout << block_note->tick << " ";

        current_tick = block_note->tick;

        if(current_tick > previous_tick){
          ticks_elapsed += current_tick - previous_tick;
        } 


        if(!block_note->m.is_meta_event()){

          // note OFF EVENTS
          if(block_note->m.get_message_type() == libremidi::message_type::NOTE_OFF || (block_note->m.get_message_type() == libremidi::message_type::NOTE_ON && (int)block_note->m.bytes[2] == 0)) {
            if(ticks_elapsed > 0){
              sequence[i].push_back(music_sequence_event{3, ticks_elapsed});
              ticks_elapsed = 0;
            }

            int note_off_value = (int)block_note->m.bytes[1];

            if (i == block_notes.size() - 1){
              note_off_value = note_off_value / 8;
            }
          
            sequence[i].push_back(music_sequence_event{2, note_off_value});

          }

          // note ON EVENTS
          else if(block_note->m.get_message_type() == libremidi::message_type::NOTE_ON){
            if(ticks_elapsed > 0){
              sequence[i].push_back(music_sequence_event{3, ticks_elapsed});
              ticks_elapsed = 0;
            }
            int note_on_value = (int)block_note->m.bytes[1];

            if (i == block_notes.size() - 1){
              note_on_value = note_on_value / 8;
            }
            
            sequence[i].push_back(music_sequence_event{1, note_on_value});
          }

        }
        previous_tick = current_tick;
        
      }

      if(block_end > current_tick){
        ticks_elapsed += (block_end - current_tick);
      }

      if(ticks_elapsed > 0){
        sequence[i].push_back(music_sequence_event{3, ticks_elapsed});
        ticks_elapsed = 0;
      }  

      //std::cout << std::endl;
      sequence[i].push_back(music_sequence_event{5, 0});


    }

  }

}

int find_last_tick(libremidi::reader& r) {
  int last_note_event_tick = -1;
  for (const auto& track : r.tracks)
  {
    for (const libremidi::track_event& event : track)
    {
      if (event.tick > last_note_event_tick) {
        last_note_event_tick = event.tick;
      }
    }
  }
  return last_note_event_tick;
}

void split_midi_2(std::vector<libremidi::track_event>& control_events, std::vector<tempo_signature_block>& midi_blocks,  int last_note_event_tick){


  int current_time_signature_1 = 4;
  int current_time_signature_2 = 4;
  int current_tempo = 120;
  int last_event_time = 0;
  int current_time = 0;

  for(const auto& event : control_events){

    current_time = event.tick;
    libremidi::meta_event_type eventType = event.m.get_meta_event_type();


    if(eventType == libremidi::meta_event_type::TEMPO_CHANGE){

      
      int tempo_ = (int)(((uint8_t)event.m.bytes[3] << 16) + ((uint8_t)event.m.bytes[4] << 8) + ((uint8_t)event.m.bytes[5]));

      if(tempo_ <= 0){
        tempo_ = 500000;
      }
      int event_tempo = 60000000/tempo_;
      // std::cout << current_time << "tempo-" << event_tempo << std::endl; 

      if(event_tempo != current_tempo && (current_time - last_event_time) > 0){
        midi_blocks.push_back(tempo_signature_block{current_tempo, current_time_signature_1, current_time_signature_2, last_event_time, current_time, -1});
        current_tempo = event_tempo;
        last_event_time = current_time;
      }
      else if(event_tempo != current_tempo && (current_time - last_event_time) == 0){
        current_tempo = event_tempo;
      }

    }


    if(eventType == libremidi::meta_event_type::TIME_SIGNATURE){

      int event_time_signature_1 = (int)event.m.bytes[3]; 
      int event_time_signature_2 = pow(2, (int)event.m.bytes[4]);
      // std::cout << current_time << "time_signature-" << event_time_signature_1 << "/" << event_time_signature_2 << std::endl; 


      if((event_time_signature_1 != current_time_signature_1 || event_time_signature_2 != current_time_signature_2) && (current_time - last_event_time) > 0){
        midi_blocks.push_back(tempo_signature_block{current_tempo, current_time_signature_1, current_time_signature_2, last_event_time, current_time, -1});
        current_time_signature_1 = event_time_signature_1;
        current_time_signature_2 = event_time_signature_2;
        last_event_time = current_time;

      }
      else if((event_time_signature_1 != current_time_signature_1 || event_time_signature_2 != current_time_signature_2) && (current_time - last_event_time) == 0){
        current_time_signature_1 = event_time_signature_1;
        current_time_signature_2 = event_time_signature_2;
      }

    }

  }

  if(last_note_event_tick - last_event_time > 0){
    midi_blocks.push_back(tempo_signature_block{current_tempo, current_time_signature_1, current_time_signature_2, last_event_time, last_note_event_tick, -1});
  }

}

bool parse_midi(std::vector<uint8_t>& bytes, libremidi::reader& r){


  // Parse
  libremidi::reader::parse_result result = r.parse(bytes);
  bool is_valid = false;

  switch (result)
  {
    case libremidi::reader::validated:
      std::cout << "\nParsing validated\n\n";
      // Parsing has succeeded, all the input data is correct MIDI.
      break;

    case libremidi::reader::complete:
      std::cout << "\nParsing complete\n\n";
      // All the input data is parsed but the MIDI file was not necessarily strict SMF
      // (e.g. there are empty tracks or tracks without END OF TRACK)
      break;

    case libremidi::reader::incomplete:
      std::cout << "\nParsing incomplete\n\n";
      // Not all the input could be parsed. For instance a track could not be read.
      break;

    case libremidi::reader::invalid:
      std::cout << "\nParsing invalid\n\n";
      // Nothing could be parsed, this is not MIDI data or we do not support it yet.
      break;
  }
  
  if (result != libremidi::reader::invalid)
  {
    is_valid = true;
    // std::cout << "ticks per beat: " << r.ticksPerBeat << std::endl;
    
  }

  return is_valid;
}

void get_control_events(libremidi::reader& r, std::vector<libremidi::track_event>& control_events){

  int tracknum = 0;
  for (const auto& track : r.tracks)
  {
    
    for (const libremidi::track_event& event : track)
    {
      
      if(event.m.is_meta_event())
      {
        libremidi::meta_event_type eventType = event.m.get_meta_event_type();
        if((eventType == libremidi::meta_event_type::TIME_SIGNATURE || eventType == libremidi::meta_event_type::TEMPO_CHANGE)){
          control_events.push_back(event);
        }              

      }
  
    }
    tracknum++;
  }

}

void get_all_possible_nes_tracks(std::vector<std::vector<int>>& all_posible_nes_tracks,  std::vector<int>& program_nums){


  std::vector<std::string> nes_tracks = { "P1", "P2", "TR", "NO" };
  int nes_tracks_num = 4;

  while(true){
    std::vector<int> nes_track_nums(nes_tracks_num, -1);
    get_track_mappings(program_nums, nes_track_nums);

    if(are_all_elements_neg_one(nes_track_nums)){
      break;
    }

    all_posible_nes_tracks.push_back(nes_track_nums);
    // std::cout << "nes track nums" << std::endl;
    // for(int i = 0; i< nes_track_nums.size(); i++){
    //   std::cout << nes_tracks[i] << ": " << nes_track_nums[i]  << " | ";
    // }
    // std::cout << std::endl;

  } 

}

void filter_and_quantize_midi_blocks(std::vector<tempo_signature_block>& used_midi_blocks, std::vector<tempo_signature_block>& midi_blocks, int sequence_length_measures, int current_ticks_per_beat, int quantized_ticks_per_beat){

  int smallest_timestep = current_ticks_per_beat/quantized_ticks_per_beat;
  // std::cout << "all midi blocks:" << "smallest timestep-" << smallest_timestep << std::endl;

  for(auto& midi_block : midi_blocks){
    int block_length = midi_block.tick_end - midi_block.tick_start;
    int ticks_per_sixteenth_unquantized = current_ticks_per_beat/4;
    int block_length_sixteenths = block_length/ticks_per_sixteenth_unquantized;
    block_length_sixteenths += (16 - block_length_sixteenths % 16);
    int block_measures = block_length_sixteenths/16;

    // std::cout << "ticks-" << midi_block.tick_start << "-" << midi_block.tick_end << "measures-" << block_measures << "time_signature-" << midi_block.time_signature_1 << "/" << midi_block.time_signature_2 << " tempo-" << midi_block.tempo << std::endl;
  
    if(midi_block.time_signature_1 == 4 && midi_block.time_signature_2 == 4){

      if(block_measures >= sequence_length_measures){


        int num_of_blocks_to_add = block_measures/sequence_length_measures;  
        // std::cout << "block_measures-" << block_measures << "numblocks_to_add-" << num_of_blocks_to_add << std::endl;

      
        int this_block_start;
        int this_block_end;

        if(midi_block.tick_start%smallest_timestep < (smallest_timestep/2)){
          this_block_start = midi_block.tick_start - midi_block.tick_start%smallest_timestep;
        }
        else{
          this_block_start = midi_block.tick_start + (smallest_timestep - midi_block.tick_start%smallest_timestep);
        }

        if(midi_block.tick_end%smallest_timestep < (smallest_timestep/2)){
          this_block_end = midi_block.tick_end - midi_block.tick_end%smallest_timestep;
        }
        else{
          this_block_end = midi_block.tick_end + (smallest_timestep - midi_block.tick_end%smallest_timestep);
        }   

        this_block_start = this_block_start/smallest_timestep;
        this_block_end = this_block_end/smallest_timestep;    

        
        for(int i = 0; i < num_of_blocks_to_add; i++){
          int this_block_division_start = this_block_start + sequence_length_measures*quantized_ticks_per_beat*4*i;
          int this_block_division_end = this_block_start + sequence_length_measures*quantized_ticks_per_beat*4*(i+1);

          tempo_signature_block new_block{midi_block.tempo, midi_block.time_signature_1, midi_block.time_signature_2, this_block_division_start, this_block_division_end, sequence_length_measures};

          used_midi_blocks.push_back(new_block);

        }  
        
      

      }
    }

  }

}


sequence_array_singletrack extract_note_sequences_from_midi_singletrack(char* midi_file_location)
{


  std::cout << "\n----------------------------------------------------------------------------------------\n" 
    << "filepath: " << midi_file_location << std::endl;

  std::ifstream file{midi_file_location, std::ios::binary};
  if(!file.is_open())
  {
    std::cerr << "Could not open " << midi_file_location << std::endl;
    return sequence_array_singletrack{NULL, 0, 0, 0, 0};
  }

  std::vector<uint8_t> bytes;
  bytes.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

  libremidi::reader r{true};

  bool is_valid = parse_midi(bytes, r);

  if(!is_valid){
    return sequence_array_singletrack{NULL, 0, 0, 0, 0};
  }

  ////// TULE SE ZARES STVARI ZAČNEJO DOGAJAT

  int last_note_event_tick = find_last_tick(r);
  int ticks_per_sixteenth = r.ticksPerBeat/4;
  int tracks_num = r.tracks.size();

  sort_events(r);  
  // print_out_sequence(r);

  std::vector<int> program_nums(tracks_num, -1);
  get_program_numbers(r, program_nums);

  // std::cout << "num of tracks: " << tracks_num << std::endl;
  // std::cout << "program nums: " << std::endl;
  // for(int i = 0; i < program_nums.size(); i++){
  //   std::cout << program_nums[i] << " ";
  // }
  // std::cout << std::endl;

  std::vector<int> program_nums_cpy = program_nums;
  std::vector<std::vector<int>> all_posible_nes_tracks;
  get_all_possible_nes_tracks(all_posible_nes_tracks, program_nums_cpy);

  if(all_posible_nes_tracks.size() == 0){
    return sequence_array_singletrack{NULL, 0, 0, 0, 0};
  }


  std::vector<libremidi::track_event> control_events;
  get_control_events(r, control_events);

  std::vector<tempo_signature_block> midi_blocks;
  split_midi_2(control_events, midi_blocks, last_note_event_tick);

  int current_ticks_per_beat = r.ticksPerBeat;
  int quantized_ticks_per_beat = 4;
  int quantized_ticks_per_sixteenth = quantized_ticks_per_beat/4;
  std::vector<tempo_signature_block> used_midi_blocks;
  int sequence_length_measures = 32;
  int sequence_length_sixteenths = quantized_ticks_per_beat * 4 * sequence_length_measures;

  int smallest_timestep = current_ticks_per_beat/quantized_ticks_per_beat;  
  if(smallest_timestep <= 0){
    return sequence_array_singletrack{NULL, 0, 0, 0, 0};
  }

  filter_and_quantize_midi_blocks(used_midi_blocks, midi_blocks, sequence_length_measures, current_ticks_per_beat, quantized_ticks_per_beat);

  
  std::vector<std::vector<note>> all_notes(tracks_num, std::vector<note>());
  extract_notes(r, all_notes);


  quantize_notes(all_notes, current_ticks_per_beat, quantized_ticks_per_beat);
  
  std::vector<std::vector<std::vector<int>>> all_blocks_sequences;

  int block_counter = 0;
  int block_num = 0;
  int nes_tracks_num = 4;
  // std::cout << std::endl << "used midi blocks:" << std::endl;

  if(used_midi_blocks.size() == 0) {
    return sequence_array_singletrack{NULL, 0, 0, 0, 0};
  }

  for(const auto& used_midi_block : used_midi_blocks){

    int block_measures = used_midi_block.length_measures;
    
    // std::cout << "measures: " << block_measures << " TIME_SIGNATURE: " << used_midi_block.time_signature_1 << "/" << used_midi_block.time_signature_2 << " TEMPO: " << used_midi_block.tempo << "nes-configs-num-" << all_posible_nes_tracks.size() << std::endl;

    int block_start;
    int block_end;

    for(int h = 0; h < all_posible_nes_tracks.size(); h++){


        std::vector<std::vector<note>> all_notes_reconfig(nes_tracks_num, std::vector<note>());
        std::vector<int> midi_programs(nes_tracks_num, -1);

        for(int j = 0; j < nes_tracks_num; j++){
            if(all_posible_nes_tracks[h][j] > -1){
            all_notes_reconfig[j] = all_notes[all_posible_nes_tracks[h][j]];
            midi_programs[j] = program_nums[all_posible_nes_tracks[h][j]];
            }
        }

        // for(int j = 0; j < nes_tracks_num; j++){
        //    std::cout << midi_programs[j] << " ";
        // }
        // std::cout << std::endl;

        block_start = used_midi_block.tick_start;
        block_end =  used_midi_block.tick_end;
        std::vector<std::vector<int>> current_block_sequence(nes_tracks_num, std::vector<int>(sequence_length_sixteenths, -1));

        // extract_sequence(all_notes_reconfig, current_block_sequence, block_start, block_end, midi_programs);

        extract_sequence_singletrack(all_notes_reconfig, current_block_sequence, quantized_ticks_per_sixteenth, sequence_length_sixteenths, block_start, block_end);

        bool is_sequence_empty = true;
        for(int i = 0; i<current_block_sequence.size(); i++){

            for(int j = 0; j<current_block_sequence[i].size(); j++){

                if(current_block_sequence[i][j] != -1){
                    is_sequence_empty = false;
                }
            }
        }

        if(!is_sequence_empty){
            all_blocks_sequences.push_back(current_block_sequence);
        }
    }

  }

  
  if(all_blocks_sequences.size() == 0) {
    return sequence_array_singletrack{NULL, 0, 0, 0, 0};
  }

  // std::cout << std::endl << std::endl << std::endl;
  int blocks_num = all_blocks_sequences.size();
  // std::cout << "blocks-num-" << blocks_num << std::endl;
  // [blocks_num][sequence_length_measures][nes_tracks_num][max_events]
  int*** sequences_ = new int** [blocks_num];

  for(int i = 0; i < blocks_num; i++){
    sequences_[i] = new int* [nes_tracks_num];
    // std::cout << "block-" << i << std::endl; 
    for(int j = 0; j < nes_tracks_num; j++){
      // std::cout << "block-" << i << "track-" << j << std::endl; 
      sequences_[i][j] = new int [sequence_length_sixteenths];
      for(int k = 0; k < sequence_length_sixteenths; k++){

        sequences_[i][j][k] = all_blocks_sequences[i][j][k];
        // std::cout << sequences_[i][j][k] << " ";

        // if((k+1)%4 == 0){
        //     std::cout << std::endl;
        // }

        // if((k+1)%16 == 0){
        //     std::cout << std::endl;
        // }
      }
      // std::cout << std::endl << std::endl;
    } 

    // std::cout << std::endl << std::endl; 
    // std::cout << std::endl << std::endl;
  }

  // std::cout << std::endl << std::endl;
  // std::cout << std::endl << std::endl;
  // for(int h = 0; h < blocks_num; h++){
  //   std::cout << "block-" << h << std::endl; 
  //   for(int i = 0; i < sequence_length_measures; i++){
  //     std::cout << "block-" << h << "__measure-" << i << std::endl; 
  //     for(int j = 0; j < nes_tracks_num; j++){
  //       for(int k = 0; k < max_events; k++){
  //         std::cout << sequences_[h][i][j][k] << " ";
  //       }
  //       std::cout << std::endl;
  //     }
  //     std::cout << std::endl << std::endl;
  //   }
  //   std::cout << std::endl << std::endl;
  // }

  return sequence_array_singletrack{sequences_, blocks_num, nes_tracks_num, sequence_length_sixteenths, 1};
}


sequence_array extract_note_sequences_from_midi(char* midi_file_location)
{


  std::cout << "\n----------------------------------------------------------------------------------------\n" 
    << "filepath: " << midi_file_location << std::endl;

  std::ifstream file{midi_file_location, std::ios::binary};
  if(!file.is_open())
  {
    std::cerr << "Could not open " << midi_file_location << std::endl;
    return sequence_array{NULL, 0, 0, 0, 0, 0};
  }

  std::vector<uint8_t> bytes;
  bytes.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

  libremidi::reader r{true};

  bool is_valid = parse_midi(bytes, r);

  if(!is_valid){
    return sequence_array{NULL, 0, 0, 0, 0, 0};
  }

  ////// TULE SE ZARES STVARI ZAČNEJO DOGAJAT

  int last_note_event_tick = find_last_tick(r);
  int ticks_per_sixteenth = r.ticksPerBeat/4;
  int tracks_num = r.tracks.size();

  sort_events(r);  
  // print_out_sequence(r);

  std::vector<int> program_nums(tracks_num, -1);
  get_program_numbers(r, program_nums);

  // std::cout << "num of tracks: " << tracks_num << std::endl;
  // std::cout << "program nums: " << std::endl;
  // for(int i = 0; i < program_nums.size(); i++){
  //   std::cout << program_nums[i] << " ";
  // }
  // std::cout << std::endl;

  std::vector<int> program_nums_cpy = program_nums;
  std::vector<std::vector<int>> all_posible_nes_tracks;
  get_all_possible_nes_tracks(all_posible_nes_tracks, program_nums_cpy);

  if(all_posible_nes_tracks.size() == 0){
    return sequence_array{NULL, 0, 0, 0, 0, 0};
  }


  std::vector<libremidi::track_event> control_events;
  get_control_events(r, control_events);

  std::vector<tempo_signature_block> midi_blocks;
  split_midi_2(control_events, midi_blocks, last_note_event_tick);

  int current_ticks_per_beat = r.ticksPerBeat;
  int quantized_ticks_per_beat = 24;
  std::vector<tempo_signature_block> used_midi_blocks;
  int sequence_length_measures = 32;

  int smallest_timestep = current_ticks_per_beat/quantized_ticks_per_beat;  
  if(smallest_timestep <= 0){
    return sequence_array{NULL, 0, 0, 0, 0, 0};
  }

  filter_and_quantize_midi_blocks(used_midi_blocks, midi_blocks, sequence_length_measures, current_ticks_per_beat, quantized_ticks_per_beat);

  
  std::vector<std::vector<note>> all_notes(tracks_num, std::vector<note>());
  extract_notes(r, all_notes);


  quantize_notes(all_notes, current_ticks_per_beat, quantized_ticks_per_beat);
  
  std::vector<std::vector<std::vector<std::vector<music_sequence_event>>>> all_blocks_sequences;

  int block_counter = 0;
  int block_num = 0;
  int nes_tracks_num = 4;
  // std::cout << std::endl << "used midi blocks:" << std::endl;

  if(used_midi_blocks.size() == 0) {
    return sequence_array{NULL, 0, 0, 0, 0, 0};
  }

  for(const auto& used_midi_block : used_midi_blocks){

    int block_measures = used_midi_block.length_measures;
    
    // std::cout << "measures: " << block_measures << " TIME_SIGNATURE: " << used_midi_block.time_signature_1 << "/" << used_midi_block.time_signature_2 << " TEMPO: " << used_midi_block.tempo << "nes-configs-num" << all_posible_nes_tracks.size() << std::endl;



    int measure_start;
    int measure_end;


    for(int h = 0; h < all_posible_nes_tracks.size(); h++){

      std::vector<std::vector<std::vector<music_sequence_event>>> current_block_sequences;

      std::vector<std::vector<note>> all_notes_reconfig(nes_tracks_num, std::vector<note>());
      std::vector<int> midi_programs(nes_tracks_num, -1);

      for(int j = 0; j < nes_tracks_num; j++){
        if(all_posible_nes_tracks[h][j] > -1){
          all_notes_reconfig[j] = all_notes[all_posible_nes_tracks[h][j]];
          midi_programs[j] = program_nums[all_posible_nes_tracks[h][j]];
        }
      }
      // for(int j = 0; j < nes_tracks_num; j++){
      //    std::cout << midi_programs[j] << " ";
      // }
      // std::cout << std::endl;

      for(int i = 0; i < block_measures; i++){
        measure_start = used_midi_block.tick_start + i*96;
        measure_end =  used_midi_block.tick_start + (i+1)*96;
        std::vector<std::vector<music_sequence_event>> sequence(nes_tracks_num, std::vector<music_sequence_event>());

        extract_sequence(all_notes_reconfig, sequence, measure_start, measure_end, midi_programs);

        current_block_sequences.push_back(sequence);
      }

      bool all_sequences_empty = true;
      for(int i = 0; i<current_block_sequences.size(); i++){
        // std::cout << "measure-" << i << std::endl;
        for(int j = 0; j<current_block_sequences[i].size(); j++){

          // for(int k = 0; k < current_block_sequences[i][j].size(); k++){
          //   std::cout << current_block_sequences[i][j][k].event_type << "-" << current_block_sequences[i][j][k].event_value << " ";
          // }
          if(current_block_sequences[i][j][0].event_type != 5){
            all_sequences_empty = false;
          }
          // std::cout << std::endl;
        }
        // std::cout << std::endl;
        // std::cout << std::endl;
      }
      // std::cout << "!!!!! AREALLEMPTY-" << all_sequences_empty << std::endl;

      // std::cout << std::endl;
      // std::cout << std::endl;
      // std::cout << std::endl;

      if(!all_sequences_empty){
        all_blocks_sequences.push_back(current_block_sequences);
      }
    }

  }

  
  if(all_blocks_sequences.size() == 0) {
    return sequence_array{NULL, 0, 0, 0, 0, 0};
  }

  // std::cout << std::endl << std::endl << std::endl;
  int max_events = 64;
  int blocks_num = all_blocks_sequences.size();
  // [blocks_num][sequence_length_measures][nes_tracks_num][max_events]
  int**** sequences_ = new int*** [blocks_num];

  for(int h = 0; h < blocks_num; h++){
    sequences_[h] = new int**[sequence_length_measures];
    // std::cout << "block-" << h << std::endl; 
    for(int i = 0; i < sequence_length_measures; i++){
      // std::cout << "block-" << h << "__measure-" << i << std::endl; 
      sequences_[h][i] = new int*[nes_tracks_num];
      for(int j = 0; j < nes_tracks_num; j++){
        sequences_[h][i][j] = new int[max_events];
        for(int k = 0; k < max_events; k++){

          sequences_[h][i][j][k] = -1;

          if(k < all_blocks_sequences[h][i][j].size()){
            // std::cout << all_blocks_sequences[h][i][j][k].event_type << "-" << all_blocks_sequences[h][i][j][k].event_value << " ";
          
            if(all_blocks_sequences[h][i][j][k].event_type == 0){
              // start token
    
              sequences_[h][i][j][k] = 360 + all_blocks_sequences[h][i][j][k].event_value - 1;
            }
            else if(all_blocks_sequences[h][i][j][k].event_type == 1){
              // note on
              sequences_[h][i][j][k] = all_blocks_sequences[h][i][j][k].event_value;
            }
            else if(all_blocks_sequences[h][i][j][k].event_type == 2){
              // note off
              sequences_[h][i][j][k] = 128 + all_blocks_sequences[h][i][j][k].event_value;
            }
            else if(all_blocks_sequences[h][i][j][k].event_type == 3){
              // time shift
              sequences_[h][i][j][k] = 2*128 + all_blocks_sequences[h][i][j][k].event_value - 1;
            }
            else if(all_blocks_sequences[h][i][j][k].event_type == 4){
              // velocity
              sequences_[h][i][j][k] = 2*128 + 96 + all_blocks_sequences[h][i][j][k].event_value - 1;
            }
            else if(all_blocks_sequences[h][i][j][k].event_type == 5){
              // end token
              sequences_[h][i][j][k] = 489;

            }

          }   

        }
        // std::cout << std::endl;
      }
      // std::cout << std::endl << std::endl;
    } 

    // std::cout << std::endl << std::endl; 
    // std::cout << std::endl << std::endl;
  }

  // std::cout << std::endl << std::endl;
  // std::cout << std::endl << std::endl;
  // for(int h = 0; h < blocks_num; h++){
  //   std::cout << "block-" << h << std::endl; 
  //   for(int i = 0; i < sequence_length_measures; i++){
  //     std::cout << "block-" << h << "__measure-" << i << std::endl; 
  //     for(int j = 0; j < nes_tracks_num; j++){
  //       for(int k = 0; k < max_events; k++){
  //         std::cout << sequences_[h][i][j][k] << " ";
  //       }
  //       std::cout << std::endl;
  //     }
  //     std::cout << std::endl << std::endl;
  //   }
  //   std::cout << std::endl << std::endl;
  // }

  return sequence_array{sequences_, blocks_num, sequence_length_measures, nes_tracks_num, max_events, 1};
}

int main(int argc, char** argv)
{

  


  // char* midi_file_url = "/Users/zigakleine/Desktop/conditioned_symbollic_music_diffusion_preprocessing/lmd_full/0/0936ab6f223888c0009d194fd4520e6d.mid";
  // char* midi_file_url = "/Users/zigakleine/Desktop/conditioned_symbollic_music_diffusion_preprocessing/lmd_full/0/046e8798271fbcd61f394b2bd1a6dd0b.mid";
  char* midi_file_url = "/Users/zigakleine/Desktop/conditioned_symbollic_music_diffusion_preprocessing/lmd_full/0/001a5555e7b2fc9c81d76458a3a08982.mid";
//   char* midi_file_url = "/Users/zigakleine/Desktop/conditioned_symbollic_music_diffusion_preprocessing/nesmdb_flat/322_SuperMarioBros__00_01RunningAbout.mid";
  extract_note_sequences_from_midi_singletrack(midi_file_url);  

}

}