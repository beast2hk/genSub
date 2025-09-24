//
//  ResIDGenerator.cpp
//  srt2xml
//
//  Created by Tommy To on 14/6/2024.
//

#include "ResIDGenerator.hpp"
#include <cstdio>

_ResIDGenerator::_ResIDGenerator():_ResIDGenerator(1) {}

_ResIDGenerator::_ResIDGenerator(uint64_t _idx):idx(_idx) {}

std::string _ResIDGenerator::next() {
  char buf[500];
  std::snprintf(buf, 100, "r%llu", idx++);
  return buf;
}
