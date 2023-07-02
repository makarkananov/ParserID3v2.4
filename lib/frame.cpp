#include "frame.h"
#include "conversions.h"

#include <iostream>

void Frame::Read(std::ifstream& mp3_file, const std::string& id) {
  id_ = id;
  mp3_file.read(reinterpret_cast<char*>(&size_), sizeof(size_));
  size_ = ConvertFromSyncSafe(size_);
  std::bitset<8> flags_byte;
  mp3_file.read(reinterpret_cast<char*>(&flags_byte), 1);
  tag_alter_mode_ = flags_byte[6];
  file_alter_mode_ = flags_byte[5];
  is_read_only_ = flags_byte[4];
  mp3_file.read(reinterpret_cast<char*>(&flags_byte), 1);
  is_in_group_ = flags_byte[6];
  is_compressed_ = flags_byte[3];
  is_encrypted_ = flags_byte[2];
  is_unsynch_ = flags_byte[1];
  has_data_length_indicator_ = flags_byte[0];
}

void TextFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  mp3_file.read(reinterpret_cast<char*>(&encoding_), 1);
  int32_t frame_bytes_left = size_ - 1;

  while(frame_bytes_left > 0){
    inner_.emplace_back();
    uint8_t byte;
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
    frame_bytes_left--;
    while (byte != 0x0 && frame_bytes_left > 0) {
      inner_[inner_.size() - 1] += char(byte);
      mp3_file.read(reinterpret_cast<char*>(&byte), 1);
      frame_bytes_left--;
    }
  }
}

void CounterFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  std::bitset<64> counter_bits;
  size_t frame_bytes_left = size_;
  if (frame_bytes_left >= 4) {
    mp3_file.read(reinterpret_cast<char*>(&counter_bits), frame_bytes_left);
    counter_ = counter_bits.to_ulong();
  }
}

void LINKFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  linked_id_.resize(4);
  mp3_file.read(&linked_id_[0], 4);
  uint8_t byte;
  mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  while (byte != 0x0) {
    url_ += char(byte);
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  }
  size_t frame_bytes_left = size_ - url_.length() - 5;
  additional_.resize(frame_bytes_left);
  mp3_file.read(&additional_[0], frame_bytes_left);
}

void CommentFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  mp3_file.read(reinterpret_cast<char*>(&encoding_), 1);
  language_.resize(3);
  mp3_file.read(&language_[0], 3);
  uint8_t byte;
  mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  while (byte != 0x0) {
    description_ += char(byte);
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  }
  size_t description_len = description_.length();

  int32_t frame_bytes_left = size_ - 5 - description_len;
  while(frame_bytes_left > 0){
    inner_.emplace_back();
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
    frame_bytes_left--;
    while (byte != 0x0 && frame_bytes_left > 0) {
      inner_[inner_.size() - 1] += char(byte);
      mp3_file.read(reinterpret_cast<char*>(&byte), 1);
      frame_bytes_left--;
    }
  }
}

void URLFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  url_.resize(size_);
  mp3_file.read(&url_[0], size_);
}

void POPMFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  uint8_t byte;
  mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  while (byte != 0x0) {
    user_email_ += char(byte);
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  }
  mp3_file.read(reinterpret_cast<char*>(&rating_), 1);
  std::bitset<64> counter_bits;
  size_t frame_bytes_left = size_ - user_email_.length() - 1;
  if (frame_bytes_left >= 4) {
    mp3_file.read(reinterpret_cast<char*>(&counter_bits), frame_bytes_left);
    counter_ = counter_bits.to_ulong();
  }
}
void ETCOFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  mp3_file.read(reinterpret_cast<char*>(&time_stamp_format_), 1);
  size_t ind = 1;
  while (ind < size_) {
    events_.emplace_back();
    mp3_file.read(reinterpret_cast<char*>(&events_[events_.size() - 1].type), 1);
    mp3_file.read(reinterpret_cast<char*>(&events_[events_.size() - 1].time_stamp), sizeof(Event::time_stamp));
    ind += 5;
  }
}
void SYLTFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  mp3_file.read(reinterpret_cast<char*>(&encoding_), 1);
  //TODO consts
  language_.resize(3);
  mp3_file.read(&language_[0], 3);
  mp3_file.read(reinterpret_cast<char*>(&time_stamp_format_), 1);
  mp3_file.read(reinterpret_cast<char*>(&type_), 1);

  uint8_t byte;
  mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  while (byte != 0x0) {
    description_ += char(byte);
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  }
  int32_t frame_bytes_left = size_ - language_.length() - description_.length() - 4;
  while (frame_bytes_left > 0) {
    events_.emplace_back();
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
    while (byte != 0x0) {
      events_[events_.size() - 1].sync_id += char(byte);
      mp3_file.read(reinterpret_cast<char*>(&byte), 1);
    }
    mp3_file.read(reinterpret_cast<char*>(&events_[events_.size() - 1].time_stamp), sizeof(Event::time_stamp));
    frame_bytes_left -= (events_[events_.size() - 1].sync_id.length() + 1);
    frame_bytes_left -= sizeof(Event::time_stamp);
  }
}

void COMRFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  mp3_file.read(reinterpret_cast<char*>(&encoding_), 1);
  uint8_t byte;
  mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  while (byte != 0x0) {
    price_ += char(byte);
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  }
  valid_until_.resize(8);
  mp3_file.read(&valid_until_[0], 8);
  mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  while (byte != 0x0) {
    url_ += char(byte);
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  }
  mp3_file.read(reinterpret_cast<char*>(&received_as_), 1);
  mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  while (byte != 0x0) {
    seller_ += char(byte);
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  }
  mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  while (byte != 0x0) {
    description_ += char(byte);
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  }
  mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  while (byte != 0x0) {
    picture_MIME_type += char(byte);
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  }
  size_t frame_bytes_left = size_ - price_.length() - valid_until_.length()
  - url_.length() - seller_.length() - description_.length()
  - picture_MIME_type.length() - 6;
  binary_ = std::vector<char>(frame_bytes_left);
  mp3_file.read(reinterpret_cast<char*>(&binary_[0]), frame_bytes_left);
}

void EncryptionFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  uint8_t byte;
  mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  while (byte != 0x0) {
    owner_id_ += char(byte);
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  }
  mp3_file.read(reinterpret_cast<char*>(&method_symbol_), 1);
  int32_t frame_bytes_left = size_ - owner_id_.length() - 2;
  binary_ = std::vector<char>(frame_bytes_left);
  mp3_file.read(reinterpret_cast<char*>(&binary_[0]), frame_bytes_left);
}

void OffsetFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  mp3_file.read(reinterpret_cast<char*>(&offset_), sizeof(offset_));
}

void RBUFFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  mp3_file.read(reinterpret_cast<char*>(&buffer_size_), sizeof(buffer_size_));
  mp3_file.read(&embedded_info_, 1);
  mp3_file.read(reinterpret_cast<char*>(&offset_), sizeof(offset_));
}

void OWNEFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  mp3_file.read(reinterpret_cast<char*>(&encoding_), 1);
  uint8_t byte;
  mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  while (byte != 0x0) {
    price_ += char(byte);
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  }
  date_.resize(8);
  mp3_file.read(reinterpret_cast<char*>(&date_[0]), 8);
  size_t frame_bytes_left = size_ - price_.length() - 9;
  seller_.resize(frame_bytes_left);
  mp3_file.read(reinterpret_cast<char*>(&seller_[0]), frame_bytes_left);
}

void OwnerIDFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  uint8_t byte;
  mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  while (byte != 0x0) {
    owner_id_ += char(byte);
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
  }
  int32_t frame_bytes_left = size_ - owner_id_.length() - 1;
  binary_ = std::vector<char>(frame_bytes_left);
  mp3_file.read(reinterpret_cast<char*>(&binary_[0]), frame_bytes_left);
}

void USERFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  mp3_file.read(reinterpret_cast<char*>(&encoding_), 1);
  language_.resize(3);
  mp3_file.read(&language_[0], 3);

  uint8_t byte;
  int32_t frame_bytes_left = size_ - 4;
  while(frame_bytes_left > 0){
    inner_.emplace_back();
    mp3_file.read(reinterpret_cast<char*>(&byte), 1);
    frame_bytes_left--;
    while (byte != 0x0 && frame_bytes_left > 0) {
      inner_[inner_.size() - 1] += char(byte);
      mp3_file.read(reinterpret_cast<char*>(&byte), 1);
      frame_bytes_left--;
    }
  }
}

void POSSFrame::Read(std::ifstream& mp3_file, const std::string& id) {
  Frame::Read(mp3_file, id);
  mp3_file.read(reinterpret_cast<char*>(&time_stamp_format_), 1);
  mp3_file.read(reinterpret_cast<char*>(&position_), sizeof(position_));
}

