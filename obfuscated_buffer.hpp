/*
    Reference: https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#The_.22inside-out.22_algorithm
*/

#ifndef OBFUSCATED_BUFFER
#define OBFUSCATED_BUFFER
    #include <functional>

    template<size_t size, size_t align>
    class obfuscated_buffer {
        private:
        unsigned __int8* memory, **routed_references;
        size_t seek_in, size_of, align_of, fast_al, fast_nt;
        std::function<unsigned long long()> rand_func;

        /*
            [Modified] Fisher Yate's Inside-Out Shuffle Algorithm:
                [Original] algorithm generates a shuffled array of numbers in O(n) time.

                The modified version below generates a shuffled list of indicies in O(n) time.
                At the same time an array of pointers is generated where each pointer points
                to the corrosponding shuffled index in this class' buffer.

                This creates a buffer where each byte written to the buffer is written to a
                random index in the buffer----via index routing via pointers.
        */
        // Generates the buffer's underlying routed references buffer.
        void build_randref() {
            size_t* routes = new size_t[size_of];

            for (size_t i = 0, j; i < size_of; i++) {
                j = (i == 0)? 0 : static_cast<size_t>(rand_func()) % (i + 1);
                
                if (j != i) {
                    routes[i] = routes[j];
                    routed_references[routes[i]] = &memory[routes[i]];
                }

                routes[j] = i;
                routed_references[routes[j]] = &memory[routes[j]];
            }

            delete[] routes;
        }

        public:
        // Allocates the buffer and generates it's underlying routed reference buffer
        template<class _Fx>
        obfuscated_buffer(_Fx&& randfn) {
            rand_func = std::function<unsigned long long()>(randfn);

            align_of = align;
            fast_al = align - 1;
            fast_nt = ~(align - 1);
            size_of = fast_align(size);

            memory = new unsigned __int8[size_of]();
            routed_references = new unsigned __int8*[size_of];

            build_randref();
        };

        // Allocates the buffer and generates it's underlying routed reference buffer
        template<typename T, class _Fx>
        obfuscated_buffer(T* rand_objc, _Fx&& randfn) {
            rand_func = std::function<unsigned long long()>(std::bind(randfn, rand_objc));

            align_of = align;
            fast_al = align - 1;
            fast_nt = ~(align - 1);
            size_of = fast_align(size);

            memory = new unsigned __int8[size_of]();
            routed_references = new unsigned __int8*[size_of];

            build_randref();
        };

        ~obfuscated_buffer() { dealloc(); }

        void inline fast_align(size_t& iter) { iter = (iter + fast_al) & fast_nt; };

        size_t inline fast_align(size_t iter) { return (iter + fast_al) & fast_nt; };

        operator unsigned __int8*() { return memory; };

        bool memexists() { return memory != nullptr; }

        size_t length() { return size_of; };

        void seek(size_t pos) { seek_in = pos; };

        size_t get_seek() { return seek_in; };

        void memzero() { for (int i = 0; i < size_of; i++) memory[i] = 0; }

        // Deallocates the buffer's memory.
        void dealloc() {
            if (memory != nullptr && routed_references != nullptr) {
                delete[] memory;
                delete[] routed_references;
                memory = nullptr;
                routed_references = nullptr;
                size_of = 0;
                align_of = 0;
            }
        }

        // Deallocates existing memmory & re-allocates the buffer and generates it's underlying routed reference buffer.
        template<size_t new_size = size, size_t new_align = align>
        void alloc() {
            dealloc();

            size_of = fast_align(new_size);
            align_of = new_align;
            fast_al = align - 1;
            fast_nt = ~(align - 1);
            
            memory = new unsigned __int8[size_of];
            routed_references = new unsigned __int8*[size_of];

            build_randref();
        }

        // Reads a value from the buffer using the buffer's underlying routed reference buffer.
        template<typename T>
        T read() { return *reinterpret_cast<T*>(routed_references + (seek_in += sizeof(T)) - sizeof(T)); };

        // Writes the indicated value to the buffer using the buffer's underlying routed reference buffer.
        template<typename T>
        void write(T value) { *reinterpret_cast<T*>(routed_references + (seek_in += sizeof(T)) - sizeof(T)) = value; };
    };
#endif
