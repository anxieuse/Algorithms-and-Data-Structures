TIMEFORMAT=%R

if [ "$1" == "-c" ]
then
    g++ -O3 dijk.cpp -o dijk
    g++ -O3 gen.cpp -o gen
    g++ -O3 ../main.cpp -o ../main
fi

for ((i=1;;i += 1))
do
    echo "#$i"

    ./gen 100000 500000 >input
    
    echo 'main: '; 
    time ../main <input >outMain 2>/dev/null

    echo 'dijk: ';
    time ./dijk <input  >outDijk 2>/dev/null

    diff -w outMain outDijk || break

    sleep 0,3
done
