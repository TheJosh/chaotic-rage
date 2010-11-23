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

        public override bool isAreaBased()
        {
            return false;
        }

        public override List<EntityType> getTypes()
        {
            List<EntityType> ret = new List<EntityType>();

            ret.Add(new WallEntityType("wall"));

            return ret;
        }
    }
}
