# BEEP

A novel sequential pattern-mining algorithm which *B*alances *E*ffectiveness and *E*fficiency when finding *P*atterns. This is based on "Beep: An Effective and Efficient Pattern Mining Algorithm for Multivariate Event Sequence Data" by anonymous author(s).

----

#### Dependency

- C++ compiler with C++20 support
- CMake

----

#### Usage

Run `quantitative_data_generator.py` to generate new random testing data which are named as `quantitative_base.dat` and `quantitative_base_generated_pattern.txt`.

```sh
python3 quantitative_data_generator.py
```

Use cmake 3.15 to build and run this project. Make sure that `quantitative_base.dat` and the executable file is under the same directory. 

```sh
cmake Beep
make
./BEEP
```

The output file will be named as `output_fileData_quantitative_base.txt`. 

----

#### File Introduction

- Attribute.h

  An object of *Attribute* class stands for one of attributes in an event. This file also contains some comparison operator for Attribute.

- Beep.h/.cpp

  Contains Beep class whose constructor is the basic algorithm flow of BEEP.  

- CodeTable.h/.cpp

  This class is used for our CodeTable in BEEP algorithm. It includes the set of current patterns and calculator of *L* value (which is an important index of MDL).

- Common.h

  Some common head files, defines and constant variables for all the other files.

- Cover.h/.cpp

  Constructing an object of *Cover* class presents generating a cover. The constructor of *Cover* class will update the usage stored in CodeTable object.

- Defines.h

  Some type definition.

- Event.h/.cpp

  An object of *Event* class stands for one of events in a sequence. This file also contains some comparison operator for *Event*.

- GetOpt.h/.cpp

  Used for getting option from arguments input.

- MathUtil.h/.cpp

  Some math tools used in BEEP, like logarithm of number of combinations and logarithm of factorial.

- Node.h/.cpp

  Node of the search tree of patterns.

- Pattern.h/.cpp

  An object of *Pattern* class stands for a pattern. There are several usages or other information for pattern in this class.

- PatternTable.h/.cpp

  A table used for counting the number of sequence in which two patterns appear at the same time.

- Sequence.h/.cpp

  Sequences that used for data mining. The constructor of this class reads all sequences from `.dat` file. 

- Window.h/.cpp

  A window for a pattern in a sequence is the occurrences of every event in the pattern in the sequence. A window will be bound with pattern when the pattern is trying to cover a sequence. Since the windows of a pattern is fixed, we only need to generate windows for a pattern at the first time that it is used to cover.

