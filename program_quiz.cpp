#include <iostream>
#include <conio.h> 
#include <fstream>  // For file handling
#include <vector>   // For main storage of this program
#include <memory>   // For Smart pointers
#include <array>
#include <fileSystem> // For remove()


// Defining Constant variables, enum, objects, and namespaces
enum questionType {BaseType, MultipleChoiceType, ShortAnswerType};
const int MAX_TEXT_LENGHT = 150;
const int LINE_LENGHT = 44;
const std::string MASTER_FILE_NAME = "master_data.csv";
const std::string QUIZ_FILE_NAME = "quiz_data.csv";
bool isTeacherMode = false;

// UTILS
//----------------------------------------------------------------------------------------------
namespace userInput{
    int choice(int maxChoiceValue){
        std::cout << "Your Input: ";
        while (true)
        {
            std::string input;
            std::getline(std::cin, input, '\n');

            // Check if all characters is a number (0 -- 9)
            bool isValid = true;
            for (char ch : input){
                if(!isdigit(ch)){
                    isValid = false;
                    break;
                }
            }

            // If not valid or not inputting any number
            if (!isValid || input.length() < 1){
                std::cout << "Input is not a valid number! Try again: ";
                continue;
            }

            // Check if the number is within acceptable range
            int value = std::stoi(input);
            if (value < 1 || value > maxChoiceValue){
                std::cout << "Input is not in the valid range! Try again: ";
                continue;
            }
            
            // Return if valid
            std::cout << std::endl;
            return value;
        }
    }

    std::string text(int maxTextLenght){
        std::cout << "Your input: ";
        while(true){
            std::string input;
            std::getline(std::cin, input, '\n');

            bool isValid = true;
            for(char ch : input){ // Check for coma
                if(ch == ','){
                    isValid = false;
                    break;
                } 
            }

            if(input == "---" || input.length() > maxTextLenght){ // Check for line and lenght
                isValid = false;
            }

            if (!isValid) {
                std::cout << "Your input is too long or invalid! Try Again: ";
                continue;
            }

            std::cout << std::endl;
            return input;
        }
    }

}


// QUESTION CLASSES
//----------------------------------------------------------------------------------------------
class Question{
    protected:
        std::string text;
    public:
        Question(std::string text): text(text) {}
        virtual ~Question() = default;

        void setText(std::string str){ text = str; }


        // Place holder function
        virtual void setAnswer(std::string str){
            //This one doesn't have anwer
            return;
        }

        virtual void addChoice(std::string str){
            //this one doesn't have any anwer
            return;
        }

        virtual void printQuestion() { std::cout << text << std::endl; }

        virtual bool checkAnswer(std::string input){
            // Pertanyaan ini gak punya jawaban
            return true;
        }


        // Change Function
        virtual bool beginChanging(){
            bool isChangeComplete = false;

            std::cout << "(1) Change Text" << std::endl;
            std::cout << "(2) Back" << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;

            switch (userInput::choice(2)){
                case 1:
                    std::cout << std::string(LINE_LENGHT, '-') << std::endl;
                    std::cout << "CHANGE TEXT "<< std::endl;
                    std::cout << "Input text:"<< std::endl;
                    setText(userInput::text(MAX_TEXT_LENGHT));
                    break;
                case 2:
                    isChangeComplete = true;
                    break;
                default:
                    std::cerr << "Error: Something(s) wrong" << std::endl;
                    break;
            }
            
            return isChangeComplete;
        }

        virtual void beginCreating(){
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "Answerless Question created!" << std::endl;
            return;
        }


        // File Management
        virtual std::vector<std::string> savingData(){
            std::vector<std::string> returnVec;

            returnVec.push_back("0");
            returnVec.push_back(text);

            return returnVec;
        }


        // Static Function
        static void displayQuestions(std::vector<std::shared_ptr<Question>> questions){
            for(auto question : questions){
                question->printQuestion();
            }
        }

        
};

class MultipleChoice : public Question {
    private:
        std::vector<std::string> choices;
        int indexOfAnswer;
    public:
        MultipleChoice(std::string text): Question(text) {}

        // Edit properties
        // Add choice need to be a virtual function, so it can use even when it's a smart pointers
        void addChoice(std::string choice) override{ choices.push_back(choice); }

        void deleteChoice(int index){ choices.erase(choices.begin() + index); }

        void setAnswer(std::string indexStr) override{ indexOfAnswer = std::stoi(indexStr); }

        void changeAnswer(int index){ indexOfAnswer = index; }


