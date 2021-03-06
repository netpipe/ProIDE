/*
 * Copyright (c) 2019 Waqar Ahmed -- <waqar.17a@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */
#ifndef QSOURCEHIGHLITER_H
#define QSOURCEHIGHLITER_H

#include <QSyntaxHighlighter>

namespace QSourceHighlite {

class QSourceHighliter : public QSyntaxHighlighter
{
public:
    enum Themes {
        Monokai = 1,
        Light = 2
    };

    explicit QSourceHighliter(QTextDocument *doc);
    QSourceHighliter(QTextDocument *doc, Themes theme);

    //languages
    /*********
     * When adding a language make sure that its value is a multiple of 2
     * This is because we use the next number as comment for that language
     * In case the language doesn't support multiline comments in the traditional C++
     * sense, leave the next value empty. Otherwise mark the next value as comment for
     * that language.
     * e.g
     * CodeCpp = 200
     * CodeCppComment = 201
     */
    enum Language {
        //languages
        CodeCpp = 200,
        CodeCppComment = 201,
        CodeJs = 202,
        CodeJsComment = 203,
        CodeC = 204,
        CodeCComment = 205,
        CodeBash = 206,
        CodePHP = 208,
        CodePHPComment = 209,
        CodeQML = 210,
        CodeQMLComment = 211,
        CodePython = 212,
        CodeRust = 214,
        CodeRustComment = 215,
        CodeJava = 216,
        CodeJavaComment = 217,
        CodeCSharp = 218,
        CodeCSharpComment = 219,
        CodeGo = 220,
        CodeGoComment = 221,
        CodeV = 222,
        CodeVComment = 223,
        CodeSQL = 224,
        CodeJSON = 226,
        CodeXML = 228,
        CodeCSS = 230,
        CodeCSSComment = 231,
        CodeTypeScript = 232,
        CodeTypeScriptComment = 233,
        CodeYAML = 234,
        CodeINI = 236,
        CodeVex = 238,
        CodeVexComment = 239,
        CodeCMake = 240,
        CodeMake = 242,
        CodeAsm = 244,
    };
    Q_ENUM(Language)

    enum Token {
        CodeBlock,
        CodeKeyWord,
        CodeString,
        CodeComment,
        CodeType,
        CodeOther,
        CodeNumLiteral,
        CodeBuiltIn,
    };
    Q_ENUM(Token)

    void setCurrentLanguage(Language language);
    Q_REQUIRED_RESULT Language currentLanguage();
    void setTheme(Themes theme);

protected:
    void highlightBlock(const QString &text) override;

private:
    void highlightSyntax(const QString &text);
    Q_REQUIRED_RESULT int highlightNumericLiterals(const QString &text, int i);
    Q_REQUIRED_RESULT int highlightStringLiterals(const QChar strType, const QString &text, int i);

    /**
     * @brief returns true if c is octal
     * @param c the char being checked
     * @returns true if the number is octal, false otherwise
     */
    Q_REQUIRED_RESULT static constexpr inline bool isOctal(const char c) {
        return (c >= '0' && c <= '7');
    }

    /**
     * @brief returns true if c is hex
     * @param c the char being checked
     * @returns true if the number is hex, false otherwise
     */
    Q_REQUIRED_RESULT static constexpr inline bool isHex(const char c) {
        return (
            (c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'f') ||
            (c >= 'A' && c <= 'F')
        );
    }

    void cssHighlighter(const QString &text);
    void ymlHighlighter(const QString &text);
    void xmlHighlighter(const QString &text);
    void makeHighlighter(const QString &text);
    void highlightInlineAsmLabels(const QString& text);
    void asmHighlighter(const QString& text);
    void initFormats();

    QHash<Token, QTextCharFormat> _formats;
    Language _language;
};
}
#endif // QSOURCEHIGHLITER_H
