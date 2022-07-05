## Assignment 6: Huffman Coding

#### What
Huffman coding is a method of file compression using the frequency of character or simply the entropy. This algorithm determines the how often a character appears in a file and assigns a bit size according to the frequency so that the most frequently occuring character would have the shortest length.


#### Build
There are three executables that can be built for this lab: encode, decode, and entropy. Encode will use a given file and compress it. Decode will take the compressed file and uncompress it. Entropy measures the randomness or uncertaintly of a file.<br/>
```
git clone https://git.ucsc.edu/cse13s/spring2021/jpark598.git
cd jpark598/asgn6
```
Once you have created the remote repository, you can build all executables:<br/>
```
make
```
To build encode:
```
make encode
```
To build decode:
```
make decode
```
To build entropy
```
make entropy
```
After building the executable, you can run by typing ``./encode``, ``./decode``, ``./entropy``.


#### Commandline options
##### Encode
-i: infile<br/>
-o: outfile<br/>
-v: statistics. How much byte is saved in compression<br/>


##### Decode
-i: infile<br/>
-o: outfile<br/>
-v: statics. How much byte is released in decompression<br/>


##### Entropy
-h: help message