        // Utils
        void printQuestion() override {
            std::cout << text << std::endl;

            int choiceInt = 1;
            for(std::string choice : choices){
                std::cout << choiceInt << ": " << choice;
                
                if(choiceInt - 1 == indexOfAnswer && isTeacherMode){
                    std::cout << " (Answer)";
                }

                std::cout << std::endl;
                choiceInt++;
            }
        }

        bool checkAnswer(std::string input) override {
            int inputedAnswer = std::stoi(input) - 1;
            return inputedAnswer == indexOfAnswer;
        }


        // Change Function
        bool beginChanging() override{
            bool isChangeComplete = false;
            int choiceCount = choices.size();

            std::cout << "(1) Change Text" << std::endl;
            std::cout << "(2) Change Answer to Different Choice" << std::endl;
            std::cout << "(3) Add Choice" << std::endl;
            std::cout << "(4) Delete Choice" << std::endl;
            std::cout << "(5) Back" << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;

            switch (userInput::choice(5)){
                case 1:
                    std::cout << std::string(LINE_LENGHT, '-') << std::endl;
                    std::cout << "CHANGE TEXT"<< std::endl;
                    std::cout << "Input text:"<< std::endl;
                    setText(userInput::text(MAX_TEXT_LENGHT));
                    break;
                case 2:
                    std::cout << std::string(LINE_LENGHT, '-') << std::endl;
                    std::cout << "CHANGE ANSWER" << std::endl;
                    std::cout << "Select choice to be the answer: ";
                    changeAnswer(userInput::choice(choiceCount) - 1);
                    break;
                case 3:
                    std::cout << std::string(LINE_LENGHT, '-') << std::endl;
                    std::cout << "ADD CHOICE" << std::endl;
                    std::cout << "Input text for new choice: " << std::endl;
                    addChoice(userInput::text(MAX_TEXT_LENGHT));
                    break;
                case 4:
                    std::cout << std::string(LINE_LENGHT, '-') << std::endl;
                    std::cout << "DELETE CHOICE" << std::endl;
                    std::cout << "Select choice to be the deleted: ";
                    deleteChoice(userInput::choice(choiceCount) - 1);
                    break;
                case 5:
                    isChangeComplete = true;
                    break;
                default:
                    std::cerr << "Error: Something(s) wrong" << std::endl;
                    break;
            }
            
            return isChangeComplete;
        }

        // File management
        void beginCreating() override{
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "How Many Choices do you want this questions have? (Max 10)" << std::endl;
            int count = userInput::choice(10);
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;

            // Adding choices to the question
            for(int i = 1; i <= count; i++){
                std::cout << "Input text for choice no." << i << std::endl;
                choices.push_back(userInput::text(MAX_TEXT_LENGHT));
            }

            // Selecting a chioce to be the answer
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            for(int i = 1; i <= count; i++){
                std::cout << "(" << i << ") " << choices[i-1] << std::endl;
            }
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "Select Which choices is the answer?" << std::endl;
            this->changeAnswer(userInput::choice(count) - 1);

            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "Multiple Choice Question created!" << std::endl;
            return;
        }

        std::vector<std::string> savingData() override{
            std::vector<std::string> returnVec;

            returnVec.push_back("1");
            returnVec.push_back(text);
            returnVec.push_back(std::to_string(indexOfAnswer));
            for(std::string choice : choices){
                returnVec.push_back(choice);
            }

            return returnVec;
        }
};

class ShortAnswer : public Question {
    private:
        std::string answer;
    public:
        ShortAnswer(std::string text): Question(text) {}

        // Edit properties
        void setAnswer(std::string str){ answer = str; }

        // Utils
        void printQuestion() override {
            std::cout << text << std::endl;
            if(isTeacherMode){
                std::cout << "Answer: " << answer << std::endl;
            }
            
        }

        // Only return function
        bool checkAnswer(std::string input) override { return answer == input; }

        // Change Function
        bool beginChanging() override{
            bool isChangeComplete = false;

            std::cout << "(1) Change Text" << std::endl;
            std::cout << "(2) Change Answer" << std::endl;
            std::cout << "(3) Back" << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;

            switch (userInput::choice(3)){
                case 1:
                    std::cout << std::string(LINE_LENGHT, '-') << std::endl;
                    std::cout << "CHANGE TEXT "<< std::endl;
                    std::cout << "Input text:"<< std::endl;
                    setText(userInput::text(MAX_TEXT_LENGHT));
                    break;
                case 2:
                    std::cout << std::string(LINE_LENGHT, '-') << std::endl;
                    std::cout << "CHANGE ANSWER";
                    std::cout << "Input text for answer:" << std::endl;
                    setAnswer(userInput::text(MAX_TEXT_LENGHT));
                case 3:
                    isChangeComplete = true;
                    break;
                default:
                    std::cerr << "Error: Something(s) wrong" << std::endl;
                    break;
            }
            
            return isChangeComplete;
        }


