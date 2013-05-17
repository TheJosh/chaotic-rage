using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace Maptool
{
    public class EntityType
    {
        protected string name;
        protected Image image;

        public string Name
        {
            get { return this.name; }
        }

        public Image Image
        {
            get { return this.image; }
            set { this.image = value; }
        }

        public override string ToString()
        {
            return this.name;
        }
    }
}
