#include <iostream>
#include <libremidi/reader.hpp>
#include <istream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <libremidi/message.hpp>
#include <cmath>
#include <set>



// void fill_sequence_array(libremidi::reader& r, std::vector<std::vector<music_sequence_event>>& sequence, int block_start, int block_end){
    
//   int i = 0;
//   for (const auto& track : r.tracks)
//   {


//     int track_event_index = 0;
//     int track_size = track.size();
//     int previous_tick = block_start;
//     int current_tick = block_start;
//     int ticks_elapsed = 0;
//     int max_seq_length = 64;
//     int seq_length = 0;

//     while((track_event_index < track_size) && track[track_event_index].tick < block_start){
//       track_event_index++;
//     }


//     sequence[i].push_back(music_sequence_event{3, 0});
//     seq_length++;


//     while((track_event_index < track_size) && track[track_event_index].tick < block_end && seq_length < (max_seq_length -2)){
      
//       current_tick = track[track_event_index].tick;

//       if(current_tick > previous_tick){
//         ticks_elapsed += current_tick - previous_tick;
//       } 

//       if(!track[track_event_index].m.is_meta_event()){

//         // note OFF EVENTS
//         if(track[track_event_index].m.get_message_type() == libremidi::message_type::NOTE_OFF || (track[track_event_index].m.get_message_type() == libremidi::message_type::NOTE_ON && (int)track[track_event_index].m.bytes[2] == 0)) {
//           if(ticks_elapsed > 0){
//             sequence[i].push_back(music_sequence_event{2, ticks_elapsed});
//             ticks_elapsed = 0;
//           }
//           sequence[i].push_back(music_sequence_event{1, (int)track[track_event_index].m.bytes[1]});

//         }
//         // note ON EVENTS
//         else if(track[track_event_index].m.get_message_type() == libremidi::message_type::NOTE_ON){
//           if(ticks_elapsed > 0){
//             sequence[i].push_back(music_sequence_event{2, ticks_elapsed});
//             ticks_elapsed = 0;
//           }
//           sequence[i].push_back(music_sequence_event{0, (int)track[track_event_index].m.bytes[1]});
//         }

//       }

//       previous_tick = current_tick;
//       track_event_index++; 
//     }

//     if(block_end > current_tick){
//       ticks_elapsed += (block_end - current_tick);
//     }

//     if(ticks_elapsed > 0){
//       sequence[i].push_back(music_sequence_event{2, ticks_elapsed});
//       ticks_elapsed = 0;
//     }  
  
//     i++;
//   }
// }

// void quantize(libremidi::reader& r, int ticks_per_sixteenth){

//   std::vector<note> all_notes;

//   for (auto& track : r.tracks)
//   {

//     std::vector<libremidi::track_event*> active_notes;
//     for (libremidi::track_event& event : track)
//     {

//       if(!event.m.is_meta_event()){
//         if(event.m.get_message_type() == libremidi::message_type::NOTE_OFF || (event.m.get_message_type() == libremidi::message_type::NOTE_ON && (int)event.m.bytes[2] == 0)) {
//           int active_note_index = 0;
//           bool found = false;
//           for(auto & active_note : active_notes){
//             if ((int)active_note->m.bytes[1] == (int)event.m.bytes[1]) {
//               found = true;
//               break;
//             }
//             active_note_index++;
//           }
//           if(found){
//             all_notes.push_back(note{active_notes[active_note_index], &event});
//             active_notes.erase(active_notes.begin() + active_note_index);
//           }

//         }
//         else if(event.m.get_message_type() == libremidi::message_type::NOTE_ON && (int)event.m.bytes[2] > 0){
//           active_notes.push_back(&event);
//         }

  

//       }
//       else{

//         if(event.tick%ticks_per_sixteenth < (ticks_per_sixteenth/2)){
//           event.tick = event.tick - event.tick%ticks_per_sixteenth;

//         }
//         else{
//           event.tick = event.tick + (ticks_per_sixteenth - event.tick%ticks_per_sixteenth);
//         }

//       }
  
//     }
//   }


//   for (note& n : all_notes)
//   {

//     if(n.end->tick - n.start->tick <= ticks_per_sixteenth){

//       if(n.start->tick%ticks_per_sixteenth < (ticks_per_sixteenth/2)){
//         n.start->tick = n.start->tick - n.start->tick%ticks_per_sixteenth;
//         n.end->tick = n.start->tick + ticks_per_sixteenth;
//       }
//       else{
//         n.start->tick = n.start->tick + (ticks_per_sixteenth - n.start->tick%ticks_per_sixteenth);
//         n.end->tick = n.start->tick + ticks_per_sixteenth;
//       }
    
//     }
//     else{
//       if(n.start->tick%ticks_per_sixteenth < (ticks_per_sixteenth/2)){
//         n.start->tick = n.start->tick - n.start->tick%ticks_per_sixteenth;
//       }
//       else{
//         n.start->tick = n.start->tick + (ticks_per_sixteenth - n.start->tick%ticks_per_sixteenth);
//       }

//       if(n.end->tick%ticks_per_sixteenth < (ticks_per_sixteenth/2)){
//         n.end->tick = n.end->tick - n.end->tick%ticks_per_sixteenth;
//       }
//       else{
//         n.end->tick = n.end->tick + (ticks_per_sixteenth - n.end->tick%ticks_per_sixteenth);
//       }

//     }
//   }
//     // std::cout << "\n\n";
// }


// void split_midi(std::vector<libremidi::track_event>& control_events, std::vector<tempo_signature_block>& midi_blocks, int ticks_per_sixteenth, int last_note_event_tick){

