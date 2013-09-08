#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <clang-c/Index.h>


#define COLOR_NC "\033[0m"
#define COLOR_WHITE "\033[1;37m"
#define COLOR_BLACK "\033[0;30m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_LIGHT_BLUE "\033[1;34m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_LIGHT_GREEN "\033[1;32m"
#define COLOR_CYAN "\033[0;36m"
#define COLOR_LIGHT_CYAN "\033[1;36m"
#define COLOR_RED "\033[0;31m"
#define COLOR_LIGHT_RED "\033[1;31m"
#define COLOR_PURPLE "\033[0;35m"
#define COLOR_LIGHT_PURPLE "\033[1;35m"
#define COLOR_BROWN "\033[0;33m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_GRAY "\033[0;30m"
#define COLOR_LIGHT_GRAY "\033[0;37m"


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


enum CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    CXFile file = client_data;
    CXString cursorString, cursorKindString, USRString;
    CXSourceRange cursorRange = clang_getCursorExtent(cursor);
    CXSourceLocation cursorStart = clang_getRangeStart(cursorRange);
    CXSourceLocation cursorEnd = clang_getRangeEnd(cursorRange);
    unsigned startOffsetCursor, endOffsetCursor;
    unsigned startLine, endLine;
    unsigned startCol, endCol;
    clang_getSpellingLocation(cursorStart, file, &startLine, &startCol, &startOffsetCursor);
    clang_getSpellingLocation(cursorEnd, file, &endLine, &endCol, &endOffsetCursor);
    cursorString = clang_getCursorSpelling(cursor);
    enum CXCursorKind cursorKind = clang_getCursorKind(cursor);
    cursorKindString = clang_getCursorKindSpelling(cursorKind);
    USRString = clang_getCursorUSR(cursor);
    printf("cursor: \"%s\" (%d:%d)=>(%d:%d) %s\tUSR: \"%s\" \n",
        clang_getCString(cursorString), startLine, startCol, endLine, endCol,
        clang_getCString(cursorKindString), clang_getCString(USRString));
    clang_disposeString(cursorString);
    clang_disposeString(cursorKindString);
    clang_disposeString(USRString);

    return CXChildVisit_Recurse;
}


const char *cursorKindSpelling(enum CXTokenKind tokenKind)
{
    switch(tokenKind)
    {
        case CXToken_Punctuation:   return "punctuation";
        case CXToken_Comment:       return "comment";
        case CXToken_Keyword:       return "keyword";
        case CXToken_Identifier:    return "identifier";
        case CXToken_Literal:       return "literal";
        default:                    return "unknown token kind";
    }
}


