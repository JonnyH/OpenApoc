#pragma once

#if defined(USE_STD_FILESYSTEM)

#include <filesystem>
namespace OpenApoc
{
namespace fs = std::filesystem;
}
#elif defined(USE_BOOST_FILESYSTEM)

#include <boost/filesystem.hpp>
namespace OpenApoc
{
namespace fs = boost::filesystem;
}
#else
#error No filesystem implementation
#endif
