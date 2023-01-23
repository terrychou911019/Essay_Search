#define FILE_EXTENSION ".txt"
#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <queue>

using namespace std;

struct three{
	int a;
	int b;
	int c;
	three(int x, int y, int z){
		a = x;
		b = y;
		c = z;
	};
};

class StreamChecker
{
public:
	struct TrieNode
	{
		bool isEnd;
		string s;
		vector<three> q;
		TrieNode *children[26];
		TrieNode *suffixLink;
		TrieNode *outputLink;
		TrieNode()
		{
			isEnd = false;
			memset(children, 0, sizeof(TrieNode *) * 26);
			suffixLink = outputLink = NULL;
		}
	};

	TrieNode *root;
	TrieNode *state;

	void buildTrie(vector<pair<string, three>> words)
	{	
		for (auto &w : words)
		{
			TrieNode *node = root;
			int len = w.first.length();
			for (int i = 0; i < len; i++)
			{
				if (!node->children[w.first[i] - 'a']){
					node->children[w.first[i] - 'a'] = new TrieNode();
				}
				node = node->children[w.first[i] - 'a'];
				//if(i == len - 1) cout << node->depth << endl;
			}
			node->q.push_back(w.second);
			node->s = w.first;		
			node->isEnd = true;
		}
	}

	void buildSuffixLink()
	{
		queue<TrieNode *> bfsQ;
		root->suffixLink = root;
		bfsQ.push(root);
		while (!bfsQ.empty())
		{
			TrieNode *curr = bfsQ.front();
			bfsQ.pop();
			for (int i = 0; i < 26; i++)
			{
				if (curr->children[i])
				{
					TrieNode *suf = curr->suffixLink;
					int cnt = 0;
					while (suf != root && !suf->children[i]){
						suf = suf->suffixLink;
						cnt++;
					}
					if (suf->children[i] && suf != curr){
						curr->children[i]->suffixLink = suf->children[i];
					}							
					else
						curr->children[i]->suffixLink = root;
					bfsQ.push(curr->children[i]);
				}
			}
		}
	}

	void buildOutputLink()
	{
		queue<TrieNode *> bfsQ;
		root->suffixLink = root;
		bfsQ.push(root);
		while (!bfsQ.empty())
		{
			TrieNode *curr = bfsQ.front();
			bfsQ.pop();
			TrieNode *suf = curr->suffixLink;
			while (suf != root && !suf->isEnd)
				suf = suf->suffixLink;
			if (suf->isEnd && suf != curr)
				curr->outputLink = suf;
			for (int i = 0; i < 26; i++)
			{
				if (curr->children[i])
					bfsQ.push(curr->children[i]);
			}
		}
	}

	// public:
	StreamChecker(vector<pair<string, three>> words)
	{
		root = new TrieNode();
		buildTrie(words);
		buildSuffixLink();
		buildOutputLink();
		state = root;
	}

	bool query(char letter)
	{
		while (state != root && !state->children[letter - 'a'])
			state = state->suffixLink;
		if (state->children[letter - 'a'])
			state = state->children[letter - 'a'];
		return state->isEnd || state->outputLink;
	}
};

// Utility Func

// string parser : output vector of strings (words) after parsing
vector<string> word_parse(vector<string> tmp_string)
{
	vector<string> parse_string;
	for (auto &word : tmp_string)
	{
		string new_str;
		for (auto &ch : word)
		{
			if (isalpha(ch))
				new_str.push_back(tolower(ch));
		}
		parse_string.emplace_back(new_str);
	}
	return parse_string;
}

vector<string> split(const string &str, const string &delim)
{
	vector<string> res;
	if ("" == str)
		return res;
	// 先將要切割的字串從string型別轉換為char*型別
	char *strs = new char[str.length() + 1]; // 不要忘了
	strcpy(strs, str.c_str());

	char *d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p)
	{
		string s = p;	  // 分割得到的字串轉換為string型別
		res.push_back(s); // 存入結果陣列
		p = strtok(NULL, d);
	}

	return res;
}

