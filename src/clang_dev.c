#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <clang-c/Index.h>
// #include <clang-c/CXCompilationDatabase.h>


const char *clang_getCompletionChunkKindSpelling(enum CXCompletionChunkKind Kind)
{
    switch (Kind)
    {
        case CXCompletionChunk_Optional: return "Optional";
        case CXCompletionChunk_TypedText: return "TypedText";
        case CXCompletionChunk_Text: return "Text";
        case CXCompletionChunk_Placeholder: return "Placeholder";
        case CXCompletionChunk_Informative: return "Informative";
        case CXCompletionChunk_CurrentParameter: return "CurrentParameter";
        case CXCompletionChunk_LeftParen: return "LeftParen";
        case CXCompletionChunk_RightParen: return "RightParen";
        case CXCompletionChunk_LeftBracket: return "LeftBracket";
        case CXCompletionChunk_RightBracket: return "RightBracket";
        case CXCompletionChunk_LeftBrace: return "LeftBrace";
        case CXCompletionChunk_RightBrace: return "RightBrace";
        case CXCompletionChunk_LeftAngle: return "LeftAngle";
        case CXCompletionChunk_RightAngle: return "RightAngle";
        case CXCompletionChunk_Comma: return "Comma";
        case CXCompletionChunk_ResultType: return "ResultType";
        case CXCompletionChunk_Colon: return "Colon";
        case CXCompletionChunk_SemiColon: return "SemiColon";
        case CXCompletionChunk_Equal: return "Equal";
        case CXCompletionChunk_HorizontalSpace: return "HorizontalSpace";
        case CXCompletionChunk_VerticalSpace: return "VerticalSpace";
    }

    return "Unknown";
}


void print_completion_contexts(unsigned long long contexts, FILE *file)
{
  fprintf(file, "Completion contexts:\n");
  if (contexts == CXCompletionContext_Unknown) {
    fprintf(file, "Unknown\n");
    return;
  }
  if (contexts & CXCompletionContext_AnyType) {
    fprintf(file, "Any type\n");
  }
  if (contexts & CXCompletionContext_AnyValue) {
    fprintf(file, "Any value\n");
  }
  if (contexts & CXCompletionContext_ObjCObjectValue) {
    fprintf(file, "Objective-C object value\n");
  }
  if (contexts & CXCompletionContext_ObjCSelectorValue) {
    fprintf(file, "Objective-C selector value\n");
  }
  if (contexts & CXCompletionContext_CXXClassTypeValue) {
    fprintf(file, "C++ class type value\n");
  }
  if (contexts & CXCompletionContext_DotMemberAccess) {
    fprintf(file, "Dot member access\n");
  }
  if (contexts & CXCompletionContext_ArrowMemberAccess) {
    fprintf(file, "Arrow member access\n");
  }
  if (contexts & CXCompletionContext_ObjCPropertyAccess) {
    fprintf(file, "Objective-C property access\n");
  }
  if (contexts & CXCompletionContext_EnumTag) {
    fprintf(file, "Enum tag\n");
  }
  if (contexts & CXCompletionContext_UnionTag) {
    fprintf(file, "Union tag\n");
  }
  if (contexts & CXCompletionContext_StructTag) {
    fprintf(file, "Struct tag\n");
  }
  if (contexts & CXCompletionContext_ClassTag) {
    fprintf(file, "Class name\n");
  }
  if (contexts & CXCompletionContext_Namespace) {
    fprintf(file, "Namespace or namespace alias\n");
  }
  if (contexts & CXCompletionContext_NestedNameSpecifier) {
    fprintf(file, "Nested name specifier\n");
  }
  if (contexts & CXCompletionContext_ObjCInterface) {
    fprintf(file, "Objective-C interface\n");
  }
  if (contexts & CXCompletionContext_ObjCProtocol) {
    fprintf(file, "Objective-C protocol\n");
  }
  if (contexts & CXCompletionContext_ObjCCategory) {
    fprintf(file, "Objective-C category\n");
  }
  if (contexts & CXCompletionContext_ObjCInstanceMessage) {
    fprintf(file, "Objective-C instance method\n");
  }
  if (contexts & CXCompletionContext_ObjCClassMessage) {
    fprintf(file, "Objective-C class method\n");
  }
  if (contexts & CXCompletionContext_ObjCSelectorName) {
    fprintf(file, "Objective-C selector name\n");
  }
  if (contexts & CXCompletionContext_MacroName) {
    fprintf(file, "Macro name\n");
  }
  if (contexts & CXCompletionContext_NaturalLanguage) {
    fprintf(file, "Natural language\n");
  }
}


