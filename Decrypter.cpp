#include "provided.h"
#include "MyHash.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

class DecrypterImpl
{
public:
    bool load(string filename);
    vector<string> crack(const string& ciphertext);
private:
    WordList m_list;
    vector<string> m_words;
    void crackBacktracking(vector<string>& res, const string& originaltext, Translator& mapping);
};

bool DecrypterImpl::load(string filename)
{
    return m_list.loadWordList(filename);
}

vector<string> DecrypterImpl::crack(const string& ciphertext)
{
    Tokenizer t(" ,;:.!()[]{}-\"#$%^&1234567890");
    m_words = t.tokenize(ciphertext);
    vector<string> res;
    Translator tr;
    crackBacktracking(res, ciphertext, tr);
    return res;  // This compiles, but may not be correct
}

void DecrypterImpl::crackBacktracking(vector<string>& res, const string& originaltext, Translator& mapping)
{
    int maxIdx = -1;
    int maxCnt = 0;
    vector<string> translation;
    for (int i = 0; i < m_words.size(); i++)
    {
        translation.push_back(mapping.getTranslation(m_words[i]));
        int cnt = 0;
        for (int j = 0; j < translation[i].size(); j++)
        {
            if (translation[i][j] == '?')
            {
                cnt++;
            }
        }
        if (cnt == 0)
        {
            if (!m_list.contains(translation[i])) // Abort if the word doesn't exist
            {
                return;
            }
        }
        if (cnt > maxCnt)
        {
            maxCnt = cnt;
            maxIdx = i;
        }
    }
    if (maxIdx == -1) // All words have been correctly translated
    {
        string fullTranslation = "";
        int trIdx = 0;
        for (int i = 0; i < originaltext.size(); i++)
        {
            if(!isalpha(originaltext[i]))
            {
                fullTranslation += originaltext[i];
            }
            else
            {
                fullTranslation += translation[trIdx];
                i += translation[trIdx].size() - 1;
                trIdx++;
            }
        }
        res.push_back(fullTranslation);
        return;
    }
    string choice = m_words[maxIdx];
    vector<string> possibilities = m_list.findCandidates(choice, translation[maxIdx]);
    
    if (possibilities.size() == 0) // Abort if no matching words are found
    {
        return;
    }
    for (int i = 0; i < possibilities.size(); i++)
    {
        string possibility = possibilities[i];
        string ciphertext = "";
        string plaintext = "";
        MyHash<char, char> m_usedChars;
        for (int j = 0; j < possibility.size(); j++)
        {
            char* prevMappingInLoop = m_usedChars.find(choice[j]);
            string prevMappingInMap = mapping.getTranslation(string(1,choice[j]));
            if (prevMappingInLoop != nullptr && *prevMappingInLoop == possibility[j])
            {
                continue;
            }
            if (prevMappingInMap == string(1,possibility[j]))
            {
                continue;
            }
            if (prevMappingInMap != "?" && prevMappingInMap != string(1,possibility[j]))
            {
                return;
            }
            ciphertext += choice[j];
            plaintext += possibility[j];
            m_usedChars.associate(choice[j], possibility[j]);
        }
        mapping.pushMapping(ciphertext, plaintext);
        crackBacktracking(res, originaltext, mapping);
        mapping.popMapping();
    }
}

//******************** Decrypter functions ************************************

// These functions simply delegate to DecrypterImpl's functions.
// You probably don't want to change any of this code.

Decrypter::Decrypter()
{
    m_impl = new DecrypterImpl;
}

Decrypter::~Decrypter()
{
    delete m_impl;
}

bool Decrypter::load(string filename)
{
    return m_impl->load(filename);
}

vector<string> Decrypter::crack(const string& ciphertext)
{
    return m_impl->crack(ciphertext);
}

