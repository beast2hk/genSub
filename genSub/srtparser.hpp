/*
 * Author   : Saurabh Shrivastava
 * Email    : saurabh.shrivastava54@gmail.com
 * Link     : https://github.com/saurabhshri
 *
 * Based on subtitle-parser by Oleksii Maryshchenko.
 * Email    : young_developer@mail.ru
 * Link     : https://github.com/young-developer/subtitle-parser
 */

#ifndef SRTPARSER_H
#define SRTPARSER_H

#include <string>
#include <vector>
#include <memory>

/**** Class definitions ****/

class SubtitleWord
{
private:
    std::string _text;
public:
    SubtitleWord(void);
    SubtitleWord(std::string text);
    virtual std::string getText() const;
    ~SubtitleWord(void);
};

class SubtitleItem
{
private:
    long int _startTime;                    //in milliseconds
    long int _endTime;
    std::string _text;                      //actual line, as present in subtitle file
    long int timeMSec(std::string value);   //converts time string into ms

    int _subNo;                              //subtitle number
    std::string _startTimeString;           //time as in srt format
    std::string _endTimeString;
    bool _ignore;                           //should subtitle be ignore; used when the subtitle is empty after processing
    std::string _justDialogue;              //contains processed subtitle - stripped style, non dialogue text removal etc.
    int _speakerCount;                      //count of number of speakers
    std::vector<std::string> _speaker;      //list of speakers in a single subtitle
    int _nonDialogueCount;                  //count of non spoken words in a subtitle
    std::vector<std::string> _nonDialogue;  //list of non dialogue words, e.g. (applause)
    int _wordCount;                         //number of words in _justDialogue
    std::vector<std::string> _word;         //list of words in dialogue
    std::vector<long int> _wordStartTime;   //start time of each word in dialogue
    std::vector<long int> _wordEndTime;     //end time of each word in dialogue
    std::vector<long int> _wordDuration;   //actual duration of each word without silence
    int _styleTagCount;                     //count of style tags in a single subtitle
    std::vector<std::string> _styleTag;     //list of style tags in that subtitle
    void extractInfo(bool keepHTML = 0, bool doNotIgnoreNonDialogues = 0,  bool doNotRemoveSpeakerNames = 0);   //process subtitle
public:
    long int getStartTime() const;          //returns starting time in ms
    long int getEndTime() const;            //returns ending time in ms
    std::string getText() const;            //returns subtitle text as present in .srt file

    int getSubNo() const;              //returns subtitle number
    std::string getStartTimeString() const; //returns sarting time as present in .srt file
    std::string getEndTimeString() const;   //returns ending time as present in .srt file
    bool getIgnoreStatus() const;           //returns status, whether the subtitle is ignorable or not after processing
    std::string getDialogue(bool keepHTML = 0, bool doNotIgnoreNonDialogues = 0,  bool doNotRemoveSpeakerNames = 0); //returns processed subtitle
    int getSpeakerCount() const;            //return speaker count
    int getNonDialogueCount() const;        //return non dialogue words count
    int getStyleTagCount() const;           //return style tags count
    int getWordCount() const;               //return words count
    std::vector<std::string> getIndividualWords(); //return string vector of individual words
    std::string getWordByIndex(int index);       //return word stored at 'index'
    std::vector<long int> getWordStartTimes();   //return long int vector of start time of individual words
    std::vector<long int> getWordEndTimes();     //return long int vector of end time of individual words
    long int getWordStartTimeByIndex(int index); //return the start time of a word based on index
    long int getWordEndTimeByIndex (int index);  //return the end time of a word based on index
    std::vector<std::string> getSpeakerNames();  //return string vector of speaker names
    std::vector<std::string> getNonDialogueWords(); //return string vector of non dialogue words
    std::vector<std::string> getStyleTags();    //return string vector of style tags


    void setStartTime(long int startTime);  //set starting time
    void setEndTime(long int endTime);      //set ending time
    void setText(std::string text);         //set subtitle text
    void setWordTimes(std::vector<long int> wordStartTime, std::vector<long int> wordEndTime, std::vector<long int> wordDuration);  //assign time to individual words

    SubtitleItem(void);
    SubtitleItem(int subNo, std::string startTime,std::string endTime, std::string text, bool ignore = false,
                 std::string justDialogue = "" , int speakerCount = 0, int nonDialogueCount = 0,
                 int styleTagCount = 0, int wordCount = 0, std::vector<std::string> speaker = std::vector<std::string>(),
                 std::vector<std::string> nonDialogue = std::vector<std::string>(),
                 std::vector<std::string> styleTags = std::vector<std::string>(),
                 std::vector<std::string> word = std::vector<std::string>());  //default constructor
    ~SubtitleItem(void);
};

class SubtitleParser
{
public:
  typedef std::vector< std::shared_ptr<SubtitleItem> > subtitles_t;
protected:
    subtitles_t _subtitles;              //stores subtitles
    std::string _fileName;                              //supplied filename
    virtual void parse(std::string fileName) = 0;
public:
    virtual subtitles_t getSubtitles();  //returns subtitles
    std::string getFileData();
    SubtitleParser(void);
    virtual ~SubtitleParser(void);
};

class SubtitleParserFactory
{
private:
    std::string _fileName;
public:
    SubtitleParser* getParser();
    SubtitleParserFactory(std::string fileName);
    ~SubtitleParserFactory(void);
};

class SubRipParser : public SubtitleParser
{
    void parse(std::string fileName);
public:
    SubRipParser(void);
    SubRipParser(std::string fileName);
    ~SubRipParser(void);
};

#endif //SRTPARSER_H
