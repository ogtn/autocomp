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
    static const char *filename;
    static const char *cmdline = "";
    CXCodeCompleteResults *results = NULL;
    CXTranslationUnit translationUnit = 0;
    unsigned long long contexts;
    unsigned completionOptions;
    unsigned parsingOptions;
    CXIndex index;
    unsigned i, n;
    int line, column;

    if(argc != 4)
    {
        fprintf(stderr, "Missing arguments!\n");
        fprintf(stderr, "Usage: ./clang_dev file line column!\n");
        return EXIT_FAILURE;
    }

    // arguments
    filename = argv[1];
    line = atoi(argv[2]);
    column = atoi(argv[3]);

    // parsing
    parsingOptions = clang_defaultEditingTranslationUnitOptions();
    parsingOptions |= CXTranslationUnit_DetailedPreprocessingRecord;
    parsingOptions |= CXTranslationUnit_CacheCompletionResults;
    parsingOptions |= CXTranslationUnit_SkipFunctionBodies;
    parsingOptions |= CXTranslationUnit_IncludeBriefCommentsInCodeCompletion;

    index = clang_createIndex(0, 1);
    translationUnit = clang_parseTranslationUnit(
        index, filename,
        &cmdline,
        0, // cmdline nbargs
        NULL, // unsaved files
        0, // nb unsaved files
        parsingOptions);

    if(translationUnit == NULL)
    {
        fprintf(stderr, "Unable to load translation unit!\n");
        return EXIT_FAILURE;
    }

    if(clang_reparseTranslationUnit(translationUnit, 0, 0, clang_defaultReparseOptions(translationUnit)))
    {
        fprintf(stderr, "Unable to reparse translation init!\n");
        return EXIT_FAILURE;
    }

    // code completion
    completionOptions = clang_defaultCodeCompleteOptions();
    completionOptions |= CXCodeComplete_IncludeMacros;
    completionOptions |= CXCodeComplete_IncludeCodePatterns;
    completionOptions |= CXCodeComplete_IncludeBriefComments;

    results = clang_codeCompleteAt(
        translationUnit, filename, line, column,
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
          printf("\n---\n");
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

    puts("=================");
    contexts = clang_codeCompleteGetContexts(results);
    print_completion_contexts(contexts, stdout);

    clang_disposeCodeCompleteResults(results);
    clang_disposeTranslationUnit(translationUnit);
    clang_disposeIndex(index);

    return EXIT_SUCCESS;
}
