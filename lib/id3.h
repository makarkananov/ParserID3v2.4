#pragma once

#include "frame.h"
#include "conversions.h"

#include <bitset>
#include <fstream>
#include <string>
#include <vector>

#include <iostream>

class Id3 {
 public:
  explicit Id3(const std::string& filepath);
 private:
  uint8_t major_;
  uint8_t revision_number_;
  bool unsynch_mode_;
  bool extended_header_mode_;
  bool experimental_mode_;
  bool footer_mode_;
  uint32_t size_;
  uint32_t extended_header_size_;
  uint8_t number_of_flag_bytes_;
  bool tag_is_update_;
  bool crc_present_;
  bool tag_restrictions_;
  std::vector<Frame> frames;
};
