using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    [Serializable]
    public class modifier_item : base_item
    {
        private string name;


        public modifier_item(string name)
        {
            this.name = name;
        }

        override public string getName()
        {
            return this.name;
        }


        [DescriptionAttribute("The name of this modifier, internal use only")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }
    }
}