        // File Management
        void beginCreating() override{
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "Input Answer" << std::endl;
            this->setAnswer(userInput::text(MAX_TEXT_LENGHT));

            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "Short Answer Question created!" << std::endl;
            return;
        }

        std::vector<std::string> savingData() override{
            std::vector<std::string> returnVec;

            returnVec.push_back("2");
            returnVec.push_back(text);
            returnVec.push_back(answer);

            return returnVec;
        }
};


// QUIZ & MASTER
//----------------------------------------------------------------------------------------------
class Quiz {
    private:
        bool assigned;
        std::string name;
        std::string questionsFileName;
        int score = -1;
        std::vector<std::shared_ptr<Question>> questions;
    public:
        Quiz(bool assigned, std::string name): assigned(assigned), name(name) {}


        // Only-return function
        std::string getName(){ return name; }

        bool getAssignedValue(){ return assigned; }

        int getScore(){ return score; }

        std::string getQuestionsFileName(){ return questionsFileName; }

        int getQuestionsCount(){ return questions.size(); }


        // Edit Properties
        void setScore(std::string scoreStr){ score = std::stoi(scoreStr); }
        
        void changeScore(int score){ this->score = score; }

        void switchAssigned(){ assigned = !assigned; }

        void resetScore(){ score = -1; }

        void setQuestionsFileName(std::string fileName){ this->questionsFileName = fileName;}

        bool changeOneQuestion(int index){ return questions[index]->beginChanging(); }

        void deleteOneQuestion(int index){ questions.erase(questions.begin() + index); }


        // File Management Function
        void addQuestion(std::string text, questionType type, bool isManualInput){
            switch (type)
            {
            case BaseType:
                questions.push_back(std::make_shared<Question>(text));
                break;
            case MultipleChoiceType:
                questions.push_back(std::make_shared<MultipleChoice>(text));
                break;
            case ShortAnswerType:
                questions.push_back(std::make_shared<ShortAnswer>(text));
                break;
            
            default:
                break;
            }

            if(isManualInput){
                int index = this->getQuestionsCount() - 1;
                questions[index]->beginCreating();
            }
            
            return;
            
        }

        void readQuestionsFile(){
            std::string headerReader;
            std::ifstream questionsFile(this->questionsFileName);

            if(!questionsFile.is_open()){
                std::cerr << "Error: Failed on opening " << this->questionsFileName << std::endl;
                return;
            }

            // Read header first until finish
            while (true){
                std::getline(questionsFile, headerReader, '\n');
                if(headerReader == "---"){
                    break;
                }
            }
            
            // Begin reading the data
            std::string readLine; // Temp optional var for reading "---"
            
            questionType currentType = BaseType;

            bool readIsComplete = false;
            int indexCount = 0;
            while(!readIsComplete) {

                // Read the question type (cast from str --> int --> questionType)
                std::string questionTypeStr;
                std::getline(questionsFile, questionTypeStr, '\n');
                if(questionTypeStr == "END"){
                    readIsComplete = true;
                    break;
                }

                // If the current qestion type is not valid, on the switch statement, skip until ---
                currentType = static_cast<questionType>(std::stoi(questionTypeStr)); 

                std::string questionText;
                std::getline(questionsFile, questionText, '\n'); 
                
                // Begin instantiate question based on the type, then passing the text of the question
                this->addQuestion(questionText, currentType, false);
                switch (currentType){
                    case BaseType:{
                        std::getline(questionsFile, readLine, '\n'); 
                        break;
                    }
                    case MultipleChoiceType:{
                        std::string answerReader;
                        std::string choiceReader;
                        std::getline(questionsFile, answerReader, '\n');
                        questions[indexCount]->setAnswer(answerReader);
                        while (true){
                            std::getline(questionsFile, choiceReader, '\n');
                            if(choiceReader == "---"){ // Same as readLine
                                break;
                            }
                            questions[indexCount]->addChoice(choiceReader);
                        }
                        break;
                    }
                    case ShortAnswerType: {
                        std::string answer;
                        std::getline(questionsFile, answer, '\n');
                        questions[indexCount]->setAnswer(answer);

                        std::getline(questionsFile, readLine, '\n');
                        break;
                    }
                    default:
                        std::cerr << "Error: Question Type not identified" << std::endl;

                        // Skip until "---" (to the next question)
                        while(true){
                            std::getline(questionsFile, readLine, '\n');
                            if(readLine == "---"){
                                break;
                            }
                        }
                        break;
                }

                indexCount++;
            }
            
        }

