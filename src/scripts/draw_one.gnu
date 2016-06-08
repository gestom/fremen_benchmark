set terminal fig color
set multiplot

set xtics nomirror ('' 1,'' 2,'' 3,'' 4,'' 5,'' 6,'' 7) offset -2.5,0 
set ytics nomirror 0,1
set size 0.49,0.38
set label 1 "Static" at -1.2,0.5 rotate center
set label 2 "model" at -0.8,0.5 rotate center
set ylabel 'p(t)' offset 3.68,0
set origin 0.05,0.60
set title 'Observed state s(t) and its probability p(t)' offset 0,-0.6
plot [0:7] [-0.2:1.2] 'b.txt' using ($0/24/6):($1) with lines lc 2 lw 2 notitle, 'b.txt' using ($0/24/6):($2) with lines lc 1 lw 2 notitle

set title 'Observed state s(t) and its estimate s''(t)' offset 0,-0.6
unset label
set ylabel 's''(t)  s(t)' offset 0.5,0
set origin 0.53,0.60
set size 0.47,0.38
unset ytics
plot [0:7] [-0.2:1.2] 'b.txt' using ($0/24/6):($1*0.45+0.6) with lines lc 2 lw 2 notitle, 'b.txt' using ($0/24/6):(($2>0.5)*0.45) with lines lc 3 lw 2 notitle

set size 0.49,0.38
set label 1 "FreMEn" at -1.2,0.5 rotate center
set label 2 "order 1" at -0.8,0.5 rotate center
set ylabel 'p(t)' offset 3.68,0
set title ' ' offset 0,-0.8
set origin 0.05,0.35
set ytics nomirror 0,1
plot [0:7] [-0.2:1.2] 'b.txt' using ($0/24/6):($1) with lines lc 2 lw 2 notitle, 'b.txt' using ($0/24/6):($3) with lines lc 1 lw 2 notitle

unset label
set ylabel 's''(t)  s(t)' offset 0.5,0
set origin 0.53,0.35
set size 0.47,0.38
unset ytics
plot [0:7] [-0.2:1.2] 'b.txt' using ($0/24/6):($1*0.45+0.6) with lines lc 2 lw 2 notitle, 'b.txt' using ($0/24/6):(($3>0.5)*0.45) with lines lc 3 lw 2 notitle

set size 0.49,0.38
set ytics nomirror 0,1
set label 1 "FreMEn" at -1.2,0.5 rotate center
set label 2 "order 4" at -0.8,0.5 rotate center
set ylabel 'p(t)' offset 3.68,0
set origin 0.05,0.10
set xtics nomirror ('Tue' 1,'Wed' 2,'Thu' 3,'Fri' 4,'Sat' 5,'Sun' 6, 'Mon' 7) offset -2.0,0.2 
plot [0:7] [-0.2:1.2] 'b.txt' using ($0/24/6):($1) with lines lc 2 lw 2 notitle, 'b.txt' using ($0/24/6):($7) with lines lc 1 lw 2 notitle

unset label
set ylabel 's''(t)  s(t)' offset 0.5,0
set origin 0.53,0.10
set size 0.47,0.38
unset ytics
plot [0:7] [-0.2:1.2] 'b.txt' using ($0/24/6):($1*0.45+0.6) with lines lc 2 lw 2 notitle, 'b.txt' using ($0/24/6):(($7>0.5)*0.45) with lines lc 3 lw 2 notitle

set size 1.00,0.15

unset border
unset ylabel 
set origin 0.00,0.05
unset xtics 
unset ytics
set key horizontal samplen 3
plot [-1:-0.5] [-0.2:1.2] \
'b.txt' using ($0/24/6):($1) with lines lc 2 lw 2 title 'Observation s(t)',\
'b.txt' using ($0/24/6):($2) with lines lc 1 lw 2 title 'Probability p(t)',\
'b.txt' using ($0/24/6):($2) with lines lc 3 lw 2 title 'Estimate s''(t)' 

