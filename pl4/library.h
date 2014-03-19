
using namespace std;
double get_jaccard_similarity (set<string> ngram1, set<string> ngram2);
string encrpytion (string to_be_encrpyted);
set<string> tokenizer (string to_be_tokenized, int n);
vector<string> ngram_tokenizer (string to_be_tokenized, int n);
vector<string> space_tokenizer (string to_be_tokenized);