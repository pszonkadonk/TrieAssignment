//
//  main.cpp
//  TrieAssignment
//
//  Created by Michael Pszonka on 12/8/15.
//  Copyright (c) 2015 Michael Pszonka. All rights reserved.
//


#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <sstream>
#include <algorithm>
#include <functional>
#include <ctype.h>
#include <stdio.h>

using namespace std;

string replaceChar(string str, char ch1, char ch2) {
    for (int i = 0; i < str.length(); ++i) {
        if (str[i] == ch1)
            str[i] = ch2;
    }
    
    return str;
}

// trim from end
// trime white space per this thread on stack overflow
//http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}



struct node {
    
    char character;
    bool marker = false;
    vector<node*> childNodes;
    
    node();
    node *findChild(char c);
    void appendChild(node *n);
};

node::node() {
    character = ' ';
    marker = false;
}

node* node::findChild(char c) {
    
    for (int i = 0; i < childNodes.size(); i++)
    {
        node* tmp = childNodes.at(i);
        if (tolower(tmp->character) == tolower(c) || toupper(tmp->character) == toupper(c))
        {
            return tmp;
        }
    }
    return NULL;
    
}

void node::appendChild(node *n) {
    
    childNodes.push_back(n);
    
}

class Trie {
    
public:
    int articleWordCount;
    string articleContent;
    node *root;
    ifstream companyFile;
    map<string, vector<string>> companyMap;
    vector<string> ignoredWords = { "a", "an", "the", "and", "or", "but" };
    
    Trie();
    void readArticle();
    void printArticle();
    void createCompanyMap();
    void identifyCompanies();
    void addWord(string s);
    bool searchWord(string s);

private:
    float getRelevance();

    
private:
    
};


Trie::Trie() {
    
    root = new node();
    articleWordCount = 0;
    
}

void Trie::readArticle() {
    
    string line;
    string buildUp;
    
    cout << "Please enter the content of the article.  Enter a \".\" on an empty line to terminate reading article : " << endl;
    cin >> line;
    //cout << "LINE: " << line << endl;
    while (line != ".") {
        //cout << "LINE: " << line << endl;
        
        addWord(line);
        if (find(ignoredWords.begin(), ignoredWords.end(), line) == ignoredWords.end()) {
            articleWordCount += 1;
        }
        articleContent = articleContent + line + " ";
        //cout << "Article Content: " << articleContent << endl;
        //cout << "Enter next line of article: " << endl;
        cin >> line;
    }
    
    rtrim(articleContent);
}

void Trie::printArticle() {
    
    cout << articleContent << endl;
}

void Trie::addWord(string s) {
    
    node *current = root;
    int wordCount = 0;
    
    if (s.length() == 0) {
        
        current->marker = true;
        return;
    }
    for (int i = 0; i < s.length(); i++) {
        
        node *child = current->findChild(s[i]);
        if (child != NULL)
        {
            current = child;
        }
        else {
            node *temp = new node();
            temp->character = s[i];
            current->appendChild(temp);
            current = temp;
            wordCount += 1;
        }
        if (i == s.length() - 1)
            current->marker = true;
    }
    
}

bool Trie::Trie::searchWord(string s)
{
    node* current = root;
    
    while (current != NULL)
    {
        for (int i = 0; i < s.length(); i++)
        {
            node* tmp = current->findChild(s[i]);
            if (tmp == NULL)
                return false;
            current = tmp;
        }
        
        if (current->marker == true)
            return true;
        else
            return false;
    }
    
    return false;
}

// private
// loosely based on this stack overflow thread
// http://stackoverflow.com/questions/22315738/find-occurrences-of-all-substrings-in-a-given-string
float Trie::getRelevance() {
    
    float count = 0;
    float frequency = 0.0;
    size_t nPos = 0;

    
    for(auto itr = companyMap.begin(); itr != companyMap.end(); ++itr) {
    
        for(auto synonyms : itr->second)
        {
            nPos = articleContent.find(synonyms, 0);
            // check for all ocurrences of synonyms
            while(nPos != string::npos)
            {
//                cout << "found synonym: " << synonyms << endl;
                count++;
                nPos = articleContent.find(synonyms, nPos+1);
            }
        }
        // check for all occurences of primary
        nPos = articleContent.find(itr->first, 0);
        while(nPos != string::npos)
        {
//            cout << "Found primary: " << itr->first <<  endl;
            count++;
            nPos = articleContent.find(itr->first, nPos+1);
        }
        frequency = (count/articleWordCount) * 100;
//        cout << itr->first <<  " WORD COUNT : " << endl;
//        cout << count << endl;
        cout << itr->first <<  " FREQUENCY COUNT : " << endl;
        printf("%0.3f %%", frequency);
        cout << "" << endl;
        count = 0;
    }
    
    return count;
}




//public
//int Trie::getRelevance() {
//    
//    int rel = getRelevance(articleContent, companyMap);
//    return rel;
//    
//}
void Trie::identifyCompanies() {
    
//    int times = 0;
    cout << "List of Companies in Article" << endl;
    cout << "------------------------- " << endl;
    for (auto itr = companyMap.begin(); itr != companyMap.end(); ++itr) {
        
        if (searchWord(itr->first)) {
            
            cout << itr->first << endl;
//            times+=1;
//            cout << "TIMES: " << times << endl;
        }
        else {
            for (auto synonym : itr->second) {
                if (searchWord(synonym)) {
                    cout << itr->first << endl;
//                    times +=1;
//                    cout << "TIMES: " << times << endl;
                }
            }
        }
    }
    getRelevance();

    
}





// parsing code is based on this stackoverflow thread
// http://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
void Trie::createCompanyMap() {
    
    std::ifstream file("companies.dat");
    
    std::string line;
    
    std::vector<std::string> tokens;
    
    while (std::getline(file, line)) {
        
        istringstream iss(line);
        string token;
        string primary;
        //string synonyms = "";
        vector<string> synonymList;
        int count = 0;
        
        while (std::getline(iss, token, '\t')) {  //delimit by tab
            if (count == 0) {
                primary.assign(token);
                count += 1;
            }
            else {
                synonymList.push_back(token);
                count += 1;
            }
        }
        companyMap.insert(pair<string, vector<string>>(primary, synonymList));
        count = 0;
        primary.assign("");
        synonymList.clear();
        
//        for (auto it = companyMap.begin(); it != companyMap.end(); ++it) {
//            cout << "KEY -> " << it->first;
//            cout << " VALUE -> ";
//            for (auto c : it->second) {
//                cout << " " << c;
//                
//            }
//            cout << "" << endl;
////            cout << it->second.size() << endl;
//
//        }
    }
}

int main() {
    
    
    Trie *t = new Trie();
    
    t->createCompanyMap();
    t->readArticle();
    t->identifyCompanies();
    
    //t->printArticle();
    
    return 0;
}	 