        void saveQuestionsFile(){
            std::ofstream questionsFile(questionsFileName);

            if(!questionsFile.is_open()){
                std::cerr << "Error: There's something wrong when opening " << questionsFileName << std::endl;
                return;
            }

            questionsFile << "type of question" << std::endl;
            questionsFile << "text" << std::endl;
            questionsFile << "answer" << std::endl;
            questionsFile << "choice (if multiple choices)" << std::endl;
            questionsFile << "---" << std::endl;

            for(auto question : questions){
                std::vector<std::string> toInput = question->savingData();
                
                for(std::string str : toInput){
                    questionsFile << str << std::endl;
                }

                questionsFile << "---" << std::endl;
            }

            questionsFile << "END";
        }


        // QUIZ DATA MENU
        void printOneQuestion(int index){ questions[index]->printQuestion(); }

        void accessDisplayQuestions(){ Question::displayQuestions(questions); }

        void printQuizData(){
            std::cout << ((assigned) ? ("Assigned") : ("NOT Assigned"))
                << " | " << name << " | " << questions.size()  << " question(s) | Score: ";
            
            if(score == -1){
                std::cout << "N/A" << std::endl;
            } else {
                std::cout << score << "%" << std::endl;
            }
        }

        int beginQuiz(){
            int correctAnswerCount = 0;

            for(auto question : questions){
                question->printQuestion();
                std::string input = userInput::text(MAX_TEXT_LENGHT);

                bool isCorrect = question->checkAnswer(input);
                if(isCorrect){
                    correctAnswerCount++;
                } 
            }

            return correctAnswerCount;
        }


        // Static function
        // filterAssigned: 0 -> no filter, 1 -> unassigned only, 2 -> assigned only
        // scoreRange: to show all use {-1, 100}
        static void displayQuiz(std::vector<Quiz> quizzes, int filterAssigned, std::array<int, 2> scoreRange){
            int i = 1;

            for (auto quiz : quizzes){
                if((quiz.getScore() >= scoreRange[0]) && (quiz.getScore() <= scoreRange[1])){ // Checking range

                    if(filterAssigned == 0){
                        std::cout << i++ << ": ";
                        quiz.printQuizData();
                    } else if((filterAssigned - 1) == quiz.getAssignedValue()) {
                        std::cout << i++ << ": ";
                        quiz.printQuizData();
                    }
                }
            }
        }

        // Function to return pointers for each assigned quizzes
        static std::vector<Quiz*> filterAssignedQuiz_ptr(std::vector<Quiz*> & quizzes){
            int filterAssigned = 1; // Assigned only
            std::vector<Quiz*> returnVec;

            for(auto quiz : quizzes){
                if(filterAssigned  == quiz->getAssignedValue()){
                    returnVec.push_back(quiz);
                }
            }

            return returnVec;
        }

        
};

class Master
{
private:
    std::string loginPassword;
    std::vector<Quiz> quizzes;

    std::string masterFileName;
    std::string quizFileName;

public:
    Master(std::string masterFileName, std::string quizFileName) : masterFileName(masterFileName), quizFileName(quizFileName) {}


    // Edit properties
    void addQuiz(bool assigned, std::string name){ quizzes.push_back(Quiz(assigned, name)); }

    // Only-return function
    std::string getLoginPassword(){ return loginPassword; }

    std::vector<Quiz> & getQuizzes(){ return quizzes; }

    int getQuizzesCount(){ return quizzes.size(); }


    // Change Function
    void changeLoginPassword(std::string loginPassword){ this->loginPassword = loginPassword; }

    void deleteOneQuiz(int index){ quizzes.erase(quizzes.begin() + index); }


    // Middleman function
    void changeAssignedStatus(int index){ quizzes[index].switchAssigned(); }

    void resetQuizScore(int index){ quizzes[index].resetScore(); }

    
    // Functional
    int beginAnswering(Quiz* quiz){
        int correctAnswerCount = quiz->beginQuiz();
        return correctAnswerCount;
    }


    // FILE MANAGEMENT
    void readMasterFile()
    {
        std::string headerReader;
        std::ifstream masterFile(masterFileName);

        if (!masterFile.is_open())
        {
            std::cerr << "Error: Failed on opening " << masterFileName << std::endl;
            return;
        }
        std::getline(masterFile, headerReader, '\n');

        std::getline(masterFile, loginPassword, '\n');
    }

