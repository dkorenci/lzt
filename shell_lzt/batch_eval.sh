exefile="./$1"
files=$2
infold=$3
outfold=$4

for f in $files; do
    infile="$infold/$f"
    dictfile="$outfold/$f.lze"
    dict2txt="$dictfile.txt"
    compressOut="$outfold/$f.compress.out"
    compareOut="$outfold/$f.cmp.txt"
    testOut="$outfold/$f.test.out"
    # compress
    $exefile c -e -i $infile -d $dictfile > $compressOut 2>&1
    # output dict content to txt file and compare with original
    $exefile s -d $dictfile -s \* > $dict2txt    
    cmp $infile $dict2txt > $compareOut 2>&1
    if ! [ -s "$compareOut" ] ; then
        rm $dict2txt
    fi    
    # time locate/extract (word/index) and check (-c) all words map to correct indices
    $exefile t -d $dictfile -r 5 -n 10000 -l -e -c > $testOut 2>&1
done


