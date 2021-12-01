#include <iostream>
#include <dirent.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <unordered_set>

using namespace std;

vector<string> readDir()
{
    DIR *direct;
    struct dirent *dRead;
    vector<string> dirFiles;

    if((direct = opendir("C:\\Users\\jaron\\Desktop\\School\\USF\\Fall2021\\Senior Project\\Logs")) != nullptr)
    {
        while((dRead = readdir(direct)) != nullptr)
        {
            dirFiles.push_back(dRead->d_name);
        }
        closedir(direct);
    }
    else
    {
        cout << "Directory is empty." << endl;
    }
    return dirFiles;
}

 int timeInSeconds(stringstream &timeStamp)
 {
    string hours, minutes, seconds;
    getline(timeStamp, hours, ':');
    getline(timeStamp, minutes, ':');
    getline(timeStamp, seconds, ':');
    return ((stoi(hours) * 60) * 60 + (stoi(minutes) * 60) + stoi(seconds));
 }

 string parseTime(stringstream &getTime)
 {
    string date, strTime;
    getTime >> date;
    getTime >> strTime;
    return strTime;
 }

vector<string> parseLine(vector<string> &tokens, string line)
{
    stringstream ss(line);
    string command, url, token;
    vector<string> lineInfo;
    char delimeter = ':';
    bool isHTTP;
    while(getline(ss, token, '\t'))
    {
        tokens.push_back(token);
    }
    stringstream sep(tokens.at(4));
    sep >> command;
    if(command == "GET")
    {
        isHTTP = true;
    }
    else
    {
        isHTTP = false;
    }
    sep >> url;
    if(isHTTP == true)
    {
        string tempUrl = url.substr(5, url.length() - 5);
        url = "http:" + tempUrl.substr(0, tempUrl.find(delimeter));

    }else
    {
        url = url.substr(0, url.find(delimeter));
    }
    lineInfo.push_back(command);
    lineInfo.push_back(url);
    return lineInfo;
}

void checkCorrect(int &correct, int &incorrect, string url, vector<string>correctUrls)
{
    bool inc = false;
    for(int i = 0; i < correctUrls.size(); i++)
    {
        if(correctUrls.at(i) == url)
        {
            correct++;
            break;
        }
        else
        {
            inc = true;
        }
    }
    if(inc == true)
    {
        incorrect++;
    }
}

bool compareTime(string t1, string t2)
{
    stringstream ss1(t1);
    stringstream ss2(t2);
    string token, dateOne, dateTwo, strTimeOne, strTimeTwo;
    vector <string> tokens1, tokens2; 
    vector<int> dateOneVec, dateTwoVec;
    while(getline(ss1, token, '\t'))
    {
        tokens1.push_back(token);
    }
    while(getline(ss2, token, '\t'))
    {
        tokens2.push_back(token);
    }
    //Check for Incorrect Number of Columns
    if(tokens1.size() != 16 || tokens2.size() != 16)
    {
        cout << "Given traffic log has incorrect format. Exiting..." << endl;
        exit(-1);
    }
    stringstream getTimeOne(tokens1.at(3));
    stringstream getTimeTwo(tokens2.at(3));
    getTimeOne >> dateOne;
    getTimeTwo >> dateTwo;
    getTimeOne >> strTimeOne;
    getTimeTwo >> strTimeTwo;
    stringstream parseTimeOne(strTimeOne);
    stringstream parseTimeTwo(strTimeTwo);
    int intTimeOne = timeInSeconds(parseTimeOne);
    int intTimeTwo = timeInSeconds(parseTimeTwo);
    if(intTimeOne < intTimeTwo && dateOne <= dateTwo)
    {
        return true;
    }
    return false;
}

void sortFile(vector<string> &trafficLog)
{
    sort(trafficLog.begin(), trafficLog.end(), compareTime);
}

