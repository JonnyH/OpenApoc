#pragma once

namespace OpenApoc
{

class SoundBackend;

class SoundBackendFactory
{
  public:
	virtual SoundBackend *create(int concurrent_sample_count) = 0;
	virtual ~SoundBackendFactory() = default;
};

SoundBackendFactory *getNullSoundBackend();
SoundBackendFactory *getSDLSoundBackend();
}; // namespace OpenApoc
