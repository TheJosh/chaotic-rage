using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using System.Diagnostics;

namespace datatool
{
    enum ConfuseTokenType
    {
        NOTHING,
        ERROR,
        IDENTIFIER,
        COMMA,
        EQUALS,
        STRING,
        NUMBER,
        SECTSTART,
        SECTEND,
    } 

    class ConfuseToken
    {
        public ConfuseTokenType type;
        public string val;
    }

    class ConfuseSection
    {
        public List<ConfuseSection> subsections;
        public Dictionary<string, object> values;

        public ConfuseSection()
        {
            subsections = new List<ConfuseSection>();
            values = new Dictionary<string, object>();
        }
    }


    class ConfuseReader
    {
        private List<ConfuseToken> tokens;


        /**
         * Does parsing
         **/
        public ConfuseSection Parse(string input)
        {
            if (! lex(input)) return null;

            return dosect(tokens);
        }


        private ConfuseSection dosect(List<ConfuseToken> tokens)
        {
            ConfuseSection sect = new ConfuseSection();

            string ident = null;
            bool eq = false;
            int subcnt = 0;
            List<ConfuseToken> sub = null;

            foreach (ConfuseToken t in tokens) {
                // In a subsection
                if (sub != null) {
                    if (t.type == ConfuseTokenType.SECTSTART) {
                        subcnt++;
                    } else if (t.type == ConfuseTokenType.SECTEND) {
                        subcnt--;
                        if (subcnt == 0) {
                            ConfuseSection subsect = dosect(sub);
                            if (subsect == null) return null;
                            sect.subsections.Add(subsect);
                            sub = null;
                        }
                    }
                    sub.Add(t);
                    continue;
                }

                // Regular
                if (ident != null && ! eq && t.type == ConfuseTokenType.IDENTIFIER) {
                    ident = t.val;

                } else if (ident != null && ! eq && t.type == ConfuseTokenType.EQUALS) {
                    eq = true;

                } else if (ident != null && eq && t.type == ConfuseTokenType.STRING) {
                    sect.values.Add(ident, t.val);
                    ident = null;
                    eq = false;

                } else if (ident != null && eq && t.type == ConfuseTokenType.NUMBER) {
                    sect.values.Add(ident, int.Parse(t.val));
                    ident = null;
                    eq = false;

                } else if (ident != null && ! eq && t.type == ConfuseTokenType.SECTSTART) {
                    sub = new List<ConfuseToken>();
                    subcnt++;
                }
            }

            return sect;
        }


        /**
         * Lexer
         **/
        private bool lex(string input)
        {
            int len = input.Length;
            Match m;

            Dictionary<string,ConfuseTokenType> regexes = new Dictionary<string,ConfuseTokenType>();

            regexes.Add("^[a-z_]+", ConfuseTokenType.IDENTIFIER);
            regexes.Add("^=", ConfuseTokenType.EQUALS);
            regexes.Add("^{", ConfuseTokenType.SECTSTART);
            regexes.Add("^}", ConfuseTokenType.SECTEND);
            regexes.Add("^,", ConfuseTokenType.COMMA);
            regexes.Add("^\"[^\"]+\"", ConfuseTokenType.STRING);
            regexes.Add("^[-0-9]+", ConfuseTokenType.NUMBER);
            regexes.Add("^\\s+", ConfuseTokenType.NOTHING);
            regexes.Add("^//.*?\\n", ConfuseTokenType.NOTHING);
            regexes.Add("^/\\*.*\\*/", ConfuseTokenType.NOTHING);
            regexes.Add("^.", ConfuseTokenType.ERROR);

            tokens = new List<ConfuseToken>();

            while (input.Length > 0) {
                foreach (KeyValuePair<string, ConfuseTokenType> kvp in regexes) {
                    m = Regex.Match(input, kvp.Key, RegexOptions.Singleline);
                    if (m.Success) {
                        if (!processMatch(m, kvp.Value)) return false;
                        input = input.Substring (m.Length);
                        break;
                    }
                }
            }

            return true;
        }

        /**
         * Creates a token
         **/
        private bool processMatch(Match m, ConfuseTokenType type)
        {
            if (type == ConfuseTokenType.NOTHING) return true;
            if (type == ConfuseTokenType.ERROR) return false;

            ConfuseToken t = new ConfuseToken();
            t.type = type;
            t.val = m.Value;
            tokens.Add(t);

            return true;
        }
    }
}
