d='door'

function extend_figure
{
	w=$(identify $1 |cut -f 3 -d ' '|cut -f 1 -d x)
	h=$(identify $1 |cut -f 3 -d ' '|cut -f 2 -d x)
	if [ $w -lt 500 ]; then	convert $1 -bordercolor white -border $(((500-$w)/2))x0 $1;fi
	if [ $h -lt 500 ]; then	convert $1 -bordercolor white -border 0x$(((500-$h)/2)) $1;fi
	convert $1 -resize 500x500 $1
	w=$(identify $1 |cut -f 3 -d ' '|cut -f 1 -d x)
	h=$(identify $1 |cut -f 3 -d ' '|cut -f 2 -d x)
	if [ $w -lt 500 ]; then	convert $1 -bordercolor white -border $(((500-$w)/2))x0 $1;fi
	if [ $h -lt 500 ]; then	convert $1 -bordercolor white -border 0x$(((500-$h)/2)) $1;fi
	convert $1 -resize 500x500 $1
}

function create_graph
{
	echo digraph 
	echo { 
	echo node [penwidth="2" fontname=\"palatino bold\"]; 
	echo edge [penwidth="2"]; 
	for m in $(cut -f 1 -d ' ' models.tmp)
	do	
		e=0
		for n in $(cut -f 1 -d ' ' models.tmp)
		do
			#echo -ne Comparing $m and $n' ';
			if [ $(paste $m.txt $n.txt|tr \\t ' '|./t-test|grep -c higher) == 1 ]
			then
				echo $(grep $n best.txt|cut -d ' ' -f 2,4|sed s/' '/_/|sed s/\_0//) '->' $(grep $m best.txt|cut -d ' ' -f 2,4|sed s/' '/_/|sed s/\_0//) ;
				e=1
			fi
		done
		if [ $e == 0 ]; then echo $(grep $m best.txt|cut -d ' ' -f 2,4|sed s/' '/_/|sed s/\_0//);fi
	done
	echo }
}

rm best.txt
grep -v '#' ../src/models/test_models.txt >models.tmp
for m in $(cut -f 1 -d ' ' models.tmp)
do
	errmin=100
	indmin=0
	for o in $(cat models.tmp |grep $m|sed  -e 's/\s\+/\ /g'|cut -f 2-100 -d ' ');
	do
		err=$(cat ../results/$d/$m\_$o.txt|head -n 1)
		sm=$(echo $err $errmin|awk '{a=0}($1 > $2){a=1}{print a}')
		if [ $sm == 0 ];
		then
			errmin=$err
			indmin=$o
		fi
	done
	cat ../results/$d/$m\_$indmin.txt >$m.txt
	echo Model $m param $indmin has $errmin error.  >>best.txt
done

create_graph |dot -Tpdf >$d.pdf
convert -density 200 $d.pdf -trim -bordercolor white $d.png 
extend_figure $d.png
cat best.txt |cut -f 2,4 -d ' '|tr ' ' _|sed s/$/.txt/|sed s/^/..\\/results\\/$d\\//
f=0
n=$((1+$(cat models.tmp|grep -c .)));
cat draw_summary_skelet.gnu|sed s/XXX/1.0\\/$n/ >draw_summary.gnu
for i in $(cut -f 1 -d ' ' models.tmp);
do 
	echo \'$(grep $i best.txt |cut -f 2,4 -d ' '|tr ' ' _|sed s/$/.txt/|sed s/^/..\\/results\\/$d\\//)\' 'using ($0+XXX):($1*100) with boxes title' \'$i\',\\|sed s/XXX/\($f-$n*0.5+1\)\\/$n\.0/ >>draw_summary.gnu;
	f=$(($f+1))
done
gnuplot draw_summary.gnu >graphs.fig
fig2dev -Lpdf graphs.fig graphs.pdf
convert -density 200 graphs.pdf graphs.png
extend_figure graphs.png 
convert -size 1100x600 xc:white \
	-draw 'Image src-over 25,100 500,500 'graphs.png'' \
	-draw 'Image src-over 575,100 500,500 '$d.png'' \
	-pointsize 32 \
	-draw 'Text 100,40 "Performance of temporal models for door state prediction"' \
	-pointsize 18 \
	-gravity North \
	-draw 'Text 0,60 "Arrow A->B means that A performs statistically significantly better that B"' summary.png;
cp summary.png  ../results/summary.png
