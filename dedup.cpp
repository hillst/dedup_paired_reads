#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include "dedup.h" 

#define HUGE_START_BIN 1600000000
#define SMALL_START_BIN 100
/*
 * TODO:
 *
 *      ***) reall the most important thing is adding options. Ask the user for the number of reads
 *        ) If they dont provide is with n-reads we should try and guess based on the size of the file
 *        ) It may be helpful to get other profiling metrics, like memory usage and such, also to catch that damn out of memory exception
 *        ) Progress bar would be nice, especially considering how much tiem we spent waiting on IO anyway, perhaps a reads/second metric
 *        ) We need various options for input sizes. Check system memory, predict usage based on input size, select datastructure baed on this, tell the user it wont work
 *        ) before running, let them force it to run anyway
 *        ) Create some method of shuffling and collapsing reads, identify problematic reads and track those instead (maybe after a first pass)
 *        
 *      1) add a maximum size to the unordered map, clear it when we hit a certain point to be less memory prohibitive.
 *          If you want to extrapolate off of this we could shuffle around data when we have to flush, some sort of merge or combinatorial checking
 *          Another option is to track which reads show up the most and retain those, like a counter or something for when we flush
 *      2) Change our start size lol
 *      3) This needsa getops module or something to help the user, right now it only supports ONE file. and we want it to support many
 *
 */

void deduplicate_reads(string left_reads, string right_reads){
    ifstream left(left_reads);
    ifstream right(right_reads);
    string left_str;
    string right_str;
    int count = 0;
    string merged;
    //map<string, vector<vector<string>> *> seen_read_pairs(HUGE_START_BIN);
    map<string, vector<vector<string>> *> seen_read_pairs;
    //vector<vector<string>> buffer(2);
    auto buffer = new vector<vector<string>>(2);
    long reads_analyzed = 0;
    long duplicated_reads = 0;
    while(getline(left, left_str)){
        //ugly pointer notation, consider a macro or function to handle this mess. would make it more readable but not any more functional
        getline(right, right_str);
        if (count == 0){
            buffer = new vector<vector<string>>(2);
        }
        if ((count % 4) == HEADER){
            buffer->at(LEFT).push_back(left_str);
            buffer->at(RIGHT).push_back(right_str);
        }
        if ((count % 4) == SEQUENCE){
            merged = left_str + right_str;
            buffer->at(LEFT).push_back(left_str);
            buffer->at(RIGHT).push_back(right_str);
        }
        if ((count % 4) == PLUS){
            buffer->at(LEFT).push_back(left_str);
            buffer->at(RIGHT).push_back(right_str);
        }
        if ((count % 4) == QUAL){
            buffer->at(LEFT).push_back(left_str);
            buffer->at(RIGHT).push_back(right_str);
        }
         
        count++;
        int current_best_quality = 0;
        int buffer_quality = 0;
        if (count == 4){
            reads_analyzed++;
            if (reads_analyzed % 10000000 == 0){
                cerr << "Reads analyzed: " << reads_analyzed * 2 << "\n";
            }
            
            if (seen_read_pairs.count(merged)){
                current_best_quality = get_qual(seen_read_pairs[merged]->at(LEFT)[QUAL]) +  get_qual(seen_read_pairs[merged]->at(RIGHT)[QUAL]);
                buffer_quality = get_qual(buffer->at(LEFT)[QUAL]) + get_qual(buffer->at(RIGHT)[QUAL]);
                if ( buffer_quality > current_best_quality ){
                    delete seen_read_pairs[merged];
                    seen_read_pairs[merged] = buffer;
                    duplicated_reads+=2;
                }
            } 
            else{
                seen_read_pairs[merged] = buffer;
            }
            count = 0;
        }
    }
    left.close();
    right.close();
    print_fastq(&seen_read_pairs, "dedup." + left_reads , "dedup." +right_reads);
    cout << "duplicated_reads: " <<duplicated_reads << "\n";
    cout << "total_reads: " << reads_analyzed << "\n";
    cout << "portion duplicated: " << (float)duplicated_reads / reads_analyzed << "\n";
}

/* Prints the paired fastq files in the associated hash map */
void print_fastq(map<string, vector<vector<string>> * > * fastq_table, string left_reads, string right_reads){
    ofstream left_out(left_reads);
    ofstream right_out(right_reads);
    //second refers to value, first refers to key
    for ( auto it = fastq_table->begin(); it != fastq_table->end(); ++it  ){
        for (int i = 0; i < 4; i++){
            left_out << it->second->at(LEFT)[i] << "\n";
        }
        for (int i = 0; i < 4; i++){
            right_out << it->second->at(RIGHT)[i] << "\n";
        }
    }
    left_out.close();
    right_out.close();
}

/* returns the mean quality scores for to quality strings */
int get_mean_quality(string qual_1, string qual_2){
    int sum_1, sum_2;
    sum_1 = get_qual(qual_1);
    sum_2 = get_qual(qual_2);
    return (sum_1 + sum_2) / (qual_1.size() + qual_2.size());
    
}

/* returns the quality for one quality string */
int get_qual(string qual){
    int sum = 0;
    for (unsigned int i = 0; i < qual.size(); i++) {
        sum += qual[i];
    }
    return sum;
}

int main(int argc, char *argv[]){
    //map<string, int> months();
    if (argc < 3){
        cerr << "USAGE:" << "\n";
        cerr << "dedup\t<LEFT.fa>\t<RIGHT.fa>\n";
        return -1;
    }
    string left_reads = argv[1];
    string right_reads = argv[2];
    deduplicate_reads(left_reads, right_reads); 
}