char nextNonBlank(const char *str)
{
    while(*str && isspace(*str))
        str++;

    return *str;
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
    // parsingOptions |= CXTranslationUnit_SkipFunctionBodies;
    parsingOptions |= CXTranslationUnit_IncludeBriefCommentsInCodeCompletion;

    strcpy(fileName, argv[1]);
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
#if 0
        printf("Choose where to perform autocompletion (line column):\n> ");

        if(fgets(cmdLine, sizeof cmdLine, stdin) == NULL)
          break;

        scanRes = sscanf(cmdLine, "%d %d", &line, &column);
        // printf("%s:%d:%d (%d)", fileName, line, column, scanRes);

        if(scanRes != 2)
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
            translationUnit, fileName, line, column,
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
        clang_disposeCodeCompleteResults(results);
#endif
        // cursors
        puts("=================");
        CXFile file = clang_getFile(translationUnit,fileName);
        CXCursor cursor = clang_getTranslationUnitCursor(translationUnit);
        // clang_visitChildren(cursor, visitor, file);

        FILE *cfile = fopen(fileName, "r");
        fseek(cfile, 0, SEEK_END);
        int code_len = ftell(cfile);
        rewind(cfile);
        char *code = malloc(code_len + 1);
        fread(code, code_len, 1, cfile);
        code[code_len] = '\0';

        CXSourceLocation loc_start = clang_getLocation(translationUnit, file, 1, 1);
        CXSourceLocation loc_end = clang_getLocationForOffset(translationUnit, file, code_len);
        CXSourceRange range = clang_getRange(loc_start, loc_end);
        CXToken *tokens;
        unsigned numTokens;
        clang_tokenize(translationUnit, range, &tokens, &numTokens);
        CXCursor *cursors = malloc(sizeof(CXCursor) * numTokens);
        clang_annotateTokens(translationUnit, tokens, numTokens, cursors);

        unsigned currentPos = 0;
        unsigned currentLine = 1;
        printf("\n%s%04d:", COLOR_NC, currentLine++);

        for(i = 0; i < numTokens; i++)
        {
            CXString tokenString, cursorString, cursorKindString;
            CXTokenKind tokenKind = clang_getTokenKind(tokens[i]);
            enum CXCursorKind cursorKind = clang_getCursorKind(cursors[i]);
            CXSourceRange tokenRange = clang_getTokenExtent(translationUnit, tokens[i]);
            CXSourceRange cursorRange = clang_getCursorExtent(cursors[i]);
            CXSourceLocation tokenStart = clang_getRangeStart(tokenRange);
            CXSourceLocation tokenEnd = clang_getRangeEnd(tokenRange);
            CXSourceLocation cursorStart = clang_getRangeStart(cursorRange);
            CXSourceLocation cursorEnd = clang_getRangeEnd(cursorRange);
            unsigned startOffsetToken, endOffsetToken;
            clang_getSpellingLocation(tokenStart, file, NULL, NULL, &startOffsetToken);
            clang_getSpellingLocation(tokenEnd, file, NULL, NULL, &endOffsetToken);
            unsigned startOffsetCursor, endOffsetCursor;
            clang_getSpellingLocation(cursorStart, file, NULL, NULL, &startOffsetCursor);
            clang_getSpellingLocation(cursorEnd, file, NULL, NULL, &endOffsetCursor);

            tokenString = clang_getTokenSpelling(translationUnit, tokens[i]);
            cursorString = clang_getCursorSpelling(cursors[i]);
            cursorKindString = clang_getCursorKindSpelling(cursorKind);

#if 0
            printf("\ntoken: \"%s\" => [%d;%d] => %s(%d)\n", clang_getCString(tokenString),
                startOffsetToken, endOffsetToken, cursorKindSpelling(tokenKind), tokenKind);

            if(!clang_isInvalid(cursorKind))
                printf("cursor: \"%s\" => [%d;%d] => %s(%d)\n", clang_getCString(cursorString),
                    startOffsetCursor, endOffsetCursor, clang_getCString(cursorKindString), cursorKind);
            else
                printf("No cursor!!!\n");
#endif

            char *currentColor;
            char nextChar = nextNonBlank(code + currentPos);
            
            // punctuation in proprocessor directives
            if(tokenKind == CXToken_Punctuation)
            {
                if(cursorKind == CXCursor_InclusionDirective && strchr("<>/.\\", nextChar))
                    currentColor = COLOR_BROWN;
                else
                    currentColor = COLOR_WHITE;
            }
            else if(tokenKind == CXToken_Comment)
                currentColor = COLOR_GREEN;
            else if(cursorKind >= CXCursor_FirstInvalid && cursorKind <= CXCursor_LastInvalid && tokenKind == CXToken_Keyword)
                currentColor = COLOR_RED;
            else
            {                
                switch(cursorKind)
                {
                    // type created in a typedef
                    case CXCursor_TypedefDecl:
                        currentColor = COLOR_LIGHT_PURPLE;
                    break;

                    // enum/struct/union
                    case CXCursor_EnumDecl:
                    case CXCursor_UnionDecl:
                    case CXCursor_StructDecl:
                    case CXCursor_FieldDecl:
                        // built in types and enum/struct/union
                        if(tokenKind == CXToken_Keyword)
                            currentColor = COLOR_LIGHT_BLUE;
                        else // struct and field name 
                            currentColor = COLOR_WHITE;
                    break;

                    // variable declaration
                    case CXCursor_VarDecl:
                        // built in types and enum/struct/union
                        if(tokenKind == CXToken_Keyword)
                            currentColor = COLOR_LIGHT_BLUE;
                        else // variable name
                            currentColor = COLOR_WHITE;
                        break;

                    // static and const in variable declaration
                    case CXCursor_DeclStmt:
                        currentColor = COLOR_RED;
                    break;

                    // sizeof operator
                    case CXCursor_UnexposedExpr:
                        if(tokenKind == CXToken_Keyword)
                            currentColor = COLOR_RED;
                        else
                            currentColor = COLOR_WHITE;
                    break;

                    // control statements
                    case CXCursor_CaseStmt:
                    case CXCursor_SwitchStmt:
                    case CXCursor_WhileStmt:
                    case CXCursor_DoStmt:
                    case CXCursor_ForStmt:
                    case CXCursor_GotoStmt:
                    case CXCursor_ContinueStmt:
                    case CXCursor_BreakStmt:
                    case CXCursor_ReturnStmt:
                        currentColor = COLOR_RED;
                        break;

                    // num literals
                    case CXCursor_IntegerLiteral:
                    case CXCursor_FloatingLiteral:
                        currentColor = COLOR_PURPLE;
                        break;

                    // string literals
                    case CXCursor_CharacterLiteral:
                    case CXCursor_StringLiteral:
                        currentColor = COLOR_BROWN;
                        break;

                    // parameter declaration
                    case CXCursor_ParmDecl:
                        if(tokenKind == CXToken_Keyword)
                            currentColor = COLOR_LIGHT_BLUE;
                        else
                            currentColor = COLOR_WHITE;
                        break;

                    // function declaration
                    case CXCursor_FunctionDecl:
                        // 
                        if(tokenKind == CXToken_Keyword)
                            currentColor = COLOR_LIGHT_BLUE;
                        else
                            currentColor = COLOR_LIGHT_GREEN;
                        break;

                    case CXCursor_DeclRefExpr:
                        // function calls
                        if(clang_getCursorType(cursors[i]).kind == CXType_FunctionProto)
                        {
                            CXCursor definition = clang_getCursorDefinition(cursors[i]);

                            if(!clang_Cursor_isNull(definition))
                            {
                                CXSourceRange defRange = clang_getCursorExtent(definition);
                                CXSourceLocation defLocation = clang_getRangeStart(defRange);
                                unsigned line;
                                clang_getSpellingLocation(defLocation, file, &line, NULL, NULL);

                                printf("// fonction %s() definie à la ligne %d\n", clang_getCString(tokenString), line);
                            }
                            currentColor = COLOR_LIGHT_BLUE;
                        }
                        else // variables and parameters references
                            currentColor = COLOR_WHITE;
                        break;

                    case CXCursor_MacroExpansion:
                        currentColor = COLOR_LIGHT_BLUE;
                    break;

                    // #if #else #endif #define
                    case CXCursor_PreprocessingDirective:
                        if(tokenKind == CXToken_Keyword)
                            currentColor = COLOR_RED;
                        else if(tokenKind == CXToken_Literal)
                            currentColor = COLOR_PURPLE;
                        else if(tokenKind == CXToken_Identifier &&
                            !strcmp(clang_getCString(tokenString), "define"))
                            currentColor = COLOR_RED;
                        else if(tokenKind == CXToken_Identifier &&
                            !strcmp(clang_getCString(tokenString), "endif"))
                            currentColor = COLOR_RED;
                        else
                            currentColor = COLOR_WHITE;
                        break;

                    // macro definition
                    case CXCursor_MacroDefinition:
                        if(tokenKind == CXToken_Literal)
                        {
                            if(strchr(clang_getCString(tokenString), '"'))
                                currentColor = COLOR_BROWN;
                            else
                                currentColor = COLOR_PURPLE;
                        }
                        else
                            currentColor = COLOR_WHITE;
                        break;

                    // includes
                    case CXCursor_InclusionDirective:
                        if(tokenKind == CXToken_Literal)
                            currentColor = COLOR_BROWN;
                        else if(tokenKind == CXToken_Identifier)
                        {
                            if(!strcmp(clang_getCString(tokenString), "include"))
                                currentColor = COLOR_RED;
                            else
                                currentColor = COLOR_BROWN;
                        }
                        else
                            currentColor = COLOR_WHITE;
                        break;
                        
                    // user defined types
                    case CXCursor_TypeRef:
                        if(clang_getCursorType(cursors[i]).kind)
                            currentColor = COLOR_LIGHT_PURPLE;
                        break;

                    default:
                        currentColor = COLOR_WHITE;
                }
            }

#if 0
            enum CXAvailabilityKind availability = clang_getCursorAvailability(cursors[i]);

            switch(availability)
            {
                case CXAvailability_Available:
                    currentColor = COLOR_WHITE;
                    break;
                case CXAvailability_Deprecated:
                    currentColor = COLOR_YELLOW;
                    break;
                case CXAvailability_NotAvailable:
                    currentColor = COLOR_GRAY;
                    break;
                case CXAvailability_NotAccessible:
                    currentColor = COLOR_RED;
                    break;
            }
#endif
            // flush blanks
            while(isspace(code[currentPos]))
            {
                if(code[currentPos] == '\n')
                    printf("\n%04d:", currentLine++);
                else
                    printf("%c", code[currentPos]);

                currentPos++;
            }

            // apply token color
            printf("%s", currentColor);

            // print token
            while(currentPos < endOffsetToken)
            {
                if(code[currentPos] == '\n')
                    printf("%s\n%04d:%s", COLOR_NC, currentLine++, currentColor);
                else
                    printf("%c", code[currentPos]);

                currentPos++;
            }

            // apply default color
            printf(COLOR_NC);

            // flush blanks
            while(isspace(code[currentPos]))
            {
                if(code[currentPos] == '\n')
                    printf("\n%04d:", currentLine++);
                else
                    printf("%c", code[currentPos]);

                currentPos++;
            }

            printf(COLOR_NC);

            clang_disposeString(tokenString);
            clang_disposeString(cursorString);
            clang_disposeString(cursorKindString);
        }

        printf("\n");
        clang_disposeTokens(translationUnit, tokens, numTokens);
        free(cursors);
        printf("-------------\n");
        break;
    }

    clang_disposeTranslationUnit(translationUnit);
    clang_disposeIndex(index);

    return EXIT_SUCCESS;
}
