using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Maptool
{
    abstract class Tool
    {
        public abstract Entity createEntity();

        public abstract bool isThisOurs(Entity ent);
    }
}