static void PrintExtent(FILE *out, unsigned begin_line, unsigned begin_column,
                        unsigned end_line, unsigned end_column) {
  fprintf(out, "[%d:%d - %d:%d]", begin_line, begin_column,
          end_line, end_column);
}


void PrintDiagnostic(CXDiagnostic Diagnostic) {
  FILE *out = stderr;
  CXFile file;
  CXString Msg;
  unsigned display_opts =
    CXDiagnostic_DisplaySourceLocation
    | CXDiagnostic_DisplayColumn
    | CXDiagnostic_DisplaySourceRanges
    | CXDiagnostic_DisplayOption
    | CXDiagnostic_DisplayCategoryId
    | CXDiagnostic_DisplayCategoryName;
  unsigned i, num_fixits;

  if (clang_getDiagnosticSeverity(Diagnostic) == CXDiagnostic_Ignored)
    return;

  Msg = clang_formatDiagnostic(Diagnostic, display_opts);
  fprintf(stderr, "%s\n", clang_getCString(Msg));
  clang_disposeString(Msg);

  clang_getSpellingLocation(clang_getDiagnosticLocation(Diagnostic),
                            &file, 0, 0, 0);
  if (!file)
    return;

  num_fixits = clang_getDiagnosticNumFixIts(Diagnostic);
  fprintf(stderr, "Number FIX-ITs = %d\n", num_fixits);
  for (i = 0; i != num_fixits; ++i) {
    CXSourceRange range;
    CXString insertion_text = clang_getDiagnosticFixIt(Diagnostic, i, &range);
    CXSourceLocation start = clang_getRangeStart(range);
    CXSourceLocation end = clang_getRangeEnd(range);
    unsigned start_line, start_column, end_line, end_column;
    CXFile start_file, end_file;
    clang_getSpellingLocation(start, &start_file, &start_line,
                              &start_column, 0);
    clang_getSpellingLocation(end, &end_file, &end_line, &end_column, 0);
    if (clang_equalLocations(start, end)) {
      /* Insertion. */
      if (start_file == file)
        fprintf(out, "FIX-IT: Insert \"%s\" at %d:%d\n",
                clang_getCString(insertion_text), start_line, start_column);
    } else if (strcmp(clang_getCString(insertion_text), "") == 0) {
      /* Removal. */
      if (start_file == file && end_file == file) {
        fprintf(out, "FIX-IT: Remove ");
        PrintExtent(out, start_line, start_column, end_line, end_column);
        fprintf(out, "\n");
      }
    } else {
      /* Replacement. */
      if (start_file == end_file) {
        fprintf(out, "FIX-IT: Replace ");
        PrintExtent(out, start_line, start_column, end_line, end_column);
        fprintf(out, " with \"%s\"\n", clang_getCString(insertion_text));
      }
      break;
    }
    clang_disposeString(insertion_text);
  }
}


