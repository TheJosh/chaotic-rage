using System;
using System.Collections.Generic;
using System.Text;

namespace Maptool
{
    class ZoneTool : Tool
    {
        public override Entity createEntity()
        {
            return new ZoneEntity();
        }

        public override bool isThisOurs(Entity ent)
        {
            return (ent is ZoneEntity);
        }

        public override bool isAreaBased()
        {
            return true;
        }

        public override List<EntityType> getTypes()
        {
            List<EntityType> ret = new List<EntityType>();
            ret.Add(new ZoneEntityType());
            return ret;
        }
    }
}
