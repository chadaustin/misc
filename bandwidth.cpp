#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <xmmintrin.h>

const unsigned GB = 1000000000;
__m128 buffer[GB / sizeof(__m128)];

int main() {
    clock_t start = clock(), end;
    unsigned i = 0;
    while (end = clock(), end < start + CLOCKS_PER_SEC) {
        __m128 zero = _mm_setzero_ps();
        for (unsigned j = 0; j < GB / sizeof(__m128); j += 8) {
            buffer[j] = zero;
            buffer[j+1] = zero;
            buffer[j+2] = zero;
            buffer[j+3] = zero;
            buffer[j+4] = zero;
            buffer[j+5] = zero;
            buffer[j+6] = zero;
            buffer[j+7] = zero;
        }
        ++i;
    }
    float elapsed = float(end - start) / CLOCKS_PER_SEC;
    printf("Write throughput GB/s: %f\n", i / elapsed);

    start = clock();
    i = 0;
    __m128 sum = _mm_setzero_ps();
    while (end = clock(), end < start + CLOCKS_PER_SEC) {
        __m128 sum1 = _mm_setzero_ps();
        __m128 sum2 = _mm_setzero_ps();
        __m128 sum3 = _mm_setzero_ps();
        __m128 sum4 = _mm_setzero_ps();
        for (unsigned j = 0; j < GB / sizeof(__m128); j += 4) {
            sum1 = _mm_add_ps(sum1, buffer[j]);
            sum2 = _mm_add_ps(sum2, buffer[j+1]);
            sum3 = _mm_add_ps(sum3, buffer[j+2]);
            sum4 = _mm_add_ps(sum4, buffer[j+3]);
        }
        sum = _mm_add_ps(
            sum,
            _mm_add_ps(
                _mm_add_ps(sum1, sum2),
                _mm_add_ps(sum3, sum4)));
        ++i;
    }
    float f[4];
    _mm_store_ps(f, sum);
    printf("Ignore this: %f %f %f %f\n", f[0], f[1], f[2], f[3]);
    elapsed = float(end - start) / CLOCKS_PER_SEC;
    printf("Read throughput GB/s: %f\n", i / elapsed);
}
