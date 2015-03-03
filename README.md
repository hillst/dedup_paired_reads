# Dedup
## Introduction
Dedup is a tool created for deduplicating a set of paired-end and mate-pair libraries. Currently there are no good tools for performing this in a memory and time efficient manner. FastUniq is the most current version of this type of program. FastUniq however uses a sorting based method with pre-allocated arrays, unfortunately this can cause problems for extremely large datasets. No efforts have been made to handle datasets larger than memory currently available. We propose a memory efficient and iterative process to this problem. Note, this is an active work in progress.

## Method
For each read pair, create a hash for the concatenated read. If the read has been seen before, keep the read with the highest quality score, otherwise record the read. Different datastructures and algorithms are used based on the size of the input used. For instance, if a dataset is approaching 2 billion reads, this would take over 1TB of memory to store all of the reads in memory and to correctly detect duplicates. By partitioning the dataset, and identifying problematic reads early (essentially any seen duplicate is kept and all others are written), we can efficiently traverse a large dataset. After each partition, the reads are shuffled and the process is run again.

## Usage and setup
make
./dedup leftreads.1.fastq rightreads.2.fastq

Output should have: dedup.leftreads.1.fastq, dedup.rightreads.2.fastq
Written to standard out is a set of deduplication statistics, total reads seen and total collapsed reads.


