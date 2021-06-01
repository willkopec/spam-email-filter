#include <iostream> //iostream
#include <fstream> //file stream package
#include <string> //included cstring package
#include "ourvector.h" //included header file for vector

using namespace std;

//Methods/Functions

// Custom Functions //
//turnIntoEmailFormat (spamListUsers, spamListDomains)
//Purpose: uses spamListUsers and spamListDomains vectors (in load function) and combines them to be in user@email.com format then adds them to spamListEmails (in main, passed by reference)
void turnIntoEmailFormat(ourvector<string> spamListUsers, ourvector<string> spamListDomains, ourvector<string> &emails){
  string email = "";
  for(int i = 0; i < spamListUsers.size(); i++){
    email = spamListUsers.at(i) + "@" + spamListDomains.at(i);
    emails.push_back(email);
  }
}

// SortVector Function //
// Sorts (the vector passed by reference) alphabetically //
void sortVector (ourvector<string> &vector){
  for(int j = 0; j < vector.size() - 1; j++){
    for(int i = 0; i < vector.size() - 1; i++){
      if(vector[i] > vector[i + 1]){
        string tempEmail;
        tempEmail = vector[i+1];
        vector[i+1] = vector[i];
        vector[i] = tempEmail;
      }
    }
  }
}

// BinarySearch //
// Checks through the email list (called in parameters) and checks the email (called in parameters) to see if the email exists in the email list
bool BinarySearch(ourvector<string> emailList, string spamEmail){
  int mid = 0;
  int low = 0;
  int high = emailList.size() - 1;

  while(low <= high){
    mid = (low + (high - low) / 2);
    string A = emailList[mid];
        if (A == spamEmail) {
            return true;
        } else if (spamEmail < A) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
  }
  return false;
}

// Check function //
// Simple boolean function that returns true or false, checks if emailSearched exists in emailList or blackListedDomains to see whether or not it is spam //
bool check(string emailSearched, ourvector<string> emailList, ourvector<string> blackListedDomains){
  if(BinarySearch(emailList, emailSearched) || BinarySearch(blackListedDomains, emailSearched.substr(emailSearched.find("@") + 1))){
    return true;
  } else {
    return false;
  }
}
// Load function //
// Loads the spam file, creates two vectors (spamListUsers and spamListDomains) and uses the turnIntoEmailFormat function to combine them and put them in the proper format //
void load (string spamListFileName, ourvector<string> &spamListEmails, ourvector<string> &allSpamDomains){
    fstream spamList;
    spamList.open (spamListFileName);
    ourvector<string> spamListUsers;
    ourvector<string> spamListDomains;
    int i = 0;

      if (spamList.is_open()) {
        cout << "Loading '" << spamListFileName << "'\n";
      } else {
        cout << "**Error, unable to open '" << spamListFileName << "'" << endl;
      }
      
      if (spamList.is_open()) {
        string line;
        while ( getline(spamList,line) ) {
          i++;
          string domain = "";
          string userName = "";
            for(unsigned int i = line.find_last_of(":")+1; i < line.size(); i++){
              userName += line[i];
            }
            spamListUsers.push_back(userName);

            for(unsigned int i = 0; i < line.find_last_of(":"); i++){
              domain += line[i];
            }
            spamListDomains.push_back(domain);
            if(userName == "*"){
              allSpamDomains.push_back(domain);
            }
            sortVector(allSpamDomains);
        }
        turnIntoEmailFormat(spamListUsers, spamListDomains, spamListEmails);
        sortVector(spamListEmails);
        spamList.close();
        spamListUsers.clear();
        spamListDomains.clear();
        cout << "# of spam entries: " << i << "\n";
      }
}
// Display function //
// Displays the current spamlist open //
void display(string fileName){
  fstream myfile;
  myfile.open (fileName);
  string line;
  while ( getline(myfile,line) ) {
        cout << line << "\n";
  }
  myfile.close();
}

// Filter function //
// Filters an email file with the current spamlist loaded and outputs all non spam emails into output file//
void filter(string emailsListFileName, string outputFileName, ourvector<string> &spamListEmails, ourvector<string> &blackListedDomains){
  fstream emailsList;
  emailsList.open (emailsListFileName);

  ofstream outFile;

  int emailsChecked = 0;
  int nonSpamEmails = 0;

  string line;
  
  if(emailsList.is_open()){
    outFile.open (outputFileName);
    //read every line of emailsList
    while (getline(emailsList,line)) {
    //find  email substring in each line of file
    int atPos = line.find("@");
    int email_start = 0;
    int email_end = line.size();
    
    for(int i = atPos; i > email_start; i--){
      if(line[i] == ' '){
        email_start = i+1;
      }
    } 
    for(int i = atPos; i <= email_end; i++){
      if(line[i] == ' '){
        email_end = i;
      }
    }
    //store found email into entire email and domain substrings
    string email =  line.substr(email_start, email_end-email_start);
    string domain = line.substr(atPos+1, email_end-atPos-1);
   
    //check if found email or domain substring is apart of blacklist
    if(BinarySearch(spamListEmails, email)){
        //spam email
    } else if (BinarySearch(blackListedDomains, domain)){
        //spam domain 
    } else {
        //allowed email
        nonSpamEmails++;
        outFile << line << endl;
    }
   
    //emails checked counter
    emailsChecked++;
  }
  //filter stats
  cout << "# emails processed: " << emailsChecked << endl;
  cout << "# non-spam emails: " << nonSpamEmails << endl;
  //close the files
  emailsList.close();
  outFile.close(); 
  } else { //else file doesnt exist
    cout << "**Error, unable to open '" << emailsListFileName << "'" << endl;
  }
  
    
      
}

//  **MAIN**    //
int main() {
  string fileName; //spamFile name
  ourvector<string> spamListEmails; //formatted spam emails
  ourvector<string> blackListedDomains; //black listed domains (*)
  cout << "** Welcome to spam filtering app **\n";

  while(true){
    //output user menu
    cout << "\nEnter command or # to exit> ";
    string userInput;
    cin >> userInput;

    //User Commands:
    //Load
    if(userInput == "load"){
      if(spamListEmails.size() != 0 || blackListedDomains.size() != 0){
        //clear previous vectors if there are any
        spamListEmails.clear();
        blackListedDomains.clear();
      }
      cin >> fileName;
      load(fileName, spamListEmails, blackListedDomains);
    }
    //Display
    else if(userInput == "display"){
      display(fileName);
    }
    //Check
    else if(userInput == "check"){
      string email = "";
      cin >> email; 
      //if bool check is true print out "email is spam"
      if(check(email, spamListEmails, blackListedDomains)){
        cout << email << " is spam\n";
      } else { // else "email is not spam"
        cout << email << " is not spam\n";
      }
    }
    //Filter
    else if(userInput == "filter"){ 
      string fileNameToSearch;
      string newFileName;
      cin >> fileNameToSearch;
      cin >> newFileName;
      filter(fileNameToSearch, newFileName, spamListEmails, blackListedDomains);
    }
    //Exit
    else if(userInput == "#"){
      exit(0);

    } else { // else not a valid command
      cout << "**invalid command\n";
    }


  }

}