//   int current_time_signature_1;
//   int current_time_signature_2;
//   int current_tempo;
//   int current_time = 0;

//   for(const auto& event : control_events){
//     std::cout << "Event at " << event.tick << " : ";
//     std::string meta_event_type_str = "";
//     libremidi::meta_event_type eventType = event.m.get_meta_event_type();
//     switch (eventType) {
//       case libremidi::meta_event_type::SEQUENCE_NUMBER: meta_event_type_str = "SEQUENCE_NUMBER"; break;  
//       case libremidi::meta_event_type::TEXT: meta_event_type_str = "TEXT"; break;
//       case libremidi::meta_event_type::COPYRIGHT: meta_event_type_str = "COPYRIGHT"; break;
//       case libremidi::meta_event_type::TRACK_NAME: meta_event_type_str = "TRACK_NAME"; break;
//       case libremidi::meta_event_type::INSTRUMENT: meta_event_type_str = "INSTRUMENT"; break;
//       case libremidi::meta_event_type::LYRIC: meta_event_type_str = "LYRIC"; break;
//       case libremidi::meta_event_type::MARKER: meta_event_type_str = "MARKER"; break;
//       case libremidi::meta_event_type::CUE: meta_event_type_str = "CUE"; break;
//       case libremidi::meta_event_type::PATCH_NAME: meta_event_type_str = "PATCH_NAME"; break;
//       case libremidi::meta_event_type::DEVICE_NAME: meta_event_type_str = "DEVICE_NAME"; break;
//       case libremidi::meta_event_type::CHANNEL_PREFIX: meta_event_type_str = "CHANNEL_PREFIX"; break;
//       case libremidi::meta_event_type::MIDI_PORT: meta_event_type_str = "MIDI_PORT"; break;
//       case libremidi::meta_event_type::END_OF_TRACK: meta_event_type_str = "END_OF_TRACK"; break;
//       case libremidi::meta_event_type::TEMPO_CHANGE: meta_event_type_str = "TEMPO_CHANGE"; break;
//       case libremidi::meta_event_type::SMPTE_OFFSET: meta_event_type_str = "SMPTE_OFFSET"; break;
//       case libremidi::meta_event_type::TIME_SIGNATURE: meta_event_type_str = "TIME_SIGNATURE"; break;
//       case libremidi::meta_event_type::KEY_SIGNATURE: meta_event_type_str = "KEY_SIGNATURE"; break;
//       case libremidi::meta_event_type::PROPRIETARY: meta_event_type_str = "PROPRIETARY"; break;
//       case libremidi::meta_event_type::UNKNOWN: meta_event_type_str = "UNKNOWN"; break;    
//       default: meta_event_type_str = "UNKNOWN"; break;
//     }
//      std::cout << "metaEvent - message_type: " << meta_event_type_str  << " data: ";  
//     for(int i = 0; i < event.m.size(); i++){
//       std::cout << (int)event.m.bytes[i] << ' ';
//     }
//     std::cout << '\n';


//     if(eventType == libremidi::meta_event_type::TEMPO_CHANGE){
//       if(event.tick == 0 && event.tick == current_time) {
//         current_tempo =  60000000/(int)(((uint8_t)event.m.bytes[3] << 16) + ((uint8_t)event.m.bytes[4] << 8) + ((uint8_t)event.m.bytes[5])); 
        
//       }
//       else if(event.tick != current_time){
//         midi_blocks.push_back(tempo_signature_block{current_tempo, current_time_signature_1, current_time_signature_2, current_time, event.tick});
//         current_tempo = 60000000/(int)(((uint8_t)event.m.bytes[3] << 16) + ((uint8_t)event.m.bytes[4] << 8) + ((uint8_t)event.m.bytes[5]));
//         current_time = event.tick;
//       }
//       else{
//         current_tempo = 60000000/(int)(((uint8_t)event.m.bytes[3] << 16) + ((uint8_t)event.m.bytes[4] << 8) + ((uint8_t)event.m.bytes[5])); 
//       }

//     }
//     else if(eventType == libremidi::meta_event_type::TIME_SIGNATURE){
//       if(event.tick == 0 && event.tick == current_time) {
//         current_time_signature_1 = (int)event.m.bytes[3]; 
//         current_time_signature_2 = pow(2, (int)event.m.bytes[4]);
//       }
//       else if(event.tick != current_time){
//         midi_blocks.push_back(tempo_signature_block{current_tempo, current_time_signature_1, current_time_signature_2, current_time, event.tick});
//         current_time_signature_1 = (int)event.m.bytes[3]; 
//         current_time_signature_2 = pow(2, (int)event.m.bytes[4]);
//         current_time = event.tick;
//       }
//       else{
//         current_time_signature_1 = (int)event.m.bytes[3]; 
//         current_time_signature_2 = pow(2, (int)event.m.bytes[4]);
//       }
//     }
//     else if(eventType == libremidi::meta_event_type::END_OF_TRACK){
//       if(event.tick > 0) {
//         midi_blocks.push_back(tempo_signature_block{current_tempo, current_time_signature_1, current_time_signature_2, current_time, event.tick});
//       }
//       else if(event.tick != current_time){
//         midi_blocks.push_back(tempo_signature_block{current_tempo, current_time_signature_1, current_time_signature_2, current_time, last_note_event_tick});
//       }

//     }
//     else{
//       std::cerr << "ojoj" << std::endl;
//     }

//   }

// }


// void get_control_events(libremidi::reader& r, std::vector<libremidi::track_event>& control_events){

