# Ciri
[![ci](https://github.com/codeworse/Ciri/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/codeworse/Ciri/actions/workflows/c-cpp.yml)&nbsp;
## Mertics
### Counters
`ciri::metrics::Counters<T>` used for counting integral mertics(e.g. cache hits) with large amount of update requests and small number of get requests.

Let's imagine the task: We have `T` threads, which increment some metric `M`. The first idea is to create one `std::atomic` for such metric. If we go further, we even can use `std::memory_order_relaxed` for increment operation. But there still will be some synchronization between threads(which we don't even need). 

The idea is simple:  devide counter into `T` parts and the result of counter will be the sum of parts. Each thread will have it's own part to increment and do not need to re-cache the value of metric.

Example:
```C++
  // ...
  ciri::metrics::Counter c(2); // Create counter with 2 parts and the type of metric is size_t(default)
  c.increment(1); // Add 1 to the counter
  c.decrement(1); // Substract 1 from the counter
  size_t answer = c.getApprox(); // Get the value of metric
  // ...
```

## TODO
- [ ] Add average counter
- [ ] Add set-counter
- [ ] Streaming Algorithms(count min/max/majority)
