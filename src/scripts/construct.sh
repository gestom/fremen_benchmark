../bin/fremen week.txt prob.txt 1 >tmp/spectral.txt
cat tmp/spectral.txt|grep Spectrum >tmp/spectrum.txt
cat week.txt |sed -n 60~60p >tmp/week.txt

for i in $(seq 0 9);
do
../bin/fremen tmp/ali.txt prob.txt $i >tmp/spectral.txt
cat tmp/spectral.txt|grep Selected >tmp/selected.txt
cat prob.txt |sed -n 60~60p >tmp/prob.txt
gnuplot plot.gnu >tmp/aha_$i.fig
convert -density 200 tmp/aha_$i.fig -rotate 90 -crop 950x450+640+620 output/model_$i.png
done