    void readQuizFile()
    {
        std::string headerReader;
        std::ifstream quizFile(quizFileName);

        if (!quizFile.is_open())
        {
            std::cerr << "Error: Failed on opening " << quizFileName << std::endl;
            return;
        }
        std::getline(quizFile, headerReader, '\n');

        std::string currentRead;
        int currentAssigned;
        int indexCount = 0;
        while (true)
        {
            std::getline(quizFile, currentRead, ','); // Read assigned value
            if (currentRead == "END"){ break; }


            // Check assigend value
            currentAssigned = std::stoi(currentRead);
            if(currentAssigned < 0 || currentAssigned > 1){
                std::cerr << "Error: Invalid assigned value" << std::endl;
                std::getline(quizFile, currentRead, '\n');
                continue;
            }

            // Read quiz name
            std::getline(quizFile, currentRead, ',');

            // Instantiate new quiz (assigned value, name)
            addQuiz(currentAssigned, currentRead);

            // Read & set score
            std::getline(quizFile, currentRead, ',');
            quizzes[indexCount].setScore(currentRead);

            // Begin reading the question
            std::getline(quizFile, currentRead, '\n');
            quizzes[indexCount].setQuestionsFileName(currentRead);
            quizzes[indexCount].readQuestionsFile();
            indexCount++;
        }

        quizFile.close();
    }

    void saveQuizFile(){
        std::ofstream quizFile(quizFileName);

        if (!quizFile.is_open())
        {
            std::cerr << "Error: Failed on opening " << quizFileName << std::endl;
            return;
        }

        quizFile << "assigned,quiz name,score,file name\n"; // Header
        for(Quiz quiz : quizzes){
            std::string inputToFile = "";
            inputToFile += std::to_string(quiz.getAssignedValue()) + ",";
            inputToFile += quiz.getName() + ",";
            inputToFile += std::to_string(quiz.getScore()) + ",";
            inputToFile += quiz.getQuestionsFileName() + "\n";

            quizFile << inputToFile;
            quiz.saveQuestionsFile();
        }
        quizFile << "END";

        quizFile.close();
    }
};

// THE PROGRAM ITSELF
//----------------------------------------------------------------------------------------------
Master master(MASTER_FILE_NAME, QUIZ_FILE_NAME);


void startAplication(){
    master.readMasterFile();
    master.readQuizFile();
}



namespace teacherMode{
    // Utils
    bool login(){
        bool stopLogin = false;
        while (!stopLogin)
        {
            std::string input;

            std::cout << "Input teacher's password: ";
            std::getline(std::cin, input, '\n');

            if (input == master.getLoginPassword()){
                return true;
            }
            
            std::cout << "The password input is wrong! Do you want to try again?" << std::endl;
            std::cout << "(1) Yes" << std::endl;
            std::cout << "(2) No" << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;


            switch (userInput::choice(2))
            {
            case 1:
                continue;
                break;
            case 2:
                stopLogin = true;
                break;
            default:
                std::cerr << "Something is wrong on the teacherLogin()";
                break;
            }
        }

        return false;
    }

