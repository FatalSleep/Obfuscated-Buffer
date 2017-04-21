#ifndef OBFUSCATED_BUFFER
#define OBFUSCATED_BUFFER
    // +1 compile time math on templates: Aligns the size of <max_size> to the specified byte alignment.
    // Force the user to pass an array that's size is byte aligned to match the byte aligned buffer size.
    template<size_t max_size, size_t byte_align, size_t aligned_size = (max_size + (byte_align - 1)) & ~(byte_align - 1)>
    class obfuscated_buffer {
        private:
        /*
            Memory = Underlying buffer to write to.
            References = Buffer of pointers that point to each byte of the underlying Memory buffer.
            Seek_In = The current seek position of the Reference buffer, not the Memory buffer.
            Fast_Al = (Byte_Align - 1) <-- fast compute byte alignemnt.
            Fast_Nt = ~(Byte_Align - 1) <-- fast compute of byte alignment.
        */
        unsigned __int8 *memory, **references;
        size_t seek_in, fast_al, fast_nt;

        public:
        // Initiates the buffer to the aligned length w/ the specified routes applied.
        obfuscated_buffer(std::array<size_t, aligned_size> routes) {
            // Preprocessing optimization for byte alignment.
            fast_al = byte_align - 1;
            fast_nt = ~fast_al;
            seek_in = 0;

            // Initiate the initial underlying memory & the references buffer for the pointer reroutes.
            memory = new unsigned __int8[aligned_size]();
            references = new unsigned __int8*[aligned_size]();

            // Re-route all of the pointers in (references) to memory[routes[i]].
            // This will assign the (pointers) in references to the pointers of each position in (memory) at the position of routes[i].
            reroute(routes);
        }

        // Calls the buffer's destructor--don't.
        ~obfuscated_buffer() {
            if (memory != nullptr)
                delete[] memory;

            if (references != nullptr)
                delete[] references;
        }

        // Re-routes the pointers of each byte in Memory to a reordered set of byte pointers in References.
        void reroute(std::array<size_t, aligned_size> routes) {
            for (size_t i = 0; i < aligned_size; i++)
                references[i] = &memory[routes[i]];
        }

        // Return whether we have memory available for reading/writting.
        bool exists() { return memory != nullptr && references != nullptr; }

        // Returns the aligned length of the buffer.
        size_t length() { return aligned_size; };

        // Returns the current seek position.
        size_t position() { return seek_in; };

        // Returns the current byte alignment.
        size_t alignment() { return byte_align; }

        // Returns the pointer to the routed references buffer--not the underlying memory buffer.
        unsigned __int8* get() { return references; };

        // Returns length of <iter> aligned to the buffer's byte alignment.
        size_t align(size_t iter) { return (iter + fast_al) & fast_nt; };

        // Sets the seek position to an un-aligned raw position.
        void rseek(size_t pos) { seek_in = pos; };

        // Sets the seek position to an ualigned position.
        void seek(size_t pos) { seek_in = align(pos); };

        // Sets each byte in the buffer to 0.
        void zero() { for (size_t i = 0; i < size_of; i++) memory[i] = 0; }

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

        // Reads a value from the buffer using the buffer's underlying routed reference buffer.
        template<typename T>
        T read() { return *reinterpret_cast<T*>(references + (seek_in += sizeof(T)) - sizeof(T)); };

        // Reads a value from the buffer using the buffer's underlying routed reference buffer via the cast (T) operator.
        template<typename T>
        operator T() { return *reinterpret_cast<T*>(references + (seek_in += sizeof(T)) - sizeof(T)); };

        // Writes the indicated value to the buffer using the buffer's underlying routed reference buffer.
        template<typename T>
        void write(T value) { *reinterpret_cast<T*>(references + (seek_in += sizeof(T)) - sizeof(T)) = value; };

        // Writes the indicated value to the buffer using the buffer's underlying routed reference buffer via the ( = ) operator.
        template<typename T>
        T operator =(T value) { *reinterpret_cast<T*>(references + (seek_in += sizeof(T)) - sizeof(T)) = value; };
    };
#endif
