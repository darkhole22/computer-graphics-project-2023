#include "Random.h"

namespace game {

std::random_device Random::m_Device;
std::mt19937 Random::m_Rng(Random::m_Device());

}