//   int tracknum = 0;
//   for (const auto& track : r.tracks)
//   {
    
//     std::cout << "\nTrack " << tracknum << "\n\n";
    
//     for (const libremidi::track_event& event : track)
//     {
      
//       if(event.m.is_meta_event() )
//       {

//         std::cout << "Event at " << event.tick << " : ";
//         std::string meta_event_type_str = "";
//         libremidi::meta_event_type eventType = event.m.get_meta_event_type();
//         switch (eventType) {
//             case libremidi::meta_event_type::SEQUENCE_NUMBER: meta_event_type_str = "SEQUENCE_NUMBER"; break;  
//             case libremidi::meta_event_type::TEXT: meta_event_type_str = "TEXT"; break;
//             case libremidi::meta_event_type::COPYRIGHT: meta_event_type_str = "COPYRIGHT"; break;
//             case libremidi::meta_event_type::TRACK_NAME: meta_event_type_str = "TRACK_NAME"; break;
//             case libremidi::meta_event_type::INSTRUMENT: meta_event_type_str = "INSTRUMENT"; break;
//             case libremidi::meta_event_type::LYRIC: meta_event_type_str = "LYRIC"; break;
//             case libremidi::meta_event_type::MARKER: meta_event_type_str = "MARKER"; break;
//             case libremidi::meta_event_type::CUE: meta_event_type_str = "CUE"; break;
//             case libremidi::meta_event_type::PATCH_NAME: meta_event_type_str = "PATCH_NAME"; break;
//             case libremidi::meta_event_type::DEVICE_NAME: meta_event_type_str = "DEVICE_NAME"; break;
//             case libremidi::meta_event_type::CHANNEL_PREFIX: meta_event_type_str = "CHANNEL_PREFIX"; break;
//             case libremidi::meta_event_type::MIDI_PORT: meta_event_type_str = "MIDI_PORT"; break;
//             case libremidi::meta_event_type::END_OF_TRACK: meta_event_type_str = "END_OF_TRACK"; break;
//             case libremidi::meta_event_type::TEMPO_CHANGE: meta_event_type_str = "TEMPO_CHANGE"; break;
//             case libremidi::meta_event_type::SMPTE_OFFSET: meta_event_type_str = "SMPTE_OFFSET"; break;
//             case libremidi::meta_event_type::TIME_SIGNATURE: meta_event_type_str = "TIME_SIGNATURE"; break;
//             case libremidi::meta_event_type::KEY_SIGNATURE: meta_event_type_str = "KEY_SIGNATURE"; break;
//             case libremidi::meta_event_type::PROPRIETARY: meta_event_type_str = "PROPRIETARY"; break;
//             case libremidi::meta_event_type::UNKNOWN: meta_event_type_str = "UNKNOWN"; break;    
//             default: meta_event_type_str = "UNKNOWN"; break;
//         }

//         if(tracknum == 0 && (meta_event_type_str == "TIME_SIGNATURE" || meta_event_type_str == "TEMPO_CHANGE" || meta_event_type_str == "END_OF_TRACK")){
//           control_events.push_back(event);
//         }              
//         std::cout << "metaEvent - message_type: " << meta_event_type_str  << " data: ";  
//         for(int i = 0; i < event.m.size(); i++){
//           std::cout <<(int)event.m.bytes[i] << ' ';
//         }
//         std::cout << '\n';
//       }
//       else
//       {
        
//         std::string note_event_type_str = "";
//         switch(event.m.get_message_type())
//         {
//           case libremidi::message_type::NOTE_ON:
//             std::cout << "Event at " << event.tick << " : ";
//             note_event_type_str = "NOTE_ON";
//             std::cout << "Note ON: "
//                       << "channel "  << event.m.get_channel() << ' '
//                       << "note "     << (int)event.m.bytes[1] << ' '
//                       << "velocity " << (int)event.m.bytes[2] << ' ' << std::endl;
//             break;
//           case libremidi::message_type::NOTE_OFF:
//             std::cout << "Event at " << event.tick << " : ";
//             note_event_type_str = "NOTE_OFF";
//             std::cout << "Note OFF: "
//                       << "channel "  << event.m.get_channel() << ' '
//                       << "note "     << (int)event.m.bytes[1] << ' '
//                       << "velocity " << (int)event.m.bytes[2] << ' ' << std::endl;
//             break;
//           default:
//           break;
//         }  
//       }
//     }

  
//     tracknum++;
    
//   }

// }


// int main(int argc, char** argv)
// {
//     using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
//     std::string myPath = "./nesdbmario";
//     int allFilesNum = 0;
//     int validFilesNum = 0;

//     int allSequencesNum = 0;
//     int validSequencesNum = 0;

//     for (const auto& dirEntry : recursive_directory_iterator(myPath)){
//       std::string filenameString  = dirEntry.path().filename().string();
//       std::cout << "\n----------------------------------------------------------------------------------------\n" 
//         << "filepath: " << filenameString << std::endl;
//       allFilesNum++;

//       // Read raw from a MIDI file
//       std::ifstream file{myPath + "/" + filenameString, std::ios::binary};
//       if(!file.is_open())
//       {
//         std::cerr << "Could not open " << filenameString << std::endl;
//         return 1;
//       }

//       std::vector<uint8_t> bytes;
//       bytes.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());


//       libremidi::reader r{true};

//       bool is_valid = parse_midi(bytes, r);

//       if(is_valid){
//         validFilesNum++;
//       }
//       else{
//         continue;
//       }

