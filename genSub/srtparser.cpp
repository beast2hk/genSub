/*
 * Author   : Saurabh Shrivastava
 * Email    : saurabh.shrivastava54@gmail.com
 * Link     : https://github.com/saurabhshri
 *
 * Based on subtitle-parser by Oleksii Maryshchenko.
 * Email    : young_developer@mail.ru
 * Link     : https://github.com/young-developer/subtitle-parser
 */

#include <fstream>
#include <sstream>
#include "srtparser.hpp"

//function for splitting sentences based on supplied delimiter
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

/**** Function definitions ****/

//1. SubtitleParserFactory class

SubtitleParserFactory::SubtitleParserFactory(std::string fileName)
{
    _fileName = fileName;
}

SubtitleParser* SubtitleParserFactory::getParser()
{
    return new SubRipParser(_fileName);                 //creates and returns SubRipParser obj
}

SubtitleParserFactory::~SubtitleParserFactory(void)
{
}

//2. SubtitleParser class

SubtitleParser::subtitles_t SubtitleParser::getSubtitles()
{
    return _subtitles;
}

std::string SubtitleParser::getFileData()           //returns whole read file i.e. contents of input.srt
{
    std::ifstream infile(_fileName);
    std::string allData = "";
    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        allData += line + "\n";
    }
    return allData;

}

SubtitleParser::SubtitleParser(void)
{}

SubtitleParser::~SubtitleParser(void)
{}

//3. SubRipParser class

SubRipParser::SubRipParser(void)
{}

void SubRipParser::parse(std::string fileName)      //srt parser
{

    std::ifstream infile(fileName);
    std::string line, start, end, completeLine = "", timeLine = "";
    int subNo = 0, turn = 0;

    /*
     * turn = 0 -> Add subtitle number
     * turn = 1 -> Add string to timeLine
     * turn > 1 -> Add string to completeLine
     */

    while (std::getline(infile, line))
    {
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());

        if (line.compare(""))
        {
            if(!turn)
            {
                subNo=atoi(line.c_str());
                turn++;
                continue;
            }

            if (line.find("-->") != std::string::npos)
            {
                timeLine += line;

                std::vector<std::string> srtTime;
                srtTime = split(timeLine, ' ', srtTime);
                start = srtTime[0];
                end = srtTime[2];

            }
            else
            {
                if (completeLine != "")
                    completeLine += "\n";

                completeLine += line;
            }

            turn++;
        }

        else
        {
            turn = 0;
            _subtitles.push_back( std::make_shared<SubtitleItem>(subNo,start,end,completeLine) );
            completeLine = timeLine = "";
        }

//        if(infile.eof())    //insert last remaining subtitle
//        {
//            _subtitles.push_back( std::make_shared<SubtitleItem>(subNo,start,end,completeLine) );
//        }
    }
  
  if (completeLine.length() > 0) {
    _subtitles.push_back( std::make_shared<SubtitleItem>(subNo,start,end,completeLine) );
  }

}

SubRipParser::SubRipParser(std::string fileName)
{
    _fileName = fileName;
    parse(fileName);
}

SubRipParser::~SubRipParser(void)
{
}

//4. SubtitleItem class

SubtitleItem::SubtitleItem(void)
{}

SubtitleItem::SubtitleItem(int subNo, std::string startTime,std::string endTime, std::string text, bool ignore,
                           std::string justDialogue, int speakerCount, int nonDialogueCount,
                           int styleTagCount, int wordCount, std::vector<std::string> speaker, std::vector<std::string> nonDialogue,
                           std::vector<std::string> styleTags, std::vector<std::string> word)
{
    _startTime = timeMSec(startTime);
    _endTime = timeMSec(endTime);
    _text = text;

    _subNo = subNo;
    _startTimeString = startTime;
    _endTimeString = endTime;
    _ignore = ignore;
    _justDialogue = justDialogue;
    _speakerCount = speakerCount;
    _nonDialogueCount = nonDialogueCount;
    _wordCount = wordCount;
    _speaker = speaker;
    _styleTagCount = styleTagCount;
    _styleTag = styleTags;
    _nonDialogue = nonDialogue;
    _word = word;

    extractInfo();
}

