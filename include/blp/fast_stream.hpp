// Inspired from https://fgiesen.wordpress.com/2011/11/21/buffer-centric-io/
// Allow zero-copy stream with variable chunk size.
// This makes it easy to compose transformation (reading a jpg in zip...) without decompressing the whole jpg first.
// Stream is a state machine and can switch read function to track state without branching.
// See also:
// https://fgiesen.wordpress.com/2016/01/02/end-of-buffer-checks-in-decompressors/
// zlib switch bit stream implementation: use fast one when far from boundary, use precise/slow one when clause to boundary.
//
// Checkout the part about Duff's Device used to simulate coroutine in C for I/O.
// http://github.com/Cyan4973/FiniteStateEntropy/
// => Yann Collet’s FSE. Have a look, use branchless I/O


// https://fgiesen.wordpress.com/2015/10/25/reading-and-writing-are-less-symmetric-than-you-probably-think/
// => fflush() is not enough to ensure change are visibile. Need to do it on the directory too (on Linux)!

