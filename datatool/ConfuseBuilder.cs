using System;
using System.Collections.Generic;
using System.Text;

namespace datatool
{
    class ConfuseBuilder
    {
        public Dictionary<string, object> values;

        public ConfuseBuilder()
        {
            values = new Dictionary<string, object>();
        }

        public override string ToString()
        {
            string o = "";

            foreach (KeyValuePair<string, object> kvp in values) {
                if (kvp.Value is string) {
                    o += kvp.Key + " = \"" + kvp.Value.ToString() + "\" ";

                } else if (kvp.Value is int) {
                    o += kvp.Key + " = " + kvp.Value.ToString() + " ";

                } else if (kvp.Value is bool) {
                    o += kvp.Key + " = " + ((bool)kvp.Value ? "1" : "0") + " ";

                } else if (kvp.Value is float) {
                    o += kvp.Key + " = " + kvp.Value.ToString() + " ";

                } else if (kvp.Value is base_item) {
                    o += kvp.Key + " = \"" + kvp.Value.ToString() + "\" ";

                } else if (kvp.Value is range) {
                    o += kvp.Key + " = {" + ((range)kvp.Value).min  + "," + ((range)kvp.Value).max + "} ";
                }
            }

            return o;
        }
    }
}