long int SubtitleItem::timeMSec(std::string value)
{
    std::vector<std::string> t, secs;
    int hours, mins, seconds, milliseconds;

    t = split(value, ':', t);
    hours = atoi(t[0].c_str());
    mins = atoi(t[1].c_str());

    secs = split(t[2], ',', secs);
    seconds = atoi(secs[0].c_str());
    milliseconds = atoi(secs[1].c_str());

    return hours * 3600000 + mins * 60000 + seconds * 1000 + milliseconds;
}

long int SubtitleItem::getStartTime() const
{
    return _startTime;
}
long int SubtitleItem::getEndTime() const
{
    return _endTime;
}

std::string SubtitleItem::getText() const
{
    return _text;
}

void SubtitleItem::setStartTime(long int startTime)
{
    _startTime = startTime;
}

void SubtitleItem::setEndTime(long int endTime)
{
    _endTime = endTime;
}

void SubtitleItem::setText(std::string text)
{
    _text = text;
}

void SubtitleItem::setWordTimes(std::vector<long int> wordStartTime, std::vector<long int> wordEndTime, std::vector<long int> wordDuration)
{
    _wordStartTime = wordStartTime;
    _wordEndTime = wordEndTime;
    _wordDuration = wordDuration;
}

int SubtitleItem::getSubNo() const
{
    return _subNo;
}

std::string SubtitleItem::getStartTimeString() const
{
    return _startTimeString;
}

std::string SubtitleItem::getEndTimeString() const
{
    return _endTimeString;
}

bool SubtitleItem::getIgnoreStatus() const
{
    if(_ignore)
        return true;

    else
        return false;

}

void SubtitleItem::extractInfo(bool keepHTML, bool doNotIgnoreNonDialogues, bool doNotRemoveSpeakerNames)   //process subtitle
{
    std::string output = _text;

    //stripping HTML tags
    if(!keepHTML)
    {
        /*
         * TODO : Before erasing, extract the words.
         * std::vector<std::string> getStyleTags();
         * int getStyleTagCount() const;
         * std::vector<std::string> _styleTag;
         * int _styleTagCount;
         */

        int countP = 0;
        for(char& c : output) // replacing <...> with ~~~~
        {
            if(c=='<')
            {
                countP++;
                c = '~';
            }

            else
            {
                if(countP!=0)
                {
                    if(c != '>')
                        c = '~';

                    else if(c == '>')
                    {
                        c = '~';
                        countP--;
                    }
                }
            }
        }
    }

    //stripping non dialogue data e.g. (applause)

    if(!doNotIgnoreNonDialogues)
    {
        /*
         * TODO : Before erasing, extract the words.
         * std::vector<std::string> getNonDialogueWords();
         * int getNonDialogueCount() const;
         * std::vector<std::string> _nonDialogue;
         * int _nonDialogueCount;
         */

        int countP = 0;
        for(char& c : output)   // replacing (...) with ~~~~
        {
            if(c=='(')
            {
                countP++;
                c = '~';
            }

            else
            {
                if(countP!=0)
                {
                    if(c != ')')
                        c = '~';

                    else if(c == ')')
                    {
                        c = '~';
                        countP--;
                    }
                }
            }
        }
    }

    output.erase(std::remove(output.begin(), output.end(), '~'), output.end()); // deleting all ~

    //Extracting speaker names
    if(!doNotRemoveSpeakerNames)
    {
        for(int i=0; output[i]!='\0';i++)
        {
            int colonIndex = 0, nameBeginIndex = 0;
            if(output[i]==':')  //speaker found; travel back
            {
                _speakerCount++;
                colonIndex = i;

                int tempIndex = 0, foundEvilColon = 0, continueFlag = 0, spaceBeforeColon = 0;

                if(output[i-1] == ' ')
                    spaceBeforeColon = 2;

                /*
                Possible Cases :

                Elon Musk: Hey Saurabh, you are pretty smart.       // First and Last Name
                Saurabh: *_* What? Elon Musk: Yes!                  // Two names in single line
                Saurabh : OMG OMG!                                  // Space before colon
                Elon: LOL World: LAMAO
                Saurabh: ._.                                        // normal

                 */

                for(int j=i - spaceBeforeColon; j>=0;j--)
                {
                    if(output[j] == '.' || output[j] == '!' || output[j] == ',' || output[j] == '?' || output[j] == '\n'
                       || output[j] == ' ' || j== 0)
                    {

                        if(output[j] == '.' || output[j] == '!' || output[j] == ',' || output[j] == '?' || j == 0)
                        {
                            if((continueFlag && j == 0))
                            {
                                if(!isupper(output[j]))
                                {
                                    nameBeginIndex = tempIndex;
                                    break;
                                }

                                else
                                    tempIndex = j;

                            }

                            else if(j!=0)
                                tempIndex = j + 1;
                        }

                        else if(output[j] == ' ' && isupper(output[j+1]))
                        {
                            tempIndex = j;
                            continueFlag = 1;

                            continue;
                        }

                        else if(output[j] == ' ' && !isupper(output[j+1] && tempIndex == 0))
                        {
                            _speakerCount--;
                            foundEvilColon = 1;
                            break;
                        }

                        nameBeginIndex = tempIndex;
                        break;
                    }
                }

                if(foundEvilColon)
                    continue;

                i = nameBeginIndex; //compensating the removal and changes in index

                //check if there's a space after colon i.e. A: Hello vs A:Hello
                int removeSpace = 0;
                if(output[colonIndex + 1]==' ')
                    removeSpace = 1;

                _speaker.push_back(output.substr(nameBeginIndex, colonIndex - nameBeginIndex));
                output.erase(nameBeginIndex, colonIndex - nameBeginIndex + removeSpace);
            }

        }

    }

    // removing more than one whitespaces with one space
    unique_copy (output.begin(), output.end(), std::back_insert_iterator<std::string>(_justDialogue),
                 [](char a,char b)
                 {
                     return isspace(a) && isspace(b);
                 });

    // trimming whitespaces
    const char* whiteSpaces = " \t\n\r\f\v";
    _justDialogue.erase(0, _justDialogue.find_first_not_of(whiteSpaces));
    _justDialogue.erase(_justDialogue.find_last_not_of(whiteSpaces) + 1);

    if(_justDialogue.empty() || _justDialogue == " ")
        _ignore = true;

    else
    {
        _word = split(_justDialogue, ' ', _word); //extracting individual words
        _wordCount = (int) _word.size();
    }
}