//       int last_note_event_tick = find_last_tick(r);
//       int ticks_per_sixteenth = r.ticksPerBeat/4;
//       int tracks_num = r.tracks.size();
//       int min_sequence_length_measures = 2;
//       //quantize(r, ticks_per_sixteenth);
//       sort_events(r);  

//       std::vector<libremidi::track_event> control_events;
//       get_control_events(r, control_events);

//       int sequence_length = last_note_event_tick;
//       int sequence_length_sixteenths = sequence_length/ticks_per_sixteenth;
//       sequence_length_sixteenths += (16 - sequence_length_sixteenths % 16);

//       int sequence_length_measures = sequence_length_sixteenths/16;

//       std::vector<std::vector<note>> all_notes(tracks_num, std::vector<note>());
//       extract_notes(r, all_notes);

//       std::vector< std::vector<std::vector<music_sequence_event>> > all_sequences;
//       int measure_start;
//       int measure_end;

//       std::cout << "sequence measures: " << sequence_length_measures << std::endl;
//       for(int i = 0; i < sequence_length_measures; i++){
//         measure_start = i*ticks_per_sixteenth*16;
//         measure_end = (i+1)*ticks_per_sixteenth*16;
//         std::vector<std::vector<music_sequence_event>> sequence(tracks_num, std::vector<music_sequence_event>());
//         fill_sequence_array(r, sequence, measure_start, measure_end);
//         all_sequences.push_back(sequence);
//       }


      
//       std::vector<tempo_signature_block> midi_blocks;
//       split_midi(control_events, midi_blocks, ticks_per_sixteenth, last_note_event_tick);

//       int tracks_num = r.tracks.size();
//       std::vector< std::vector<std::vector<music_sequence_event>> > all_sequences;
//       std::vector<tempo_signature_block> used_midi_blocks;
//       int sequence_length_measures = 2;

//       for(const auto& midi_block : midi_blocks){

//         if(midi_block.time_signature_1 == 4 && midi_block.time_signature_2 == 4){
//           int block_length = midi_block.tick_end - midi_block.tick_start;
//           int block_length_sixteenths = block_length/ticks_per_sixteenth;
//           if(block_length >= ticks_per_sixteenth*16*sequence_length_measures){


//             block_length_sixteenths += (16 - block_length_sixteenths % 16);
        
//             std::vector<std::vector<music_sequence_event>> sequence(tracks_num, std::vector<music_sequence_event>());
        
//             int this_block_start = midi_block.tick_start;
//             int this_block_end = midi_block.tick_end;  

//             tempo_signature_block new_block{midi_block.tempo, midi_block.time_signature_1, midi_block.time_signature_2, this_block_start, this_block_end};
//             all_sequences.push_back(sequence);
//             used_midi_blocks.push_back(new_block);
//             validSequencesNum++;

//           }
//         }
//         allSequencesNum++;
//       }


//       int block_counter = 0;
//       std::cout << "\n\n";
//       for(const auto& used_midi_block : used_midi_blocks){
//         std::cout << "start: " << used_midi_block.tick_start << ", end: " << used_midi_block.tick_end << " TIME_SIGNATURE: " << used_midi_block.time_signature_1 << "/" << used_midi_block.time_signature_2 << " TEMPO: " << used_midi_block.tempo << std::endl;
//         int block_length = used_midi_block.tick_end - used_midi_block.tick_start;
//         int block_length_sixteenths = block_length/ticks_per_sixteenth;
//         block_length_sixteenths += (16 - block_length_sixteenths % 16);
//         std::cout << "so far so fgood1" << std::endl;
//         fill_sequence_array(r, ticks_per_sixteenth, tracks_num, all_sequences[block_counter], used_midi_block.tick_start, used_midi_block.tick_end, block_length_sixteenths);
//         block_counter++;

//       }
      


//       int sequence_num = 0;

//       for(const auto& sequence : all_sequences) {
//         std::cout << "...........................Generated Sequence: " << sequence_num << " !!!!!!!!!............................" << tracks_num << " " << sequence[0].size() << std::endl;
//         for(int i = 0; i < tracks_num; i++){
//           for(int j = 0; j < sequence[i].size(); j++) {
//             std::cout << sequence[i][j].event_type << "-" << sequence[i][j].event_value << ", ";
//           }
//           std::cout << std::endl;
//         }
//         sequence_num++;
//       }

//     }

//     std::cout << "\n\n";
//     std::cout << "ALL FILES: " << allFilesNum << std::endl;
//     std::cout << "VALID FILES: " << validFilesNum << std::endl;
//     std::cout << "VALID FILES PERCENTAGE: " << ((double) validFilesNum)/((double) allFilesNum) << std::endl;

//     std::cout << "VALID sequences: " << validSequencesNum << std::endl;
//     std::cout << "ALL sequences: " << allSequencesNum << std::endl;
//     std::cout << "VALID sequences percentage: " << ((double) validSequencesNum)/((double) allSequencesNum) << std::endl;
//     std::cout << "\n-------------------------------------------------------------------------------------------------------------\n";

// }



















// void extract_note_sequences_from_midi_lakh(std::string midi_file_location)
// {


//   using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
//   std::string myPath = "./lakh_hec";
//   int allFilesNum = 0;
//   int validFilesNum = 0;

//   int allSequencesNum = 0;
//   int validSequencesNum = 0;

//   for (const auto& dirEntry : recursive_directory_iterator(myPath)){
//     std::string filenameString  = dirEntry.path().filename().string();
//     std::cout << "\n----------------------------------------------------------------------------------------\n" 
//       << "filepath: " << filenameString << std::endl;
//     allFilesNum++;

