#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>
using namespace std;

class TokenizerImpl
{
public:
    TokenizerImpl(string separators);
    vector<string> tokenize(const std::string& s) const;
private:
    unordered_set<char> m_separators;
};

TokenizerImpl::TokenizerImpl(string separators)
{
    for(int i = 0; i < separators.size(); i++)
    {
        m_separators.insert(separators[i]);
    }
}

vector<string> TokenizerImpl::tokenize(const std::string& s) const
{
    vector<string> res;
    string word = "";
    int i = 0;
    while (i < s.size())
    {
        if(m_separators.find(s[i]) != m_separators.end())
        {
            if (word != "")
            {
                res.push_back(word);
                word = "";
            }
        }
        else
        {
            word += s[i];
            if (i == s.size() - 1)
            {
                res.push_back(word);
            }
        }
        i++;
    }
    return res;
}

//******************** Tokenizer functions ************************************

// These functions simply delegate to TokenizerImpl's functions.
// You probably don't want to change any of this code.

Tokenizer::Tokenizer(string separators)
{
    m_impl = new TokenizerImpl(separators);
}

Tokenizer::~Tokenizer()
{
    delete m_impl;
}

vector<string> Tokenizer::tokenize(const std::string& s) const
{
    return m_impl->tokenize(s);
}

