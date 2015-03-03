#define HEADER 0
#define SEQUENCE 1
#define PLUS 2
#define QUAL 3
#define LEFT 0
#define RIGHT 1

using namespace std;
void deduplicate_reads(string left_reads, string right_reads);
int get_mean_quality(string qual_1, string qual_2);
int get_qual(string qual);
void print_fastq(map<string, vector<vector<string>> * > *fastq_table, string left_reads, string right_reads);