//     // Read raw from a MIDI file
//     std::ifstream file{myPath + "/" + filenameString, std::ios::binary};
//     if(!file.is_open())
//     {
//       std::cerr << "Could not open " << filenameString << std::endl;
//       return 1;
//     }

//     std::vector<uint8_t> bytes;
//     bytes.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
 

//     libremidi::reader r{true};

//     bool is_valid = parse_midi(bytes, r);
//     if(is_valid){
//       validFilesNum++;
//     }
//     else{
//       continue;
//     }


//   ////// TULE SE ZARES STVARI ZAČNEJO DOGAJAT

//     int last_note_event_tick = find_last_tick(r);
//     int ticks_per_sixteenth = r.ticksPerBeat/4;
//     int tracks_num = r.tracks.size();

//     // quantize(r, ticks_per_sixteenth);
//     sort_events(r);  

//     print_out_sequence(r);

//     std::vector<int> program_nums(tracks_num, -1);
//     get_program_numbers(r, program_nums);
//     std::cout << "num of tracks: " << tracks_num << std::endl;
//     std::cout << "program nums: " << std::endl;
//     for(int i = 0; i < program_nums.size(); i++){
//       std::cout << program_nums[i] << " ";
//     }
//     std::cout << std::endl;



//     int nes_tracks_num = 4;

//     std::vector<std::string> nes_tracks = { "P1", "P2", "TR", "NO" };
//     std::vector<std::vector<int>> all_posible_nes_tracks;

//     while(true){
//       std::vector<int> nes_track_nums(nes_tracks_num, -1);
//       get_track_mappings(program_nums, nes_track_nums);

//       if(are_all_elements_neg_one(nes_track_nums)){
//         break;
//       }

//       all_posible_nes_tracks.push_back(nes_track_nums);
//       std::cout << "nes track nums" << std::endl;
//       for(int i = 0; i< nes_track_nums.size(); i++){
//         std::cout << nes_tracks[i] << ": " << nes_track_nums[i]  << " | ";
//       }
//       std::cout << std::endl;


//     } 
    

//     std::vector<libremidi::track_event> control_events;
//     get_control_events(r, control_events);

//     std::vector<tempo_signature_block> midi_blocks;
//     split_midi_2(control_events, midi_blocks, last_note_event_tick);


//     std::vector<tempo_signature_block> used_midi_blocks;
//     int sequence_length_measures = 32;

//     std::cout << "all midi blocks:" << std::endl;

//     for(const auto& midi_block : midi_blocks){
//       int block_length = midi_block.tick_end - midi_block.tick_start;
//       int block_length_sixteenths = block_length/ticks_per_sixteenth;
//       block_length_sixteenths += (16 - block_length_sixteenths % 16);
//       int block_measures = block_length_sixteenths/16;

//       std::cout << "measures-" << block_measures << "time_signature-" << midi_block.time_signature_1 << "/" << midi_block.time_signature_2 << " tempo-" << midi_block.tempo << std::endl;
   
//       if(midi_block.time_signature_1 == 4 && midi_block.time_signature_2 == 4){

//         if(block_measures >= sequence_length_measures){
        
   
//           block_length_sixteenths += (16 - block_length_sixteenths % 16);
          
          
//           int this_block_start = midi_block.tick_start;
//           int this_block_end = midi_block.tick_end;  

//           tempo_signature_block new_block{midi_block.tempo, midi_block.time_signature_1, midi_block.time_signature_2, this_block_start, this_block_end};

//           used_midi_blocks.push_back(new_block);
//           validSequencesNum++;

//         }
//       }
//       allSequencesNum++;
//     }


    

//     int block_counter = 0;
//     std::cout << std::endl << "used midi blocks:" << std::endl;
//     for(const auto& used_midi_block : used_midi_blocks){

//       int block_length = used_midi_block.tick_end - used_midi_block.tick_start;
//       int block_length_sixteenths = block_length/ticks_per_sixteenth;
//       block_length_sixteenths += (16 - block_length_sixteenths % 16);
//       int block_measures = block_length_sixteenths/16;
      
//       std::cout << "measures: " << block_measures << " TIME_SIGNATURE: " << used_midi_block.time_signature_1 << "/" << used_midi_block.time_signature_2 << " TEMPO: " << used_midi_block.tempo << std::endl;



//       // int block_length = used_midi_block.tick_end - used_midi_block.tick_start;
//       // int block_length_sixteenths = block_length/ticks_per_sixteenth;
//       // block_length_sixteenths += (16 - block_length_sixteenths % 16);
//       // fill_sequence_array(r, ticks_per_sixteenth, tracks_num, all_sequences[block_counter], used_midi_block.tick_start, used_midi_block.tick_end, block_length_sixteenths);
//       // block_counter++;

//     }

//     // int sequence_num = 0;

//     // for(const auto& sequence : all_sequences) {
//     //   std::cout << "...........................Generated Sequence: " << sequence_num << " !!!!!!!!!............................" << tracks_num << " " << sequence[0].size() << std::endl;
//     //   for(int i = 0; i < tracks_num; i++){
//     //     for(int j = 0; j < sequence[0].size(); j++) {
//     //       std::cout << sequence[i][j] << " ";
//     //     }
//     //     std::cout << std::endl;
//     //   }
//     //   sequence_num++;
//     // }
   
//   }

//   std::cout << "\n\n";
//   std::cout << "ALL FILES: " << allFilesNum << std::endl;
//   std::cout << "VALID FILES: " << validFilesNum << std::endl;
//   std::cout << "VALID FILES PERCENTAGE: " << ((double) validFilesNum)/((double) allFilesNum) << std::endl;

