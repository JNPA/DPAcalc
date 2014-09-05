__kernel void matrixMul_new(
    __global float* C, 
    __global float* pm_buf, 
    __global float* B, 
    int numtraces, 
    float inv_traces, 
    const int offset_pm,
    const int offset_sum_pm,
    const int offset_sum_pow_2_pm,
    const int offset_sum_traces,
    const int offset_sum_pow_2_traces,
    const int numkeys, 
    const int numbatches)
{
    // Matrix index
    __global float* somatorio_pm = pm_buf + offset_sum_pm;
    __global float* somatorio_pm_pow_2 = pm_buf + offset_sum_pow_2_pm;
    __global float* somatorio_traces = B + offset_sum_traces;
    __global float* somatorio_traces_pow_2 = B + offset_sum_pow_2_traces;
    __global float* A = pm_buf + offset_pm;

    // Block index
    int bx = get_group_id(0);
    int by = get_group_id(1);
 
    // Thread index
    int tx = get_local_id(0);
    int ty = get_local_id(1);
 
    // Index of the first sub-matrix of A processed 
    // by the block
    int aBegin = 16 * bx;
 
    // Index of the last sub-matrix of A processed 
    // by the block
    int aEnd   = aBegin + numtraces * 256 - 1 ;
 
    // Step size used to iterate through the 
    // sub-matrices of A
    int aStep  = 16 * 256;
 
    // Index of the first sub-matrix of B processed 
    // by the block
    int bBegin = 16 * by;
 
    // Step size used to iterate through the 
    // sub-matrices of B
    int bStep  = 16 * 256;

    float Csub = 0.0; 
    // Loop over all the sub-matrices of A and B
    // required to compute the block sub-matrix
    int a;
    int b;
    for ( a = aBegin, b = bBegin; ( a + aStep ) <= aEnd; a += aStep, b += bStep) {
        // Declaration of the local memory array As 
        // used to store the sub-matrix of A
        __local float As[16][16];
        // Declaration of the local memory array Bs 
        // used to store the sub-matrix of B
        __local float Bs[16][16];
        // Load the matrices from global memory
        // to local memory; each thread loads
        // one element of each matrix
        As[ty][tx] = A[a + 256 * ty + tx];
        Bs[ty][tx] = B[b + 256 * ty + tx];
        // Synchronize to make sure the matrices 
        // are loaded
        barrier(CLK_LOCAL_MEM_FENCE);
        // Multiply the two matrices together;
        // each thread computes one element
        // of the block sub-matrix
        for (int k = 0; k < 16; ++k)
            Csub += As[k][ty] * Bs[k][tx];
        // Synchronize to make sure that the preceding
        // computation is done before loading two new
        // sub-matrices of A and B in the next iteration
        barrier(CLK_LOCAL_MEM_FENCE);
    }

        // Declaration of the local memory array As 
        // used to store the sub-matrix of A
        __local float As[16][16];
        // Declaration of the local memory array Bs 
        // used to store the sub-matrix of B
        __local float Bs[16][16];
        // Load the matrices from global memory
        // to local memory; each thread loads
        // one element of each matrix
        As[ty][tx] = A[a + 256 * ty + tx];
        Bs[ty][tx] = B[b + 256 * ty + tx];
        // Synchronize to make sure the matrices 
        // are loaded
        barrier(CLK_LOCAL_MEM_FENCE);
        // Multiply the two matrices together;
        // each thread computes one element
        // of the block sub-matrix
        int limit = numtraces % 16;
        if(limit == 0)
            limit = 16;
        for (int k = 0; k < limit; ++k) {
            Csub += As[k][ty] * Bs[k][tx];
        }
        // Synchronize to make sure that the preceding
        // computation is done before loading two new
        // sub-matrices of A and B in the next iteration
        barrier(CLK_LOCAL_MEM_FENCE);

    // Write the block sub-matrix to device memory;
    // each thread writes one element
    int c = 256 * 16 * bx + 16 * by;
    int i = 16 * bx + ty; 
    int j = 16 * by + tx; 
    //if(bx == 0 && by == 0 && ty == 0 && tx == 15 ) {
    //    C[0] = Csub;
    //    C[1] = somatorio_pm[0];
    //    C[2] = somatorio_traces[15];
    //    C[3] = somatorio_pm_pow_2[0];
    //    C[4] = somatorio_traces_pow_2[15];
    //    C[5] = A[0];
    //    C[6] = B[15 + 256];
    //    C[7] = A[256];
    //    C[8] = B[15];
    //}
    C[c + 256 * ty + tx] = 
        ( Csub - ( somatorio_pm[i] * somatorio_traces[j]) ) / 
        ( somatorio_pm_pow_2[i] * somatorio_traces_pow_2[j] ); 
}
