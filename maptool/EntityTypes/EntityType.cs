using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Maptool
{
    class EntityType
    {
        protected string name;

        public string Name
        {
            get { return this.name; }
        }

        public override string ToString()
        {
            return this.name;
        }
    }
}
