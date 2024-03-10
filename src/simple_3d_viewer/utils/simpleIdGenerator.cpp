#include <simple_3d_viewer/utils/simpleIdGenerator.hpp>

uint64_t generateSimpleId()
{
  static uint64_t id = 0;
  return id++;
}