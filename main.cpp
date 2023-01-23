#define FILE_EXTENSION ".txt"
#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <queue>

#define EXACT 0
#define PREFIX 1
#define SUFFIX 2

using namespace std;

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

struct triple{
	int mode;
	int row;
	int col;
	triple(int m, int r, int c){
		mode = m;
		row = r;
		col = c;
	};
};

class StreamChecker
{
public:
	struct TrieNode
	{
		bool isEnd;
		string s;
		vector<triple> vt;
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

	void buildTrie(vector<pair<string, triple>> words)
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
			node->vt.push_back(w.second);
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
					while (suf != root && !suf->children[i]){
						suf = suf->suffixLink;
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

	StreamChecker(vector<pair<string, triple>> words)
	{
		root = new TrieNode();
		buildTrie(words);
		buildSuffixLink();
		buildOutputLink();
		state = root;
	}

	/*bool query(char letter)
	{
		while (state != root && !state->children[letter - 'a'])
			state = state->suffixLink;
		if (state->children[letter - 'a'])
			state = state->children[letter - 'a'];
		return state->isEnd || state->outputLink;
	}*/
	pair<bool, TrieNode*> query(char letter)
	{
		while (state != root && !state->children[letter - 'a'])
			state = state->suffixLink;
		if (state->children[letter - 'a'])
			state = state->children[letter - 'a'];
		return make_pair(state->isEnd || state->outputLink, state);
	}
};

int main(int argc, char *argv[])
{

	// INPUT :
	// 1. data directory in data folder
	// 2. number of txt files
	// 3. output route

	string data_dir = argv[1] + string("/");
	string query = string(argv[2]);
	string output = string(argv[3]);

	vector<pair<string, triple>> table;

	// reading query and build trie
	int chart_row = 0, chart_col = 0, max_col = 0;
	vector<vector<int>> op(1000);
	fstream fi;
	string title_name, tmp;
	vector<string> tmp_string;
	fi.open(query, ios::in);
	
	while (getline(fi, tmp))
	{
		tmp_string = split(tmp, " ");
		for(auto seg : tmp_string){
			if(seg == "+") 
				op[chart_row].push_back(1);
			else if(seg == "/") 
				op[chart_row].push_back(2);
			else {
				if(seg[0] == '"'){
					string t = "";
					for (auto alpha : seg)
					{
						if (isalpha(alpha))
							t += tolower(alpha);	
					}
					table.push_back({t, triple(0, chart_row, chart_col)});
				}
				else if(seg[0] == '*'){
					string t = "";
					for (auto alpha : seg)
					{
						if (isalpha(alpha))
							t += tolower(alpha);	
					}
					table.push_back({t, triple(2, chart_row, chart_col)});
				}
				else {
					string t = "";
					for (auto alpha : seg)
					{
						if (isalpha(alpha))
							t += tolower(alpha);	
					}
					table.push_back({t, triple(1, chart_row, chart_col)});
				}
				if(chart_col > max_col){
					max_col = chart_col;
				}
				chart_col++;
			}
		}
		chart_col = 0;
		chart_row++;
	}
	max_col++;

	fi.close();
	StreamChecker sc(table);

	vector<vector<int>> found_query(chart_row, vector<int>(max_col, 0));
	vector<vector<string>> out(chart_row);
	
	int data_idx = 0;

	while(1){
		fi.open(data_dir + to_string(data_idx) + FILE_EXTENSION, ios::in);
		if (!fi)
			break;

		// GET TITLENAME
		getline(fi, title_name);		
		fi.close();
		fi.open(data_dir + to_string(data_idx) + FILE_EXTENSION, ios::in);

		// GET CONTENT LINE BY LINE
		while (getline(fi, tmp)){
			// GET CONTENT WORD VECTOR
			tmp_string = split(tmp, " ");
			// PARSE CONTENT
			vector<string> content = word_parse(tmp_string);

			for(auto &word : content){
				int word_len = word.length();
				string check = "";
				for(int i = 0; i < word_len ; i++){
					check += word[i];
					auto t = sc.query(word[i]);
					if(t.first){ // to the terminal
						int len_vt = t.second->vt.size();
						for(int j = 0; j < len_vt ; j++){
							if(t.second->vt[j].mode == EXACT){
								if(word == t.second->s && t.second->isEnd){
									found_query[t.second->vt[j].row][t.second->vt[j].col] = 1;
								}
							}
							if((t.second->vt[j].mode == PREFIX)){
								if(check == t.second->s){
									found_query[t.second->vt[j].row][t.second->vt[j].col] = 1;
								}
							}
							if(t.second->vt[j].mode == SUFFIX){
								if (check == word)
									found_query[t.second->vt[j].row][t.second->vt[j].col] = 1;
							}
						}
					}
					if(i == word_len - 1){
						if(t.second->suffixLink){
							for(auto a: t.second->suffixLink->vt){
								if(a.mode == SUFFIX){
									found_query[a.row][a.col] = 1;
								}
							}
						}
					}
				}
				sc.state = sc.root;
			}
		}
		fi.close();

		// deal with the operator
		for(int i = 0; i < chart_row; i++){
			int tmp_ans = found_query[i][0];
			int len = op[i].size();
			for(int x = 0, y = 1; x < len; x++){
				if (op[i][x] == 1){ // '+'
					tmp_ans = tmp_ans & found_query[i][y++];
				}
				else if (op[i][x] == 2){ // '/'
					tmp_ans = tmp_ans | found_query[i][y++];
				}
			}
			if(tmp_ans == 1){
				out[i].push_back(title_name);
			}				
		}
		for(int i = 0; i < chart_row; i++){
			for(int j = 0; j < max_col; j++){
				found_query[i][j] = 0;
			}
		}
		data_idx++;	
	}

	ofstream fo;
	fo.open(output);
	for (auto i : out)
	{
		if (i.size() == 0)
			fo << "Not Found!\n";
		for (auto j : i)
			fo << j << "\n";
	}
	fo.close();
}
