#include <fstream>
#include <string>

#include <range_layer/range.hpp>
#include <range_layer/algorithm.hpp>
#include <range_layer/array_range.hpp>
#include <range_layer/string.hpp>
#include <range_layer/stream.hpp>

using std::ifstream;
using std::ofstream;
using std::string;

using range_layer::range;
using range_layer::write;
using range_layer::read;
using range_layer::execution_policy::sequenced;
using range_layer::advance;
using range_layer::sub_range;
using range_layer::has_readable;
using range_layer::has_writable;
using range_layer::back_insert;

string
  html_head {"<!DOCTYPE html><html><head>"}
, html_foot {"</body></html>"}
, html_empty_note {"<tr class=\"note\"><td> </td>"}
, html_qcell_end {"</td></tr>"}
, html_qcell_begin {"<td class=\"question\">"}
, html_qrow_begin {"<tr><td></td><td class=\"question\">"}
, html_qrow_end {"<td class=\"question\"></td></tr>"}
, html_cell_end {"</td>"}
, html_table_end {"</table>"}
, html_nrow_begin {"<tr class=\"note\"><td>"}

, html_body_head {
  "</head><body><input type=\"button\" value=\"hide"
  " notes\" onclick=\"hideNotes()\">"
  }

, html_table_head
  {"<table><tr><th>Notes</th><th>Question</th></tr>"};

template <typename Range, typename T>
int count_until (
  Range _range
, T const & _pred
){
typename range_layer::range_trait::size_type<Range>::type n = 0;

  for (; has_readable(_range); advance(_range)){
    if (_pred == read(_range)) ++n;
    else break;
  }
return n;
}

int main() {

sequenced exe_policy{};
unsigned int head_count {0};
bool in_note_section = false;
ofstream output_file("study.html");
auto output = range(output_file);
{
ifstream input{"head.html"};
write (
  exe_policy
, output
, range (html_head)
, range (input)
, range (html_body_head)
);
}
ifstream question_file("question_file.md");
ifstream note_file("note_file.md");

string section_heading = "";
auto question = range(question_file);
string temp_question;

for (
  auto note = range(note_file)
; has_readable(note)
; advance(note)
){
char temp = read(note);

  // Output section heading
  if (temp == '#') {
    if (in_note_section){
    in_note_section = false; // start new section
    write (exe_policy, output, range(html_table_end));
    }
  auto h_count = 1+count_until(note, '#');
  ++head_count;
  write (exe_policy, output, range("<h", 2), h_count, range(">", 1));

    if (h_count == 1)
    write(exe_policy, output, head_count, range(". ", 2));

  write (
    exe_policy
  , output
  , sub_range(note, '\n')
  , range("</h", 3), h_count, range(">", 1)
  );

  write (
    exe_policy
  , back_insert(range(section_heading))
  , sub_range(note, '\n')
  );

  write (
    exe_policy
  , back_insert(range(temp_question))
  , sub_range(question, '\n')
  );
    if (section_heading.compare(temp_question) != 0)
    return 1;// mis-match sections
  advance(question);
  continue;
  }

  if (! in_note_section){ // start new note section
  in_note_section = true;
  write (exe_policy, output, range(html_table_head));
  }

// output note
string temp_note;
  write (
    exe_policy
  , back_insert(range(temp_note))
  , sub_range(note, '\n')
  );

  if (temp_note.size() == 0){
  write (exe_policy, output, range(html_empty_note));
  continue; // ignore blank lines for notes.
  }
write (
  exe_policy
, output
, range(html_nrow_begin)
, range(temp_note)
, range(html_cell_end)
);

unsigned int question_count = 0;
// output questions
  while (has_readable(question)){
  write (
    exe_policy
  , back_insert(range(temp_question))
  , sub_range(question, '\n')
  );

    // cant check above cause its UB to (*) defrence a past the end iterator.
    // no questions left for section, keep skipping.
    if (temp_question[0] == '#') break;

    if (temp_question.size() == 0){
    advance(question);
    break; // get next note
    }

  // Output question
    if (question_count == 0){
    write (
      exe_policy
    , output
    , range(html_qcell_begin)
    , read(question)
    , range(html_qcell_end)
    );
    } else {
    write (
      exe_policy
    , output
    , range(html_qrow_begin)
    , read(question)
    , range(html_qcell_end)
    );
    }
  ++question_count;
  advance(question);
  }

  if (question_count == 0){// end the row
  write (
    exe_policy
  , output
  , range(html_qrow_end)
  );
  }
}

write (exe_policy, output, range(html_foot));
return 0;
}
