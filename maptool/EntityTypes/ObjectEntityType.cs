using System;
using System.Collections.Generic;
using System.Text;

namespace Maptool
{
    class ObjectEntityType : EntityType
    {
        public ObjectEntityType(string name)
        {
            this.name = name;
            this.image = null;
        }
    }
}
