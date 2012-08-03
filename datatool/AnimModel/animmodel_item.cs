using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    public class animmodel_item : base_item 
    {
        private string name;


        public animmodel_item(string name)
        {
            this.name = name;
        }

        override public string getName()
        {
            return this.name;
        }


        [DescriptionAttribute("The name of this item, internal use only")]
        [Category("General")]
        [ConfuseString("name")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }


    }
}
