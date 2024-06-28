#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <unordered_set>
#include <map>
#include <functional>
#include <queue>
#include <climits>
#include <list>
#include <stack>

using namespace std;

using DocumentID = int;
using Term = string;
using DocumentFrq = map<DocumentID, int>;
using Links = vector<DocumentID>;

struct Node {
    string name;
    list<pair<string, int>> edges;
    bool isFile;

    Node(string n = "", bool file = false) : name(n), isFile(file) {}
};

class AdjList {
    list<Node> nodes;

public:
    void insertNode(const string& name, bool isFile) {
        nodes.push_back(Node(name, isFile));
    }

    void insertEdge(const string& name1, const string& name2, int weight) {
        for (auto& node : nodes) {
            if (node.name == name1) {
                node.edges.push_back({ name2, weight });
            }
            else if (node.name == name2) {
                node.edges.push_back({ name1, weight });
            }
        }
    }

    void BFS(const string& start) {
        vector<string> visited;
        queue<string> q;
        q.push(start);

        while (!q.empty()) {
            string current = q.front();
            q.pop();

            if (find(visited.begin(), visited.end(), current) == visited.end()) {
                cout << current << " ";
                visited.push_back(current);
            }

            for (const auto& node : nodes) {
                if (node.name == current) {
                    for (const auto& neighbor : node.edges) {
                        if (find(visited.begin(), visited.end(), neighbor.first) == visited.end()) {
                            q.push(neighbor.first);
                        }
                    }
                }
            }
        }
        cout << endl;
    }

    void DFS(const string& start) {
        vector<string> visited;
        stack<string> s;
        s.push(start);

        while (!s.empty()) {
            string current = s.top();
            s.pop();

            if (find(visited.begin(), visited.end(), current) == visited.end()) {
                cout << current << " ";
                visited.push_back(current);
            }

            for (const auto& node : nodes) {
                if (node.name == current) {
                    for (const auto& neighbor : node.edges) {
                        if (find(visited.begin(), visited.end(), neighbor.first) == visited.end()) {
                            s.push(neighbor.first);
                        }
                    }
                }
            }
        }
        cout << endl;
    }

    void dijkstra(const string& start, const string& end) {
        unordered_map<string, pair<int, string>> dijkstraTable;
        for (const auto& node : nodes) {
            dijkstraTable[node.name] = { INT_MAX, "" };
        }
        dijkstraTable[start].first = 0;

        priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;
        pq.push({ 0, start });

        while (!pq.empty()) {
            string current = pq.top().second;
            int currentDist = pq.top().first;
            pq.pop();

            if (currentDist > dijkstraTable[current].first) {
                continue;
            }

            for (const auto& node : nodes) {
                if (node.name == current) {
                    for (const auto& neighbor : node.edges) {
                        int newDist = dijkstraTable[current].first + neighbor.second;
                        if (newDist < dijkstraTable[neighbor.first].first) {
                            dijkstraTable[neighbor.first] = { newDist, current };
                            pq.push({ newDist, neighbor.first });
                        }
                    }
                }
            }
        }

        printPath(dijkstraTable, start, end);
    }

    void printPath(const unordered_map<string, pair<int, string>>& table, const string& start, const string& end) {
        if (table.at(end).first == INT_MAX) {
            cout << "No path from " << start << " to " << end << endl;
            return;
        }

        string path = "";
        for (string v = end; !v.empty(); v = table.at(v).second) {
            path = v + " -> " + path;
        }
        path.pop_back();
        path.pop_back();
        path.pop_back();
        path.pop_back();
        cout << "Shortest path from " << start << " to " << end << ": " << path << endl;
    }
    void displayAdjList() {
        for (const auto& node : nodes) {
            cout << "Node: " << node.name << "\n";
            cout << "Edges:\n";
            for (const auto& edge : node.edges) {
                cout << "  " << edge.first << " (Weight: " << edge.second << ")\n";
            }
            cout << endl;
        }
    }
};

