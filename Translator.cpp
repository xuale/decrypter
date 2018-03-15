#include "provided.h"
#include "MyHash.h"
#include <string>
#include <cctype>
#include <vector>
using namespace std;

class TranslatorImpl
{
public:
    TranslatorImpl();
    bool pushMapping(string ciphertext, string plaintext);
    bool popMapping();
    string getTranslation(const string& ciphertext) const;
private:
    MyHash<char, vector<char> > m_table; // Maps from a character to a history of its translations
    vector<string> m_history;
};

TranslatorImpl::TranslatorImpl()
{
    string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < alphabet.size(); i++)
    {
        vector<char> history(1, '?');
        m_table.associate(alphabet[i], history);
    }
}

bool TranslatorImpl::pushMapping(string ciphertext, string plaintext)
{
    if (ciphertext.size() != plaintext.size()) return false;
    for (int i = 0; i < ciphertext.size(); i++)
    {
        vector<char>* history = m_table.find(toupper(ciphertext[i])); // Every character in our table will be uppercased
        history->push_back(toupper(plaintext[i]));
    }
    m_history.push_back(ciphertext); // We will uppercase this later, when we need it
    return true;  // This compiles, but may not be correct
}

bool TranslatorImpl::popMapping()
{
    if (m_history.size() <= 0)
    {
        return false;
    }
    string mostRecent = m_history[m_history.size() - 1];
    for (int i = 0; i < mostRecent.size(); i++)
    {
        vector<char>* history = m_table.find(toupper(mostRecent[i]));
        history->pop_back();
    }
    m_history.pop_back();
    return true;  // This compiles, but may not be correct
}

string TranslatorImpl::getTranslation(const string& ciphertext) const
{
    string res = "";
    for (int i = 0; i < ciphertext.size(); i++)
    {
        const vector<char>* p = m_table.find(toupper(ciphertext[i]));
        if (isalpha(ciphertext[i]))
        {
            if (p == nullptr)
            {
                res += '?';
            }
            else
            {
                if(islower(ciphertext[i]))
                {
                    res += tolower((*p)[p->size() - 1]);
                }
                else
                {
                    res += (*p)[p->size() - 1];
                }
            }
        }
        else
        {
            res += ciphertext[i];
        }
    }
//    cout << ciphertext << " " << res << endl;
    return res; // This compiles, but may not be correct
}

//******************** Translator functions ************************************

// These functions simply delegate to TranslatorImpl's functions.
// You probably don't want to change any of this code.

Translator::Translator()
{
    m_impl = new TranslatorImpl;
}

Translator::~Translator()
{
    delete m_impl;
}

bool Translator::pushMapping(string ciphertext, string plaintext)
{
    return m_impl->pushMapping(ciphertext, plaintext);
}

bool Translator::popMapping()
{
    return m_impl->popMapping();
}

string Translator::getTranslation(const string& ciphertext) const
{
    return m_impl->getTranslation(ciphertext);
}

