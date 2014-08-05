awk '{for(i=2;i<=NF;i++) if(($i>0?$i:-$i) > 0.15){print i-1 ": " $i}}'