//   std::cout << "VALID sequences: " << validSequencesNum << std::endl;
//   std::cout << "ALL sequences: " << allSequencesNum << std::endl;
//   std::cout << "VALID sequences percentage: " << ((double) validSequencesNum)/((double) allSequencesNum) << std::endl;
//   std::cout << "\n-------------------------------------------------------------------------------------------------------------\n";
// }



// void fill_sequence_array(libremidi::reader& r, int ticks_per_sixteenth, int tracks_num, std::vector<std::vector<int>>& sequence, int block_start, int block_end, int sequence_length){
//   int i = 0;
//   for (const auto& track : r.tracks)
//   {

//     int note_playing = -1;
//     int track_event_index = 0;
//     int track_size = track.size();
//     std::vector<active_note> active_notes;
//     // std::cout << "\nTrack " << i << " ,size " << track_size << "\n\n";

//     while((track_event_index < track_size) && track[track_event_index].tick < block_start){
//       track_event_index++;
//     }

//     for(int j = 0; j < sequence_length; j++) {

//       int start_tick = block_start + ticks_per_sixteenth*j;
//       int end_tick = block_start + ticks_per_sixteenth*(j+1);

//       while((track_event_index < track_size) && track[track_event_index].tick < block_end && track[track_event_index].tick < end_tick){
        
//         if(!track[track_event_index].m.is_meta_event()){
//           if(track[track_event_index].m.get_message_type() == libremidi::message_type::NOTE_OFF || (track[track_event_index].m.get_message_type() == libremidi::message_type::NOTE_ON && (int)track[track_event_index].m.bytes[2] == 0)) {
//             int active_note_index = 0;
//             for(active_note& n : active_notes){
//               if (n.notenum == (int)track[track_event_index].m.bytes[1]) {
//                 active_notes.erase(active_notes.begin() + active_note_index);
//                 break;
//               }
//               active_note_index++;
//             }

//           }
//           else if(track[track_event_index].m.get_message_type() == libremidi::message_type::NOTE_ON){
//             active_notes.push_back(active_note{(int)track[track_event_index].m.bytes[1], track[track_event_index].tick});

//           }

    

//         }
//         track_event_index++;   
//       }  

//       if(active_notes.size() > 0) {

//         bool found = false;
//         bool is_new = false;
//         for(active_note& n : active_notes){
//           // std::cout << "position: " << j << " note: " << n.notenum << " starttick: " << n.start_tick << std::endl;
//           if(n.notenum == note_playing) {
//             if(n.start_tick >= start_tick){
//               is_new = true;
//             }
//             found = true;
//             break;
//           }
//         }
//         if(found && !is_new) {
//           sequence[i][j] = -2;
        
//         }
//         else{

//           int highest_new = -1;
//           for(active_note& n : active_notes){
//             if(n.notenum > highest_new && n.start_tick >= start_tick) {
//               highest_new = n.notenum;
//             }
//           }

//           if(highest_new > -1){
//             sequence[i][j] = highest_new;
//             note_playing = highest_new;
//           }
//           else{
//             sequence[i][j] = -1;
//             note_playing =-1;
//           }

//         }
      
//       }
//       else{
//         sequence[i][j] = -1;
//         note_playing =-1;

//       }
//       //  std::cout << "\n\n";   
//     }
//     i++;
//   }
// }



// void quantize(libremidi::reader& r, int ticks_per_sixteenth){

//   std::vector<note> all_notes;

//   for (auto& track : r.tracks)
//   {

//     std::vector<libremidi::track_event*> active_notes;
//     for (libremidi::track_event& event : track)
//     {

//       if(!event.m.is_meta_event()){
//         if(event.m.get_message_type() == libremidi::message_type::NOTE_OFF || (event.m.get_message_type() == libremidi::message_type::NOTE_ON && (int)event.m.bytes[2] == 0)) {
//           int active_note_index = 0;
//           bool found = false;
//           for(auto & active_note : active_notes){
//             if ((int)active_note->m.bytes[1] == (int)event.m.bytes[1]) {
//               found = true;
//               break;
//             }
//             active_note_index++;
//           }
//           if(found){
//             all_notes.push_back(note{active_notes[active_note_index], &event});
//             active_notes.erase(active_notes.begin() + active_note_index);
//           }

//         }
//         else if(event.m.get_message_type() == libremidi::message_type::NOTE_ON && (int)event.m.bytes[2] > 0){
//           active_notes.push_back(&event);
//         }

  

//       }
//       else{

//         if(event.tick%ticks_per_sixteenth < (ticks_per_sixteenth/2)){
//           event.tick = event.tick - event.tick%ticks_per_sixteenth;

//         }
//         else{
//           event.tick = event.tick + (ticks_per_sixteenth - event.tick%ticks_per_sixteenth);
//         }

//       }
  
//     }
//   }


//   for (note& n : all_notes)
//   {

//     if(n.end->tick - n.start->tick <= ticks_per_sixteenth){

//       if(n.start->tick%ticks_per_sixteenth < (ticks_per_sixteenth/2)){
//         n.start->tick = n.start->tick - n.start->tick%ticks_per_sixteenth;
//         n.end->tick = n.start->tick + ticks_per_sixteenth;
//       }
//       else{
//         n.start->tick = n.start->tick + (ticks_per_sixteenth - n.start->tick%ticks_per_sixteenth);
//         n.end->tick = n.start->tick + ticks_per_sixteenth;
//       }
    