std::string SubtitleItem::getDialogue(bool keepHTML, bool doNotIgnoreNonDialogues,  bool doNotRemoveSpeakerNames)
{
    if(_justDialogue.empty())
        extractInfo(keepHTML, doNotIgnoreNonDialogues, doNotRemoveSpeakerNames);

    return _justDialogue;
}

int SubtitleItem::getSpeakerCount() const
{
    return _speakerCount;
}

int SubtitleItem::getNonDialogueCount() const
{
    return _nonDialogueCount;
}

int SubtitleItem::getStyleTagCount() const
{
    return _styleTagCount;
}

int SubtitleItem::getWordCount() const
{
    return _wordCount;
}

std::vector<std::string> SubtitleItem::getSpeakerNames()
{
    return _speaker;
}

std::vector<std::string> SubtitleItem::getNonDialogueWords()
{
    return _nonDialogue;
}

std::vector<std::string> SubtitleItem::getIndividualWords()
{
    return _word;
}

std::string SubtitleItem::getWordByIndex(int index)
{
    return _word[index];
}

std::vector<long int> SubtitleItem::getWordStartTimes()
{
    return _wordStartTime;
}

std::vector<long int> SubtitleItem::getWordEndTimes()
{
    return _wordEndTime;
}

long int SubtitleItem::getWordStartTimeByIndex(int index)
{
    return _wordStartTime[index];
}

long int SubtitleItem::getWordEndTimeByIndex(int index)
{
    return _wordEndTime[index];
}

std::vector<std::string> SubtitleItem::getStyleTags()
{
    return _styleTag;
}

SubtitleItem::~SubtitleItem(void)
{}


//5. SubtitleWordclass

SubtitleWord::SubtitleWord(void)
{
    _text = "";
}

SubtitleWord::SubtitleWord(std::string text)
{
    _text = text;
}

std::string SubtitleWord::getText() const
{
    return _text;
}

SubtitleWord::~SubtitleWord(void)
{}