int main(int argc, char *argv[])
{

	// INPUT :
	// 1. data directory in data folder
	// 2. number of txt files
	// 3. output route

	string data_dir = argv[1] + string("/");
	string query = string(argv[2]);
	string output = string(argv[3]);

	vector<pair<string, three>> save;

	// reading query and build trie
	string temp;
	three t(-1, 0, 0);
	int max_col = 0;
	vector<vector<int>> op(1000);
	fstream fq;
	vector<string> temp_string;
	fq.open(query, ios::in);
	while (getline(fq, temp))
	{
		temp_string = split(temp, " ");
		int len = temp_string.size();
		for(int i = 0; i < len ; i++){
			string word =  temp_string[i];
			if(word == "/" || word == "+"){
				if(temp_string[i] == "+") {
					op[t.b].push_back(1);
				}
				else if(temp_string[i] == "/") {
					op[t.b].push_back(2);
				}
			}
			else {
				if(word[0] == '"'){
					word = word.substr(1, word.length() - 2);
					save.push_back({word, three(0, t.b, t.c)});
				}
				else if(word[0] == '*'){
					word = word.substr(1, word.length() - 2);
					save.push_back({word, three(2, t.b, t.c)});
				}
				else {
					save.push_back({word, three(1, t.b, t.c)});
				}
				if(t.c > max_col){
					max_col = t.c;
				}
				t.c++;
			}
		}
		t.c = 0;
		t.b++;
	}
	max_col++;

	fq.close();
	StreamChecker sc(save);

	vector<vector<int>> ans(t.b, vector<int>(max_col, 0));
	vector<vector<pair<string, int>>> title_save(t.b);
	string file, title_name, tmp;
	fstream fi;
	vector<string> tmp_string;
	int num = 0;
	string s = data_dir + "/0.txt";
	while(1){
		fi.open(s, ios::in);
		if (!fi)
			break;
		else
		{
			vector<string> words; // for saveing word
			// read each file
			getline(fi, title_name);
			// GET TITLENAME WORD ARRAY
			tmp_string = split(title_name, " ");
			vector<string> title = word_parse(tmp_string);
			for(auto &word : title){
				int len1 = word.length();
				for(int i = 0; i < len1 ; i++){
					if(sc.query(word[i]) == 1){
						int len2 = sc.state->q.size();
						for(int j = 0; j < len2 ; j++){
							if(sc.state->q[j].a == 0){
								if(word == sc.state->s && sc.state->isEnd){
									ans[sc.state->q[j].b][sc.state->q[j].c] = 1;
								}
							}
							if((sc.state->q[j].a == 1)){
								int len3 = sc.state->s.length();
								if(word.substr(0, len3) == sc.state->s){
									ans[sc.state->q[j].b][sc.state->q[j].c] = 1;
								}
							}
							if(sc.state->q[j].a == 2){
								int len3 = sc.state->s.length();
								string s = word.substr(len1 - len3, len3);
								if(s == sc.state->s)ans[sc.state->q[j].b][sc.state->q[j].c] = 1;
							}
						}
					}
					if(i == len1 - 1){
						if(sc.state->suffixLink){
							for(auto a: sc.state->suffixLink->q){
								if(a.a == 2){
									ans[a.b][a.c] = 1;
								}
							}
						}
					}
				}	
			sc.state = sc.root;
			}
			// GET CONTENT LINE BY LINE
			while (getline(fi, tmp)){
				// GET CONTENT WORD VECTOR
				tmp_string = split(tmp, " ");

				// PARSE CONTENT
				vector<string> content = word_parse(tmp_string);
				int content_flag = 0;
				for(auto &word : content){
					int len1 = word.length();
					for(int i = 0; i < len1 ; i++){
						if(sc.query(word[i]) == 1){
							int len2 = sc.state->q.size();
							for(int j = 0; j < len2 ; j++){
								if(sc.state->q[j].a == 0){
									if(word == sc.state->s && sc.state->isEnd){
										ans[sc.state->q[j].b][sc.state->q[j].c] = 1;
									}
								}
								if((sc.state->q[j].a == 1)){
									int len3 = sc.state->s.length();
									if(word.substr(0, len3) == sc.state->s){
										ans[sc.state->q[j].b][sc.state->q[j].c] = 1;
									}
								}
								if(sc.state->q[j].a == 2){
									int len3 = sc.state->s.length();
									string s = word.substr(len1 - len3, len3);
									if(s == sc.state->s)ans[sc.state->q[j].b][sc.state->q[j].c] = 1;
								}
							}
						}
						if(i == len1 - 1){
							if(sc.state->suffixLink){
								for(auto a: sc.state->suffixLink->q){
									if(a.a == 2){
										ans[a.b][a.c] = 1;
									}
								}
							}
						}
					}
					sc.state = sc.root;
				}
			}
			// CLOSE FILE
			fi.close();
			for(int i = 0; i < t.b; i++){
				int answer = ans[i][0];
				int len = op[i].size();
				for(int x = 0, y = 1; x < len; x++){
					if(op[i][x] == 1){
						answer = answer & ans[i][y++];
					}
					else if(op[i][x] == 2){
						answer = answer | ans[i][y++];
					}
				}
				if(answer == 1){
					title_save[i].push_back({title_name, num});
				}				
			}
			for(int i = 0; i < t.b; i++){
				for(int j = 0; j < max_col; j++){
					ans[i][j] = 0;
				}
			}
			// update varible
			num++;
			s = data_dir + "/" + to_string(num) + ".txt";
		}
	}
	//cout << num << endl;
	//cout << s << endl;
	ofstream fout;
	fout.open(output);
	for(int i = 0; i < t.b; i++){
		int len = title_save[i].size();
		//fout << "=======================" << endl;
		if(len == 0) fout << "Not Found!\n";
		//else fout << i + 1<< ' ' << len << endl;
		for(int j = 0; j < len; j++){
			fout << title_save[i][j].first << endl;
		}
	}
}
