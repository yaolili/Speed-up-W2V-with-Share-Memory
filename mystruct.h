#define N 10                // number of closest words that will be shown
#define max_size 2000       // max length of strings
#define max_w 20            // max length of vocabulary entries
#define M_size 900000000    // words * size
#define vocab_size 60000000 // words * max_w

struct shared_use_st
{

	long long words, size;	
	char ch;
	float M[M_size],len;
	char vocab[vocab_size];
};