class InvertedIndex {
    unordered_map<Term, DocumentFrq> index;
    Links linkIDs;
    unordered_map<DocumentID, string> docIDToPath; // Mapping from DocumentID to file path
public:
    // Function to add a document to the inverted index
    void addDocument(DocumentID docID, vector<Term>& terms, const string& filePath) {
        for (auto& term : terms) {
            index[term][docID]++;
            if (term.find("txt") != string::npos) {
                linkIDs.push_back(docID);
            }
        }
        docIDToPath[docID] = filePath; // Store the mapping from docID to file path
    }
    Links& getLinks() {
        return linkIDs;
    }
    DocumentFrq searchAnd(vector<Term>& termsOfQuery) {
        DocumentFrq res;
        if (termsOfQuery.empty()) {
            return res;
        }
        if (index.find(termsOfQuery[0]) != index.end()) {
            res = index.at(termsOfQuery[0]);
        }
        else {
            return DocumentFrq();
        }
        for (int i = 1; i < termsOfQuery.size(); ++i) {
            DocumentFrq temp;
            if (index.find(termsOfQuery[i]) != index.end()) {
                DocumentFrq& present = index.at(termsOfQuery[i]);
                for (auto& [docID, freq] : res) {
                    if (present.find(docID) != present.end()) {
                        temp[docID] = freq + present.at(docID);
                    }
                }
            }
            res = temp;
        }
        return res;
    }
    DocumentFrq searchOr(vector<Term>& termsOfQuery) {
        DocumentFrq res;
        for (auto& term : termsOfQuery) {
            if (index.find(term) != index.end()) {
                DocumentFrq& present = index.at(term);
                for (auto& [docID, freq] : present) {
                    res[docID] += freq;
                }
            }
        }
        return res;
    }
    DocumentFrq searchWithHyphens(vector<Term>& termsOfQuery) {
        DocumentFrq res = searchAnd(termsOfQuery);
        for (auto& term : termsOfQuery) {
            int hyphenPos = term.find('-');
            if (hyphenPos != string::npos) {
                string sp1 = term.substr(0, hyphenPos);
                string sp2 = term.substr(hyphenPos + 1);
                vector<Term> merged = { sp1, sp2 };
                DocumentFrq temp = searchAnd(merged);
                for (auto& [docID, freq] : temp) {
                    res[docID] += freq;
                }
            }
        }
        return res;
    }
    DocumentFrq search(string& query) {
        vector<Term> termsOfQuery;
        stringstream stream(query);
        string word;
        bool And = false, Or = false, Hyphen = false;
        while (stream >> word) {
            if (word == "AND" || word == "and") {
                And = true;
            }
            else if (word == "OR" || word == "or") {
                Or = true;
            }
            else {
                if (word.find('-') != string::npos) {
                    Hyphen = true;
                }
                termsOfQuery.push_back(word);
            }
        }
        if (And) {
            return searchAnd(termsOfQuery);
        }
        else if (Or) {
            return searchOr(termsOfQuery);
        }
        else if (Hyphen) {
            return searchWithHyphens(termsOfQuery);
        }
        else {
            return searchAnd(termsOfQuery);
        }
    }
    unordered_map<Term, int> getQueryTermFrequency(vector<Term>& termsOfQuery, DocumentID docID) {
        unordered_map<Term, int> termFreq;
        for (auto& term : termsOfQuery) {
            auto it = index.find(term);
            if (it != index.end()) {
                auto docIt = it->second.find(docID);
                if (docIt != it->second.end()) {
                    termFreq[term] = docIt->second;
                }
                else {
                    termFreq[term] = 0;
                }
            }
            else {
                termFreq[term] = 0;
            }
        }
        return termFreq;
    }

    void printDocumentIDs() {
        for (const auto& [docID, filePath] : docIDToPath) {
            cout << "Document ID: " << docID << " -> " << filePath << endl;
        }
    }
};

vector<Term> tokenizeAndFilter(string& text, unordered_set<Term>& stopWords) {
    vector<Term> terms;
    stringstream stream(text);
    string word;
    while (stream >> word) {
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
        if (stopWords.find(word) == stopWords.end()) {
            terms.push_back(word);
        }
    }
    return terms;
}

void replaceBackslashes(string& path) {
    string modifiedPath;
    for (char c : path) {
        if (c == '\\') {
            modifiedPath += "\\\\";
        }
        else {
            modifiedPath += c;
        }
    }
    path = modifiedPath;
}

bool readFromHistory(string& query, string& historyPath) {
    ifstream historyFile(historyPath);
    if (!historyFile.is_open()) {
        return false;
    }
    string line;
    bool queryFound = false;
    while (getline(historyFile, line)) {
        if (line.find("Search: " + query) != string::npos) {
            queryFound = true;
        }
        if (queryFound) {
            cout << line << endl;
            if (line.find("Search: ") != string::npos && line.find(query) == string::npos) {
                break;
            }
        }
    }
    historyFile.close();
    return queryFound;
}

