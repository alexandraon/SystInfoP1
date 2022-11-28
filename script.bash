#!/usr/bin/bash
echo "nThreads,run 1,run 2,run 3,run 4,run 5" > file.csv;
for n_threads in 1 2 4 8 16 32 64 128
do 
    echo -n "$n_threads">> file.csv
    for n_essai in 1 2 3 4 5
    do         
        time=$(/usr/bin/time -f "%e" make -s -j $n_threads 2>&1|tail -n 1)
        echo -n ",$time" >> file.csv
        make clean -s
    done
    echo "">> file.csv
done
