#ifndef NO_COPY
#define NO_COPY(CLASS) CLASS(const CLASS& other) = delete; \
                       CLASS operator=(const CLASS& other) = delete;
#endif // NO_COPY