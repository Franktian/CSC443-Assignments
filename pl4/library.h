
using namespace std;
double get_jaccard_similarity (const set<string> &ngram1, const set<string> &ngram2);
double get_jaccard_similarity_modified (const set<string> &ngram1, const set<string> &ngram2);
string encrpytion (string to_be_encrpyted);
set<string> tokenizer (string to_be_tokenized, int n);
vector<string> ngram_tokenizer (string to_be_tokenized, int n);
set<string> ngram_tokenizer_modified (string to_be_tokenized, int n);
vector<string> space_tokenizer (string to_be_tokenized);