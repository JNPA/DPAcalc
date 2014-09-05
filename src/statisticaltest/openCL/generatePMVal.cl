int countSetBitsInInt(int i)
{
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return ((i + (i >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
}


__kernel void generatePMVal(
    __global float* intval,
    const int byteidx
    ) 
{
    int keyidx = get_global_id(0);
    int trcidx = get_global_id(1);
    int number = intval[trcidx * KEY_NUM + keyidx];
    intval[trcidx * KEY_NUM + keyidx] = countSetBitsInInt(number);
}
                                                                            