[Languages::] Programming Languages.

@Purpose: To characterise the relevant differences in behaviour between the
various programming languages supported.

@p Languages.
The conventions for writing, weaving and tangling a web are really quite
independent of the programming language being written, woven or tangled;
Knuth began literate programming with Pascal, but now uses C, and the original
Pascal webs were mechanically translated into C ones with remarkably little
fuss or bother. Modern LP tools, such as |noweb|, aim to be language-agnostic.
But of course if you act the same on all languages, you give up the benefits
which might follow from knowing something about the languages you actually
write in.

The idea, then, is that Chapters 1 to 3 of the Inweb code treat all
material the same, and Chapter 4 contains all of the funny little exceptions
and special cases for particular programming languages. (This means Chapter 4
can't be understood without having at least browsed Chapters 1 to 3 first.)

Each language supported by Inweb has an instance of the following structure:

@c
typedef struct programming_language {
	char *language_name;
	char *file_extension; /* by default output to a file whose name has this extension */
	char *source_file_extension; /* by default input from a file whose name has this extension */
	char *shebang; /* compulsory content on line 1 */
	int c_like; /* does this belong to the C family of languages? */
	MEMORY_MANAGEMENT
} programming_language;

@ And these are the languages supported:

@c
programming_language *C_FOR_INFORM_LANGUAGE = NULL;
programming_language *C_LANGUAGE = NULL;
programming_language *CPP_LANGUAGE = NULL; /* never actually tested */
programming_language *I6_LANGUAGE = NULL;
programming_language *I7_LANGUAGE = NULL;
programming_language *PERL_LANGUAGE = NULL; /* Perl 5, that is, though 6 may also work */
programming_language *PLAIN_LANGUAGE = NULL;

programming_language *NO_LANGUAGE = NULL; /* a dummy language for error recovery */

programming_language *Languages::default(void) {
	return C_LANGUAGE;
}

@p Creation.
This must be performed very early in Inweb's run.

@c
void Languages::create_programming_languages(void) {
	C_FOR_INFORM_LANGUAGE = Languages::new_language("C for Inform", ".c");
	C_FOR_INFORM_LANGUAGE->c_like = TRUE;
	C_LANGUAGE = Languages::new_language("C", ".c");
	C_LANGUAGE->c_like = TRUE;
	CPP_LANGUAGE = Languages::new_language("C++", ".cpp");
	CPP_LANGUAGE->c_like = TRUE;
	I6_LANGUAGE = Languages::new_language("Inform 6", ".i6");
	I6_LANGUAGE->source_file_extension = ".i6t";
	I7_LANGUAGE = Languages::new_language("Inform 7", ".i7x");
	PERL_LANGUAGE = Languages::new_language("Perl", ".pl");
	PERL_LANGUAGE->shebang = "#!/usr/bin/perl\n\n";
	PLAIN_LANGUAGE = Languages::new_language("Plain Text", ".txt");
	/* something of an exception: */
	NO_LANGUAGE = Languages::new_language("None", "");
}

@ Here we create from a given name...

@c
programming_language *Languages::new_language(char *name, char *ext) {
	programming_language *pl = CREATE(programming_language);
	pl->language_name = name; pl->file_extension = ext;
	pl->shebang = ""; pl->c_like = FALSE;
	pl->source_file_extension = ".w";
	return pl;
}

@ ...and here we parse one:

@c
programming_language *Languages::language_with_name(char *lname) {
	programming_language *pl;
	LOOP_OVER(pl, programming_language)
		if (CStrings::eq(lname, pl->language_name))
			return pl;
	Errors::fatal_with_C_string("unsupported programming language '%s'", lname);
	return NULL;
}

@p Bibliographic extras.

@c
int Languages::special_data(OUTPUT_STREAM, programming_language *pl, char *data) {
	if (pl == C_FOR_INFORM_LANGUAGE) return CForInform::special_data(OUT, data);
	return FALSE;
}

@p Parsing extras.
We have already parsed the web into chapters, sections and paragraphs, but
for some languages we will need a more detailed picture -- for example,
detecting all C function declarations. These hooks provide for that:

@c
void Languages::further_parsing(web *W, programming_language *pl) {
	if (pl->c_like) CLike::further_parsing(W);
	if (pl == C_FOR_INFORM_LANGUAGE) CForInform::further_parsing(W);
}

void Languages::subcategorise_code(programming_language *pl, source_line *L) {
	if (pl->c_like) CLike::subcategorise_code(pl, L);
}

@p Tangling extras.
Not all languages tangle, in principle at least:

@c
int Languages::tangles(programming_language *pl) {
	if (pl == NO_LANGUAGE) return FALSE;
	return TRUE;
}

@ The top of the tangled file is a header called the "shebang":

@c
void Languages::shebang(OUTPUT_STREAM, programming_language *pl, web *W, tangle_target *target) {
	WRITE("%s", pl->shebang);
	if (pl != I7_LANGUAGE)
		Languages::comment(OUT, pl, "Tangled output generated by inweb-C: do not edit");
	if (pl->c_like) CLike::shebang(OUT, pl, W, target);
}

@ Then we have some "definitions". The following routines handle the |@d|
escape, writing a definition of the constant named |term| as the value given.
If the value spans multiple lines, the first-line part is supplied to
|Languages::start_definition| and then subsequent lines are fed in order to
|Languages::prolong_definition|. At the end, |Languages::end_definition| is
called.

@c
void Languages::start_definition(OUTPUT_STREAM, programming_language *pl,
	char *term, char *start, section *S, source_line *L) {
	if (pl->c_like) {
		WRITE("#define %s ", term);
		Tangler::tangle_code(OUT, start, S, L);
	} else if (pl == PERL_LANGUAGE) {
		WRITE("%s = ", term);
		Tangler::tangle_code(OUT, start, S, L);
	} else Main::error_in_web("this programming language does not support @d", L);
}

void Languages::prolong_definition(OUTPUT_STREAM, programming_language *pl,
	char *more, section *S, source_line *L) {
	if (pl->c_like) {
		WRITE("\\\n    ");
		Tangler::tangle_code(OUT, more, S, L);
	} else Main::error_in_web("this programming language does not support multiline @d", L);
}

void Languages::end_definition(OUTPUT_STREAM, programming_language *pl,
	section *S, source_line *L) {
	if (pl->c_like) WRITE("\n");
	else if (pl == PERL_LANGUAGE) WRITE("\n;\n");
}

@ Then we have some "predeclarations"; for example, for C-like languages we
automatically predeclare all functions, obviating the need for header files.

@c
void Languages::additional_predeclarations(OUTPUT_STREAM, programming_language *pl, web *W) {
	if (pl->c_like) CLike::additional_predeclarations(OUT, pl, W);
	if (pl == C_FOR_INFORM_LANGUAGE) CForInform::additional_predeclarations(OUT, W);
}

@ The following routines make it possible for languages to tangle unorthodox
lines into code. Ordinarily, only |CODE_BODY_LCAT| lines are tangled, but
we can intervene to say that we want to tangle a different line; and if we
do so, we should then act on that basis.

@c
int Languages::will_insert_in_tangle(programming_language *pl, source_line *L) {
	if (pl == C_FOR_INFORM_LANGUAGE) return CForInform::will_insert_in_tangle(L);
	return FALSE;
}

void Languages::insert_in_tangle(OUTPUT_STREAM, programming_language *pl, source_line *L) {
	if (pl == C_FOR_INFORM_LANGUAGE) CForInform::insert_in_tangle(OUT, L);
}

@ In order for C compilers to report C syntax errors on the correct line,
despite rearranging by automatic tools, C conventionally recognises the
preprocessor directive |#line| to tell it that a contiguous extract follows
from the given file; we generate this automatically. In its usual zany way,
Perl recognises the exact same syntax, thus in principle overloading its
comment notation |#|.

@c
void Languages::insert_line_marker(OUTPUT_STREAM, programming_language *pl, source_line *L) {
	if ((pl->c_like) || (pl == PERL_LANGUAGE)) CLike::insert_line_marker(OUT, pl, L);
}

@ The following hooks are provided so that we can top and/or tail the expansion
of CWEB macros in the code. For C-like languages, we place macro bodies inside
braces |{| and |}|, and since Perl shares the same syntax:

@c
void Languages::before_macro_expansion(OUTPUT_STREAM, programming_language *pl, cweb_macro *cwm) {
	if ((pl->c_like) || (pl == PERL_LANGUAGE)) CLike::before_macro_expansion(OUT, pl, cwm);
}

void Languages::after_macro_expansion(OUTPUT_STREAM, programming_language *pl, cweb_macro *cwm) {
	if ((pl->c_like) || (pl == PERL_LANGUAGE)) CLike::after_macro_expansion(OUT, pl, cwm);
}

@ Now a routine to write a comment. Languages without comment should write nothing.

@c
void Languages::comment(OUTPUT_STREAM, programming_language *pl, char *comm) {
	if (pl->c_like) CLike::comment(OUT, pl, comm);
	if (pl == PERL_LANGUAGE) WRITE("# %s\n", comm);
	if (pl == I6_LANGUAGE) WRITE("! %s\n", comm);
	if (pl == I7_LANGUAGE) WRITE("[%s]\n", comm);
}

@ And a similar routine for parsing one.

@c
int Languages::parse_comment(programming_language *pl,
	char *line, char *part_before_comment, char *part_within_comment) {
	if (pl->c_like)
		return CLike::parse_comment(pl, line, part_before_comment, part_within_comment);
	if (pl == PERL_LANGUAGE) {
		string found_text1;
		string found_text2;
		if (ISORegexp::match_1(line, "# (%c*?) *", found_text1)) {
			CStrings::copy(part_before_comment, "");
			CStrings::copy(part_within_comment, found_text1);
			return TRUE;
		}
		if (ISORegexp::match_2(line, "(%c*) # (%c*?) *", found_text1, found_text2)) {
			CStrings::copy(part_before_comment, found_text1);
			CStrings::copy(part_within_comment, found_text2);
			return TRUE;
		}
	}
	return FALSE;
}

@ The inner code tangler now acts on all code known not to contain CWEB
macros or double-square substitutions. In almost every language this simply
passes the code straight through, printing |original| to |OUT|.

@c
void Languages::tangle_code(OUTPUT_STREAM, programming_language *pl, char *original) {
	if (pl == C_FOR_INFORM_LANGUAGE) CForInform::tangle_code(OUT, original);
	else WRITE("%s", original);
}

@ The bottom of the tangled file is a footer called the "gnabehs":

@c
void Languages::gnabehs(OUTPUT_STREAM, programming_language *pl, web *W) {
	if (pl == C_FOR_INFORM_LANGUAGE) CForInform::gnabehs(OUT, W);
}

@p Additional tangling.
This provides for any auxiliary files, besides the main code file being
tangled, which need to be written.

@c
void Languages::additional_tangling(programming_language *pl, web *W, tangle_target *target) {
	if (pl == C_FOR_INFORM_LANGUAGE)
		CForInform::additional_tangling(pl, W, target);
}

@p Weaving.

@c
void Languages::begin_weave(section *S, weave_target *wv) {
	if (S->sect_language->c_like) CLike::begin_weave(S, wv);
	if (S->sect_language == C_FOR_INFORM_LANGUAGE) CForInform::begin_weave(S, wv);
}

@

@c
void Languages::new_tag_declared(theme_tag *tag) {
	CForInform::new_tag_declared(tag);
}

int Languages::skip_in_weaving(programming_language *pl, weave_target *wv, source_line *L) {
	if (pl == C_FOR_INFORM_LANGUAGE) return CForInform::skip_in_weaving(wv, L);
	return FALSE;
}

int Languages::syntax_colour(OUTPUT_STREAM, programming_language *pl, weave_target *wv,
	web *W, chapter *C, section *S, source_line *L, char *matter, char *colouring) {
	CStrings::copy(colouring, matter);
	for (int i=0; matter[i]; i++) colouring[i] = PLAIN_CODE;
	if (pl->c_like)
		if (CLike::syntax_colour(OUT, pl, wv, W, C, S, L, matter, colouring))
			return TRUE;
	if (pl == C_FOR_INFORM_LANGUAGE)
		if (CForInform::syntax_colour(OUT, wv, W, C, S, L, matter, colouring))
			return TRUE;
	return FALSE;
}

int Languages::weave_code_line(OUTPUT_STREAM, programming_language *pl, weave_target *wv,
	web *W, chapter *C, section *S, source_line *L, char *matter, char *concluding_comment) {
	if (pl->c_like)
		if (CLike::weave_code_line(OUT, pl, wv, W, C, S, L, matter, concluding_comment))
			return TRUE;
	if (pl == C_FOR_INFORM_LANGUAGE)
		if (CForInform::weave_code_line(OUT, wv, W, C, S, L, matter, concluding_comment))
			return TRUE;
	return FALSE;
}

@p Analysis.

@c
void Languages::analysis(programming_language *pl, section *S, int functions_too) {
	if (pl->c_like) CLike::analysis(pl, S, functions_too);	
}

void Languages::analyse_code(programming_language *pl, web *W) {
	if (pl->c_like) CLike::analyse_code(pl, W);	
	if (pl == C_FOR_INFORM_LANGUAGE) CForInform::analyse_code(pl, W);
}

void Languages::post_analysis(programming_language *pl, web *W) {
	if (pl->c_like) CLike::post_analysis(pl, W);	
}