vector<string> parseLogs(vector<string> dirFiles)
{
    vector<string> blockListTerms = {"apple.com", "api.", "connect.facebook.net", "googletag", "google-analytic", ".api", "googleapis", "apis.", "ads", "update", "mzstatic", "useast1a", "inappcheck", "cdn", "nod.", "nikecloud", "gateway.", "init.push", "smp-device", "central1", "spclient", "prod.net", "tiktokv", "gstatic", "us-west", "adblock", "mobile-data", ".app."};
    for(int i = 0; i < dirFiles.size(); i++)
    {
        vector<string> trace, fileToSort, traceData;
        vector<int> holdTimes;
        ifstream read, correctFile;
        ofstream traceFile;
        string line;
        bool newLink = true, getInitTime = true, getIpAddress = true, getCorrect = true;
        int lineNumber = 0, timeSpent = 0, correct = 0, incorrect = 0, numOfUrls = 0;
        vector<string> correctUrls;
        read.open(dirFiles.at(i));
        cout << dirFiles.at(i) << endl;
        if(!read.is_open())
        {
            cout << "Dir : File not found" << endl;
        }
        while(getline(read, line))
        {
            fileToSort.push_back(line);
        }
        read.close();
        sortFile(fileToSort);
        ofstream newFile("testSort.txt");
        for(int k = 0; k < fileToSort.size(); k++)
        {
            newFile << fileToSort.at(k) << endl;
        }
        //cout << dirFiles.at(i) << endl;
        read.open("testSort.txt");
        while(getline(read, line))
        {
            vector<string> tokens;
            vector<string> lineInfo = parseLine(tokens, line);
            string command = lineInfo.at(0), url = lineInfo.at(1), strTime, date;
            if(getCorrect == true)
            {
                string correctLine;
                int pos = dirFiles.at(i).find_last_of("\\");
                int pos2 = dirFiles.at(i).find(".txt");
                string filename = dirFiles.at(i).substr(pos, pos2 - pos);
                correctFile.open("answers/" + filename + "Correct.txt");
                if(!correctFile.is_open())
                {
                    cout << "File could not be opened." << endl;
                }
                
                while(getline(correctFile, correctLine))
                {
                    correctUrls.push_back(correctLine);
                    numOfUrls++;
                }
                correctFile.close();
                getCorrect = false;
            }
            if(getIpAddress == true)
            {
                traceFile.open("traces/trace" + tokens.at(2) + ".txt", ios::app);
                if(!traceFile.is_open())
                {
                    cout << "File could not be opened." << endl;
                }                
                getIpAddress = false;
            }
            if(stoi(tokens.at(6)) < 50000)
            {
                continue;
            }
            //If GET Find HTTP
            if(command == "GET")
            {
                if(tokens.at(10) != "-")
                {
                    if(trace.size() > 0)
                    {
                        if(trace.at(trace.size() - 1) == tokens.at(10))
                        {
                            newLink = false;
                        }
                    }
                    if(newLink)
                    {
                            if(tokens.at(5) != "0")
                            {
                            //Find Info On HTTP URL
                            int j = lineNumber - 10;
                            if(j < 0)
                            {
                                j = 0;
                            }
                            for(; j < fileToSort.size(); j++)
                            {
                                string searchCmd, searchUrl;
                                vector<string> searchLineInfo, searchTokens;
                                searchLineInfo = parseLine(searchTokens, fileToSort.at(j));
                                searchCmd = searchLineInfo.at(0);
                                searchUrl = searchLineInfo.at(1);
                                //Info Captured
                                if(searchCmd == "GET" && searchUrl == tokens.at(10))
                                {
                                    stringstream ssGetTime(searchTokens.at(3));
                                    strTime = parseTime(ssGetTime);
                                    stringstream ssParsedTime(strTime);
                                    timeSpent = timeInSeconds(ssParsedTime);
                                    holdTimes.push_back(timeSpent);
                                    trace.push_back(tokens.at(10));
                                    traceData.push_back(searchTokens.at(3) + "\t" + searchUrl + "\t" + searchTokens.at(10) + "\t" + searchTokens.at(5) + "\t" + searchTokens.at(12));
                                    //Check if Correct
                                    checkCorrect(correct, incorrect, searchUrl, correctUrls);
                                    break;
                                }
                            }
                            //Get Info If They Do not Have Referrer
                            if(j == fileToSort.size())
                            {
                                stringstream ssGetTime(tokens.at(3));
                                strTime = parseTime(ssGetTime);
                                stringstream ssParsedTime(strTime);
                                timeSpent = timeInSeconds(ssParsedTime);
                                holdTimes.push_back(timeSpent);
                                traceData.push_back(tokens.at(3) + "\t" + url + "\t" + tokens.at(10) + "\t" + tokens.at(5) + "\t" + tokens.at(12));
                            }
                        }
                    }
                    newLink = true;
                }
            }
            //If CONNECT Find HTTPS
            else if(command == "CONNECT")
            {
                if(trace.size() > 1)
                {
                    if(trace.at(trace.size() - 1) == url)
                    {
                        continue;
                    }
                }
                //Check User Agent for Browser
                if((tokens.at(7).find("Mozilla") != string::npos && tokens.at(7).find("Safari") != string::npos && tokens.at(7).find("Chrome") != string::npos) || tokens.at(7) == "Client/21.10.13.18 CFNetwork/1240.0.4 Darwin/20.6.0" || tokens.at(7) == "-")
                {
                    //Check blocklist
                    bool blockListed = false;
                    vector<string> urlTokens;
                    string urlToken;
                    stringstream ss1(url);
                    while(getline(ss1, urlToken, '.'))
                    {
                        urlTokens.push_back(urlToken);
                    }
                    if(urlTokens.at(0) != "www" && urlTokens.at(0) != "mobile")
                    {
                        if(urlTokens.size() > 2)
                        {
                            continue;
                        }
                    }
                    for(int z = 0; z < blockListTerms.size(); z++)
                    {
                        if(url.find(blockListTerms.at(z)) != string::npos)
                        {
                            blockListed = true;
                        }
                    }
                    if(blockListed == false)
                    {
                        //Add to trace
                        stringstream ssGetTime(tokens.at(3));
                        strTime = parseTime(ssGetTime);
                        stringstream ssParsedTime(strTime);
                        timeSpent = timeInSeconds(ssParsedTime);

                        holdTimes.push_back(timeSpent);
                        trace.push_back(url);
                        traceData.push_back(tokens.at(3) + "\t" + url + "\t" + tokens.at(10) + "\t" + tokens.at(5) + "\t" + tokens.at(12));
                        //Check if Correct
                        checkCorrect(correct, incorrect, url, correctUrls);
                    }
                }
            }
            lineNumber++;
        }
        newFile.close();
        //Send to File, Process time
        int prevTimeSpent;
        for(int x = 0; x < traceData.size(); x++)
        {
            if(x + 1 < holdTimes.size())
            {
                timeSpent = holdTimes.at(x + 1) - holdTimes.at(x);
            }
            else
            {
                timeSpent = -1;
            }
            stringstream ss1(traceData.at(x));
            string finalToken;
            vector<string> finalTokens;
            while(getline(ss1, finalToken, '\t'))
            {
                finalTokens.push_back(finalToken);
            }
            if(finalTokens.at(1) == "www.instagram.com")
            {
                if(x != 0)
                {
                    if(prevTimeSpent < 10)
                    {
                        continue;
                    }
                }
            }
            prevTimeSpent = timeSpent;
            traceFile << traceData.at(x) << "\t" << timeSpent << endl;
        }
        
        traceFile << "Correctness = " << ((double)correct / (double)(numOfUrls)) * 100 << "%" << endl;
        //return trace;
    }
    //Delete
    vector<string> temp;
    return temp;
}

int main(int argc, char* argv[])
{
    vector<string> dirFiles, trace, sendPaths;
    for(int k = 1; k < argc; k++)
    {
        string path = argv[k];
        dirFiles.push_back(path);
    }
    int lineNumber = 0;
    for(int j = 1; j < argc; j++)
    {
        string arg = argv[j];
        sendPaths.push_back(arg);
    }
    if(argc == 1)
    {
        cout << "No files provided" << endl;
        exit(-1);
    }
    trace = parseLogs(sendPaths);
    for(int i = 0; i < trace.size(); i++)
    {
        cout << trace.at(i) << endl;
    }
    return 0;
}