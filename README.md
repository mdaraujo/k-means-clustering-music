# System for the automatic identification of music 
#### Algorithmic Information Theory (2018/19)
#### Lab work nº1

The main objective of this work is to develop and test a system for the automatic identification
of music, using small samples for querying the database (for example, 10 seconds of audio), based
on data compression. The problem can be stated as follows. Suppose that a certain database contains
representations of several (complete) musics, denoted *mi* , and that we want to classify a segment of
music (possibly affected by noise), *x*, as belonging to one of the *mi* . For each music in the database,
a codebook, denoted *ci* , based on vector quantization, will be first created. When making a query, the
segment *x* is encoded and then decoded using each of the codebooks. We denote by *˜xi* the decoded
versions of *x*. Finally, we attribute the class (the music that was guessed) according to the *˜xi* that is
more similar to *x*.

### Relevant exercises
5. Implement a program, named *wavcb*, that computes a vector quantization codebook of an audio
file. The program should accept as input parameters, at least, the block (vector) size, the overlap
factor and the codebook size. To build the codebook, use the **k-means clustering algorithm**.
6. Implement a program, named *wavfind*, that accepts a collection of codebooks (for example,
placed in a directory) and a sample of an audio file. The program should return the most probable
music to which the audio sample belongs.


## Usage

```bash
To build:
	make

To test:
	../bin-example/wavcp sample.wav copy.wav // copies "sample.wav" into "copy.wav"
	../bin-example/wavhist sample.wav 0 // outputs the histogram of channel 0 (left) and the mono version
	../bin-example/wavquant sample.wav 8 0 // quantize "sample.wav" to 8 bits. Exports both signal in channel 0, and full wav file
	../bin-example/wavcmp quants/sample_quant_nbits8.wav sample.wav // outputs SNR and max error, between quantized and original file
	../bin-example/wavcb samples/sample04.wav 16 0 256 100 3 // creates the model (codebook) for sample04, with specified parameters
	../bin-example/wavfind samples/sample04_cut.wav codebooks // guess the best match for sample04_cut, given the codebook directories

```


## License
[MIT](https://choosealicense.com/licenses/mit/)