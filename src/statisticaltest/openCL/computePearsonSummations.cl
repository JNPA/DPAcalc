__kernel void person_corr_red(
    __global float* matrix, 
    const int wMatrix, 
    const int numtraces, 
    const float inv_traces,
    const int is_pm_matrix,
    const int offset_matrix,
    const int offset_sum,
    const int offset_sum_pow_2)
{ 
    __global float* pmm_matrix = matrix + offset_matrix;
    __global float* somatorio = matrix + offset_sum;
    __global float* somatorio_pow_2 = matrix + offset_sum_pow_2;
    int i = get_global_id(0);
    int index = i;
    if(i < wMatrix ) { 
        somatorio[ i ] = 0.0; 
        somatorio_pow_2[ i ] = 0.0; 
        for (int t = 0; t < numtraces; t++, index += wMatrix) { 
            somatorio[ i ] += pmm_matrix[index]; 
            somatorio_pow_2[ i ] += pmm_matrix[index] * pmm_matrix[index];
        } 
        somatorio_pow_2[ i ] = sqrt(somatorio_pow_2[ i ] - ( inv_traces * somatorio[ i ] * somatorio[ i ] ) ); 
        if(is_pm_matrix) {
            somatorio[ i ] *= inv_traces; 
        }
    }
} 
