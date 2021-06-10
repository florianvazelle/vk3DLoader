#ifndef COMPUTE_PARTICLE_HPP
#define COMPUTE_PARTICLE_HPP

namespace vkl {

  struct ComputeParticle {
    float deltaT;  // Frame delta time
    size_t particleCount;
  };

}  // namespace vkl

#endif  // COMPUTE_PARTICLE_HPP