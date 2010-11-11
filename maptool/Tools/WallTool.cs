using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Maptool
{
    class WallTool : Tool
    {
        public override Entity createEntity()
        {
            return new WallEntity();
        }

        public override bool isThisOurs(Entity ent)
        {
            return (ent is WallEntity);
        }
    }
}
