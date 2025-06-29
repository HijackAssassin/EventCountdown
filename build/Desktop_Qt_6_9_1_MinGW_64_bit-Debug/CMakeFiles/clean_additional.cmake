# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\EventCountdowns_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\EventCountdowns_autogen.dir\\ParseCache.txt"
  "EventCountdowns_autogen"
  )
endif()
