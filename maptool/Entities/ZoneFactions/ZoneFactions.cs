using System;
using System.Collections.Generic;
using System.Text;

namespace Maptool
{
    public enum Faction
    {
        Individual = 0,
        Team1 = 1,
        Team2 = 2,
        Team3 = 3,
        Team4 = 4,
        Team5 = 5,
        Team6 = 6,
        Team7 = 7,
        Team8 = 8,
        Common = 9,
    };

    public class ZoneFactions
    {
        private bool[] factions = new bool[10];

        public override string ToString()
        {
            string o = "";

            for (int i = 0; i < 10; i++) {
                if (factions[i]) {
                    o += ((Faction)i).ToString() + ", ";
                }
            }

            char[] trimchars = { ' ', ',' };
            return o.Trim(trimchars);
        }

        public bool getValue(Faction fac)
        {
            return this.factions[(int)fac];
        }

        public void setValue(Faction fac, bool val)
        {
            this.factions[(int)fac] = val;
        }
    }
}
