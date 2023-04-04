#ifndef NO_COPY
#define NO_COPY(CLASS) CLASS(const CLASS& other) = delete; \
                       CLASS operator=(const CLASS& other) = delete;
#endif // NO_COPY

#include <memory>

namespace vulture {

template <class T>
using Ref = std::shared_ptr<T>;

template <class T>
using WRef = std::weak_ptr<T>;

} // namespace vulture
