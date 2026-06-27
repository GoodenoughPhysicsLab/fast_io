#include "./include/fast_io.h"

int main() {
    fast_io::linux_clone3_thread t([]() {
        fast_io::println("Hello from the thread!");
    });

    t.join();
    return 0;
}
