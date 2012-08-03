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

    public class ConfuseSection
    {
        public string name;
        public List<ConfuseSection> subsections;
        public Dictionary<string, object> values;

        public ConfuseSection()
        {
            subsections = new List<ConfuseSection>();
            values = new Dictionary<string, object>();
        }

        /**
         * Get a string value. If not available, returns the default
         **/
        public string get_string(string name, string def)
        {
            if (!(this.values.ContainsKey(name))) return def;
            if (! (this.values[name] is string)) return def;
            return (string)this.values[name];
        }

        /**
         * Get a integer value. If not available, returns the default
         **/
        public int get_int(string name, int def)
        {
            if (!(this.values.ContainsKey(name))) return def;
            if (!(this.values[name] is int)) return def;
            return (int)this.values[name];
        }

        /**
         * Get a float value. If not available, returns the default
         **/
        public float get_float(string name, float def)
        {
            if (!(this.values.ContainsKey(name))) return def;
            if (!(this.values[name] is float)) return def;
            return (float)this.values[name];
        }

        /**
         * Get a bool value. If not available, returns the default
         **/
        public bool get_bool(string name, bool def)
        {
            if (!(this.values.ContainsKey(name))) return def;
            if (!(this.values[name] is int)) return def;

            return ((int)this.values[name] == 1) ? true : false;
        }

        /**
         * Get a range value. If not available, returns the default
         **/
        public range get_range(string name, range def)
        {
            if (!(this.values.ContainsKey(name))) return def;

            if (this.values[name] is List<object>) {
                return new range((int)((List<object>)this.values[name])[0], (int)((List<object>)this.values[name])[1]);
            } else if (this.values[name] is int) {
                return new range((int) this.values[name]);
            } else {
                return def;
            }
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
            lex(input);
            return dosect(tokens);
        }


        private ConfuseSection dosect(List<ConfuseToken> tokens)
        {
            ConfuseSection sect = new ConfuseSection();

            string ident = null;
            bool eq = false;
            bool list = false;

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
                            subsect.name = ident;
                            sect.subsections.Add(subsect);
                            sub = null;
                            ident = null;
                            continue;
                        }
                    }
                    sub.Add(t);
                    continue;
                }

                // Indetifier
                if (ident == null && !eq && !list && t.type == ConfuseTokenType.IDENTIFIER) {
                    ident = t.val;

                // Equals
                } else if (ident != null && !eq && !list && t.type == ConfuseTokenType.EQUALS) {
                    eq = true;

                // Regular string, float, integer
                } else if (ident != null && eq && !list && t.type == ConfuseTokenType.STRING) {
                    sect.values.Add(ident, t.val);
                    ident = null;
                    eq = false;

                } else if (ident != null && eq && !list && t.type == ConfuseTokenType.NUMBER) {
                    if (t.val.Contains(".")) {
                        sect.values.Add(ident, float.Parse(t.val));
                    } else {
                        sect.values.Add(ident, int.Parse(t.val));
                    }
                    ident = null;
                    eq = false;


                // List start
                } else if (ident != null && eq && !list && t.type == ConfuseTokenType.SECTSTART) {
                    sect.values.Add(ident, new List<object>());
                    list = true;

                // List comma (do nothing)
                } else if (ident != null && eq && list && t.type == ConfuseTokenType.COMMA) {

                // List string, number
                } else if (ident != null && eq && list && t.type == ConfuseTokenType.STRING) {
                    List<object> obj = (List<object>)sect.values[ident];
                    obj.Add(t.val);

                } else if (ident != null && eq && list && t.type == ConfuseTokenType.NUMBER) {
                    List<object> obj = (List<object>)sect.values[ident];
                    if (t.val.Contains(".")) {
                        obj.Add(float.Parse(t.val));
                    } else {
                        obj.Add(int.Parse(t.val));
                    }


                // List end
                } else if (ident != null && eq && list && t.type == ConfuseTokenType.SECTEND) {
                    ident = null;
                    eq = false;
                    list = false;


                // Section start
                } else if (ident != null && !eq && !list && t.type == ConfuseTokenType.SECTSTART) {
                    sub = new List<ConfuseToken>();
                    subcnt++;


                // Not sure
                } else {
                    throw new Exception("Unexpected token " + t.type.ToString() + " while parsing config file. ident = " + ident + " eq=" + eq + " list=" + list);
                }
            }

            return sect;
        }


        /**
         * Lexer
         **/
        private void lex(string input)
        {
            int len = input.Length;
            Match m;

            tokens = new List<ConfuseToken>();

            Dictionary<string,ConfuseTokenType> regexes = new Dictionary<string,ConfuseTokenType>();

            regexes.Add("^[a-z_]+", ConfuseTokenType.IDENTIFIER);
            regexes.Add("^=", ConfuseTokenType.EQUALS);
            regexes.Add("^{", ConfuseTokenType.SECTSTART);
            regexes.Add("^}", ConfuseTokenType.SECTEND);
            regexes.Add("^,", ConfuseTokenType.COMMA);
            regexes.Add("^\"([^\"]+)\"", ConfuseTokenType.STRING);
            regexes.Add("^[-0-9.]+", ConfuseTokenType.NUMBER);
            regexes.Add("^\\s+", ConfuseTokenType.NOTHING);
            regexes.Add("^//.*?\\n", ConfuseTokenType.NOTHING);
            regexes.Add("^/\\*.*\\*/", ConfuseTokenType.NOTHING);
            regexes.Add("^.", ConfuseTokenType.ERROR);

            while (input.Length > 0) {
                foreach (KeyValuePair<string, ConfuseTokenType> kvp in regexes) {
                    m = Regex.Match(input, kvp.Key, RegexOptions.Singleline);
                    if (m.Success) {
                        processMatch(m, kvp.Value);
                        input = input.Substring (m.Length);
                        break;
                    }
                }
            }
        }

        /**
         * Creates a token
         **/
        private void processMatch(Match m, ConfuseTokenType type)
        {
            if (type == ConfuseTokenType.NOTHING) return;
            if (type == ConfuseTokenType.ERROR) throw new Exception("Unable to find a valid token for '" + m.Value + "'.");

            ConfuseToken t = new ConfuseToken();
            t.type = type;
            tokens.Add(t);

            if (type == ConfuseTokenType.STRING) {
                t.val = m.Groups[1].Value;
            } else {
                t.val = m.Value; 
            }

            return;
        }
    }
}
