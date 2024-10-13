#pragma once

using namespace std;

#include <gsl/gsl>
#include <cstdint>
#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <wrl/client.h>

// Note: disabling noexcept on throwing function as most of failures from the functions are unrecoverable (e.g. rendering pipeline initialization)
#pragma warning(push)
#pragma warning(disable : 26447)

using namespace Microsoft::WRL;