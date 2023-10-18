
cmake_minimum_required (VERSION 3.12)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("msa")
add_executable (msa "msa.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET msa PROPERTY CXX_STANDARD 20)
endif()

find_package(libmodbus REQUIRED)
find_package(simdjson REQUIRED)
target_link_libraries(msa PRIVATE modbus simdjson::simdjson)
