#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;
namespace fs = filesystem;

const vector<string> Smost_frequent_words = {"a", "and", "an", "of", "in", "the"};
const int total_files = 64;
const int most_frequent_words = 100; 
   vector<string> fileNames;
struct BookData {
    unordered_map<string, double> wordFrequency;
    double totalWords = 0.0;
};


string normalizeWord(const string& word) {
    string normalized;
    for (char c : word) {
        if (isalnum(c)) { 
            normalized += toupper(c); 
        }
    }
    return normalized;
}

BookData readBook(const string& filePath) {
    BookData data;
    ifstream file(filePath);
    string word;
    
    if (file) {
        while (file >> word) {
            string normalizedWord = normalizeWord(word);
            if (!normalizedWord.empty() &&
                find(Smost_frequent_words.begin(), Smost_frequent_words.end(), normalizedWord) == Smost_frequent_words.end()) {
                data.wordFrequency[normalizedWord]++;
                data.totalWords++;
            }
        }
    } else {
        cerr << "Cannot open file: " << filePath << endl;
    }
    
    for (auto& pair : data.wordFrequency) {
        pair.second /= data.totalWords; 
    }
    
    return data;
}

double calculateSimilarity(const BookData& book1, const BookData& book2) {
    double similarityIndex = 0.0;
    for (const auto& pair : book1.wordFrequency) {
        const string& word = pair.first;
        if (book2.wordFrequency.find(word) != book2.wordFrequency.end()) {
            similarityIndex += pair.second + book2.wordFrequency.at(word);
        }
    }
    return similarityIndex;
}



void calcSM(const vector<BookData>& books, vector<vector<double>>& similarityMatrix) {
    for (size_t i = 0; i < books.size(); ++i) {
        for (size_t j = i + 1; j < books.size(); ++j) {
            similarityMatrix[i][j] = calculateSimilarity(books[i], books[j]);
            similarityMatrix[j][i] = similarityMatrix[i][j]; 
        }
    }
}



void Top10(const vector<vector<double>>& similarityMatrix) {

    vector<pair<double, pair<int, int>>> similarities;
    for (int i = 0; i < similarityMatrix.size(); ++i) {
        for (int j = i + 1; j < similarityMatrix.size(); ++j) {
            similarities.emplace_back(similarityMatrix[i][j], make_pair(i, j));
        }
    }
    
    sort(similarities.begin(), similarities.end(), greater<pair<double, pair<int, int>>>());
    cout << "10 Most Similar Books:\n";
    for (int i = 0; i < 10 && i < similarities.size(); ++i) {
        cout << i + 1 << ":" << "Books " << fileNames[similarities[i].second.first] << " and " 
             << fileNames[similarities[i].second.second] << " with similarity index: " 
             << fixed << setprecision(4) << similarities[i].first << "\n";
    }
}

int main() {
    string folderPath;
    cout<<"Enter the path where folder is present:";
    cin>>folderPath;
    vector<BookData> books;
    vector<vector<double>> similarityMatrix(total_files, vector<double>(total_files, 0.0));
 

    if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        cerr << "The specified folder does not exist or is not a directory." << endl;
        return 1;
    }
    
   
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            BookData bookData = readBook(entry.path().string());
            books.push_back(bookData);
            fileNames.push_back(entry.path().stem().string());
        }
    }
    
 
    calcSM(books, similarityMatrix);
    
    
    Top10(similarityMatrix);
    
    return 0;
}  