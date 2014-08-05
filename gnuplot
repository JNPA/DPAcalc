     set   autoscale                        # scale axes automatically
      set xtic auto                          # set xtics automatically
      set ytic auto                          # set ytics automatically
      set title "DPAcalc graphical output"
      set xlabel "Time"
      set ylabel "Pearson coefficient"

plot "exampleout" u 1:2 t "Key A" with lines, "exampleout" u 1:3 t "Key B"  with lines, "exampleout" u 1:4 t "Key C" with lines, "exampleout" u 1:5 t "Key D" with lines,"exampleout" u 1:6 t "Key E" with lines;