int main(int argc, char const *argv[])
{
    char cmdLine[64];
    char fileName[64];
    CXCodeCompleteResults *results = NULL;
    CXTranslationUnit translationUnit = 0;
    unsigned long long contexts;
    unsigned completionOptions;
    unsigned parsingOptions;
    CXIndex index;
    unsigned i, n;
    int line, column;
    // int first = 1;

    // arguments
    if(argc < 2)
    {
        fprintf(stderr, "Missing arguments!\n");

        return EXIT_FAILURE;
    }

    // parsing
    parsingOptions = clang_defaultEditingTranslationUnitOptions();
    parsingOptions |= CXTranslationUnit_DetailedPreprocessingRecord;
    parsingOptions |= CXTranslationUnit_CacheCompletionResults;
    parsingOptions |= CXTranslationUnit_SkipFunctionBodies;
    parsingOptions |= CXTranslationUnit_IncludeBriefCommentsInCodeCompletion;

    index = clang_createIndex(0, 1);
    translationUnit = clang_parseTranslationUnit(
        index, NULL, (const char * const *)argv, argc,
        NULL, // unsaved files
        0, // nb unsaved files
        parsingOptions);

    if(translationUnit == NULL)
    {
        fprintf(stderr, "Unable to load translation unit!\n");
        return EXIT_FAILURE;
    }

    while(1)
    {
        int scanRes;

        printf("Choose where to perform autocompletion (file:line:column):\n> ");

        if(fgets(cmdLine, sizeof cmdLine, stdin) == NULL)
          break;

        scanRes = sscanf(cmdLine, "%[^:]:%d:%d", fileName, &line, &column);
        // printf("%s:%d:%d (%d)", fileName, line, column, scanRes);

        if(scanRes != 3)
          break;

        // updating parsing if files have been modified
        // if(!first)
        // {
        //     if(clang_reparseTranslationUnit(translationUnit, 0, 0, clang_defaultReparseOptions(translationUnit)))
        //     {
        //         fprintf(stderr, "Unable to reparse translation init!\n");
        //         return EXIT_FAILURE;
        //     }

        //     first = 0;
        // }

        // code completion
        completionOptions = clang_defaultCodeCompleteOptions();
        completionOptions |= CXCodeComplete_IncludeMacros;
        completionOptions |= CXCodeComplete_IncludeCodePatterns;
        completionOptions |= CXCodeComplete_IncludeBriefComments;

        results = clang_codeCompleteAt(
            translationUnit, "../test/test_a.c", line, column,
            NULL, 0, completionOptions);

        if(results == NULL)
        {
            fprintf(stderr, "No completion found!\n");
            return EXIT_FAILURE;
        }

        // alnum sort
        clang_sortCodeCompletionResults(results->Results, results->NumResults);

        for(i = 0; i < results->NumResults; i++)
        {
            CXCompletionResult *completionResult = results->Results + i;
            CXCompletionString *completionString = completionResult->CompletionString;

            if(completionResult->CursorKind != CXCursor_MacroDefinition
              && completionResult->CursorKind != CXCursor_NotImplemented)
            {
              int j;
              int nbChunks;
              CXString cursorKind;
              const char *str;
              CXString briefComment;

              cursorKind = clang_getCursorKindSpelling(completionResult->CursorKind);
              nbChunks = clang_getNumCompletionChunks(completionString);
              printf("%s => ", clang_getCString(cursorKind));
              clang_disposeString(cursorKind);

              for(j = 0; j < nbChunks; j++)
              {
                  CXString text;
                  enum CXCompletionChunkKind completionKind;

                  completionKind = clang_getCompletionChunkKind(completionString, j);
                  text = clang_getCompletionChunkText(completionString, j);
                  str = clang_getCString(text);
                  printf("%s ", /*clang_getCompletionChunkKindSpelling(completionKind),*/ str ? str : "");
                  clang_disposeString(text);

              }

              briefComment = clang_getCompletionBriefComment(completionString);
              str = clang_getCString(briefComment);

              if(str && str[0])
                printf("// %s", str);

              clang_disposeString(briefComment);
              printf("\n");
            }
        }

        puts("=================");
        n = clang_codeCompleteGetNumDiagnostics(results);

        for(i = 0; i != n; i++)
        {
          CXDiagnostic diag = clang_codeCompleteGetDiagnostic(results, i);
          PrintDiagnostic(diag);
          clang_disposeDiagnostic(diag);
        }

        // completion context
        puts("=================");
        contexts = clang_codeCompleteGetContexts(results);
        print_completion_contexts(contexts, stdout);

        // cursors
        puts("=================");
        CXFile file = clang_getFile(translationUnit, "../test/test_a.c");
        CXSourceLocation loc = clang_getLocation(translationUnit, file, line, column);
        CXCursor cursor = clang_getCursor(translationUnit, loc);
        CXSourceRange range = clang_getCursorExtent(cursor);
        CXToken *tokens;
        unsigned numTokens;
        clang_tokenize(translationUnit, range, &tokens, &numTokens);

        for(i = 0; i < numTokens; i++)
        {
            CXString string;
            const char *kind;
            CXTokenKind tokenKind = clang_getTokenKind(tokens[i]);

            switch(tokenKind)
            {
                case CXToken_Punctuation:
                    kind = "Punctuation";
                    break;
                case CXToken_Keyword:
                    kind = "Keyword";
                    break;
                case CXToken_Identifier:
                    kind = "Identifier";
                    break;
                case CXToken_Literal:
                    kind = "Literal";
                    break;
                case CXToken_Comment:
                    kind = "Comment";
                    break;
            }

            string = clang_getTokenSpelling(translationUnit, tokens[i]);
            printf("token \"%s\" (%s)\n", clang_getCString(string), kind);
            clang_disposeString(string);
        }

        clang_disposeTokens(translationUnit, tokens, numTokens);
        clang_disposeCodeCompleteResults(results);
        printf("-------------\n\n");
    }

    clang_disposeTranslationUnit(translationUnit);
    clang_disposeIndex(index);

    return EXIT_SUCCESS;
}