//     }
//     else{
//       if(n.start->tick%ticks_per_sixteenth < (ticks_per_sixteenth/2)){
//         n.start->tick = n.start->tick - n.start->tick%ticks_per_sixteenth;
//       }
//       else{
//         n.start->tick = n.start->tick + (ticks_per_sixteenth - n.start->tick%ticks_per_sixteenth);
//       }

//       if(n.end->tick%ticks_per_sixteenth < (ticks_per_sixteenth/2)){
//         n.end->tick = n.end->tick - n.end->tick%ticks_per_sixteenth;
//       }
//       else{
//         n.end->tick = n.end->tick + (ticks_per_sixteenth - n.end->tick%ticks_per_sixteenth);
//       }

//     }
//   }
//     // std::cout << "\n\n";
// }



// void split_midi(std::vector<libremidi::track_event>& control_events, std::vector<tempo_signature_block>& midi_blocks, int ticks_per_sixteenth, int last_note_event_tick){

//   int current_time_signature_1;
//   int current_time_signature_2;
//   int current_tempo;
//   int current_time = 0;

//   for(const auto& event : control_events){
//     current_time = event.tick;
//     std::cout << "Event at " << event.tick << " : ";
//     std::string meta_event_type_str = "";
//     libremidi::meta_event_type eventType = event.m.get_meta_event_type();
//     switch (eventType) {
//       case libremidi::meta_event_type::SEQUENCE_NUMBER: meta_event_type_str = "SEQUENCE_NUMBER"; break;  
//       case libremidi::meta_event_type::TEXT: meta_event_type_str = "TEXT"; break;
//       case libremidi::meta_event_type::COPYRIGHT: meta_event_type_str = "COPYRIGHT"; break;
//       case libremidi::meta_event_type::TRACK_NAME: meta_event_type_str = "TRACK_NAME"; break;
//       case libremidi::meta_event_type::INSTRUMENT: meta_event_type_str = "INSTRUMENT"; break;
//       case libremidi::meta_event_type::LYRIC: meta_event_type_str = "LYRIC"; break;
//       case libremidi::meta_event_type::MARKER: meta_event_type_str = "MARKER"; break;
//       case libremidi::meta_event_type::CUE: meta_event_type_str = "CUE"; break;
//       case libremidi::meta_event_type::PATCH_NAME: meta_event_type_str = "PATCH_NAME"; break;
//       case libremidi::meta_event_type::DEVICE_NAME: meta_event_type_str = "DEVICE_NAME"; break;
//       case libremidi::meta_event_type::CHANNEL_PREFIX: meta_event_type_str = "CHANNEL_PREFIX"; break;
//       case libremidi::meta_event_type::MIDI_PORT: meta_event_type_str = "MIDI_PORT"; break;
//       case libremidi::meta_event_type::END_OF_TRACK: meta_event_type_str = "END_OF_TRACK"; break;
//       case libremidi::meta_event_type::TEMPO_CHANGE: meta_event_type_str = "TEMPO_CHANGE"; break;
//       case libremidi::meta_event_type::SMPTE_OFFSET: meta_event_type_str = "SMPTE_OFFSET"; break;
//       case libremidi::meta_event_type::TIME_SIGNATURE: meta_event_type_str = "TIME_SIGNATURE"; break;
//       case libremidi::meta_event_type::KEY_SIGNATURE: meta_event_type_str = "KEY_SIGNATURE"; break;
//       case libremidi::meta_event_type::PROPRIETARY: meta_event_type_str = "PROPRIETARY"; break;
//       case libremidi::meta_event_type::UNKNOWN: meta_event_type_str = "UNKNOWN"; break;    
//       default: meta_event_type_str = "UNKNOWN"; break;
//     }
//      std::cout << "metaEvent - message_type: " << meta_event_type_str  << " data: ";  
//     for(int i = 0; i < event.m.size(); i++){
//       std::cout << (int)event.m.bytes[i] << ' ';
//     }
//     std::cout << '\n';


//     if(eventType == libremidi::meta_event_type::TEMPO_CHANGE){
//       if(event.tick == 0 && event.tick == current_time) {
//         current_tempo =  60000000/(int)(((uint8_t)event.m.bytes[3] << 16) + ((uint8_t)event.m.bytes[4] << 8) + ((uint8_t)event.m.bytes[5])); 
        
//       }
//       else if(event.tick != current_time){
//         midi_blocks.push_back(tempo_signature_block{current_tempo, current_time_signature_1, current_time_signature_2, current_time, event.tick});
//         current_tempo = 60000000/(int)(((uint8_t)event.m.bytes[3] << 16) + ((uint8_t)event.m.bytes[4] << 8) + ((uint8_t)event.m.bytes[5]));
//         current_time = event.tick;
//       }
//       else{
//         current_tempo = 60000000/(int)(((uint8_t)event.m.bytes[3] << 16) + ((uint8_t)event.m.bytes[4] << 8) + ((uint8_t)event.m.bytes[5])); 
//       }

