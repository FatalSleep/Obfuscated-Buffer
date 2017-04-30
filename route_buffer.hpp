#ifndef ROUTE_BUFFER
#define ROUTE_BUFFER

#include <functional>

typedef unsigned __int8 __uint8;

template<size_t length, size_t alignment>
class route_buffer {
    private:
    const size_t falign_delta = (alignment - 1), falign_theta = ~falign_delta;
    size_t aligned_length;
    
    __uint8* buffer;
    __uint8** routes;

    // Function pointer to call a function to build a new routing table.
    std::function<void(__uint8*, __uint8**)> reroute;

    protected:
    // Base initialization cosntructor to allocate underlying memory.
    route_buffer() {
        aligned_length = (length + falign_delta) & falign_theta;
        buffer = new __uint8[aligned_length];
        routes = new __uint8*[aligned_length];
    }

    public:
    // Construct a new route_buffer and intialize using a local object's function call to build a new routing table.
    template<typename T, class F = void(__uint8*, __uint8**)>
    route_buffer(T* object, F&& function) : route_buffer() {
        reroute = std::function<void(__uint8*, __uint8**)>(function, object, std::placeholders::_2);
        reroute(buffer, routes);
    }

    // Construct a new route_buffer and intialize using a static function to build a new routing table.
    template<class F = void(__uint8*, __uint8**)>
    route_buffer(F&& function) : route_buffer() {
        reroute = std::function<void(__uint8*, __uint8**)>(function, std::placeholders::_2);
        reroute(buffer, routes);
    }

    // Construct a new route_buffer and initialize it with an existing routing table.
    route_buffer(size_t* routes) : route_buffer() {
        for (size_t i = 0; i < aligned_length; i++) {
            for (size_t i = 0; i < aligned_size; i++)
                this->routes[i] = &this->buffer[routes[i]];
        }
    }

    // Returns the aligned length of the buffer.
    size_t size() { return aligned_size; };

    // Returns the current seek position.
    size_t position() { return seek_in; };

    // Returns the current byte alignment.
    size_t alignment() { return byte_align; }

    // Returns length of <iter> aligned to the buffer's byte alignment.
    size_t align(size_t iter) { return (iter + fast_al) & fast_nt; };

    // Sets the seek position to an aligned position.
    void seek(size_t pos) { seek_in = align(pos); };

    // Sets each byte in the buffer to 0.
    void zero() { for (size_t i = 0; i < size_of; i++) memory[i] = 0; }

    // Resize buffer & rebuild the routing table.
    void resize(size_t length, size_t* routes = nullptr) {
        if (buffer != nullptr)
            delete[] buffer;
        if (this->routes != nullptr)
            delete[] routes;
        
        size_t align_len = align(length);
        buffer = new __uint8[align_len];
        this->routes = new __uint8*[align_len];

        if (routes == nullptr) {
            reroute(buffer, this->routes);
        } else {
            for (size_t i = 0; i < aligned_length; i++) {
                for (size_t i = 0; i < aligned_size; i++)
                    this->routes[i] = &this->buffer[routes[i]];
            }
        }
    }

    /*------------------------------------------------------------------------------------------------------
    Below allows us to write any instance or integeral type <T> to the buffer and move the write/read
    pointer position forward in the buffer based on the number of bytes written/read.

    This basically copies the low-level bytes of any instance of type <T> to the equivallently sized group
    of bytes in references from references[seek_in] to references[seek_in + sizeof(T)].

    Due to the fact that we're copying only the low level bytes, we can technically not only write integeral
    data, but we can write the underlying byte data of any instance as well--the ever so evil and diabolical
    beauty of reinterpret_cast.

    ** A hack using reinterpret_cast<T*>() to convert the bytes in the reference buffer to the indicated
    type of <T> so we can copy over the bytes of the data of type <T> being written/read from/to the buffer.

    Of course however though, we can take the same approach but convert type data of type <T> to a byte
    array pointer of __int8*. However it is more convient to convert to <T> instead of __int8*.
    ------------------------------------------------------------------------------------------------------*/

    // Writes the indicated value to the buffer using the buffer's underlying routed reference buffer.
    template<typename T>
    void write(T value) { *reinterpret_cast<T*>(references + (seek_in += sizeof(T)) - sizeof(T)) = value; };

    // Writes the indicated value to the buffer using the buffer's underlying routed reference buffer via the ( = ) operator.
    template<typename T>
    T operator =(T value) { *reinterpret_cast<T*>(references + (seek_in += sizeof(T)) - sizeof(T)) = value; };
};

#endif
