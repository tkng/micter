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

## Performance Test

test data: crowled Japanese blogs entries (roughly 250MB for train, 250MB for test. character encoding is UTF-8.)

train data and test data is splitted with mecab.

### open test (tested with train data):

    accuracy:  0.941353
    precision: 0.976557
    recall:    0.913756

### closed test:

    accuracy:  0.942232
    precision: 0.977311
    recall:    0.914956

## TODO

* strict parsing of model file (current implemantation is not robust).
* implement feature vector iterator.