#if 0
int main(int argc, char *argv[])
{
  CXString string;
  CXIndex index = clang_createIndex(0, 0);
  CXTranslationUnit tu = clang_parseTranslationUnit(index, 0, (const char * const *)argv, argc, 0, 0, CXTranslationUnit_None);

  for(unsigned i = 0, n = clang_getNumDiagnostics(tu); i != n; i++)
  {
    CXDiagnostic diag = clang_getDiagnostic(tu, i);

    printf("Diagnostic:\n");
    string = clang_formatDiagnostic(diag, clang_defaultDiagnosticDisplayOptions());
    fprintf(stderr, "%s\n", clang_getCString(string));
    clang_disposeString(string);

    // core diagnostic information
    printf("Core diagnostic information:\n");
    string = clang_getDiagnosticSpelling(diag);
    fprintf(stderr, "%s\n", clang_getCString(string));
    clang_disposeString(string);
    // clang_getDiagnosticSeverity(diag);
    // clang_getDiagnosticLocation(diag);

    // fix it
    printf("Fixit:\n");
    for(unsigned j = 0, nfixit = clang_getDiagnosticNumFixIts(diag); j < nfixit; j++)
    {
      CXSourceRange range;
      string = clang_getDiagnosticFixIt(diag, j, &range);
      fprintf(stderr, "%s\n", clang_getCString(string));
      clang_disposeString(string);
    }

    printf("\n");
  }

  // cursors
  unsigned line = 13;
  unsigned column = 7;
  CXFile file = clang_getFile(tu, "test_a.c");
  CXSourceLocation loc = clang_getLocation(tu, file, line, column);
  CXCursor cursor = clang_getCursor(tu, loc);
  printf("Unified Symbol Resolution:\n");
  string = clang_getCursorUSR(cursor);
  fprintf(stderr, "%s\n", clang_getCString(string));
  clang_disposeString(string);

  // syntax highlighting
  // clang_tokenize(...);
  // clang_annotateTokens(...);

  clang_disposeTranslationUnit(tu);
  clang_disposeIndex(index);

  return 0;
}
#endif
