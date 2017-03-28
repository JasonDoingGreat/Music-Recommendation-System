//
//  main.cpp
//  musicRecommendation3
//
//  Created by Zezhou Li on 11/9/15.
//  Copyright © 2015 Zezhou Li. All rights reserved.
//

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <numeric>
#include <cmath>

using namespace std;

#define file1 "/Users/zezhouli/Documents/Big Data/project/YahooMusic/testTrack_hierarchy.txt"
#define file2 "/Users/zezhouli/Documents/Big Data/project/data_in_matrixForm/trainIdx2_matrix.txt"

fstream test_user, train_user;
ofstream out("/Users/zezhouli/Desktop/result22.txt");
//ofstream out1("/Users/zezhouli/Desktop/result14.txt");
vector <string> result[6],result2;
string line,line2, former_line;
string delim = "|";
string user_id;

int flag = 1, score[6][20], firsttime=0;
bool flag2 = false,flag3 = false;
float weight = 0;
int GenreNum = 0;
int GenreScore = 0;
double P[6][5];
double Q[5][20];
double err = 0;
double dotproduct = 0;
double newscore[6][20];
//float aver = 0;

struct scoreset {
    int id;
    double score;
    int recresult;
};

vector<string> split(string str,string pattern) {
    
    string::size_type pos;
    vector<string> result;
    str += pattern;//扩展字符串以方便操作
    long size = str.size();
    
    for(long i=0; i<size; i++)
    {
        pos=str.find(pattern,i);
        if(pos<size)
        {
            string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}

void clearScore() {
    for (int m = 0; m<6; m++) {
        for (int n = 0; n<20; n++) {
            score[m][n]=-1;
        }
    }
}

void matrix_factorization() {
    ((double) rand() / (RAND_MAX));
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 3; col++) {
            P[row][col] = ((double) rand() / (RAND_MAX));
        }
    }
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 20; col++) {
            Q[row][col] = ((double) rand() / (RAND_MAX));
        }
    }
    for (int step = 0; step<10000; step++) {
        for (int row = 0; row<6; row++) {
            for (int col = 0; col<20; col++) {
                if (score[row][col] != -1) {
                    for (int col2 = 0; col2 < 3; col2++) {
                        dotproduct += P[row][col2]*Q[col2][col];
                    }
                    err = score[row][col] - dotproduct;
                    dotproduct = 0;
                    for (int k = 0; k<3; k++) {
                        P[row][k] = P[row][k] + 0.0002*(2*err*Q[k][col] - 0.02*P[row][k]);
                        Q[k][col] = Q[k][col] + 0.0002*(2*err*P[row][k] - 0.02*Q[k][col]);
                    }
                }
            }
        }
        double e = 0;
        for (int row = 0; row<6; row++) {
            for (int col = 0; col<20; col++) {
                if (score[row][col] != -1) {
                    for (int col2 = 0; col2 < 3; col2++) {
                        dotproduct += P[row][col2]*Q[col2][col];
                    }
                    e += pow((score[row][col] - dotproduct),2);
                    dotproduct = 0;
                    for (int k = 0; k<3; k++) {
                        e += 0.01*(pow(P[row][k],2)+pow(Q[k][col], 2));
                    }
                }
            }
        }
        if (e < 0.0001) {
            break;
        }
    }
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 20; col++) {
            newscore[row][col] = 0;
            for (int k = 0; k < 3; k++) {
                newscore[row][col] += (P[row][k]*Q[k][col]);
            }
        }
    }
}

void readTestTrack() {
    for (int i = 0; i < 6; i++) {
        if(getline(test_user, line)) {
            result[i] = split(line, delim);
        } else {
            flag2 = true;
            break;
        }
    }
    user_id = result[0][0];
}

void readTrainTrack() {
    if (flag3 && firsttime!=0) {
        result2 = split(former_line, delim);
        if (result2[0] == user_id) {
            for (int m = 0; m<6; m++) {
                for (int n = 2; n < result[m].size(); n++) {
                    if (result2[1] == result[m][n]) {
                        score[m][n] = atoi(result2[2].c_str());
                    }
                }
            }
        }
        flag3 = false;
    }
    firsttime=1;
    getline(train_user, line2);
    result2 = split(line2, delim);
    if (result2[0] == user_id) {
        for (int m = 0; m<6; m++) {
            for (int n = 2; n < result[m].size(); n++) {
                if (result2[1] == result[m][n]) {
                    score[m][n] = atoi(result2[2].c_str());
                }
            }
        }
    }
}

void sortscore(scoreset a[], int size) {
    scoreset temp;
    for(int pass=1;pass<size;pass++)
    {
        for(int k=0;k<size-pass;k++) {
            if(a[k].score < a[k+1].score)
            {
                temp=a[k];
                a[k]=a[k+1];
                a[k+1]=temp;
            }
        }
    }
    for (int num = 0; num<3; num++) {
        a[num].recresult = 1;
    }
    for (int num = 3; num<6; num++) {
        a[num].recresult = 0;
    }
    for (int pass=1; pass<size; pass++) {
        for (int k = 0; k<size-pass; k++) {
            if (a[k].id > a[k+1].id) {
                temp = a[k];
                a[k]=a[k+1];
                a[k+1]=temp;
            }
        }
    }
}

void getScore() {
    if (atoi(result2[0].c_str()) > atoi(result[0][0].c_str())) {
        former_line = line2;
        flag3 = true;
        flag = 0;
        scoreset scores[6];
        
        // Calculate the score for each track for current user_id.
        
        matrix_factorization();

        for (int m = 0; m<6; m++) {
            scores[m].score = 0;
            scores[m].id = m+1;
            scores[m].recresult = 0;
            for (int n = 2; n<8; n++) {
                if (n==2) {
                    scores[m].score += 0.6*score[m][n];
                } else if (n==3) {
                    scores[m].score += 0.4*score[m][n];
                } else if (n==4) {
                    scores[m].score += 0.3*score[m][n];
                } else {
                    scores[m].score += 0.05*score[m][n];
                }
            }
            //scores[m].score /= (0.6+0.5+0.4+0.1);
            if (m == 5) {
                sortscore(scores, 6);
                for (int i = 0; i<6; i++) {
                    out << scores[i].recresult << "\n";
                }
            }
        }
        clearScore();
    }
}

void recommendProg() {
    while (1) {
        readTestTrack();
        if (flag2) {
            break;
        }
        while (flag) {
            readTrainTrack();
            getScore();
        }
        flag = 1;
    }
}

int main(int argc, const char * argv[]) {
    
    test_user.open(file1);
    train_user.open(file2);
    
    clearScore();
    
    recommendProg();
    
    test_user.close();
    train_user.close();
    
    return 0;
}