void writeToHistory(string query, DocumentFrq result, vector<pair<DocumentID, int>> sortedResults, InvertedIndex index, vector<Term> termsOfQuery, string historyPath) {
    ofstream historyFile(historyPath, ios::app);
    if (!historyFile.is_open()) {
        cout << "Error opening history file for writing." << endl;
        return;
    }
    historyFile << "Search: " << query << endl;
    historyFile << "Top 3 Search Results:" << endl;
    for (int i = 0; i < min<int>(3, sortedResults.size()); ++i) {
        historyFile << "Document ID: " << sortedResults[i].first << ", Frequency: " << sortedResults[i].second << endl;

        // Display the frequency of each query term in the document
        unordered_map<Term, int> termFreq = index.getQueryTermFrequency(termsOfQuery, sortedResults[i].first);
        historyFile << "Query Term Frequencies in Document " << sortedResults[i].first << ":" << endl;
        for (auto& [term, freq] : termFreq) {
            historyFile << term << ": " << freq << endl;
        }
    }
    historyFile.close();
}
bool naturalSort(const filesystem::path& a, const filesystem::path& b) {
    const string& nameA = a.filename().string();
    const string& nameB = b.filename().string();

    auto itA = nameA.begin();
    auto itB = nameB.begin();

    while (itA != nameA.end() && itB != nameB.end()) {
        if (isdigit(*itA) && isdigit(*itB)) {
            string numStrA, numStrB;
            while (itA != nameA.end() && isdigit(*itA)) {
                numStrA += *itA;
                ++itA;
            }
            while (itB != nameB.end() && isdigit(*itB)) {
                numStrB += *itB;
                ++itB;
            }

            int numA = stoi(numStrA);
            int numB = stoi(numStrB);
            if (numA != numB) {
                return numA < numB;
            }
        }
        else {
            if (*itA != *itB) {
                return *itA < *itB;
            }
            ++itA;
            ++itB;
        }
    }

    return nameA.size() < nameB.size();
}
void processFiles(const string& path, InvertedIndex& index, unordered_set<string>& stopWords, int& docID, AdjList& graph, const string& parentNode) {
    vector<filesystem::path> filePaths;

    // Collect all file paths
    for (const auto& entry : filesystem::recursive_directory_iterator(path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            filePaths.push_back(entry.path());
        }
    }

    // Sort the file paths
    sort(filePaths.begin(), filePaths.end(), naturalSort);

    // Process sorted file paths
    for (const auto& filePath : filePaths) {
        ifstream file(filePath);
        if (file.is_open()) {
            stringstream read;
            read << file.rdbuf();
            string content = read.str();
            vector<string> terms = tokenizeAndFilter(content, stopWords);
            string filePathStr = filePath.string();
            index.addDocument(docID++, terms, filePathStr);
            graph.insertNode(filePathStr, true);
            graph.insertEdge(parentNode, filePathStr, 1);

            // Handle hyperlinks
            stringstream ss(content);
            string word;
            while (ss >> word) {
                if (word.find("txt") != string::npos) {
                    string linkedFile = path + "//" + word;
                    if (filesystem::exists(linkedFile) && filesystem::is_regular_file(linkedFile)) {
                        graph.insertEdge(filePathStr, linkedFile, 1);
                    }
                }
            }
        }
    }

    // Process directories
    for (const auto& entry : filesystem::recursive_directory_iterator(path)) {
        if (entry.is_directory()) {
            string dirName = entry.path().string();
            graph.insertNode(dirName, false);
            graph.insertEdge(parentNode, dirName, 1);
            processFiles(dirName, index, stopWords, docID, graph, dirName);
        }
    }
}

int main() {
    InvertedIndex index;
    unordered_set<string> stopWords = { "is", "in", "am", "the", "this", "a", "an", "and", "or", "but", "for", "to", "our", "its", "it's", "these", "their", "them", "those", "he", "she", "it", "they", "have", "has", "had" };
    string path, query;

    cout << "Enter the path to the directory containing text documents: ";
    getline(cin, path);
    replaceBackslashes(path);

    cout << "Enter the search query (use 'AND'/'OR' for boolean search): ";
    getline(cin, query);

    string historyPath = "D:\\Classes\\4th Semester\\DS\\History.txt";
    if (readFromHistory(query, historyPath)) {
        return 0;
    }

    vector<Term> termsOfQuery = tokenizeAndFilter(query, stopWords);

    int docID = 0;
    AdjList graph;
    graph.insertNode(path, false); // Root node for the directory

    processFiles(path, index, stopWords, docID, graph, path);

    DocumentFrq results = index.search(query);
    vector<pair<DocumentID, int>> sortedResults(results.begin(), results.end());
    sort(sortedResults.begin(), sortedResults.end(), [](const pair<DocumentID, int>& a, const pair<DocumentID, int>& b) {
        return b.second < a.second;
        });

    cout << "Top 3 Search Results:" << endl;
    for (size_t i = 0; i < min<size_t>(3, sortedResults.size()); ++i) {
        cout << "Document ID: " << sortedResults[i].first << ", Frequency: " << sortedResults[i].second << endl;
        unordered_map<Term, int> termFreq = index.getQueryTermFrequency(termsOfQuery, sortedResults[i].first);
        cout << "Query Term Frequencies in Document " << sortedResults[i].first << ":" << endl;
        for (const auto& [term, freq] : termFreq) {
            cout << term << ": " << freq << endl;
        }
    }

    graph.displayAdjList();

    cout << "Documents containing hyperlink:" << endl;
    for (const auto& docID : index.getLinks()) {
        cout << docID << endl;
    }


    writeToHistory(query, results, sortedResults, index, termsOfQuery, historyPath);

    return 0;
}
