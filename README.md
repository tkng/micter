# Micter

Micter is a machine learning based word segmenter. Word splitting is a bit difficult task in agglutinative language like Japanese. Micter could be used for such languages (if you can arrange learning data.)

Micter's learning algorithm is Support Vector Machine with L1 regularization. Optimization algorithm is FOBOS(Forward-Backward Splitting). To know details of FOBOS, you may want to read to "Efficient Learning using Forward-Backward Splitting" by Duchi and Singer. ([pdf](http://www.cs.berkeley.edu/~jduchi/projects/DuchiSi09b.pdf))

## How to build

    ./waf configure
    ./waf build

You need python and g++ 4.1 or higher to build micter for now.

## How to use

    ./build/default/micter-train -m modelfile.txt learndata1.txt learndata2.txt ...
    ./build/default/micter -m modelfile.txt
    type some sentence here.

micter has benchmark mode. 

    ./build/default/micter --bench -m modelfile.txt learndata1.txt

will output accuracy, precision and recall of closed test.

## learning data format

1 word in 1 line. empty line is treated as a sentence break.

## TODO

* throw exception when error occured.
* strict parsing of model file.
* implement feature vector iterator.

