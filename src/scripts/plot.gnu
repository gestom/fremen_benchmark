set terminal fig color
unset ytics 
set title 'Frequency domain'
set xlabel 'Frequency [1/week]'
set size 0.4,0.77
set origin 0.6,0.06
plot [-0.5:10.5][0:0.3] \
'tmp/spectrum.txt' using 2:($3-0.005) with impulses lw 2 notitle,\
'tmp/spectrum.txt' using 2:3:(0.3) with circle lw 1 lc 1 notitle,\
'tmp/selected.txt' using 2:($3-0.005) with impulses lw 2 lc 2 notitle,\
'tmp/selected.txt' using 2:3:(0.3) with circle lw 1 lc 2 notitle

unset title
set size   0.65,0.38
set origin 0.0,0.1
set ytics nomirror 0,1
unset xlabel
set ylabel 'p(t)' offset 3.68,0
set xtics nomirror ('Tue' 1,'Wed' 2,'Thu' 3,'Fri' 4,'Sat' 5,'Sun' 6, 'Mon' 7) offset -3.0,0.2 
plot [0:7] [-0.2:1.2] \
'tmp/week.txt' using ($0/24/60):1 with lines lc 1 lw 2 notitle,\
'tmp/prob.txt' using ($0/24/60):1 with lines lc 2 lw 2 notitle

set title 'Time domain'
set size   0.65,0.44
set origin 0.0,0.39
set ytics nomirror 0,1
set ylabel 's(t)' offset 3.68,0
set xtics nomirror ('' 1,'' 2,'' 3,'' 4,'' 5,'' 6, '' 7) 
plot [0:7] [-0.2:1.2] \
'tmp/prob.txt' using ($0/24/60):1 with lines lc 2 lw 2 notitle,\
'tmp/prob.txt' using ($0/24/60):2 with lines lc 3 lw 2 notitle

unset title
set size 0.65,0.15
unset border
unset ylabel 
set origin 0.00,0.01
unset xtics 
unset ytics
set key horizontal samplen 3
plot [-1:-0.5] [-0.2:1.2] \
'b.txt' using ($0/24/6):($1) with lines lc 1 lw 2 title 'Observed',\
'b.txt' using ($0/24/6):($2) with lines lc 2 lw 2 title 'Model',\
'b.txt' using ($0/24/6):($2) with lines lc 3 lw 2 title 'Reconst.' 
