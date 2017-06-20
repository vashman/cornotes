#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>

using std::ifstream;
using std::ofstream;
using std::string;
using std::istream_iterator;
using std::ostream_iterator;

class line : public string { 

friend std::istream & operator >> (std::istream & is, line & _line) {   
return std::getline(is, _line);
}
}; /* line */

unsigned int header_count (string const & _str){
unsigned int temp = 0;
  while (_str[temp] == '#') ++temp;
return temp;
}

int main() {

unsigned int head_count = 0;
bool in_note_section = false;
ofstream output("study.html");
output << "<!DOCTYPE html><html><head>";
{
ifstream head("head.html");
copy(istream_iterator<line>(head), istream_iterator<line>(), ostream_iterator<line>(output));
}
output << "</head><body><input type=\"button\" value=\"hide notes\" onclick=\"hideNotes()\">";

ifstream question_file("question_file.md");
ifstream note_file("note_file.md");

string section_heading = "";
auto question = istream_iterator<line>(question_file);

for (
  auto note = istream_iterator<line>(note_file), end  = istream_iterator<line>()
; note != end
; std::advance(note, 1)
){

  // Output section heading
  if ((*note)[0] == '#') {
    if (in_note_section){
    in_note_section = false; // start new section
    output << "</table>";
    }
  unsigned int h_count = header_count(*note);
  ++head_count;
  output << "<h" << h_count << ">";
    if (h_count == 1) output << head_count << ". ";
  output << (*note).c_str() <<  "</h" << h_count << ">";
  section_heading = *note;
    if (section_heading.compare(*question) != 0) return 1;// mis-match sections
  std::advance(question, 1);
  continue;
  }

  if (! in_note_section){ // start new note section
  in_note_section = true;
  output << "<table><tr><th>Notes</th><th>Question</th></tr>";
  }

// output note
  if ((*note).size() == 0){
  output << "<tr class=\"note\"><td> </td>";
  continue; // ignore blank lines for notes.
  }
output << "<tr class=\"note\"><td>" << (*note).c_str() << "</td>";

unsigned int question_count = 0;
// output questions
  while (question != end){

    // cant check above cause its UB to (*) defrence a past the end iterator.
    if ((*question)[0] == '#') break; // no questions left for section, keep skipping.

    if ((*question).size() == 0){
    std::advance(question, 1);
    break; // get next note
    }

  // Output question
    if (question_count == 0) output << "<td class=\"question\">" << (*question).c_str() << "</td></tr>";
    else output << "<tr><td></td><td class=\"question\">" << (*question).c_str() << "</td></tr>";
  ++question_count;
  std::advance(question, 1);
  }

  if (question_count == 0) output << "<td class=\"question\"></td></tr>"; // end the row
}

output << "</body></html>";

return 0;
}
