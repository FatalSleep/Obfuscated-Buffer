# Route Buffer
A buffer that obfuscates it's memory via pointer re-routing.


This is done by passing an array of integer unordered indices to the buffer. The buffer then allocates an array of **pointers** which is used for the routes. From `0 - N` in the route buffer in **order** we map each pointer in the route buffer to the corrosponding pointer at index `R` in the memory buffer. Where `R` is the index we pulled from the route table at position `X` in range of `0 - N`.
