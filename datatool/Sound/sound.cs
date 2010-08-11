using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    [Serializable]
    public class sound : base_item
    {
        private string name;

        override public string getName()
        {
            return this.name;
        }


        [DescriptionAttribute("The name of this sound, internal use only")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }
    }
}