    void deleteFiles(std::vector<std::string> toDelete){
        for(std::string fileName : toDelete){
            try
            {
                if(std::filesystem::remove(fileName) ){
                    std::cout << "Deleted " << fileName << std::endl;
                } else {
                    std::cerr << "There's something wrong when deleting " << fileName << std::endl;
                }
            }
            catch(const std::filesystem::filesystem_error& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
    }


    // QUESTION LEVEL FUNCTION
    void editQuestion(Quiz quiz, int questionIndex){
        while(true){
            std::cout << std::string(LINE_LENGHT, '=') << std::endl;
            std::cout << "EDIT QUESTION - " << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "Question Number: " << questionIndex+1 << std::endl;

            quiz.printOneQuestion(questionIndex);

            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            if (quiz.changeOneQuestion(questionIndex)){ // Kalau sudah selesai change
                break;
            };
        }
    }

    void deleteQuestion(Quiz & quiz, int questionIndex){
            std::cout << std::string(LINE_LENGHT, '=') << std::endl;
            std::cout << "DELETE QUESTION - " << quiz.getName() << std::endl;
            std::cout << "Question Number: " << questionIndex + 1 << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;

            quiz.printOneQuestion(questionIndex);

            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "Are you sure to do delete this question?"<< std::endl;
            std::cout << "(1) Yes" << std::endl;
            std::cout << "(2) NO" << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;

            switch(userInput::choice(2)){
                case 1:
                    quiz.deleteOneQuestion(questionIndex);
                    break;
                case 2:
                    return;
                    break;

                default:
                    std::cerr << "Something wrong with deleting question choice" << std::endl;
                    break;
            }
    }

    void addQuestion(Quiz & quiz){
        std::cout << std::string(LINE_LENGHT, '=') << std::endl;
        std::cout << "ADD QUESTION - " << quiz.getName() << std::endl;
        std::cout << std::string(LINE_LENGHT, '-') << std::endl;
        std::cout << "Select Question type" << std::endl;
        std::cout << "(1) Answerless" << std::endl;
        std::cout << "(2) Multiple Choice" << std::endl;
        std::cout << "(3) Short Answer" << std::endl;
        std::cout << "(4) Cancel" << std::endl;
        std::cout << std::string(LINE_LENGHT, '-') << std::endl;

        questionType currentType = BaseType;
        int input = userInput::choice(4);
        switch(input){
            case 1 ... 3:
                currentType = static_cast<questionType>(input-1);
            break;
            case 4:
                return;
            break;
            default:
                std::cerr << "Error: Choice invalid" << std::endl;
            break;
        }
        
        std::cout << std::string(LINE_LENGHT, '=') << std::endl;
        std::cout << "ADD QUESTION - " << quiz.getName() << std::endl;
        std::cout << std::string(LINE_LENGHT, '-') << std::endl;
        std::cout << "Input question text: " << std::endl;
        quiz.addQuestion(userInput::text(MAX_TEXT_LENGHT), currentType, true);
    }

    void accessQuiz(int index){
        int questionIndex = 0;
        Quiz & quiz = master.getQuizzes()[index];

        while(true){
            std::cout << std::string(LINE_LENGHT, '=') << std::endl;
            std::cout << "QUIZ VIEW - " << quiz.getName() << std::endl;
            std::cout << "Question Number: " << questionIndex + 1 << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;

            quiz.printOneQuestion(questionIndex);

            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "(1) Next" << std::endl;
            std::cout << "(2) Previous" << std::endl;
            std::cout << "(3) Edit Question" << std::endl;
            std::cout << "(4) Delete this Question" << std::endl;
            std::cout << "(5) Add New Question" << std::endl;
            std::cout << "(6) Back" << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;


            switch (userInput::choice(7)){
                case 1:
                    if(questionIndex < (quiz.getQuestionsCount() - 1)){
                        questionIndex++;
                    }
                    break;
                case 2:
                    if(questionIndex > 0 ){
                        questionIndex--;
                    }
                    break;
                case 3:
                    editQuestion(quiz, questionIndex);
                    break;
                case 4:
                    if(quiz.getQuestionsCount() == 1){
                        std::cout << "You can't delete the last question!" << std::endl;
                        break;
                    }
                    deleteQuestion(quiz, questionIndex);
                    questionIndex = 0;
                    break;
                case 5:
                    addQuestion(quiz);
                    break;
                case 6:
                    return;
                    break;
                
                default:
                    break;
            }
        }
    }


    // TRANSITION (QUIZ -> QUESTION)
    void openQuizExact(std::vector<Quiz> quizzes){
        int quizCount = quizzes.size();

        while(true){
            std::cout << std::string(LINE_LENGHT, '=') << std::endl;
            std::cout << "QUIZ LIST - OPEN QUIZ" << std::endl;
            Quiz::displayQuiz(quizzes, 0, {-1, 100});

            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "Select Quiz to Open (" << quizCount + 1 << " to back): " << std::endl;
            
            int input = userInput::choice(quizCount + 1);
            if(input == quizCount+1){
                return;
            } else {
                accessQuiz(input-1);
            }
        }
        
    }


    // QUIZ LEVEL FUNCTION
    void createQuiz(std::vector<Quiz> & quizzes){
        while (true){
            int newQuizIndex = master.getQuizzesCount();

            std::cout << std::string(LINE_LENGHT, '=') << std::endl;
            std::cout << "CREATE QUIZ" << std::endl;

            Quiz::displayQuiz(quizzes, 0, {-1, 100});

            // Input name
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "Input quiz name: " << std::endl;
            std::string name = userInput::text(MAX_TEXT_LENGHT);

            int nameIsUsed = false;
            for(Quiz quiz : quizzes){
                if(name == quiz.getName()){
                    std::cout << "There's already a quiz with this name! Try another one" << std::endl;
                    nameIsUsed = true;
                    break;
                }
            }

            if(nameIsUsed){
                continue;
            }

            // Make new quiz
            master.addQuiz(0, name);
            quizzes[newQuizIndex].setQuestionsFileName(name + ".txt");
            
            quizzes[newQuizIndex].addQuestion("Delete this question if needed", BaseType, false);

            std::cout << "New Quiz created!" << std::endl;
            return; // Stop the loop
        }
    }

    std::string deleteQuiz(std::vector<Quiz> quizzes){
        int amountOfQuiz = master.getQuizzesCount();

        std::cout << std::string(LINE_LENGHT, '=') << std::endl;
        std::cout << "DELETE QUIZ" << std::endl;

        Quiz::displayQuiz(quizzes, 0, {-1, 100});

        std::cout << std::string(LINE_LENGHT, '-') << std::endl;
        std::cout << "Select Quiz to delete: " << std::endl;
        int indexToDelete = userInput::choice(amountOfQuiz) - 1;

        std::string file = master.getQuizzes()[indexToDelete].getQuestionsFileName();
        master.deleteOneQuiz(indexToDelete);

        std::cout << "Added " + file + " to delete" << std::endl;
        return file;
    }

    void changeAssignedStatus(std::vector<Quiz> quizzes){
        int amountOfQuiz = master.getQuizzesCount();

        std::cout << std::string(LINE_LENGHT, '=') << std::endl;
        std::cout << "CHANGE ASSGINED STATUS" << std::endl;

        Quiz::displayQuiz(quizzes, 0, {-1, 100});

        std::cout << std::string(LINE_LENGHT, '-') << std::endl;
        std::cout << "Select Quiz's status to change:" << std::endl;
        master.changeAssignedStatus(userInput::choice(amountOfQuiz) - 1);
    }

    void resetScore(std::vector<Quiz> quizzes){
        int amountOfQuiz = master.getQuizzesCount();

        std::cout << std::string(LINE_LENGHT, '=') << std::endl;
        std::cout << "RESET SCORE" << std::endl;

        Quiz::displayQuiz(quizzes, 0, {-1, 100});

        std::cout << std::string(LINE_LENGHT, '-') << std::endl;
        std::cout << "Select Quiz's score to reset:" << std::endl;
        master.resetQuizScore(userInput::choice(amountOfQuiz) - 1);

    }

    void openQuizList(){
        std::vector<std::string> quizFileToDelete;

        while(true){    
            std::vector<Quiz> & quizVec = master.getQuizzes();    

            std::cout << std::string(LINE_LENGHT, '=') << std::endl;
            std::cout << "QUIZ LIST" << std::endl;

            Quiz::displayQuiz(quizVec, 0, {-1, 100});

            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "Select Action: " << std::endl;
            std::cout << "(1) Open Quiz" << std::endl;
            std::cout << "(2) Create Quiz" << std::endl;
            std::cout << "(3) Delete Quiz" << std::endl;
            std::cout << "(4) Assign/Unassign Quiz" << std::endl;
            std::cout << "(5) Reset Score on a Quiz" << std::endl;
            std::cout << "(6) Save & Back to menu" << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;

            switch(userInput::choice(6)){
                case 1:
                    openQuizExact(quizVec);
                    break;
                case 2:
                    createQuiz(quizVec);
                    break;
                case 3: {
                    quizFileToDelete.push_back(deleteQuiz(quizVec));
                    break;
                }
                case 4:
                    changeAssignedStatus(quizVec);
                    break;
                case 5:
                    resetScore(quizVec);
                    break;
                case 6:
                    master.saveQuizFile();
                    deleteFiles(quizFileToDelete);
                    return;
                    break;
                
                default:
                    break;
            }
        }
        
    }
 

    // START MENUS
    void showQuizzes(){
        int assignedFilter = 0; // 0 = NONE, 1 = Unassigned, 2 = Assigned
        std::array<int, 2> rangeFilter = {-1, 100};
        std::vector<Quiz> quizVec = master.getQuizzes();

        while(true){    
            std::cout << std::string(LINE_LENGHT, '=') << std::endl;
            std::cout << "QUIZ LIST" << std::endl;
            std::cout << "Filter:" << std::endl;
            std::cout << "Assigned Value: " << assignedFilter - 1 << std::endl;
            std::cout << "Score Range: " << rangeFilter[0] << " to " << rangeFilter[1] << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;

            Quiz::displayQuiz(quizVec, assignedFilter, rangeFilter);

            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "Select Action: " << std::endl;
            std::cout << "(1) Reset Filter" << std::endl;
            std::cout << "(2) Filter Assigned Value" << std::endl;
            std::cout << "(3) Filter Score" << std::endl;
            std::cout << "(4) Back to start menu" << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;

            switch (userInput::choice(4)){
                case 1:
                    assignedFilter = 0;
                    rangeFilter = {-1, 100};
                    break;
                case 2:
                    std::cout << "Select mode:" << std::endl;
                    std::cout << "(1) Show all" << std::endl;
                    std::cout << "(2) Show only NOT Assigned" << std::endl;
                    std::cout << "(3) Show only Assigned" << std::endl;
                    assignedFilter = userInput::choice(3) - 1;
                    break;
                case 3:
                    std::cout << "Input minimal value" << std::endl;
                    rangeFilter[0] = userInput::choice(100);
                    std::cout << "Input maximum value" << std::endl;
                    rangeFilter[1] = userInput::choice(100);
                    break;
                case 4:
                    return;
                    break;
                default:
                    break;
            }

        }
    }

    void start(){
        if(!teacherMode::login()){
            return;
        }

        isTeacherMode = true;        
        while (true){
            std::cout << std::string(LINE_LENGHT, '=') << std::endl;
            std::cout << "TEACHER MODE - START MENU" << std::endl;
            std::cout << "Welcome teacher to the pseudo quizziz!" << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "Select Action: " << std::endl;
            std::cout << "(1) Edit Quizzes" << std::endl;
            std::cout << "(2) Show Quizzes" << std::endl;
            std::cout << "(3) Back to start menu" << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;

            switch (userInput::choice(3)){
            case 1:
                openQuizList();
                break;
            case 2:
                showQuizzes();
                break;
            case 3:
                return;
                break;
            default:
                break;
            }
        }
    }

}

namespace studentMode{
    void selectQuiz(){
        while (true)
        {
            // Get quiz
            std::vector<Quiz> & quizVec = master.getQuizzes();

            // Get All assigned quiz as pointer
            std::vector<Quiz*> quizPtrVec;
            for(Quiz &quiz : quizVec){
                quizPtrVec.push_back(&quiz);
            }
            

            std::vector<Quiz*> quizPtrVec_filtered;
            quizPtrVec_filtered = Quiz::filterAssignedQuiz_ptr(quizPtrVec);
            int amountOfQuiz = quizPtrVec_filtered.size();

            // Output display begin
            std::cout << std::string(LINE_LENGHT, '=') << std::endl;
            std::cout << "BEGIN QUIZ" << std::endl;

            Quiz::displayQuiz(quizVec, 2, {-1, 100}); // Show only assigned quiz
            
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "Select Quiz (" << amountOfQuiz + 1 << " to cancel)"  << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;

            // Input checker
            int input = userInput::choice(amountOfQuiz + 1);
            if(input == amountOfQuiz + 1){ 
                std::cout << "Back to main menu..." << std::endl;
                return; // Exit to main menu
            } 

            // Check if the quiz hasn't yet started
            Quiz *selectedQuiz = quizPtrVec_filtered[input - 1];
            if (selectedQuiz->getScore() > -1)
            {
                std::cout << "Someone already finished this quiz!" << std::endl;
                std::cout << "Ask your teacher to reset the score if you want to redo it" << std::endl;
                continue;
            }
            
            // Answering the quiz
            int correctAnswerCount = master.beginAnswering(selectedQuiz);

            // Score management
            float score = static_cast<float>(correctAnswerCount) / static_cast<float>(selectedQuiz->getQuestionsCount()) * 100.0;
            if(score == 0){
                score++;
            }
            selectedQuiz->changeScore(static_cast<int>(score));
            std::cout << "Your score: " << selectedQuiz->getScore() << std::endl;
        }
    }

    void start(){
        isTeacherMode = false;

        while (true)
        {
            std::cout << std::string(LINE_LENGHT, '=') << std::endl;
            std::cout << "STUDENT MODE - START MENU" << std::endl;
            std::cout << "Welcome to the pseudo quizziz!" << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;
            std::cout << "Select Action: " << std::endl;
            std::cout << "(1) Start Quiz" << std::endl;
            std::cout << "(2) Save & Back to start menu" << std::endl;
            std::cout << std::string(LINE_LENGHT, '-') << std::endl;

            switch (userInput::choice(3))
            {
            case 1:
                selectQuiz();
                break;
            case 2:
                master.saveQuizFile();
                return;
                break;
            default:
                break;
            }
        }
        
    }
}


void mainMenu(){

    while (true){
        std::cout << std::string(LINE_LENGHT, '=') << std::endl;
        std::cout << "START MENU" << std::endl;
        std::cout << "Welcome to pseudo quizziz" << std::endl;
        std::cout << std::string(LINE_LENGHT, '-') << std::endl;
        std::cout << "Select Account to continue:" << std::endl;
        std::cout << "(1) Teacher Account" << std::endl;
        std::cout << "(2) Student Account" << std::endl;
        std::cout << "(3) Exit Program" << std::endl;
        std::cout << std::string(LINE_LENGHT, '-') << std::endl;


        switch (userInput::choice(3))
        {
        case 1:
            teacherMode::start();
            break;
        case 2:
            studentMode::start();
            break;
        case 3:
            std::cout << "Exiting program..." << std::endl;
            return;
            break;

        default:
            std::cout << "Something(s) wrong" << std::endl;
            break;
        }
    }
}

int main(){
    startAplication();
    mainMenu();
    
    getch();
}