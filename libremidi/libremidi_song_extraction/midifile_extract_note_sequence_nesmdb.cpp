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
  int*** sequence;
  int dim1;
  int dim2;
  int dim3;
};

void test_library(){
  std::cout << "hehe" << std::endl;
}

bool compare_tick(libremidi::track_event i1, libremidi::track_event i2)
{
  return (i1.tick < i2.tick);
}

bool compare_tick_pointers(libremidi::track_event* i1, libremidi::track_event* i2)
{
  return (i1->tick < i2->tick);
}

void sort_events(libremidi::reader& r){
  for (auto& track : r.tracks)
  {
    std::sort(track.begin(), track.end(), compare_tick);
  }
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

void print_out_sequence(libremidi::reader& r){

  int tracknum = 0;
  for (const auto& track : r.tracks)
  {
    
    std::cout << "\nTrack " << tracknum << "\n\n";
    
    for (const libremidi::track_event& event : track)
    {
      
      if(event.m.is_meta_event() )
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


        block_notes[i].push_back(all_notes[i][j]);
        notes_num++;
      }
    }
    unique_time_ticks.insert(block_end);
    // std::cout << std::endl;

    int events_num = 3 + 2 * notes_num + unique_time_ticks.size();

    // std::cout << "eventsnum " << events_num << std::endl;
    // std::cout << "notesnum " << notes_num << std::endl;

    if(events_num > 64) {
      int events_to_erase = events_num - 64;
      int notes_to_erase = events_to_erase / 2 + (events_to_erase % 2);

      if(notes_to_erase > block_notes[i].size()){
        notes_to_erase = block_notes[i].size();
      }

      //  std::cout << "notestoerase-" << notes_to_erase << "block_notes-"<< block_notes[i].size() << std::endl;

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
          
            sequence[i].push_back(music_sequence_event{2, (int)block_note->m.bytes[1]});

          }

          // note ON EVENTS
          else if(block_note->m.get_message_type() == libremidi::message_type::NOTE_ON){
            if(ticks_elapsed > 0){
              sequence[i].push_back(music_sequence_event{3, ticks_elapsed});
              ticks_elapsed = 0;
            }
            sequence[i].push_back(music_sequence_event{1, block_note->m.bytes[1]});
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


sequence_array extract_note_sequences_from_midi(char* midi_file_location){

  std::ifstream file{midi_file_location, std::ios::binary};
  if(!file.is_open())
  {
    std::cerr << "Could not open " << midi_file_location << std::endl;
    return sequence_array{};
  }

  std::vector<uint8_t> bytes;
  bytes.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

  libremidi::reader r{true};

  bool is_valid = parse_midi(bytes, r);

  if(!is_valid){
    return sequence_array{};
  }

  int last_note_event_tick = find_last_tick(r);

  int ticks_per_sixteenth = r.ticksPerBeat/4;
  int tracks_num = r.tracks.size();

  int sequence_length = last_note_event_tick;
  int sequence_length_sixteenths = sequence_length/ticks_per_sixteenth;
  sequence_length_sixteenths += (16 - sequence_length_sixteenths % 16);
  int sequence_length_measures = sequence_length_sixteenths/16;

  sort_events(r);    
  // print_out_sequence(r); 

  std::vector<int> program_nums(tracks_num, 0);
  get_program_numbers(r, program_nums);
  // std::cout << "program nums: " << std::endl;
  // for(int i = 0; i < program_nums.size(); i++){
  //   std::cout << program_nums[i] << " ";
  // }


  // std::cout << std::endl;
  
  std::vector<std::vector<note>> all_notes(tracks_num, std::vector<note>());
  extract_notes(r, all_notes);

  // for(int i = 0; i < all_notes.size(); i++){
  //   std::cout << all_notes[i].size() << "-> ";
  //   for(int j = 0; j < all_notes[i].size(); j++){
  //     std::cout << all_notes[i][j].start->tick << "-" << all_notes[i][j].end->tick << " ";
  //   }
  //   std::cout << std::endl;
  // }

  // std::cout << std::endl;

  int current_ticks_per_beat = r.ticksPerBeat;
  int quantized_ticks_per_beat = 24;
  quantize_notes(all_notes, current_ticks_per_beat, quantized_ticks_per_beat);

  
  std::vector< std::vector<std::vector<music_sequence_event>> > all_sequences;

  // for(int i = 0; i < all_notes.size(); i++){
  //   std::cout << "tracksize-" << all_notes[i].size() << "-> ";
  //   for(int j = 0; j < all_notes[i].size(); j++){
  //     std::cout << all_notes[i][j].start->tick << "-" << all_notes[i][j].end->tick << " ";
  //   }
  //   std::cout << std::endl;
  // }

  // std::cout  << std::endl;

  int measure_start;
  int measure_end;

  for(int i = 0; i < sequence_length_measures; i++){
    measure_start = i*96;
    measure_end = (i+1)*96;
    std::vector<std::vector<music_sequence_event>> sequence(tracks_num, std::vector<music_sequence_event>());

    extract_sequence(all_notes, sequence, measure_start, measure_end, program_nums);

    // std::cout << i << "-extracted" << std::endl;
    all_sequences.push_back(sequence);
  }

  // std::cout << "alldone"  << std::endl;

  std::cout << std::endl << std::endl << std::endl;
  int max_events = 64;
  // [sequence_length_measures][tracks_num][max_events]
  int*** sequences_ = new int** [sequence_length_measures];
  for(int i = 0; i < sequence_length_measures; i++){
    sequences_[i] = new int*[tracks_num];
    std::cout << "measure-" << i << std::endl;
    for(int j = 0; j < tracks_num; j++){
      sequences_[i][j] = new int[max_events];
      for(int k = 0; k < max_events; k++){

        sequences_[i][j][k] = -1;

        if(k < all_sequences[i][j].size()){

          std::cout << all_sequences[i][j][k].event_type << "-" << all_sequences[i][j][k].event_value << " ";
          if(all_sequences[i][j][k].event_type == 0){
            // start token
  
            sequences_[i][j][k] = 360 + all_sequences[i][j][k].event_value - 1;
          }
          else if(all_sequences[i][j][k].event_type == 1){
            // note on
            sequences_[i][j][k] = all_sequences[i][j][k].event_value;
          }
          else if(all_sequences[i][j][k].event_type == 2){
            // note off
            sequences_[i][j][k] = 128 + all_sequences[i][j][k].event_value;
          }
          else if(all_sequences[i][j][k].event_type == 3){
            // time shift
            sequences_[i][j][k] = 2*128 + all_sequences[i][j][k].event_value - 1;
          }
          else if(all_sequences[i][j][k].event_type == 4){
            // velocity
            sequences_[i][j][k] = 2*128 + 96 + all_sequences[i][j][k].event_value - 1;
          }
          else if(all_sequences[i][j][k].event_type == 5){
            // end token
            sequences_[i][j][k] = 489;

          }

        }   

      }
      std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;
  }

  // std::cout << std::endl << std::endl;
  // std::cout << std::endl << std::endl;
  // for(int i = 0; i < sequence_length_measures; i++){
  //   std::cout << "measure-" << i << std::endl;
  //   for(int j = 0; j < tracks_num; j++){
  //     for(int k = 0; k < max_events; k++){
  //       std::cout << sequences_[i][j][k] << " ";
  //     }
  //     std::cout << std::endl;
  //   }
  //   std::cout << std::endl << std::endl;
  // }


  return sequence_array{sequences_, sequence_length_measures, tracks_num, max_events};
}




int main(int argc, char** argv)
{

  char* midi_file_url = "/Users/zigakleine/Desktop/scraping_and_cppmidi/nesmdb_flat/378_WaiWaiWorld2_SOS__ParsleyJou_10_11BlossomsofOedo2.mid";
  int transposition = 1;
  bool transposition_sign = True;
  extract_note_sequences_from_midi(midi_file_url);  

}

}