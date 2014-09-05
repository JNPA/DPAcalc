__kernel void reduce(
    __global float* buffer,
    __local float* scratch,
    __local float* scratch_index,
    const int length,
    __global float* result,
    __global float* result_index) 
{
    int global_index = get_global_id(0);
    float accumulator = -INFINITY;
    int index = -1;
    // Loop sequentially over chunks of input vector
    while (global_index < length) {
        float element = fabs(buffer[global_index]);
        accumulator = (accumulator > element) ? accumulator : element;
        index = (accumulator > element) ? index : global_index;
        global_index += get_global_size(0);
    }
    // Perform parallel reduction
    int local_index = get_local_id(0);
    scratch[local_index] = accumulator;
    scratch_index[local_index] = index;
    barrier(CLK_LOCAL_MEM_FENCE);
    for(int offset = get_local_size(0) / 2; offset > 0; offset = offset / 2) {
        if (local_index < offset) {
            if(scratch[local_index] < scratch[local_index + offset]) {
                scratch[local_index] = scratch[local_index + offset];
                scratch_index[local_index] = scratch_index[local_index + offset];
            }
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    if (local_index == 0) {
        result[get_group_id(0)] = scratch[0];
        result_index[get_group_id(0)] = scratch_index[0];
    }
}
                                                                            