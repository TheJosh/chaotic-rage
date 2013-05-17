using System;
using System.Collections.Generic;
using System.Text;
using datatool;

namespace Maptool
{
    class LightTool : Tool
    {
        private List<EntityType> types;

        public LightTool()
        {
            types = new List<EntityType>();
            types.Add(new LightEntityType("point"));
        }


        public override Entity createEntity()
        {
            return new LightEntity();
        }

        public override bool isThisOurs(Entity ent)
        {
            return (ent is LightEntity);
        }

        public override bool isAreaBased()
        {
            return true;
        }

        public override List<EntityType> getTypes()
        {
            return types;
        }

        public override string saveConfig(Entity ent)
        {
            string o = "";
            LightEntity z = (LightEntity)ent;

            o += "light {";
            o += "  x = " + z.X;
            o += "  y = " + z.Y;
            o += "}";

            return o;
        }

        public override Entity loadConfig(ConfuseSection sect)
        {
            LightEntity ent = new LightEntity();
            ent.Type = types[0];

            ent.X = sect.get_int("x", 0);
            ent.Y = sect.get_int("y", 0);

            return ent;
        }
    }
}