//     }
//     else if(eventType == libremidi::meta_event_type::TIME_SIGNATURE){
//       if(event.tick == 0 && event.tick == current_time) {
//         current_time_signature_1 = (int)event.m.bytes[3]; 
//         current_time_signature_2 = pow(2, (int)event.m.bytes[4]);
//       }
//       else if(event.tick != current_time){
//         midi_blocks.push_back(tempo_signature_block{current_tempo, current_time_signature_1, current_time_signature_2, current_time, event.tick});
//         current_time_signature_1 = (int)event.m.bytes[3]; 
//         current_time_signature_2 = pow(2, (int)event.m.bytes[4]);
//         current_time = event.tick;
//       }
//       else{
//         current_time_signature_1 = (int)event.m.bytes[3]; 
//         current_time_signature_2 = pow(2, (int)event.m.bytes[4]);
//       }
//     }
//     else if(eventType == libremidi::meta_event_type::END_OF_TRACK){
//       if(event.tick > 0) {
//         midi_blocks.push_back(tempo_signature_block{current_tempo, current_time_signature_1, current_time_signature_2, current_time, event.tick});
//       }
//       else if(event.tick != current_time){
//         midi_blocks.push_back(tempo_signature_block{current_tempo, current_time_signature_1, current_time_signature_2, current_time, last_note_event_tick});
//       }

//     }
//     else{
//       std::cerr << "ojoj" << std::endl;
//     }

//   }

// }


   // int block_length = used_midi_block.tick_end - used_midi_block.tick_start;
    // int block_length_sixteenths = block_length/ticks_per_sixteenth;
    // block_length_sixteenths += (16 - block_length_sixteenths % 16);
    // fill_sequence_array(r, ticks_per_sixteenth, tracks_num, all_sequences[block_counter], used_midi_block.tick_start, used_midi_block.tick_end, block_length_sixteenths);
    // block_counter++;


//     void extract_sequence(libremidi::reader& r,  std::vector<std::vector<note>>& all_notes, std::vector<std::vector<music_sequence_event>>& sequence, int block_start, int block_end){

//   std::vector<std::vector<note>> block_notes(all_notes.size(), std::vector<note>());

//   std::cout << "block-ticks " << block_start << "-" << block_end << std::endl;

//   for(int i = 0; i < all_notes.size(); i++){

//     int notes_num = 0;
//     std::set<int> unique_time_ticks;

//     for(int j = 0; j < all_notes[i].size(); j++){

//       // std::cout << all_notes[i][j].start->tick << "-" << all_notes[i][j].end->tick << " ";
//       if(all_notes[i][j].start->tick >= block_start && all_notes[i][j].start->tick < block_end){


//         if(all_notes[i][j].end->tick > block_end){
//           all_notes[i][j].end->tick = block_end;
//         }

//         unique_time_ticks.insert(all_notes[i][j].start->tick);
//         unique_time_ticks.insert(all_notes[i][j].end->tick);

//         if(all_notes[i][j].start->tick > 0){
//           unique_time_ticks.insert(all_notes[i][j].start->tick);
//         }
//         if(all_notes[i][j].end->tick > 0){
//           unique_time_ticks.insert(all_notes[i][j].end->tick);
//         }

//         block_notes[i].push_back(all_notes[i][j]);
//         notes_num++;
//       }
//     }
//     unique_time_ticks.insert(block_end);
//     // std::cout << std::endl;

//     int events_num = 3 + 2 * notes_num + unique_time_ticks.size();

//     //std::cout << "eventsnum " << events_num << std::endl;

//     if(events_num > 64) {
//       int events_to_erase = events_num - 64;
//       int notes_to_erase = events_to_erase / 2 + (events_to_erase % 2);

//       for (int k = 0; k < notes_to_erase; k++){
//         block_notes[i].pop_back();
//       }

//     }

//   }

//   for (int i = 0; i < block_notes.size(); i++){
    

//     std::vector<libremidi::track_event*> block_notes_events;

//     for(int j = 0; j < block_notes[i].size(); j++){

//       block_notes_events.push_back(block_notes[i][j].start);
//       block_notes_events.push_back(block_notes[i][j].end);
//     }   

//     std::sort(block_notes_events.begin(), block_notes_events.end(), compare_tick_pointers);

//     int previous_tick = block_start;
//     int current_tick = block_start;
//     int ticks_elapsed = 0;

//     sequence[i].push_back(music_sequence_event{0, r.tracks[i][0].track});
//     sequence[i].push_back(music_sequence_event{4, 1});

//     for (auto& block_note : block_notes_events){

//       std::cout << block_note->tick << " ";

//       current_tick = block_note->tick;

//       if(current_tick > previous_tick){
//         ticks_elapsed += current_tick - previous_tick;
//       } 


//       if(!block_note->m.is_meta_event()){

//         // note OFF EVENTS
//         if(block_note->m.get_message_type() == libremidi::message_type::NOTE_OFF || (block_note->m.get_message_type() == libremidi::message_type::NOTE_ON && (int)block_note->m.bytes[2] == 0)) {
//           if(ticks_elapsed > 0){
//             sequence[i].push_back(music_sequence_event{3, ticks_elapsed});
//             ticks_elapsed = 0;
//           }
         
//           sequence[i].push_back(music_sequence_event{2, (int)block_note->m.bytes[1]});

//         }

//         // note ON EVENTS
//         else if(block_note->m.get_message_type() == libremidi::message_type::NOTE_ON){
//           if(ticks_elapsed > 0){
//             sequence[i].push_back(music_sequence_event{3, ticks_elapsed});
//             ticks_elapsed = 0;
//           }
//           sequence[i].push_back(music_sequence_event{1, block_note->m.bytes[1]});
//         }

//       }
//       previous_tick = current_tick;
      
//     }

//     if(block_end > current_tick){
//       ticks_elapsed += (block_end - current_tick);
//     }

//     if(ticks_elapsed > 0){
//       sequence[i].push_back(music_sequence_event{3, ticks_elapsed});
//       ticks_elapsed = 0;
//     }  

//     std::cout << std::endl;
//     sequence[i].push_back(music_sequence_event{5, 0});

//   }

// }
