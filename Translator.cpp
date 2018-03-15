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
    vector<string> m_history; // Stores previous ciphertexts
    MyHash<char, vector<char> > m_tableReverse; // Maps from a character to a history of its translations
    vector<string> m_historyReverse; // Stores previous ciphertexts
    
};

TranslatorImpl::TranslatorImpl()
{
    string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // All uppercase
    for (int i = 0; i < alphabet.size(); i++)
    {
        vector<char> history(1, '?'); // default starting history
        m_table.associate(alphabet[i], history);
        m_tableReverse.associate(alphabet[i], history);
    }
}

bool TranslatorImpl::pushMapping(string ciphertext, string plaintext)
{
    if (ciphertext.size() != plaintext.size()) return false;
    for (int i = 0; i < ciphertext.size(); i++)
    {
        vector<char>* history = m_table.find(toupper(ciphertext[i]));
        if (history == nullptr) continue;
        char plain = (*history)[history->size() - 1];
        if (plain != '?' && plain != plaintext[i])
        {
            return false;
        }
        vector<char>* historyReverse = m_tableReverse.find(toupper(plaintext[i]));
        if(historyReverse == nullptr) continue;
        char cipher = (*historyReverse)[historyReverse->size() - 1];
        if (cipher != '?' && cipher != ciphertext[i])
        {
            return false;
        }
    }
    for (int i = 0; i < ciphertext.size(); i++)
    {
        vector<char>* history = m_table.find(toupper(ciphertext[i])); // Every character in our table will be uppercased
        history->push_back(toupper(plaintext[i])); // Add a new translation to the end
        vector<char>* historyReverse = m_tableReverse.find(toupper(plaintext[i])); // Every character in our table will be uppercased
        historyReverse->push_back(toupper(ciphertext[i])); // Add a new translation to the end
    }
    m_history.push_back(ciphertext); // We will uppercase this later, when we need it
    m_historyReverse.push_back(plaintext);
    return true;
}

bool TranslatorImpl::popMapping()
{
    if (m_history.size() <= 0)
    {
        return false;
    }
    string mostRecent = m_history[m_history.size() - 1]; // Our most recent translation
    for (int i = 0; i < mostRecent.size(); i++)
    {
        vector<char>* history = m_table.find(toupper(mostRecent[i]));
        history->pop_back(); // Undo our most recent translation by rolling each array back by one
    }
    string mostRecentReverse = m_historyReverse[m_historyReverse.size() - 1];
    for (int i = 0; i < mostRecent.size(); i++)
    {
        vector<char>* historyReverse = m_tableReverse.find(toupper(mostRecentReverse[i]));
        historyReverse->pop_back(); // Undo our most recent translation by rolling each array back by one
    }
    m_history.pop_back();
    m_historyReverse.pop_back();
    return true;  // This compiles, but may not be correct
}

string TranslatorImpl::getTranslation(const string& ciphertext) const
{
    string res = "";
    for (int i = 0; i < ciphertext.size(); i++)
    {
        const vector<char>* p = m_table.find(toupper(ciphertext[i])); // Find corresponding translation
        if (isalpha(ciphertext[i]))
        {
            if (p == nullptr)
            {
                res += '?'; // No existing translations
            }
            else
            {
                if(islower(ciphertext[i])) // Case characters appropriately, given that they're stored in uppercase
                {
                    res += tolower((*p)[p->size() - 1]); // Last element is the most recent translation
                }
                else
                {
                    res += (*p)[p->size() - 1];
                }
            }
        }
        else
        {
            res += ciphertext[i]; // Add all non-alphabetic characters
        }
    }
    return res;
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

