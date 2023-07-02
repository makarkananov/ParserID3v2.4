#pragma once

#include <bitset>
#include <fstream>
#include <string>
#include <vector>

/*  Frame
 *    TextFrame
 *        CommentFrame
 *    CounterFrame
 *        POPMFrame
 *    BinaryFrame
 *        OwnerIDFrame
 *            EncryptionFrame
 *        COMRFrame
 *    OffsetFrame
 *        RBUFFrame
 *    URLFrame
 *        LINKFrame
 *    SYLTFrame
 *    ETCOFrame
 *    POSSFrame
 */

class Frame {
 public:
  virtual void Read(std::ifstream& mp3_file, const std::string& id);
  [[nodiscard]] size_t size() const {
    return size_;
  }
  virtual void Print();
  virtual ~Frame() = default;
 private:
  std::string id_;
  bool tag_alter_mode_;
  bool file_alter_mode_;
  bool is_read_only_;
  bool is_in_group_;
  bool is_compressed_;
  bool is_encrypted_;
  bool is_unsynch_;
  bool has_data_length_indicator_;
 protected:
  uint32_t size_;
};

class TextFrame : public Frame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 protected:
  uint8_t encoding_;
  std::vector<std::string> inner_;
};

class USERFrame : public TextFrame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 private:
  std::string language_;
};

class CommentFrame : public TextFrame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 private:
  std::string language_;
  std::string description_;
};

class CounterFrame : public Frame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 protected:
  uint64_t counter_ = 0;
};

class BinaryFrame : public Frame {
 public:
  void Print() override;
 protected:
  std::vector<char> binary_;
};

class URLFrame : public Frame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 protected:
  std::string url_;
};

class LINKFrame : public URLFrame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 private:
  std::string linked_id_;
  std::string additional_;
};

class POPMFrame : public CounterFrame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 private:
  std::string user_email_;
  uint8_t rating_;
};

class ETCOFrame : public Frame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 private:
  struct Event {
    uint8_t type;
    uint32_t time_stamp;
  };
  std::vector<Event> events_;
  uint8_t time_stamp_format_;
};

class SYLTFrame : public Frame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 private:
  struct Event {
    std::string sync_id;
    uint32_t time_stamp;
  };
  std::vector<Event> events_;
  uint8_t encoding_;
  std::string language_;
  uint8_t time_stamp_format_;
  uint8_t type_;
  std::string description_;
};

class COMRFrame : public BinaryFrame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 private:
  uint8_t encoding_;
  std::string price_;
  std::string valid_until_;
  std::string url_;
  uint8_t received_as_;
  std::string seller_;
  std::string description_;
  std::string picture_MIME_type;
};

class OwnerIDFrame : public BinaryFrame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 protected:
  std::string owner_id_;
};

class EncryptionFrame : public OwnerIDFrame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 private:
  uint8_t method_symbol_;
};

class OffsetFrame : public Frame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 protected:
  uint32_t offset_;
};

class RBUFFrame : public OffsetFrame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 private:
  uint32_t buffer_size_;
  char embedded_info_;
};

class OWNEFrame : public Frame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 private:
  uint8_t encoding_;
  std::string price_;
  std::string date_;
  std::string seller_;
};

class POSSFrame : public Frame {
 public:
  void Read(std::ifstream& mp3_file, const std::string& id) override;
  void Print() override;
 private:
  char time_stamp_format_;
  uint32_t position_;
};