#include "provided.h"
#include "MyHash.h"
#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <cctype>
#include <iostream>
using namespace std;

class WordListImpl
{
public:
    bool loadWordList(string filename);
    bool contains(string word) const;
    vector<string> findCandidates(string cipherWord, string currTranslation) const;
private:
    MyHash<string, vector<string> > m_table; // Map of patterns to matching words
    MyHash<string, bool> m_set; // Set of existing words
    
    string generateLetterPattern(string str) const;
};

bool WordListImpl::loadWordList(string filename)
{
    m_table.reset();
    m_set.reset();
    ifstream infile(filename);
    if (!infile)
    {
        return false;
    }
    string s;
    while(getline(infile, s))
    {
        bool continueFlag = true;
        for (int i = 0; i < s.size(); i++)
        {
            if(!isalpha(s[i]) && s[i] != '\'' ) // Make sure only apostrophes and characters pass through
            {
                continueFlag = false;
            }
        }
        if (!continueFlag) continue;
        string pattern = generateLetterPattern(s);
        vector<string>* ptr = m_table.find(pattern);
        if (ptr == nullptr)
        {
            vector<string> vec(1, s);
            m_table.associate(pattern, vec); // Create new vector with pattern if not already mapped
        }
        else
        {
            if (find(ptr->begin(), ptr->end(), s) == ptr->end()) // Eliminates repeated words
            {
                ptr->push_back(s); // Add to existing pattern if mapping already exists
            }
        }
        for (int i = 0; i < s.size(); i++)
        {
            s[i] = tolower(s[i]); // store lowercased for consistency
        }
        m_set.associate(s, true); // mark as existing
    }
    return true;
}

bool WordListImpl::contains(string word) const
{
    for (int i = 0; i < word.size(); i++)
    {
        word[i] = tolower(word[i]); // lowercase all letters, since we store them lowercased
    }
    return m_set.find(word) == nullptr ? false : true;
}

vector<string> WordListImpl::findCandidates(string cipherWord, string currTranslation) const
{
    vector<string> res;
    for (int i = 0; i < cipherWord.size(); i++)
    {
        if(((currTranslation[i] == '?' || isalpha(currTranslation[i])) && !isalpha(cipherWord[i]))
           || (currTranslation[i] == '\'' && cipherWord[i] != '\''))
        {
            return res;
        }
        
    }
    string pattern = generateLetterPattern(cipherWord);
    const vector<string>* patternMatches = m_table.find(pattern);
    if (patternMatches == nullptr)
    {
        return res;
    }
    for(int i = 0; i < patternMatches->size(); i++)
    {
        string match = (*patternMatches)[i];
        bool matches = true;
        for(int j = 0; j < match.size(); j++)
        {
            if (isalpha(currTranslation[j]) && currTranslation[j] != match[j]) // If the curr translation has a definite letter
            {
                matches = false;
            }
            if (currTranslation[j] == '?' && !isalpha(match[j])) // If the curr translation has a ?, then match must have a letter
            {
                matches = false;
            }
            if(currTranslation[j] == '\'' && match[j] != '\'') // They must both have an apostrophe if one does
            {
                matches = false;
            }
        }
        if(matches)
        {
            res.push_back(match);
        }
    }
    return res;  // This compiles, but may not be correct
}

string WordListImpl::generateLetterPattern(string str) const
{
    string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int alphIdx = 0;
    MyHash<char, char> charset;
    string res = "";
    for(int i = 0; i < str.size(); i++)
    {
        str[i] = tolower(str[i]); // Keep lowercased for consistency
        if (str[i] == '\'') // Keep apostrophes intact
        {
            res += '\'';
            continue;
        }
        char* idx = charset.find(str[i]); // Check if this character was already mapped to a generic pattern one
        if(idx == nullptr)
        {
            charset.associate(str[i], alphabet[alphIdx]); // If not, choose the next alphabetic character to map to it
            res += alphabet[alphIdx];
            alphIdx++;
        }
        else
        {
            res += *idx; // If yes, use the existing one
        }
    }
    return res;
}

//***** hash functions for string, int, and char *****

unsigned int hash(const std::string& s)
{
    return std::hash<std::string>()(s);
}

unsigned int hash(const int& i)
{
    return std::hash<int>()(i);
}

unsigned int hash(const char& c)
{
    return std::hash<char>()(c);
}

//******************** WordList functions ************************************

// These functions simply delegate to WordListImpl's functions.
// You probably don't want to change any of this code.

WordList::WordList()
{
    m_impl = new WordListImpl;
}

WordList::~WordList()
{
    delete m_impl;
}

bool WordList::loadWordList(string filename)
{
    return m_impl->loadWordList(filename);
}

bool WordList::contains(string word) const
{
    return m_impl->contains(word);
}

vector<string> WordList::findCandidates(string cipherWord, string currTranslation) const
{
    return m_impl->findCandidates(cipherWord, currTranslation);
}

