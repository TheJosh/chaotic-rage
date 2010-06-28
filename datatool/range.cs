using System;
using System.Collections.Generic;
using System.Text;

namespace datatool
{
    public class range
    {
        public int min;
        public int max;


        public range()
        {
        }

        public range(int num)
        {
            this.min = num;
            this.max = num;
        }

        public range(int min, int max)
        {
            this.min = min;
            this.max = max;
        }


        /**
         * Converts a range to a string
         **/
        public string toString()
        {
            if (min == max) return min.ToString();
            return min + "," + max;
        }

        /**
         * Parses a string and returns a range, or NULL on error
         **/
        static public range fromString(string input)
        {
            int min = 0, max = 0;

            string[] parts = input.Split(","[0]);

            if (parts.Length == 1) {
                try {
                    min = max = int.Parse(parts[0].Trim());
                } catch {}

            } else if (parts.Length == 2) {
                try {
                    min = int.Parse(parts[0].Trim());
                    max = int.Parse(parts[1].Trim());
                    return new range(min, max);
                } catch {}
            }

            return new range(min, max);
        }
    }
}
