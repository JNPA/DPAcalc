__kernel void person_corr_new(
    __global float* stat,
    __global float* pm_buffer, 
    __global float* traces,     
    const int numtraces, 
    const float inv_traces, 
    const int offset_m,
    const int offset_pm, 
    const int offset_pm_pow_2, 
    const int offset_traces, 
    const int offset_traces_pow_2,
    const int numkeys, 
    const int numbatches)
{ 
  __global float* pm = pm_buffer + offset_m;
  __global float* somatorio_pm = pm_buffer + offset_pm; 
  __global float* somatorio_pm_pow_2 = pm_buffer + offset_pm_pow_2; 
  __global float* somatorio_traces = traces + offset_traces; 
  __global float* somatorio_traces_pow_2 = traces + offset_traces_pow_2;
  int i = get_global_id(0); 
  int j = get_global_id(1); 
  if(i >= 256 || j >= numbatches) {return;} 
  float aux = 0.0; 
  int pm_index = i; 
  int traces_index = j; 
  for (int k = 0; k < numtraces; k++, traces_index += numbatches, pm_index += numkeys) { 
      aux += pm[pm_index] * traces[traces_index]; 
  } 
  stat[ i * numbatches + j] = 
    ( aux - ( somatorio_pm[i] * somatorio_traces[j]) ) / 
    ( somatorio_pm_pow_2[i] * somatorio_traces_pow_2[j] ); 
}  
                                                                            