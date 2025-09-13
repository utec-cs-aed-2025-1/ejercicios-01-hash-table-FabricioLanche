#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "chainhash.h"

using namespace std;

string toLowerCase(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

vector<string> tokenize(const string& text) {
    vector<string> words;
    stringstream ss(text);
    string word;
    
    while (ss >> word) {
        word.erase(remove_if(word.begin(), word.end(), 
                   [](char c) { return !isalnum(c); }), word.end());
        
        if (!word.empty()) {
            words.push_back(toLowerCase(word));
        }
    }
    return words;
}

ChainHash<string, vector<int>> bagOfWords(const vector<string>& documentos) {
    ChainHash<string, vector<int>> result(13);
    vector<string> tokens;
    for (size_t i = 0; i < documentos.size(); i++) {
        tokens = tokenize(documentos[i]);
        for (auto token: tokens){
            vector<int> index_appear;
            index_appear.push_back(i);
            result.set(token, index_appear);
        }
    }
    return result;
}

void printBagOfWords(ChainHash<string, vector<int>>& bow) {
    cout << "{\n";
    
    for (int i = 0; i < bow.bucket_count(); i++) {
        if (bow.bucket_size(i) > 0) {
            for (auto it = bow.begin(i); it != bow.end(i); ++it) {
                cout << " \"" << (*it).key << "\": [";
                
                const vector<int>& docs = (*it).value;
                for (size_t j = 0; j < docs.size(); j++) {
                    cout << docs[j];
                    if (j < docs.size() - 1) cout << ", ";
                }
                cout << "],\n";
            }
        }
    }
    cout << "}\n";
}

int main() {
    vector<string> documentos = {
        "La casa es grande",
        "El gato está en la casa", 
        "La casa es bonita y grande",
        "El sol brilla sobre la casa"
    };
    
    cout << "Documentos de entrada:\n";
    for (size_t i = 0; i < documentos.size(); i++) {
        cout << "[" << i << "] \"" << documentos[i] << "\"\n";
    }
    cout << "\n";
    
    ChainHash<string, vector<int>> bow = bagOfWords(documentos);
    cout << "Resultado de Bag of Words (palabra -> [documentos]):\n";
    printBagOfWords(bow);
    
    return 0;
}
