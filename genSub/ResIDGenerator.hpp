//
//  ResIDGenerator.hpp
//  srt2xml
//
//  Created by Tommy To on 14/6/2024.
//

#ifndef __RESIDGENERATOR__
#define __RESIDGENERATOR__

#include <string>
#include <pointer.hpp>


BE_SINGLETON(ResIDGenerator, false)

class _ResIDGenerator {
private:
  uint64_t idx;
  
public:
  _ResIDGenerator();
  _ResIDGenerator(uint64_t _idx);  // default: start with 1, override with it with _idx;
  
  std::string next();
};


#endif /* __RESIDGENERATOR__ */