void Frame::Print() {
  std::cout << "frame id: " << id_ << std::endl;
  std::cout << "frame size: " << size_ << "B" << std::endl;
  std::cout << "frame 1st byte flags: " << tag_alter_mode_ << file_alter_mode_ << is_read_only_ << std::endl;
  std::cout << "frame 2st byte flags: " << is_in_group_ << is_compressed_ << is_encrypted_ << is_unsynch_
            << has_data_length_indicator_ << std::endl;
}

void TextFrame::Print() {
  Frame::Print();
  std::cout << "encoding: " << (int) encoding_ << std::endl;
  std::cout << "inner: " << std::endl;
  for(const auto& el : inner_){
    std::cout << el << std::endl;
  }
}

void CounterFrame::Print() {
  Frame::Print();
  std::cout << "counter: " << counter_ << std::endl;
}

void CommentFrame::Print() {
  TextFrame::Print();
  std::cout << "language: " << language_ << std::endl;
  std::cout << "description: " << description_ << std::endl;
}

void URLFrame::Print() {
  Frame::Print();
  std::cout << "url: " << url_ << std::endl;
}

void LINKFrame::Print() {
  URLFrame::Print();
  std::cout << "linked frame id: " << linked_id_ << std::endl;
  std::cout << "additional: " << additional_ << std::endl;
}

void POPMFrame::Print() {
  CounterFrame::Print();
  std::cout << "email: " << user_email_ << std::endl;
  std::cout << "rating: " << (int) rating_ << std::endl;
}

void ETCOFrame::Print() {
  Frame::Print();
  std::cout << "time stamp format: " << (int) time_stamp_format_ << std::endl;
  for (size_t i = 0; i < events_.size(); ++i) {
    std::cout << "event# " << (i + 1) << std::endl;
    std::cout << "\ttype of event: " << (int) events_[i].type << std::endl;
    std::cout << "\ttime stamp: " << events_[i].time_stamp << std::endl;
  }
}

void SYLTFrame::Print() {
  Frame::Print();
  std::cout << "encoding: " << (int) encoding_ << std::endl;
  std::cout << "language: " << language_ << std::endl;
  std::cout << "time stamp format: " << (int) time_stamp_format_ << std::endl;
  std::cout << "content type: " << (int) type_ << std::endl;
  std::cout << "description: " << description_ << std::endl;
  for (size_t i = 0; i < events_.size(); ++i) {
    std::cout << "event#" << (i + 1);
    std::cout << events_[i].sync_id << " ";
    std::cout << events_[i].time_stamp << std::endl;
  }
}

void OffsetFrame::Print() {
  Frame::Print();
  std::cout << "offset: " << offset_ << std::endl;
}

void OwnerIDFrame::Print() {
  BinaryFrame::Print();
  std::cout << "owner id: " << owner_id_ << std::endl;
}

void BinaryFrame::Print() {
  Frame::Print();
  for(auto el : binary_){
    std::cout << "binary data: ";
    std::cout << std::bitset<8>(el);
  }
  std::cout << std::endl;
}

void COMRFrame::Print() {
  BinaryFrame::Print();
  std::cout << "encoding: " << (int)encoding_ << std::endl;
  std::cout << "price: " << price_ << std::endl;
  std::cout << "valid until: " << valid_until_ << std::endl;
  std::cout << "contact url: " << url_ << std::endl;
  std::cout << "received as: " << (int)received_as_ << std::endl;
  std::cout << "name of seller: " << seller_ << std::endl;
  std::cout << "description: " << description_ << std::endl;
}

void EncryptionFrame::Print() {
  OwnerIDFrame::Print();
  std::cout << "method symbol: " << method_symbol_ << std::endl;
}

void OWNEFrame::Print() {
  Frame::Print();
  std::cout << "encoding: " << (int)encoding_ << std::endl;
  std::cout << "price: " << price_ << std::endl;
  std::cout << "date: " << date_ << std::endl;
  std::cout << "seller: " << seller_ << std::endl;
}

void USERFrame::Print() {
  TextFrame::Print();
  std::cout << "language: " << language_ << std::endl;
}

void RBUFFrame::Print() {
  OffsetFrame::Print();
  std::cout << "buffer size: " << buffer_size_ << std::endl;
  std::cout << "embedded info: " <<  (int)embedded_info_ << std::endl;
}

void POSSFrame::Print() {
  Frame::Print();
  std::cout << "time stamp format: " <<  (int)time_stamp_format_ << std::endl;
  std::cout << "position: " << position_ << std::endl;
}
