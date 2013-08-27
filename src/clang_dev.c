#include <stdio.h>
#include <stdlib.h>
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


void print_completion_string(CXCompletionString completion_string, FILE *file)
{
  int I, N;

  N = clang_getNumCompletionChunks(completion_string);
  printf("\nN = %d\n", N);
  for (I = 0; I != N; ++I) {
    CXString text;
    const char *cstr;
    enum CXCompletionChunkKind Kind
      = clang_getCompletionChunkKind(completion_string, I);

    if (Kind == CXCompletionChunk_Optional) {
      fprintf(file, "{Optional ");
      print_completion_string(
                clang_getCompletionChunkCompletionString(completion_string, I),
                              file);
      fprintf(file, "}");
      continue;
    }

    if (Kind == CXCompletionChunk_VerticalSpace) {
      fprintf(file, "{VerticalSpace  }");
      continue;
    }

    text = clang_getCompletionChunkText(completion_string, I);
    cstr = clang_getCString(text);
    fprintf(file, "{%s %s}",
            clang_getCompletionChunkKindSpelling(Kind),
            cstr ? cstr : "");
    clang_disposeString(text);
  }

}


void print_completion_result(CXCompletionResult *completion_result,
                             CXClientData client_data) {
  FILE *file = (FILE *)client_data;
  CXString ks = clang_getCursorKindSpelling(completion_result->CursorKind);
  unsigned annotationCount;
  enum CXCursorKind ParentKind;
  CXString ParentName;
  CXString BriefComment;
  const char *BriefCommentCString;

  fprintf(file, "%s:", clang_getCString(ks));
  clang_disposeString(ks);

  print_completion_string(completion_result->CompletionString, file);
  fprintf(file, " (%u)",
          clang_getCompletionPriority(completion_result->CompletionString));
  switch (clang_getCompletionAvailability(completion_result->CompletionString)){
  case CXAvailability_Available:
    break;

  case CXAvailability_Deprecated:
    fprintf(file, " (deprecated)");
    break;

  case CXAvailability_NotAvailable:
    fprintf(file, " (unavailable)");
    break;

  case CXAvailability_NotAccessible:
    fprintf(file, " (inaccessible)");
    break;
  }

  annotationCount = clang_getCompletionNumAnnotations(
        completion_result->CompletionString);
  if (annotationCount) {
    unsigned i;
    fprintf(file, " (");
    for (i = 0; i < annotationCount; ++i) {
      if (i != 0)
        fprintf(file, ", ");
      fprintf(file, "\"%s\"",
              clang_getCString(clang_getCompletionAnnotation(
                                 completion_result->CompletionString, i)));
    }
    fprintf(file, ")");
  }

  if (!getenv("CINDEXTEST_NO_COMPLETION_PARENTS")) {
    ParentName = clang_getCompletionParent(completion_result->CompletionString,
                                           &ParentKind);
    if (ParentKind != CXCursor_NotImplemented) {
      CXString KindSpelling = clang_getCursorKindSpelling(ParentKind);
      fprintf(file, " (parent: %s '%s')",
              clang_getCString(KindSpelling),
              clang_getCString(ParentName));
      clang_disposeString(KindSpelling);
    }
    clang_disposeString(ParentName);
  }

  BriefComment = clang_getCompletionBriefComment(
                                        completion_result->CompletionString);
  BriefCommentCString = clang_getCString(BriefComment);
  if (BriefCommentCString && *BriefCommentCString != '\0') {
    fprintf(file, "(brief comment: %s)", BriefCommentCString);
  }
  clang_disposeString(BriefComment);

  fprintf(file, "\n");
}


int main(int argc, char const *argv[])
{
    static const char *filename;
    static const char *cmdline = "";
    CXCodeCompleteResults *results = NULL;
    CXTranslationUnit translationUnit = 0;
    unsigned completionOptions;
    unsigned parsingOptions;
    CXIndex index;
    unsigned i;
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
    parsingOptions = CXTranslationUnit_DetailedPreprocessingRecord;
    parsingOptions |= clang_defaultEditingTranslationUnitOptions();
    parsingOptions |= CXTranslationUnit_CacheCompletionResults;
    parsingOptions &= ~CXTranslationUnit_CacheCompletionResults;
    parsingOptions |= CXTranslationUnit_SkipFunctionBodies;
    parsingOptions |= CXTranslationUnit_IncludeBriefCommentsInCodeCompletion;

    index = clang_createIndex(0, 0);
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
        int j;
        int nbChunks;
        CXString cursorKind;
        CXCompletionResult *completionResult = results->Results + i;
        CXCompletionString *completionString = completionResult->CompletionString;

        cursorKind = clang_getCursorKindSpelling(completionResult->CursorKind);
        nbChunks = clang_getNumCompletionChunks(completionString);
        printf("%s => ", clang_getCString(cursorKind));
        clang_disposeString(cursorKind);

        for(j = 0; j < nbChunks; j++)
        {
            const char *str;
            CXString text;
            enum CXCompletionChunkKind completionKind;

            completionKind = clang_getCompletionChunkKind(completionString, j);
            text = clang_getCompletionChunkText(completionString, j);
            str = clang_getCString(text);
            printf("%s ", /*clang_getCompletionChunkKindSpelling(completionKind),*/ str ? str : "");
            clang_disposeString(text);
        }

        printf("\n");

        // print_completion_result(completionResult, stdout);
        // print_completion_string(completionString, stdout);
    }

    clang_disposeCodeCompleteResults(results);
    clang_disposeTranslationUnit(translationUnit);
    clang_disposeIndex(index);

    return EXIT_SUCCESS;
}
