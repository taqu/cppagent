#include "apiprovider.h"

namespace cppagent
{
// IAPIProvider::ChatCompletionResponse
    bool IAPIProvider::ChatCompletionResponse::isSuccess() const
{
        return 200 == status_;
}

// IAPIProvider
IAPIProvider::IAPIProvider()
{
}

IAPIProvider::~IAPIProvider()
{
}

} // namespace cppagent