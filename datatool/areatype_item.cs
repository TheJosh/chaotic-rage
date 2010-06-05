using System;
using System.Collections.Generic;
using System.Text;

namespace datatool
{
    public class areatype_item : base_item
    {
        private string name;

        public areatype_item(string name)
        {
            this.name = name;
        }

        override public string getName()
        {
            return this.name;
        }
    }
}
