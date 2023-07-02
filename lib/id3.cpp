#include "id3.h"

Id3::Id3(const std::string& filepath) {
  std::ifstream mp3_file(filepath, std::ios::binary);
  if (!mp3_file.is_open()) {
    std::cerr << "MP3 file is not found" << std::endl;
    exit(1);
  }
  std::cout << "Starting ID3 parsing process for file: " << filepath << std::endl;
  std::string tag_name(3, ' ');
  mp3_file.read(reinterpret_cast<char*>(&tag_name[0]), 3);
  std::cout << tag_name;
  if (tag_name != "ID3") {
    std::cerr << "Unknown MP3 tag" << std::endl;
    exit(1);
  }
  mp3_file.read(reinterpret_cast<char*>(&major_), 1);
  mp3_file.read(reinterpret_cast<char*>(&revision_number_), 1);
  std::cout << "v2." << static_cast<int>(major_) << '.' << static_cast<int>(revision_number_) << std::endl;
  if (major_ != 4) {
    std::cerr << "Unknown major of ID3 tag" << std::endl;
    exit(1);
  }
  std::bitset<8> flags_byte;
  mp3_file.read(reinterpret_cast<char*>(&flags_byte), 1);
  unsynch_mode_ = flags_byte[7];
  extended_header_mode_ = flags_byte[6];
  experimental_mode_ = flags_byte[5];
  footer_mode_ = flags_byte[4];
  std::cout << "tag flags: " << unsynch_mode_ << extended_header_mode_ << experimental_mode_ << footer_mode_
            << std::endl;
  if(footer_mode_) size_ -= 10;
  if(extended_header_mode_){
    mp3_file.read(reinterpret_cast<char*>(&extended_header_size_), sizeof(extended_header_size_));
    size_ = ConvertFromSyncSafe(extended_header_size_);
    mp3_file.read(reinterpret_cast<char*>(&size_), number_of_flag_bytes_);
    mp3_file.read(reinterpret_cast<char*>(&flags_byte), 1);
    tag_is_update_ = flags_byte[6];
    crc_present_ = flags_byte[5];
    tag_restrictions_ = flags_byte[4];
    std::cout << "extended flags: " << tag_is_update_ << crc_present_ << tag_restrictions_ << std::endl;
    size_t skip_bytes = extended_header_size_ - 6;
    mp3_file.ignore(skip_bytes);
  }
  mp3_file.read(reinterpret_cast<char*>(&size_), sizeof(size_));
  size_ = ConvertFromSyncSafe(size_);
  std::cout << "tag size: " << size_ << "B" << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  size_t frame_ind = 0;
  while (mp3_file.tellg() < size_) {
    frame_ind++;
    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << frame_ind << ") ";
    std::string frame_id(4, ' ');
    mp3_file.read(reinterpret_cast<char*>(&frame_id[0]), 4);
    if (frame_id[0] == 'T') {
      TextFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id == "USER") {
      USERFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id == "COMM" || frame_id == "USLT") {
      CommentFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id[0] == 'W') {
      URLFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id == "POPM") {
      POPMFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id == "PCNT") {
      CounterFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id == "ETCO") {
      ETCOFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id == "SYLT") {
      SYLTFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id == "LINK") {
      LINKFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id == "ENCR" || frame_id == "GRID") {
      EncryptionFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id == "PRIV" || frame_id == "UFID") {
      OwnerIDFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id == "COMR") {
      COMRFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id == "POSS") {
      POSSFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id == "SEEK") {
      OffsetFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id == "RBUF") {
      RBUFFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id == "OWNE") {
      OWNEFrame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
    } else if (frame_id[0] == 0) {
      std::cout << "Padding found" << std::endl;
      break;
    } else {
      std::cout << "Unknown frame!" << std::endl;
      Frame CurFrame;
      CurFrame.Read(mp3_file, frame_id);
      CurFrame.Print();
      mp3_file.ignore(CurFrame.size());
    }
  }
}