count-matrix-downsizer
===================
This tool rarefies OTUs or genes raw abundance profiles to even sequencing depth across samples.

Requirements
-------------
- A C++ compiler
- [Boost C++ Libraries](http://www.boost.org)
 
Usage
-------------
````
./count-matrix-downsizer -h
````

Input count matrix example
-------------
````
gene_id sample_1 sample_2 sample 3
gene_1 10 0 3
gene_2 0 21 8
gene_3 12 9 3
gene_4 10 12 11
````
