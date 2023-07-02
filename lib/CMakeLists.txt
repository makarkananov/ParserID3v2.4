add_library(parsing parsing.cpp parsing.h)
add_library(conversions conversions.cpp conversions.h)
add_library(frame frame.cpp frame.h conversions)
add_library(id3 id3.cpp id3.h frame conversions)